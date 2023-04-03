#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cerrno>
#include <cstring>

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
    std::ifstream f(argv[1], std::ios::binary | std::ios::ate);
    if (!f) {
        std::cout << "failed to open input file ";
        std::cout << std::strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    std::streamsize fs = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<char> buffer(fs);
    if (f.read(buffer.data(), fs)) {
        std::cout << "-I- read " << buffer.size() << " bytes " << std::endl;
        auto hexstr = hexStr(buffer.data(), buffer.size());
        for (size_t i = 0; i < hexstr.size(); i += 2) {
            std::cout << "0x" << hexstr[i] << hexstr[i+1] << ", ";
            if (i % 8 == 6) std::cout << "\n";
        }
    }
    return EXIT_SUCCESS;
}
