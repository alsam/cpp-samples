#pragma once

#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION  120
#include <CL/cl2.hpp>

#include <iostream>
#include <sstream>
#include <exception>
#include <chrono>
#include <utility>
#include <list>

#include "optional.hh"

namespace cl_tutorial
{
    /* generic string utilities ========================================== */
    /*! \brief get the head of a range
     * \param r a ranged object (having begin and end methods)
     * \return reference to first element of the range.
     */
    template <typename R>
    auto head(R const &r) -> decltype(*r.begin())
    {
        return *r.begin();
    }

    template <typename T>
    class Tail
    {
        public:
            typedef typename T::value_type value_type;
            typedef typename T::const_iterator const_iterator;

            Tail(T const &a):
                b(a.begin()), e(a.end())
            {
                ++b;
            }

            const_iterator begin() const { return b; }
            const_iterator end() const { return e; }
            const_iterator cbegin() const { return b; }
            const_iterator cend() const { return e; }

        private:
            const_iterator b, e;
    };

    /*! \brief get the tail of a range
     * \param r a range object (having begin and end methods).
     * \return a Tail object combining begin()++ and end().
     */
    template <typename T>
    Tail<T> tail(T const &a) { return Tail<T>(a); }

    template <typename T>
    std::optional<T> from_string(std::string const &s)
    {
        T value;
        std::istringstream iss(s);

        try {
            iss >> value;
            if (iss.fail()) {
                return std::nullopt;
            }
        }
        catch (std::istream::failure const &e) {
            return std::nullopt;
        }

        return value;
    }

    template <typename T>
    std::string to_string(T const &value)
    {
        std::ostringstream out;
        out << value;
        return out.str();
    }

    template <typename R>
    std::string string_join(R const &r, std::string const &d)
    {
        std::string a = head(r);
        for (auto b : tail(r))
            a += d + b;

        return a;
    }

    inline void format_to(std::ostream &out) {}

    template <typename First, typename ...Rest>
    void format_to(std::ostream &out, First a, Rest &&...rest)
    {
        out << a;
        format_to(out, std::forward<Rest>(rest)...);
    }

    template <typename ...Args>
    std::string format(Args &&...args)
    {
        std::ostringstream ss;
        format_to(ss, std::forward<Args>(args)...);
        return ss.str();
    }

    class exception: public std::exception
    {
        std::string msg;

        public:
            exception(std::string const &msg):
                msg(msg) {}

            virtual char const *what() const throw()
            {
                return msg.c_str();
            }

            virtual ~exception() throw() {}
    };

    template <typename I>
    void split(std::string const &s, char d, I inserter)
    {
        size_t p, q = 0;
        while (q < s.length())
        {
            p = s.find_first_not_of(d, q);
            if (p == std::string::npos) return;

            q = s.find_first_of(d, p);
            if (q == std::string::npos)
            {
                *inserter = s.substr(p, s.length() - p);
                return;
            }
            else
            {
                *inserter = s.substr(p, q-p);
                ++inserter;
            }
        }
    }

    class Log
    {
        std::list<std::string> m_indent;

        public:
            Log &push(std::string const &i)
            {
                m_indent.push_back(i);
                return *this;
            }

            template <typename ...Args>
            Log &pop(Args &&...args)
            {
                m_indent.pop_back();
                std::string m = format(std::forward<Args>(args)...);
                if (m != "")
                    msg(m);
                msg();
                return *this;
            }

            template <typename ...Args>
            Log &msg(Args &&...args)
            {
                for (std::string const &i : m_indent)
                    std::cerr << i;
                std::cerr << format(std::forward<Args>(args)...) << std::endl;
                return *this;
            }

            template <typename ...Args>
            Log &error(Args &&...args)
            {
                std::string m = format(std::forward<Args>(args)...);
                push("\033[1;31m[error]\033[m ");
                std::vector<std::string> lines;
                split(m, '\n', std::back_inserter(lines));
                for (std::string const &l : lines)
                {
                    msg(l);
                }
                pop();
                return *this;
            }
    };

    extern Log console;

    class Timer
    {
        using clock = std::chrono::high_resolution_clock;
        using unit_of_time  = std::chrono::milliseconds;

        clock::time_point t1;
        std::string msg;

        public:
            Timer() = default;

            void start(std::string const &msg_)
            {
                msg = msg_;
                console
                    .msg("\033[34;1m⏲ \033[m start timer [", msg, "]")
                    .push("\033[34m│\033[m   ");
                t1 = clock::now();
            }

            void stop() const
            {
                clock::time_point t2 = clock::now();
                double duration = std::chrono::duration<double, std::milli>(t2 - t1).count();
                console
                    .pop("\033[34m┕\033[m stop timer [", msg, "]: ", duration, " ms");
            }
    };

    template <typename ...Args>
    inline void checkErr(cl_int err, Args &&...args)
    {
        if (err != CL_SUCCESS)
        {
            std::string msg = format(std::forward<Args>(args)...);
            console.error(err, msg);
            throw exception(msg);
        }
    }
}

