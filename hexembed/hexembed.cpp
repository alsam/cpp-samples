#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <cerrno>
#include <cstring>

// not portable: Linux/Unix `mmap`
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

namespace fs = std::filesystem;

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string hexStr(const uint8_t *data, size_t len)
{
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[ data[i] & 0x0F];
    }
    return s;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "gimme input file\n";
        return EXIT_FAILURE;
    }
    fs::path filepath(fs::absolute(fs::path(argv[1])));
    std::streamsize fsize = fs::exists(filepath) ? fs::file_size(filepath) : 0;
    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cout << "failed to open input file " << filepath << " : ";
        std::cout << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    uint8_t *data = reinterpret_cast<uint8_t*>(mmap(nullptr, fsize, PROT_READ, MAP_SHARED, fd, 0));
    if (nullptr == data) {
        std::cout << "failed to mmap input file " << filepath << " : ";
        std::cout << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    auto hexstr = hexStr(data, fsize);
    const size_t line_break = 8;
    for (size_t i = 0; i < fsize; ++i) {
        std::cout << "0x" << hexstr[2 * i]
                          << hexstr[2 * i + 1] << ", ";
        if (i % line_break == (line_break - 1))
            std::cout << "\n";
    }
    return EXIT_SUCCESS;
}
