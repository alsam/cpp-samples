#include <iostream>
#include <thread>
#include <atomic>

int main()
{
    auto some_shared_ptr = std::make_shared<int>(42);
    bool is_lock_free = std::atomic_is_lock_free(&some_shared_ptr);
    std::cout << std::boolalpha << "atomic is lock-free for shared_ptr: " << is_lock_free << std::endl;
}
