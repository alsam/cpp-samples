#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

// from here [C++11 Implementation of Spinlock using <atomic>](http://stackoverflow.com/questions/26583433/c11-implementation-of-spinlock-using-atomic)
// see also [C++11 spinlock](http://anki3d.org/spinlock/)
// and [Code Review: ++11 recursive atomic spinlock](http://codereview.stackexchange.com/questions/95590/c11-recursive-atomic-spinlock)

class SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT ;
public:
    void lock() {
        while (locked.test_and_set(std::memory_order_acquire)) { ; }
    }
    void unlock() {
        locked.clear(std::memory_order_release);
    }
};

SpinLock lock;
//std::mutex lock;
int count[8] = {0, 0, 0, 0, 0, 0, 0, 0};
 
void foo(unsigned n)
{
    const unsigned cnt = 10000000;
    for(unsigned i = 0; i < cnt; i++) {
        lock.lock();
        ++count[n];
        lock.unlock();
    }
}
 
int main()
{
    using namespace std::literals::chrono_literals;
 
    // warming up
    SpinLock spinlock;
    spinlock.lock();
    std::this_thread::sleep_for(0.2s);
    spinlock.unlock();
    std::cout << "spinlock unlocked\n";

    // benchmark
    std::vector<std::thread> v;
    for(int n = 0; n < 8; ++n) {
        v.emplace_back(foo, n);
    }
 
    for (auto& t : v) {
        t.join();
    }
 
    return count[0] + count[1] + count[2] + count[3]
        + count[4] + count[5] + count[6] + count[7];

}
