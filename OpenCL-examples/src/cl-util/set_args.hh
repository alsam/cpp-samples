#pragma once
#include "../base.hh"
#include <utility>

namespace cl_tutorial
{
    template <unsigned N>
    void set_args_n(cl::Kernel &k) {}

    template <unsigned N, typename First, typename ...Rest>
    void set_args_n(cl::Kernel &k, First &&first, Rest &&...rest)
    {
        k.setArg(N, first);
        set_args_n<N+1>(k, std::forward<Rest>(rest)...);
    }

    template <typename ...Args>
    void set_args(cl::Kernel &k, Args &&...args)
    {
        set_args_n<0>(k, std::forward<Args>(args)...);
    }
}

