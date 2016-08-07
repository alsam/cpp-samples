// see [A multi-threaded Producer Consumer with C++11](http://codereview.stackexchange.com/questions/84109/a-multi-threaded-producer-consumer-with-c11)
// mote. that `lock_guard` is recommended instead of `unique_lock` as it is releases lock automatically follow RAII leaving the lock scope.
// see also [](http://www.boost.org/doc/libs/1_54_0/doc/html/lockfree/examples.html) for atomics instead of locks.
//

#include <iostream>
#include <thread>
#include <vector> // for creating an equivalent to `boost::thread_group`
#include <mutex>
#include <condition_variable>
#include <deque>
#include <random>

class Buffer
{
public:
    void add(int num) {
        while (true) {
            std::unique_lock<std::mutex> locker(mu);
            cond.wait(locker, [this](){return buffer_.size() < size_;});
            std::cout << "add(): pushed " << num << "\n";
            buffer_.push_back(num);
            locker.unlock();
            cond.notify_all();
            return;
        }
    }
    int remove() {
        while (true) {
            std::unique_lock<std::mutex> locker(mu);
            cond.wait(locker, [this](){return buffer_.size() > 0;});
            int back = buffer_.back();
            buffer_.pop_back();
            std::cout << "remove(): popped " << back << "\n";
            locker.unlock();
            cond.notify_all();
            return back;
        }
    }
    Buffer() {}
private:
    std::mutex mu;
    std::condition_variable cond;

    std::deque<int> buffer_;
    const unsigned int size_ = 10;
};

void join_all(std::vector<std::thread>& grp) {
    for (auto& thr : grp)
        if (thr.joinable())
            thr.join();
}

main()
{
    // Non-deterministic pseudo-random numbers generator
    thread_local std::random_device rd;
    // Pseudo-random engine
    thread_local std::mt19937 engine(rd());
    // Linear distribution in [0, 100[
    thread_local std::uniform_int_distribution<int> dist(0, 99);

    std::vector<std::thread> producer_grp, consumer_grp;

    // FIXME : seems it has deadlock
    // sample run
    // ./producer_consumer 
    // add(): pushed add(): pushed 01
    // add(): pushed 
    // 2
    // add(): pushed 3
    //
    const int producer_thread_count = 4;
    const int consumer_thread_count = 4;
    for (int i = 0; i < producer_thread_count; i++) {
        Buffer producer;
        producer_grp.emplace_back(&Buffer::add, &producer, i);
    }
    for (int i = 0; i < consumer_thread_count; i++) {
        Buffer consumer;
        consumer_grp.emplace_back(&Buffer::remove, &consumer);
    }

    join_all(producer_grp);
    join_all(consumer_grp);
}

