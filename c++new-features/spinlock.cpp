#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>


// from here [C++11 Implementation of Spinlock using <atomic>](http://stackoverflow.com/questions/26583433/c11-implementation-of-spinlock-using-atomic)

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


int main()
{
    using namespace std::literals::chrono_literals;
    SpinLock spinlock;
    spinlock.lock();
    std::this_thread::sleep_for(1.2s);
    spinlock.unlock();
    std::cout << "spinlock unlocked\n";
}
