#include <iostream>
#include <string>
#include <bitset>

int zero_gap(int val)
{
    std::bitset<32> bs(val);
    std::string bit_repr{bs.to_string()};
    std::cout << bit_repr << std::endl;
    size_t max_gap = 0, cur_gap = 0;
    bool gap = false;
    for (size_t i = 0; i < bit_repr.size(); ++i) {
        if (bit_repr[i] == '0') {
            if (!gap) {
                gap = true;
                cur_gap = 1;
            } else {
                ++cur_gap;
            }
        } else {
            gap = false;
            if (cur_gap > max_gap)
                max_gap = cur_gap;
        }
    }
    if (cur_gap > max_gap)
        max_gap = cur_gap;

    return max_gap;
}

int main(int argc, char** argv)
{
    if (argc < 2) return -1;

    std::cout << "zero gap: " << zero_gap(std::stoi(argv[1])) << std::endl;
}
