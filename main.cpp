#include "Timer.h"
#include "FileReader.h"

#include <iostream>
#include <string_view>
#include <cstdlib>

constexpr std::string_view USAGE_MESSAGE = R"(
Filename is empty.

Usage: ./ip_address_count <filename>
<filename> - full path for the file containing IP addresses.)";

constexpr std::string_view UNIQUE_IP_ADDRESSES = "Number of unique IP addresses: ";

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << USAGE_MESSAGE << std::endl;
        std::exit(EXIT_FAILURE);
    }

    Timer timer;
    std::string_view fileName = argv[1];
    FileReader fileReader{std::string{fileName}};

    timer.start();
    fileReader.calculate();
    timer.elapsed();

    std::cout << UNIQUE_IP_ADDRESSES << fileReader.count() << std::endl;

    return EXIT_SUCCESS;
}
