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
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <cmath>
#include "parameters.hpp"
#include "body_ax_geo.hpp"

parameters
body_ax_geo(program_options const& popt)
{
    std::ifstream ifs;
    parameters params;
    // TODO consider using generic variadic lambdas
    // e.g. [Fun with Lambdas: C++14 Style (part 3)](http://cpptruths.blogspot.ru/2014/08/fun-with-lambdas-c14-style-part-3.html)
    auto get_item = [&ifs](auto& item) {
        std::string line;
        std::getline(ifs, line);
        std::istringstream iss(line);
        iss >> item;
    };
    auto get_2items = [&ifs](auto& item1, auto& item2) {
        std::string line;
        std::getline(ifs, line);
        std::istringstream iss(line);
        iss >> item1 >> item2;
    };
    // TODO
    // http://stackoverflow.com/questions/27375089/what-is-the-easiest-way-to-print-a-variadic-parameter-pack-using-stdostream
    // auto get_items = [&ifs](auto& ...items) {
    //     std::string line;
    //     std::getline(ifs, line);
    //     std::istringstream iss(line);
    //     iss >> ...items;
    // };
    if (popt.flow_type == to_underlying(FlowType::SPHERE)) {
        std::string fname = (popt.input_data == "") ? "sphere.dat" : popt.input_data;
        ifs.open(fname);
        if (ifs) {
            get_item(params.ngl);
            get_item(params.sphere_params.rad);

        }
    } else if (popt.flow_type == to_underlying(FlowType::THORUS)) {
        std::string fname = (popt.input_data == "") ? "torus_trgl.dat" : popt.input_data;
        ifs.open(fname);
        if (ifs) {
            get_item(params.ngl);
            get_2items(params.thorus_params.xfirst, params.thorus_params.yfirst);
        }
    }
    return std::move(params);
} // body_ax_geo

