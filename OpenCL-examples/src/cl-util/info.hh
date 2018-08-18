#pragma once
#include "../base.hh"
#include <vector>
#include <iostream>
#include <string>

namespace cl_tutorial
{
    extern void print_opencl_info(std::vector<cl::Platform> const &platform_list);
}

