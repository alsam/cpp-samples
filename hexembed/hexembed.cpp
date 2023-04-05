#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <cerrno>
#include <cstring>

namespace fs = std::filesystem;

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string hexStr(const char *data, size_t len)
{
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i]     = hexmap[(static_cast<uint8_t>(data[i]) & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[ static_cast<uint8_t>(data[i]) & 0x0F];
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
    std::ifstream f(filepath, std::ios::binary | std::ios::in);
    if (!f) {
        std::cout << "failed to open input file ";
        std::cout << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    std::vector<char> buffer(fsize);
    if (f.read(buffer.data(), fsize)) {
        std::cout << "-I- read " << buffer.size() << " bytes " << std::endl;
        auto hexstr = hexStr(buffer.data(), buffer.size());
        const size_t line_break = 8;
        for (size_t i = 0; i < buffer.size(); ++i) {
            std::cout << "0x" << hexstr[2 * i]
                              << hexstr[2 * i + 1] << ", ";
            if (i % line_break == (line_break - 1))
                std::cout << "\n";
        }
    }
    return EXIT_SUCCESS;
}
