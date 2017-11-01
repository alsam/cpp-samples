#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>

template<class T>
auto operator<<(std::ostream& os, const T& t) -> decltype(t.print(os), os)
{
  t.print(os);
  return os;
}

class TypedMessage
{
public:

  bool toRequest(int)
  {
    std::cout << "called TypedMessage::toRequest()\n";
    return false;
  }
};

class TaskMessage: public TypedMessage
{
  // no seq_id_
};

class OperationMessage: public TypedMessage
{
public:

  OperationMessage(int32_t id) : seq_id_(id) {}

  int32_t getSequence() const { return seq_id_; }

  bool toRequest(int)
  {
    std::cout << "called OperationMessage::toRequest()\n";
    return true;
  }

private:
  int32_t seq_id_;
};

class DigitalMessage: public OperationMessage
{
public:
  DigitalMessage(int32_t id, int32_t out) : OperationMessage(id), digital_out_(out) {}

private:
  int32_t digital_out_;
};

class Message
{

public:

  Message();

  // constructor for more convenience
  template <typename T> Message(T msg);

  Message(Message&&);

  // to avoid problems with std::unique_ptr
  // while using pImpl
  ~Message();

  template <typename T> bool from(T&& msg);

  template <typename T> bool to(T& msg);

  int32_t getId() const;

  void setId(int32_t id);

  void print(std::ostream& strm) const
  {
    strm << "Message with id: " << getId() << "\n";
  }

// pImpl idiom in action
private:
  class Impl;
  std::unique_ptr<Impl> pImpl_;
};

class Message::Impl
{
public:

  using InternalMessageType = int;

public:

  Impl()
  : id_(-7)
  {
  }

  ~Impl()
  {
  }

  template <typename T> bool serialize(T&& msg)
  {
    bool status = msg.toRequest(sm_);
    return status;
  }

private:
  int32_t id_;

  InternalMessageType sm_;

friend class Message;
};

Message::Message()
: pImpl_(new Impl)
{
}

// a helper class for setting Message::id_
template <typename M, typename T, bool = std::is_base_of<OperationMessage, T>::value>
struct SetId
{
  SetId(M* t) : target_(t) {}
  void operator()(T)
  {
    std::cout << "SetId with -1\n";
    target_->setId(-1);
  }
private:
  M* target_;
};

// partial specialization
template <typename M, typename T>
struct SetId<M, T, true>
{
  SetId(M* t) : target_(t) {}
  void operator()(T msg)
  {
    std::cout << "SetId with msg.getSequence()\n";
    target_->setId(msg.getSequence());
  }
private:
  M* target_;
};

template <typename T> Message::Message(T msg)
: pImpl_(new Impl)
{
  SetId<Message, T> setter(this);
  setter(msg);

  this->from(std::move(msg));
}

Message::Message(Message&& m)
: pImpl_(std::move(m.pImpl_))
{
}

Message::~Message()
{
}

int32_t Message::getId() const
{
  return pImpl_->id_;
}

void Message::setId(int32_t id)
{
  pImpl_->id_ = id;
}

template <typename T>
bool Message::to(T& msg)
{
  msg = std::move(pImpl_->sm_);
}

template <typename T>
bool Message::from(T&& msg)
{
  return pImpl_->serialize(std::move(msg));
}

int main()
{
  TaskMessage task_message;
  DigitalMessage digital_message(42, 3);

  Message m1(task_message), m2(digital_message);

  std::cout << "m1: " << m1 << "m2: " << m2 << std::endl;
}

