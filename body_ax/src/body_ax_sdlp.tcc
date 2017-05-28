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
#include "lin_alg_types.hpp"
#include "gauss_legendre.hpp"
#include "parameters.hpp"
#include "math_consts.hpp"
#include "lgf_ax_fs.tcc"

template <typename T>
void
body_ax_sdlp(T x0, T y0, T t0, T x1, T y1, T t1,
             T x2, T y2, T t2, int ngl, bool ising, int itype,
             T rad, T xcnt, T ycnt,
             T &qqq, T &www)
{
    // Local variables
    T g;
    T t, x, y;
    T dr, cs, td, pi, xd, yd, tm, sn, xm, ym,
            vnx, vny, dgdx, dgdy;
    int iopt;
    T ornt;

// =========================================
// FDLIB, BEMLIB, CFDLAB

// Copyright by C. Pozrikidis, 1999
// All rights reserved.

// This program is to be used only under the
// stipulations of the licensing agreement.
// =========================================
// ----------------------------------------------------------
// Compute the single-layer and double-layer potential over
// a straight segment or circular arc

// LEGEND:
// -------

// QQQ: single-layer potential
// WWW: double-layer potential
// ----------------------------------------------------------
// -----------
// initialize
// -----------
    iopt = 2;
// for the Green's function
    qqq = ZERO<T>;
    www = ZERO<T>;
// ---------------------------
// prepare for the quadrature 
// ---------------------------
    if (itype == 1) { // straight segments
        xm = HALF<T> * (x2 + x1);
        xd = HALF<T> * (x2 - x1);
        ym = HALF<T> * (y2 + y1);
        yd = HALF<T> * (y2 - y1);
        dr = sqrt(xd * xd + yd * yd);
        vnx =  yd / dr; // unit normal vector
        vny = -xd / dr;
    } else {          // circular arcs
        tm = HALF<T> * (t2 + t1);
        td = HALF<T> * (t2 - t1);
        dr = rad * std::abs(td);
        ornt = std::copysign( ONE<T>, td ); // orientation index
        //ornt = (td < ZERO<T>) ? MINUS_ONE<T> : ONE<T>;
    }
// --- 
// loop over Gaussian points
// --- 
//
    gauss_legendre<T> gl(ngl);
    auto const& zz = gl.z();
    auto const& ww = gl.w();
    for (int i = 0; i < ngl; ++i) {
        if (itype == 1) {
            x = xm + xd * zz[i];
            y = ym + yd * zz[i];
        } else {
            t   = tm + td * zz[i];
            cs  = std::cos(t);
            sn  = std::sin(t);
            x   = xcnt + rad * cs;
            y   = ycnt + rad * sn;
            vnx = cs * ornt; // unit normal vector
            vny = sn * ornt; // normal vector points away from center
                             // when arc is counter-clockwise,
        }
        lgf_ax_fs<T>(iopt, x, y, x0, y0, g, dgdx, dgdy);
// --------------------------------------------------
//  treat the slp singularity

//  Subtract off
//  the logarithmic singularity corresponding to the
//  free-space Green's function

//  NOTE: The double-layer singularity is not treated

// --------------------------------------------------
        if (ising) {
            T dists;
            if (itype == 1) {
                T d1 = x - x0;
                T d2 = y - y0;
                dists = d1 * d1 + d2 * d2;
            }
            if (itype == 2) {
                T d1 = rad * (t0 - t);
                dists = d1 * d1;
            }
            g += std::log(dists) / FOUR_PI<T>;
        }
        qqq += g * y * ww[i];
        www += (dgdx * vnx + dgdy * vny) * y * ww[i];
    }
// -------------------------
// finish up the quadrature
// -------------------------
    qqq *= dr;
    www *= dr;
// ------------------------------------
// add slp singularity back to the slp
// ------------------------------------
    if (ising) {
        qqq -= dr * TWO<T> * (std::log(dr) - ONE<T>) / TWO_PI<T>;
    }
// -----
// Done
// -----
} // body_ax_sdlp

