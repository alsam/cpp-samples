// -*- C++ -*-

// The MIT License (MIT)
//
// Copyright (c) 2017 Alexander Samoilov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>

struct program_options
{
    bool verbose                     = {false};
    int  flow_type                   = {0};            // legacy Fortran constants for flow type, 50 for sphere, 51 for torus
    std::string input_data           = {""};           // name of the input file e.g. `sphere.dat`, `torus_trgl.dat`
    std::string asy_name             = {""};           // dump streamlines to vector `.asy` file if name is given
};

std::ostream& operator<<(std::ostream& os, program_options const& popt)
{
    os << "program options: " << std::boolalpha << " verbose: " << popt.verbose << std::endl
       << " flow_type: " << popt.flow_type << " input_data: " << popt.input_data << " asy_name: " << popt.asy_name << std::endl;

    return os;
}

program_options parse_command_line(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc("allowed options");
    desc.add_options()
            ("help",                   "describe arguments")
            ("verbose",                "be verbose")
            ("flow_type",              po::value<int>(), "legacy Fortran constants for flow type, 50 for sphere, 51 for torus")
            ("input_data",             po::value<std::string>(), "name of the input file e.g. `sphere.dat`, `torus_trgl.dat`")
            ("asy_name",               po::value<std::string>(), "dump streamlines to vector `.asy` file if name is given");
    try
    {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        program_options popt;

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            std::exit(1);
        }

        popt.verbose               = vm.count("verbose");
        if (vm.count("flow_type")) {
            popt.flow_type = vm["flow_type"].as<int>();
        }
        if (vm.count("input_data")) {
            popt.input_data = vm["input_data"].as<std::string>();
        }
        if (vm.count("asy_name")) {
            popt.asy_name = vm["asy_name"].as<std::string>();
        }

        return popt;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << desc << std::endl;
        std::exit(-1);
    }
}

int main(int argc, char **argv)
{
    auto popt = parse_command_line(argc, argv);
    if (popt.verbose) {
        std::cout << popt;
    }

}

