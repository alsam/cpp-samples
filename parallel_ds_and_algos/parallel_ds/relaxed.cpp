#include <cassert>
#include <thread>
#include <atomic>
#include <cstdlib>

std::atomic <bool> x, y;

//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
void thr1()
{
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_relaxed);
}

//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
void thr2()
{
    bool temp=false;
    while (!temp)
        temp = y.load(std::memory_order_relaxed);
    assert (x.load(std::memory_order_relaxed) == true);
}

//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
int main ( int argc, char **argv )
{
    std::thread t1(thr1);
    std::thread t2(thr2);
    t1.join();
    t2.join();
    return EXIT_SUCCESS;
}
