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

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <functional>
#include <iterator>
#include <cmath>
#include "math_consts.hpp"
#include "parameters.hpp"
#include "body_ax_geo.hpp"

namespace {

size_t read(std::istream&)
{
    return 0;
}

template <typename T, typename... Ts>
size_t read(std::istream& is, T& var, Ts&... ts) {
    is >> var;
    return read(is, ts...) + 1;
}

template <typename... Ts>
size_t readln(std::istream& is, Ts&... ts)
{
    std::string line;
    std::getline(is, line);
    if (sizeof...(ts) == 0) {
        return 0;
    } else {
        std::istringstream iss(line);
        return read(iss, ts...);
    }
}

}

template <typename T>
parameters<T>
body_ax_geo(program_options const& popt)
{
    std::ifstream ifs;
    parameters<T> params;
    vecg_t<T, MAX_ELEMS + 1> xe, ye, te, se;
    vecg_t<T, MAX_ELEMS> xm, ym, sm;
    vecg_t<T, MAX_SEGMENTS> rt;
    // `get_items` is unused, left for posterity
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
            readln(ifs, params.ngl);
            readln(ifs, params.sphere.rad);
            readln(ifs, params.sphere.xcenter);
            readln(ifs, params.vx);                 // velocity of indident flow
            readln(ifs, params.cr);                 // line vortex ring strength
            readln(ifs);
            readln(ifs, params.ne[0]);              // one segment consisting of arc elems
            readln(ifs);
            readln(ifs, params.xwmin, params.xwmax);
            readln(ifs, params.ywmin, params.ywmax);

            //--------------------------------
            // place the lvr inside the center
            //--------------------------------

            params.xlvr = params.sphere.xcenter;
            params.ylvr = HALF<T> * params.sphere.rad;

            //---
            // one semi-circular contour
            // with evenly distributed arcs
            //---

            params.sphere.ycenter = ZERO<T>;        // sphere center is on the x axis
            T dth = PI<T> / params.ne[0];
            for (int i = 0; i <= params.ne[0]; ++i) {
                T angle = i * dth;
                te[i] = angle;
                se[i] = angle * params.sphere.rad;
                xe[i] = params.sphere.xcenter + params.sphere.rad * std::cos(angle);
                ye[i] =                         params.sphere.rad * std::sin(angle);
            }

            //---
            // prepare
            //---

            params.nsg = 1;                         // one circular segment
            params.actis[0] = params.sphere.rad;
            params.xcntr[0] = params.sphere.xcenter;
            params.ycntr[0] = params.sphere.ycenter;

            //---
            // semicircular contour
            //---
            params.itp[0] = 2;
            for (int i = 0; i < params.ne[0]+1; ++i) {
                params.tw(0,i) = te[i];
                params.xw(0,i) = xe[i];
                params.yw(0,i) = ye[i];
            }

            //---
            // Collocation points
            //---
            params.ncl = params.ne[0];
            for (int i = 0; i < params.ne[0]; ++i) {
                params.t0[i] = HALF<T>*(te[i]+te(i+1));
                params.x0[i] = params.sphere.xcenter + params.sphere.rad*std::cos(params.t0[i]);
                params.y0[i] = params.sphere.ycenter + params.sphere.rad*std::sin(params.t0[i]);
                params.s0[i] = HALF<T>*(se[i]+se[i+1]);

                params.arel[i] =  dth * params.sphere.rad * TWO_PI<T> * params.y0[i];
                params.tnx0[i] = -std::sin(params.t0[i]);
                params.tny0[i] =  std::cos(params.t0[i]);
                params.vnx0[i] =  params.tny0[i];
                params.vny0[i] = -params.tnx0[i];

                params.dphidn0[i] = -params.vx * params.vnx0[i];

            }

        }
    } else if (popt.flow_type == to_underlying(FlowType::THORUS)) {
        std::string fname = (popt.input_data == "") ? "torus_trgl.dat" : popt.input_data;
        ifs.open(fname);
        if (ifs) {
            readln(ifs, params.ngl);
            readln(ifs, params.thorus.xfirst,  params.thorus.yfirst);
            readln(ifs, params.thorus.xsecond, params.thorus.ysecond);
            readln(ifs, params.thorus.xthird,  params.thorus.ythird);
            readln(ifs, params.vx);
            readln(ifs, params.cr);
            readln(ifs);
            readln(ifs, params.ne[0], rt[0]);
            readln(ifs, params.ne[1], rt[1]);
            readln(ifs, params.ne[2], rt[2]);
            readln(ifs);
            readln(ifs, params.xwmin, params.xwmax);
            readln(ifs, params.ywmin, params.ywmax);

        }
    }
    return std::move(params);
} // body_ax_geo

