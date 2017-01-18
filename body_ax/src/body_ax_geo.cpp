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
#include <array>
#include <vector>
#include <functional>
#include <iterator>
#include <cmath>
#include "parameters.hpp"
#include "body_ax_geo.hpp"

parameters
body_ax_geo(program_options const& popt)
{
    std::ifstream ifs;
    parameters params;
    std::array<double, MAX_SEGMENTS> rt;
    auto get_items = [&ifs](auto&& ...items) {
        std::string line;
        std::getline(ifs, line);
        std::istringstream iss(line);
        for (auto&& item : {items...}) {
            iss >> item;
        }
    };
    if (popt.flow_type == to_underlying(FlowType::SPHERE)) {
        std::string fname = (popt.input_data == "") ? "sphere.dat" : popt.input_data;
        ifs.open(fname);
        if (ifs) {
            get_items(std::ref(params.ngl));
            get_items(std::ref(params.sphere_params.rad));
        }
    } else if (popt.flow_type == to_underlying(FlowType::THORUS)) {
        std::string fname = (popt.input_data == "") ? "torus_trgl.dat" : popt.input_data;
        ifs.open(fname);
        if (ifs) {
            get_items(std::ref(params.ngl));
            get_items(std::ref(params.thorus_params.xfirst),  std::ref(params.thorus_params.yfirst));
            get_items(std::ref(params.thorus_params.xsecond), std::ref(params.thorus_params.ysecond));
            get_items(std::ref(params.thorus_params.xthird),  std::ref(params.thorus_params.ythird));
            get_items(std::ref(params.thorus_params.vx));
            get_items(std::ref(params.thorus_params.cr));
            // FIXME error: unable to deduce ‘std::initializer_list<auto>&&’ from ‘<brace-enclosed initializer list>()’
            //get_items();
            // FIXME error: unable to deduce ‘std::initializer_list<auto>&&’ from ‘{items#0, items#1}’
            // note:   deduced conflicting types for parameter ‘auto’ (‘std::reference_wrapper<int>’ and ‘std::reference_wrapper<double>’)
            //get_items(std::ref(params.ne[0]),  std::ref(rt[0]));
        }
    }
    return std::move(params);
} // body_ax_geo

