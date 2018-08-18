#include "compile.hh"
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <iostream>

using namespace cl_tutorial;

std::string cl_tutorial::load_source(std::string const &filename)
{
    std::ifstream file(filename);
    checkErr(file.is_open() ? CL_SUCCESS : -1,
        "could not open file: " + filename);

    std::string program(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));

    return program;
}

cl::Program cl_tutorial::get_program_from_file(
    cl::Context &cx,
    std::vector<cl::Device> const &devices,
    std::string const &filename)
{
    console.msg("Compiling ", filename, " ...");
    cl::Program::Sources sources;
    sources.push_back(load_source(filename));
    cl::Program program(cx, sources);

    auto err_code = program.build(devices, "");
    checkErr(err_code,
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]));

    return program;
}

