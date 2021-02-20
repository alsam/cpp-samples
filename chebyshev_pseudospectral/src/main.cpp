// -*- C++ -*-

// The MIT License (MIT)
//
// Copyright (c) 2021 Alexander Samoilov
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

#include <boost/program_options.hpp>
#include "PoissonProblem.hpp"

struct program_options
{
    bool verbose                    = {false};
    unsigned M                      = {32};    // grid size along `x` direction
    unsigned N                      = {32};    // grid size along `y` direction
    double x_min                    = {-1.0};  // minimum `x` value
    double x_max                    = { 1.0};  // maximum `x` value
    double y_min                    = {-1.0};  // minimum `y` value
    double y_max                    = { 1.0};  // maximum `y` value
    std::string asy_name            = {""};    // dump streamlines to vector `.asy` file if name is given
};

program_options parse_command_line(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc("allowed options");
    desc.add_options()
            ("help",                   "describe arguments")
            ("verbose",                "be verbose")
            ("M",                      po::value<unsigned>(),    "`M`     -- grid size along `x` direction")
            ("N",                      po::value<unsigned>(),    "`N`     -- grid size along `y` direction")
            ("x_min",                  po::value<double>(),      "`x_min` -- minimum `x` value")
            ("x_max",                  po::value<double>(),      "`x_max` -- maximum `x` value")
            ("y_min",                  po::value<double>(),      "`y_min` -- minimum `y` value")
            ("y_max",                  po::value<double>(),      "`y_max` -- maximum `y` value")
            ("asy_name",               po::value<std::string>(), "dump streamlines to vector `.asy` file if name is given");
    try
    {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        program_options popt;


        if (vm.count("help")) {
            std::cout << desc << std::endl;
            std::exit(0);
        }

        popt.verbose = vm.count("verbose");

        if (vm.count("M")) {
            popt.M = vm["M"].as<unsigned>();
        }

        if (vm.count("N")) {
            popt.N = vm["N"].as<unsigned>();
        }

        if (vm.count("x_min")) {
            popt.x_min = vm["x_min"].as<double>();
        }

        if (vm.count("x_max")) {
            popt.x_max = vm["x_max"].as<double>();
        }

        if (vm.count("y_min")) {
            popt.y_min = vm["y_min"].as<double>();
        }

        if (vm.count("y_max")) {
            popt.y_max = vm["y_max"].as<double>();
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
    program_options po = parse_command_line(argc, argv);

    if (po.verbose) {
        std::cout << "problem parameters: M: " << po.M << " N: " << po.N
                  << " x_min: " << po.x_min << " x_max: " << po.x_max
                  << " y_min: " << po.y_min << " y_max: " << po.y_max
                  << std::endl;
    }

    PoissonProblem problem(po.M, po.N, po.x_min, po.x_max, po.y_min, po.y_max, po.verbose);

    return EXIT_SUCCESS;
}
