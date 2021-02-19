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
    bool verbose                    = {true};
    unsigned M                      = {32};    // grid size along `x` direction
    unsigned N                      = {32};    // grid size along `y` direction
    double x_min                    = {-1.0};  // minimum `x` value
    double x_max                    = { 1.0};  // maximum `x` value
    double y_min                    = {-1.0};  // minimum `y` value
    double y_max                    = { 1.0};  // maximum `y` value
    std::string asy_name            = {""};    // dump streamlines to vector `.asy` file if name is given
};

int main(int argc, char **argv)
{
    program_options po;
    PoissonProblem problem(po.M, po.N, po.x_min, po.x_max, po.y_min, po.y_max, po.verbose);

    return EXIT_SUCCESS;
}
