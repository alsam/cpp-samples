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

#include <cmath>
#include "lin_alg_types.hpp"
#include "gauss_legendre.hpp"
#include "parameters.hpp"
#include "math_consts.hpp"
#include "lgf_ax_fs.tcc"

void
body_ax_sdlp(FLOATING_TYPE x0, FLOATING_TYPE y0, FLOATING_TYPE t0, FLOATING_TYPE x1, FLOATING_TYPE y1, FLOATING_TYPE t1,
             FLOATING_TYPE x2, FLOATING_TYPE y2, FLOATING_TYPE t2, int ngl, int ising, int itype,
             FLOATING_TYPE rad, FLOATING_TYPE xcnt, FLOATING_TYPE ycnt,
             FLOATING_TYPE &qqq, FLOATING_TYPE &www)
{
    // Local variables
    FLOATING_TYPE g;
    FLOATING_TYPE t, x, y;
    FLOATING_TYPE dr, cs, td, pi, xd, yd, tm, sn, xm, ym,
            vnx, vny, dgdx, dgdy;
    int iopt;
    FLOATING_TYPE ornt;

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
    qqq = ZERO<FLOATING_TYPE>;
    www = ZERO<FLOATING_TYPE>;
// ---------------------------
// prepare for the quadrature 
// ---------------------------
    if (itype == 1) { // straight segments
        xm = HALF<FLOATING_TYPE> * (x2 + x1);
        xd = HALF<FLOATING_TYPE> * (x2 - x1);
        ym = HALF<FLOATING_TYPE> * (y2 + y1);
        yd = HALF<FLOATING_TYPE> * (y2 - y1);
        dr = sqrt(xd * xd + yd * yd);
        vnx =  yd / dr; // unit normal vector
        vny = -xd / dr;
    } else {          // circular arcs
        tm = HALF<FLOATING_TYPE> * (t2 + t1);
        td = HALF<FLOATING_TYPE> * (t2 - t1);
        dr = rad * std::abs(td);
        ornt = std::copysign( ONE<FLOATING_TYPE>, td ); // orientation index
        //ornt = (td < ZERO<FLOATING_TYPE>) ? MINUS_ONE<FLOATING_TYPE> : ONE<FLOATING_TYPE>;
    }
// --- 
// loop over Gaussian points
// --- 
//
    // TODO parameterize over float type
    gauss_legendre<FLOATING_TYPE> gl(ngl);
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
        // TODO parameterize over float type
        lgf_ax_fs(iopt, x, y, x0, y0, g, dgdx, dgdy);
// --------------------------------------------------
//  treat the slp singularity

//  Subtract off
//  the logarithmic singularity corresponding to the
//  free-space Green's function

//  NOTE: The double-layer singularity is not treated

// --------------------------------------------------
        if (ising == 1) {
            FLOATING_TYPE dists;
            if (itype == 1) {
                FLOATING_TYPE d1 = x - x0;
                FLOATING_TYPE d2 = y - y0;
                dists = d1 * d1 + d2 * d2;
            }
            if (itype == 2) {
                FLOATING_TYPE d1 = rad * (t0 - t);
                dists = d1 * d1;
            }
            g += std::log(dists) / FOUR_PI<FLOATING_TYPE>;
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
    if (ising == 1) {
        qqq -= dr * TWO<FLOATING_TYPE> * (std::log(dr) - ONE<FLOATING_TYPE>) / TWO_PI<FLOATING_TYPE>;
    }
// -----
// Done
// -----
} // body_ax_sdlp

