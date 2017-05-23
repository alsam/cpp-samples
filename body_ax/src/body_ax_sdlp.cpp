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

using namespace std;

void
body_ax_sdlp(double x0, double y0, double t0, double x1, double y1, double t1, 
             double x2, double y2, double t2, int ngl, int ising, int itype,
             double rad, double xcnt, double ycnt,
             double &qqq, double &www)
{
    // System generated locals
    double d__1, d__2;

    // Local variables
    double g;
    double t, x, y;
    extern /* Subroutine */ void lgf_ax_fs(int, double, double, double, double,
            double&, double&, double&);
    double dr, cs, td, pi, xd, yd, tm, sn, xm, ym,
            vnx, vny, dgdx, dgdy;
    int iopt;
    double ornt, dists;

/* ========================================= */
/* FDLIB, BEMLIB, CFDLAB */

/* Copyright by C. Pozrikidis, 1999 */
/* All rights reserved. */

/* This program is to be used only under the */
/* stipulations of the licensing agreement. */
/* ========================================= */
/* ---------------------------------------------------------- */
/* Compute the single-layer and double-layer potential over */
/* a straight segment or circular arc */

/* LEGEND: */
/* ------- */

/* QQQ: single-layer potential */
/* WWW: double-layer potential */
/* ---------------------------------------------------------- */
/* ----------- */
/* initialize */
/* ----------- */
    iopt = 2;
/* for the Green's function */
    qqq = 0.0;
    www = 0.0;
/* --------------------------- */
/* prepare for the quadrature  */
/* --------------------------- */
    if (itype == 1) { // straight segments
        xm = 0.5 * (x2 + x1);
        xd = 0.5 * (x2 - x1);
        ym = 0.5 * (y2 + y1);
        yd = 0.5 * (y2 - y1);
        dr = sqrt(xd * xd + yd * yd);
        vnx =  yd / dr; // unit normal vector
        vny = -xd / dr;
    } else {          // circular arcs
        tm = 0.5 * (t2 + t1);
        td = 0.5 * (t2 - t1);
        dr = rad * abs(td);
        ornt = (td < 0.0) ? -1.0 : 1.0;    // orientation index, TODO use copysign
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
            cs  = cos(t);
            sn  = sin(t);
            x   = xcnt + rad * cs;
            y   = ycnt + rad * sn;
            vnx = cs * ornt; // unit normal vector */
            vny = sn * ornt; // normal vector points away from center
                             // when arc is counter-clockwise,
        }
        lgf_ax_fs(iopt, x, y, x0, y0, g, dgdx, dgdy);
/* -------------------------------------------------- */
/*  treat the slp singularity */

/*  Subtract off */
/*  the logarithmic singularity corresponding to the */
/*  free-space Green's function */

/*  NOTE: The double-layer singularity is not treated */

/* -------------------------------------------------- */
        if (ising == 1) {
            if (itype == 1) {
                d__1 = x - x0;
                d__2 = y - y0;
                // Computing 2nd power
                dists = d__1 * d__1 + d__2 * d__2;
            }
            if (itype == 2) {
                // Computing 2nd power
                d__1 = rad * (t0 - t);
                dists = d__1 * d__1;
            }
            g += std::log(dists) / FOUR_PI<FLOATING_TYPE>;
        }
        qqq += g * y * ww[i];
        www += (dgdx * vnx + dgdy * vny) * y * ww[i];
    }
/* ------------------------- */
/* finish up the quadrature */
/* ------------------------- */
    qqq *= dr;
    www *= dr;
/* ------------------------------------ */
/* add slp singularity back to the slp */
/* ------------------------------------ */
    if (ising == 1) {
        qqq -= dr * TWO<FLOATING_TYPE> * (std::log(dr) - ONE<FLOATING_TYPE>) / TWO_PI<FLOATING_TYPE>;
    }
// -----
// Done
// -----
} // body_ax_sdlp

