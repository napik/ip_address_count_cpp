#include "FileReader.h"

#include <future>
#include <vector>
#include <stdexcept>
#include <bitset>
#include <iostream>
#include <atomic>
#include <thread>
#include <filesystem>
#include <memory>
#include <cctype>
#include <algorithm>

FileReader::FileReader() {
    std::ranges::fill(bitArray.begin(), bitArray.end(), 0);
}

FileReader::FileReader(const std::string &filePath) {
    namespace fs = std::filesystem;
    if (!fs::exists(filePath)) {
        throw std::invalid_argument("File does not exist");
    }

    fileStream.open(filePath, std::ios::in | std::ios::binary);
    if (!fileStream.is_open()) {
        throw std::invalid_argument("Error opening file");
    }
    fileStream.seekg(0, std::ios::beg);
}

void FileReader::calculate() noexcept {
    readLinesToQueue();
    executor.waitUntilAllTasksDone();
}

void FileReader::readLinesToQueue() noexcept {
    while (true) {
        auto buffer = std::make_shared<std::vector<char>>(bufferSize);
        fileStream.read(buffer->data(), static_cast<std::streamsize>(bufferSize));
        std::size_t bytesRead = fileStream.gcount();

        if (bytesRead == 0) {
            if (fileStream.eof()) {
                std::cout << "EOF" << std::endl;
            } else {
                std::cerr << "Error reading from file stream" << std::endl;
            }
            break;
        }

        buffer->resize(bytesRead);

        if (auto end = std::find(buffer->rbegin(), buffer->rend(), '\n'); end != buffer->rend()) {
            auto newSize = bytesRead - std::distance(buffer->rbegin(), end);
            buffer->resize(newSize);
            fileStream.seekg(static_cast<std::streamoff>(newSize - bytesRead), std::ios_base::cur);
        }

        auto func = [this, buffer]() { processChunk(buffer); };
        executor.enqueue(func);
    }

    fileStream.close();
}

FileReader::~FileReader() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

uint64_t FileReader::count() const noexcept {
    size_t size = bitArray.size();
    if (size == 0) return 0;

    size_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;

    size_t chunkSize = size / numThreads;
    std::vector<std::future<uint64_t>> futures;

    auto countBitsInRange = [](const std::vector<std::atomic<uint32_t>> &array, size_t start, size_t end) {
        uint64_t localSum = 0;
        for (size_t i = start; i < end; ++i) {
            localSum += __builtin_popcount(array[i].load(std::memory_order_relaxed));
        }
        return localSum;
    };

    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? size : (i + 1) * chunkSize;
        futures.emplace_back(std::async(std::launch::async, countBitsInRange, std::cref(bitArray), start, end));
    }

    uint64_t sum = 0;
    for (auto &future: futures) {
        sum += future.get();
    }

    return sum;
}

void FileReader::processChunk(std::shared_ptr<std::vector<char>> buffer) noexcept {
    auto processSegment = [&](uint32_t ipAddress, uint32_t segment) {
        ipAddress = (ipAddress << SEGMENT_SIZE) | segment;
        uint32_t index = ipAddress >> FIVE;
        uint32_t bitMask = BIT_MASK_ONE << (ipAddress & IPADDR_MASK);
        bitArray[index].fetch_or(bitMask, std::memory_order_relaxed);
    };

    uint32_t ipAddress = 0;
    uint32_t segment = 0;
    for (char byte: *buffer) {
        if (std::isdigit(byte)) {
            segment = segment * TEN + (byte - '0');
        } else if (byte == DOT_CODE) {
            ipAddress = (ipAddress << SEGMENT_SIZE) | segment;
            segment = 0;
        } else if (byte == NEWLINE_CODE) {
            processSegment(ipAddress, segment);
            segment = 0;
            ipAddress = 0;
        }
    }
    buffer.reset();
}
