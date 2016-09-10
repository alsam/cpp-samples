#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char **argv)
{
    // c++14 with `chrono_literals`
    // [chrono_literals is not a namespace-name](http://stackoverflow.com/questions/35856969/chrono-literals-is-not-a-namespace-name)
    using namespace std::literals::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(2s);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Waited " << elapsed.count() << " ms\n";

    // tradional c++11 without `chrono_literals`
    start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    end = std::chrono::high_resolution_clock::now();
    elapsed = end-start;
    std::cout << "Waited " << elapsed.count() << " ms\n";
}

