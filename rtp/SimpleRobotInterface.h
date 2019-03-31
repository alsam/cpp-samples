// The MIT License (MIT)
//
// Copyright (c) 2019 Alexander Samoilov
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

#pragma once

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <memory>

#include <rwc_msgs/RobotCommand.h>

template<class T>
auto operator<<(std::ostream& os, const T& t) -> decltype(t.print(os), os)
{
  t.print(os);
  return os;
}

// to store robot commands and for message queue
template <typename T>
class Buffer
{
public:
    void add(T&& item)
    {
      std::unique_lock<std::mutex> locker(mu_);
      cond_.wait(locker, [this] { return buffer_.size() < size_; } );
      std::cout << "add(): pushed " << item << "\n";
      buffer_.push_back(item);
      cond_.notify_all();
      return;
    }

    T remove()
    {
      std::unique_lock<std::mutex> locker(mu_);
      cond_.wait(locker, [this] { return !buffer_.empty(); } );
      auto back = buffer_.back();
      buffer_.pop_back();
      std::cout << "remove(): popped " << back << "\n";
      cond_.notify_all();
      return back;
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

// base class for all messages
class Message
{
};

class SimpleRobotInterface
{

// type definitions
public:

  enum Responce
  {
    OK, NO_CONNECTION, COMMAND_UNSUPPORTED, COMMAND_FAILED,
  };

  using MessageQueue = Buffer<std::unique_ptr<Message>>;

// methods
public:

  /// a constructor: error handlers should be set before the first task execution
  SimpleRobotInterface(std::vector<rwc_msgs::RobotErrorHandler>&& error_handlers)
    : error_handlers_(error_handlers)
  {}

  Responce executeTask(rwc_msgs::RobotTask&& tasks);

//methods
private:

  // either task or error handler
  template <typename T> Responce start(T&& task);

  // ditto either task or error handler
  template <typename T> Responce execute(T&& task);

// data
private:

  std::vector<rwc_msgs::RobotErrorHandler> error_handlers_;

  MessageQueue message_queue_;
};

