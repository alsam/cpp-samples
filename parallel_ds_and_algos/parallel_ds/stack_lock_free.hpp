#pragma once

#include <atomic>

template <typename T>
class stack_lock_free
{

private:

    template <typename W>
    struct Node {
        W data;
        Node <W> *next;
    };

    std::atomic<Node<T>*> head ;

public:
 
    T pop ();
 
    void push (const T&);
 
    stack_lock_free (){ head.store(nullptr);}

    stack_lock_free (const stack_lock_free &) = delete ;

    stack_lock_free &operator=(const stack_lock_free&) = delete;
 };

