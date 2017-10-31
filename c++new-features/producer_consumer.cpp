// see [A multi-threaded Producer Consumer with C++11](http://codereview.stackexchange.com/questions/84109/a-multi-threaded-producer-consumer-with-c11)
// note that `lock_guard` is recommended instead of `unique_lock` as it releases lock automatically follow RAII leaving the lock scope.
// see also [](http://www.boost.org/doc/libs/1_54_0/doc/html/lockfree/examples.html) for atomics instead of locks.
//

#include <iostream>
#include <thread>
#include <vector> // for creating an equivalent to `boost::thread_group`
#include <mutex>
#include <condition_variable>
#include <deque>
#include <random>

template<class T>
auto operator<<(std::ostream& os, const T& t) -> decltype(t.print(os), os)
{
  t.print(os);
  return os;
}

// to store robot commands
template <typename T>
class Buffer
{
public:
    void add(T&& item)
    {
      std::unique_lock<std::mutex> locker(mu_);
      cond_.wait(locker, [this] { return buffer_.size() < size_; } );
      std::cout << "add(): pushed " << item << "\n";
      buffer_.emplace_back(std::move(item));
      cond_.notify_all();
    }

    T remove()
    {
      std::unique_lock<std::mutex> locker(mu_);
      cond_.wait(locker, [this] { return !buffer_.empty(); } );
      auto front = std::move(buffer_.front());
      buffer_.pop_front();
      std::cout << "remove(): popped " << front << "\n";
      cond_.notify_all();
      return front;
    }

    bool empty() const
    {
      return buffer_.empty();
    }

    Buffer() {}

private:
    std::mutex mu_;
    std::condition_variable cond_;

    std::deque<T> buffer_;
    const unsigned size_ = 128;
};

struct RobotCommand
{
  int task_id, sequence_id, command_type;

  void print(std::ostream& strm) const
  {
    strm << "task_id: " << task_id
         << " sequence_id: " << sequence_id
         << " command_type: " << command_type;
  }
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

    const int producer_thread_count = 12;
    const int consumer_thread_count = 12;
    Buffer<int> buffer;
    for (int i = 0; i < producer_thread_count; i++) {
        int num = dist(engine);
        producer_grp.emplace_back(&Buffer<int>::add, &buffer, num);
    }
    for (int i = 0; i < consumer_thread_count; i++) {
        consumer_grp.emplace_back(&Buffer<int>::remove, &buffer);
    }

    join_all(producer_grp);
    join_all(consumer_grp);
}

