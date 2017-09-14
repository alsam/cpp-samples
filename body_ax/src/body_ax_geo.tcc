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
#include "lvr_fs.tcc"
#include "elm_line.tcc"

namespace {

template <typename... Ts>
size_t readln(std::istream& is, Ts& ...ts)
{
    std::string line;
    std::getline(is, line);
    size_t nargs = sizeof...(ts);
    if (nargs > 0) {
        std::istringstream iss(line);
        (iss >> ... >> ts);
    }
    return nargs;
}

}

template <typename T>
parameters<T>
body_ax_geo(program_options const& popt)
{
    std::ifstream ifs;
    parameters<T> params;
    vecg_t<T> xe, ye, te, se;
    vecg_t<T> xm, ym, sm;
    vecg_t<T> rt;

    T psi, ulvr, vlvr;

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
            //---
            // prepare: resize
            //---
            params.nsg = 1;                         // one circular segment

            params.actis.resize(params.nsg);
            params.xcntr.resize(params.nsg);
            params.ycntr.resize(params.nsg);

            // resize ne, itp
            params.ne.resize(params.nsg);
            params.itp.resize(params.nsg);
            rt.resize(params.nsg);

            T rad, xcenter, ycenter;

            readln(ifs, params.ngl);
            readln(ifs, rad);
            readln(ifs, xcenter);
            readln(ifs, params.vx);                 // velocity of incident flow
            readln(ifs, params.cr);                 // line vortex ring strength
            readln(ifs);
            readln(ifs, params.ne[0]);              // one segment consisting of arc elems
            readln(ifs);
            readln(ifs, params.xwmin, params.xwmax);
            readln(ifs, params.ywmin, params.ywmax);

            size_t max_size = params.ne[0] + 1;

            xe.resize(max_size);
            ye.resize(max_size);
            te.resize(max_size);
            se.resize(max_size);

            xm.resize(max_size - 1);
            ym.resize(max_size - 1);
            sm.resize(max_size - 1);

            params.x0.resize(max_size);
            params.y0.resize(max_size);
            params.t0.resize(max_size);
            params.s0.resize(max_size);
            params.dphidn0.resize(max_size);
            params.tnx0.resize(max_size);
            params.tny0.resize(max_size);
            params.vnx0.resize(max_size);
            params.vny0.resize(max_size);
            params.arel.resize(max_size);

            params.tw.resize(params.nsg, max_size);
            params.xw.resize(params.nsg, max_size);
            params.yw.resize(params.nsg, max_size);

            //--------------------------------
            // place the lvr inside the center
            //--------------------------------

            params.xlvr = xcenter;
            params.ylvr = HALF<T> * rad;

            //---
            // one semi-circular contour
            // with evenly distributed arcs
            //---

            ycenter = ZERO<T>;        // sphere center is on the x axis
            T dth = PI<T> / params.ne[0];
            for (int i = 0; i <= params.ne[0]; ++i) {
                T angle = i * dth;
                te[i] = angle;
                se[i] = angle * rad;
                xe[i] = xcenter + rad * std::cos(angle);
                ye[i] =           rad * std::sin(angle);
            }


            params.actis[0] = rad;
            params.xcntr[0] = xcenter;
            params.ycntr[0] = ycenter;

            //---
            // semicircular contour
            //---
            params.itp[0] = 2;
            for (int i = 0; i <= params.ne[0]; ++i) {
                params.tw(0, i) = te[i];
                params.xw(0, i) = xe[i];
                params.yw(0, i) = ye[i];
            }

            //---
            // Collocation points
            //---
            params.ncl = params.ne[0];
            for (int i = 0; i < params.ne[0]; ++i) {
                params.t0[i] = HALF<T>*(te[i]+te(i+1));
                params.x0[i] = xcenter + rad * std::cos(params.t0[i]);
                params.y0[i] = ycenter + rad * std::sin(params.t0[i]);
                params.s0[i] = HALF<T>*(se[i]+se[i+1]);

                params.arel[i] =  dth * rad * TWO_PI<T> * params.y0[i];
                params.tnx0[i] = -std::sin(params.t0[i]);
                params.tny0[i] =  std::cos(params.t0[i]);
                params.vnx0[i] =  params.tny0[i];
                params.vny0[i] = -params.tnx0[i];

                params.dphidn0[i] = -params.vx * params.vnx0[i];

                int iopt = 1;
                lvr_fs<T>(iopt, params.x0[i], params.y0[i], params.xlvr, params.ylvr, ulvr, vlvr, psi);

                params.dphidn0[i] -= params.cr*(ulvr*params.vnx0[i]+vlvr*params.vny0[i]);
            }
        }
    } else if (popt.flow_type == to_underlying(FlowType::THORUS)) {
        std::string fname = (popt.input_data == "") ? "torus_trgl.dat" : popt.input_data;
        ifs.open(fname);
        if (ifs) {
            //-------------
            // preparations
            //-------------

            int isym     = 1;
            params.nsg   = 3;

            // resize ne, itp
            params.ne.resize(params.nsg);
            params.itp.resize(params.nsg);
            rt.resize(params.nsg);

            params.actis.resize(params.nsg);
            params.xcntr.resize(params.nsg);
            params.ycntr.resize(params.nsg);

            T xfirst,  yfirst;  // 1st vertex
            T xsecond, ysecond; // 2nd vertex
            T xthird,  ythird;  // 3d vertex

            readln(ifs, params.ngl);
            readln(ifs, xfirst,  yfirst);
            readln(ifs, xsecond, ysecond);
            readln(ifs, xthird,  ythird);
            readln(ifs, params.vx);
            readln(ifs, params.cr);
            readln(ifs);
            readln(ifs, params.ne[0], rt[0]);
            readln(ifs, params.ne[1], rt[1]);
            readln(ifs, params.ne[2], rt[2]);
            readln(ifs);
            readln(ifs, params.xwmin, params.xwmax);
            readln(ifs, params.ywmin, params.ywmax);

            //--------------------------------
            // place the lvr at the centroid
            // of the triangle
            //--------------------------------

            params.xlvr = ( xfirst + xsecond + xthird ) / THREE<T>;
            params.ylvr = ( yfirst + ysecond + ythird ) / THREE<T>;

            int ic       = -1;        // collocation point counter
            T sinit      = ZERO<T>;   // initialize arc length

            // FIXME
            size_t max_size = 0;
            for (size_t i = 0; i < params.nsg; ++i) {
                max_size += params.ne[ i ];
            }
            // final adjustment
            ++max_size;

            xe.resize(max_size);
            ye.resize(max_size);
            te.resize(max_size);
            se.resize(max_size);

            xm.resize(max_size - 1);
            ym.resize(max_size - 1);
            sm.resize(max_size - 1);

            params.x0.resize(max_size);
            params.y0.resize(max_size);
            params.t0.resize(max_size);
            params.s0.resize(max_size);
            params.dphidn0.resize(max_size);
            params.tnx0.resize(max_size);
            params.tny0.resize(max_size);
            params.vnx0.resize(max_size);
            params.vny0.resize(max_size);
            params.arel.resize(max_size);

            params.tw.resize(params.nsg, params.ne.maxCoeff() + 1 );
            params.xw.resize(params.nsg, params.ne.maxCoeff() + 1 );
            params.yw.resize(params.nsg, params.ne.maxCoeff() + 1 );

            //---
            // side # 1
            //---

            params.itp[0] = 1;    // straight segment

            elm_line(params.ne[0], rt[0],
                     xfirst,  yfirst,
                     xsecond, ysecond,
                     sinit, isym,
                     xe, ye, se, xm, ym, sm);

            for (size_t i = 0; i <= params.ne[0]; ++i) {
                params.xw(0, i) = xe[i];
                params.yw(0, i) = ye[i];
            }

            //---
            // collocation points
            //---

            for (size_t i = 0; i < params.ne[0]; ++i) {
                ++ic;
                params.x0[ic] = xm[i];
                params.y0[ic] = ym[i];
                params.s0[ic] = sm[i];

                T ddx = xe[i+1]-xe[i];
                T ddy = ye[i+1]-ye[i];
                T ddl = std::sqrt(ddx*ddx + ddy*ddy);

                params.tnx0[ic] = ddx / ddl;
                params.tny0[ic] = ddy / ddl;
                params.vnx0[ic] =  params.tny0[ic];
                params.vny0[ic] = -params.tnx0[ic];

                params.arel[ic] = ddl * TWO_PI<T> * params.y0[ic];

                params.dphidn0[ic] = -params.vx * params.vnx0[ic];

                int iopt = 1;
                lvr_fs<T>(iopt, params.x0[ic], params.y0[ic], params.xlvr, params.ylvr, ulvr, vlvr, psi);

                params.dphidn0[ic] -= params.cr*(ulvr*params.vnx0[ic]+vlvr*params.vny0[ic]);
            }

            sinit = se[params.ne[0]];

            //---
            // side # 2
            //---

            params.itp[1] = 1;    // straight segment

            elm_line(params.ne[1], rt[1],
                     xsecond, ysecond,
                     xthird,  ythird,
                     sinit, isym,
                     xe, ye, se, xm, ym, sm);

            for (size_t i = 0; i <= params.ne[1]; ++i) {
                params.xw(1, i) = xe[i];
                params.yw(1, i) = ye[i];
            }

            //---
            // collocation points
            //---

            for (size_t i = 0; i < params.ne[1]; ++i) {
                ++ic;
                params.x0[ic] = xm[i];
                params.y0[ic] = ym[i];
                params.s0[ic] = sm[i];

                T ddx = xe[i+1]-xe[i];
                T ddy = ye[i+1]-ye[i];
                T ddl = std::sqrt(ddx*ddx + ddy*ddy);

                params.tnx0[ic] = ddx / ddl;
                params.tny0[ic] = ddy / ddl;
                params.vnx0[ic] =  params.tny0[ic];
                params.vny0[ic] = -params.tnx0[ic];

                params.arel[ic] = ddl * TWO_PI<T> * params.y0[ic];

                params.dphidn0[ic] = -params.vx * params.vnx0[ic];

                int iopt = 1;
                lvr_fs<T>(iopt, params.x0[ic], params.y0[ic], params.xlvr, params.ylvr, ulvr, vlvr, psi);

                params.dphidn0[ic] -= params.cr*(ulvr*params.vnx0[ic]+vlvr*params.vny0[ic]);
            }

            sinit = se[params.ne[1]];


            //---
            // side # 3
            //---

            params.itp[2] = 1;    // straight segment

            elm_line(params.ne[2], rt[2],
                     xthird,  ythird,
                     xfirst,  yfirst,
                     sinit, isym,
                     xe, ye, se, xm, ym, sm);

            for (size_t i = 0; i <= params.ne[2]; ++i) {
                params.xw(2, i) = xe[i];
                params.yw(2, i) = ye[i];
            }

            //---
            // collocation points
            //---

            for (size_t i = 0; i < params.ne[2]; ++i) {
                ++ic;
                params.x0[ic] = xm[i];
                params.y0[ic] = ym[i];
                params.s0[ic] = sm[i];

                T ddx = xe[i+1]-xe[i];
                T ddy = ye[i+1]-ye[i];
                T ddl = std::sqrt(ddx*ddx + ddy*ddy);

                params.tnx0[ic] = ddx / ddl;
                params.tny0[ic] = ddy / ddl;
                params.vnx0[ic] =  params.tny0[ic];
                params.vny0[ic] = -params.tnx0[ic];

                params.arel[ic] = ddl * TWO_PI<T> * params.y0[ic];

                params.dphidn0[ic] = -params.vx * params.vnx0[ic];

                int iopt = 1;
                lvr_fs<T>(iopt, params.x0[ic], params.y0[ic], params.xlvr, params.ylvr, ulvr, vlvr, psi);

                params.dphidn0[ic] -= params.cr*(ulvr*params.vnx0[ic]+vlvr*params.vny0[ic]);
            }

            params.ncl = ic + 1;  // number of collocation points

        }
    }

    // read x00, y00 for isolines
    readln(ifs);
    for(;;) {
        T x00s, y00s;
        constexpr T eps = T(1e-7);

        readln(ifs, x00s, y00s);
        if (std::fabs(x00s-99) < eps || std::fabs(y00s-99) < eps) {
            break;
        }
        params.x00.push_back(x00s);
        params.y00.push_back(y00s);
    }

    for (size_t i = 0; i < params.x00.size(); ++i) {
        std::cout << "x00[" << i << "]: " << params.x00[i] << " y00[" << i << "]: " << params.y00[i] << std::endl;
    }

    ifs.close();
    params.gl = gauss_legendre<T>(params.ngl);

    //-----
    // Done
    //-----

    // return std::move(params);
    // don't use `std::move` for local objects Effective Modern C++ Item 25
    return params;
} // body_ax_geo

