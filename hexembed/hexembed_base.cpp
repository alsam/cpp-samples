// -*- C++ -*-

// The MIT License (MIT)
//
// Copyright (c) 2023 Alexander Samoilov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
