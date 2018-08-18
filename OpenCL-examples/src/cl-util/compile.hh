#pragma once
#include "../base.hh"
#include <string>

namespace cl_tutorial
{
    extern std::string load_source(std::string const &filename);

    extern cl::Program get_program_from_file(
        cl::Context &cx,
        std::vector<cl::Device> const &devices,
        std::string const &filename);
}

