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

int
body_ax_geo(program_options const& popt)
{
    std::ifstream ifs;
    auto get_item = [&ifs](auto& item) {
        std::string line;
        std::getline(ifs, line);
        std::istringstream iss(line);
        iss >> item;
    };
    if (popt.flow_type == to_underlying(FlowType::SPHERE)) {
        std::string fname = (popt.input_data == "") ? "sphere.dat" : popt.input_data;
        ifs.open(fname);
        if (ifs) {
            int ngl;
            double rad;
            get_item(ngl);
            get_item(rad);

            return ngl;
        }
    } else if (popt.flow_type == to_underlying(FlowType::THORUS)) {
    }
} // body_ax_geo

