// from [How to create timer events using C++ 11?](https://stackoverflow.com/questions/14650885/how-to-create-timer-events-using-c-11)

#include <functional>
#include <chrono>
#include <future>
#include <iostream>

class later
{
public:
    template <class callable, class... arguments>
    later(int after, bool async, callable&& f, arguments&&... args)
    {
        std::function<typename std::result_of<callable(arguments...)>::type()>
            task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        if (async)
        {
            std::thread([after, task]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
            }).detach();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }
};

int main()
{
    later later_test1(3000, false, [](){ std::cout << "test1 called\n"; } );
    later later_test2(3000, false, [](int a){  std::cout << "test2 called a: " << a << '\n'; }, 101);

    return 0;
}
