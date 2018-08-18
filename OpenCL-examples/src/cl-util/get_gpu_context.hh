#pragma once
#include "../base.hh"
#include <tuple>
#include <vector>

namespace cl_tutorial
{
    extern std::tuple<std::vector<cl::Device>, cl::Context> get_default_gpu_context();
}

