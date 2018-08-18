#pragma once
#include "../base.hh"
#include <tuple>
#include <string>

namespace cl_tutorial
{
    extern std::tuple<unsigned long, unsigned long> get_runtime(cl::Event const &event);
    extern void print_runtime_msg(std::tuple<unsigned long, unsigned long> const &t, std::string const &msg);
}

