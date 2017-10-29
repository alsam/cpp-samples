#include <iostream>
#include <memory>
#include <vector>

template <template<typename,typename> class Cont, typename Elem, typename Alloc>
std::ostream& operator<<(std::ostream& stream, Cont<Elem,Alloc> const& cont)
{
    stream << "{";
    for (auto const& c : cont) {
        stream << c << ", ";
    }
    stream << "}" << std::endl;
    return stream;
}

void sub(std::vector<int> const& vec_)
{
    std::vector<int> vec(vec_);
    vec.push_back(42);
    std::cout << "vec: " << vec << std::endl;
}

void sub(std::vector<int> && vec)
{
    vec.push_back(42);
    std::cout << "vec: " << vec << std::endl;
}


template<class T>
auto operator<<(std::ostream& os, const T& t) -> decltype(t.print(os), os)
{
  t.print(os);
  return os;
}

// base class for all messages
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

  template <typename T> bool from(T msg);

  template <typename T> bool to(T&& msg);

  void print(std::ostream& strm) const
  {
    strm << "Message\n";
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
  InternalMessageType sm_;

friend class Message;
};

Message::Message()
: pImpl_(new Impl)
{
}

template <typename T> Message::Message(T msg)
: pImpl_(new Impl)
{
  this->from(std::move(msg));
}

Message::Message(Message&& m)
: pImpl_(std::move(m.pImpl_))
{
}

Message::~Message()
{
}

template <typename T>
bool Message::to(T&& msg)
{
  msg = std::move(pImpl_->sm_);
}

template <typename T>
bool Message::from(T msg)
{
  return pImpl_->serialize(std::move(msg));
}

int main()
{
    std::vector<int> v = {1,2,3};
    std::cout << "v: " << v << std::endl;
    sub(v);
    std::cout << "v: " << v << std::endl;
    sub({1,2,3});

}
