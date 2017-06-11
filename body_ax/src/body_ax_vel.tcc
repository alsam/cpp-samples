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

#include <cmath>
#include "math_consts.hpp"
#include "parameters.hpp"
#include "body_ax_sdlp.tcc"
#include "lvr_fs.tcc"
#include "body_ax_vel.tcc"

template <typename T>
void
velocity(parameters<T> const& params,
         matg_t<T> const& phi, vecg_t<T> const& dphidn0, // in
         T x00, T y00,                                   // in
         T& ux, T& uy)                                   // out
{
// =========================================
// FDLIB, BEMLIB, CFDLAB

// Copyright by C. Pozrikidis, 1999
// All rights reserved.

// This program is to be used only under the
// stipulations of the licensing agreement.
// =========================================
// ---------------------------------------------------
// Compute the velocity at the point: X00, Y00
// using the boundary-integral
// representation

// Velocity is computed by numerical differentiation
// of the potential
// -----------------------------------------------------

// --------
// prepare
// --------
    T eps2 = TWO<T> * EPS<T>;
// for centered differences
    bool ising = false;
// ---
// initialize
// ---
    T phi1 = ZERO<T>;
    T phi2 = ZERO<T>;
    T phi3 = ZERO<T>;
    T phi4 = ZERO<T>;
    T unused = ZERO<T>;
    T x01 = x00 - EPS<T>;
    T x02 = x00 + EPS<T>;
    T y01 = y00 - EPS<T>;
    T y02 = y00 + EPS<T>;
// ---------------------------------
// Boundary integral representation
// ---------------------------------
    int j = 0; // collocation point counter
    for (int k = 0; k < params.nsg; ++k) {
        T rad  = params.actis[k];
        T xcnt = params.xcntr[k];
        T ycnt = params.ycntr[k];
        for (int l = 1; l < params.ne[k]; ++l) {
            T x1 = params.xw(k, l);
            T y1 = params.yw(k, l);
            T t1 = params.tw(k, l);

            T x2 = params.xw(k, l + 1);
            T y2 = params.yw(k, l + 1);
            T t2 = params.tw(k, l + 1);

            ++j;
 
            T qqq, www;

            auto integrate = [&](T x, T y, T& phis) {
                body_ax_sdlp(x, y, unused, x1, y1, t1, x2, y2, t2,
                             params.gl, ising, params.itp[k], rad, xcnt, ycnt,
                             qqq, www);
                phis = phis - params.dphidn0[j] * qqq + phi(k,l) * www;
            };

            integrate(x01, y00, phi1);
            integrate(x02, y00, phi2);
            integrate(x00, y01, phi3);
            integrate(x00, y02, phi4);
        }
    }
// --------------------------
// numerical differentiation
// --------------------------
    ux = (phi2 - phi1) / eps2;
    uy = (phi4 - phi3) / eps2;
// --------------------------------
// add the incident streaming flow
// --------------------------------
    ux += params.vx;
// ---------------------------------------------
// add the velocity due to the line vortex ring
// ---------------------------------------------
    int iopt = 1;
    T ulvr, vlvr, psi;
    lvr_fs(iopt, x00, y00, params.xlvr, params.ylvr, ulvr, vlvr, psi);
    ux += params.cr * ulvr;
    uy += params.cr * vlvr;
// -----
// Done
// -----
} // velocity

