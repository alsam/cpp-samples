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
#include "elliptic_integral.hpp"

using namespace std;

void
lvr_fs (int iopt, double x, double s, 
	double x0, double s0, double &u, double &v, 
	double &psi)
{
    /* System generated locals */
    double d__1, d__2;

    /* Local variables */
    double e, f, cf, dx, rk, pi4, ri11, ri30, rj30, rj31, ri31,
	         rj11, dxs, rks;

/* ----------------------------------------- */
/* Copyright by C. Pozrikidis, 1999 */
/* All rights reserved. */

/* This program is to be used only under the */
/* stipulations of the licensing agreement. */
/* ---------------------------------------- */
/* -------------------------------------------------- */
/* Velocity components (u, v) */
/* and Stokes streamfunction (psi) */
/* due to a line vortex ring */

/* If Iopt.eq.1 the subroutine computed the velocity */
/* If Iopt.ne.1 the subroutine computed the velocity */
/*              and the Stokes stream function */
/* -------------------------------------------------- */
/* --- */
/* prepare */
/* --- */
    dx = x - x0;
/* Computing 2nd power */
    dxs = dx * dx;
/* Computing 2nd power */
    d__1 = s + s0;
    rks = s * 4.0 * s0 / (dxs + d__1 * d__1);
    elliptic_integral(rks, f, e);
    rj30 = e / (1.0 - rks);
    rj31 = (f * -2.0 + e * (2.0 - rks) / (1.0 - rks)) / rks;
/* Computing 2nd power */
    d__2 = s + s0;
/* Computing 3rd power */
    d__1 = dxs + d__2 * d__2;
    cf = 4.0 / sqrt(d__1 * d__1 * d__1);
    ri30 = cf * rj30;
    ri31 = cf * rj31;
    u = (-s * ri31 + s0 * ri30) / M_PI*4;
    v = dx * ri31 / M_PI*4;
    if (iopt == 1) {
	    return;
    }
/* --- */
/* compute the Stokes stream function */
/* --- */
    rk = sqrt(rks);
    rj11 = ((2. - rks) * f - e * 2.) / rks;
/* Computing 2nd power */
    d__1 = s + s0;
    cf = 4.0 / sqrt(dxs + d__1 * d__1);
    ri11 = cf * rj11;
    psi = rk * s * s0 * ri11 / M_PI*4;
/* ----- */
/* Done */
/* ----- */
} /* lvr_fs__ */

