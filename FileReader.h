#pragma once

#include <fstream>
#include "ThreadPool.h"

class FileReader {
public:
    FileReader() ;

    explicit FileReader(const std::string &filePath);

    FileReader(const FileReader &) = delete;

    FileReader &operator=(const FileReader &) = delete;

    ~FileReader();

    void calculate() noexcept;

    uint64_t count() const noexcept;

private:
    std::ifstream fileStream;
    ThreadPool executor{};

    void processChunk(std::shared_ptr<std::vector<char>> buffer) noexcept;

    void readLinesToQueue() noexcept;

    static constexpr std::size_t bufferSize = 8192 * 1024 * 4;
    static constexpr uint8_t TEN = 10;
    static constexpr uint8_t DOT_CODE = '.';
    static constexpr uint8_t NEWLINE_CODE = '\n';
    static constexpr uint32_t SEGMENT_SIZE = 8;
    static constexpr uint32_t FIVE = 5;
    static constexpr uint32_t BIT_MASK_ONE = 1;
    static constexpr uint32_t IPADDR_MASK = 31;
    static constexpr std::size_t CAPACITY = (static_cast<std::size_t>(1) << 32);

    alignas(64) std::vector<std::atomic<uint32_t>> bitArray = std::vector<std::atomic<uint32_t>>(CAPACITY);;
};
