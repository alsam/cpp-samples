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
#include "elliptic_integral.hpp"

// free space
template <typename T>
void
lvr_fs (int iopt,
        T x, T s, T x0, T s0, // input parameters
        T &u, T &v, T &psi)   // output parameters
{
// -----------------------------------------
// Copyright by C. Pozrikidis, 1999
// All rights reserved.

// This program is to be used only under the
// stipulations of the licensing agreement.
// ----------------------------------------
// --------------------------------------------------
// Velocity components (u, v)
// and Stokes streamfunction (psi)
// due to a line vortex ring
//
// If Iopt.eq.1 the subroutine computed the velocity
// If Iopt.ne.1 the subroutine computed the velocity
//              and the Stokes stream function
// --------------------------------------------------
// ---
// prepare
// ---
    T dx = x - x0;   // \delta x
    T dxs = dx * dx; // {\delta x}^2
    T sps0 = s + s0;
    T rks = FOUR<T> * s * s0 / (dxs + sps0 * sps0);
    T f, e;
    elliptic_integral(rks, f, e);
    T rj30 = e / (ONE<T> - rks);
    T rj31 = (-TWO<T> * f + e * (TWO<T> - rks) / (ONE<T> - rks)) / rks;
    T d1 = dxs + sps0 * sps0;
    T cf = FOUR<T> / std::sqrt(d1 * d1 * d1);
    T ri30 = cf * rj30;
    T ri31 = cf * rj31;
    u = (-s * ri31 + s0 * ri30) / FOUR_PI<T>;
    v = dx * ri31 / FOUR_PI<T>;
    if (iopt == 1) {
	    return;
    }
// ---
// compute the Stokes stream function
// ---
    T rk = std::sqrt(rks);
    T rj11 = ((TWO<T> - rks) * f - e * TWO<T>) / rks;
    cf = FOUR<T> / std::sqrt(dxs + sps0 * sps0);
    T ri11 = cf * rj11;
    psi = rk * s * s0 * ri11 / FOUR_PI<T>;
// -----
// Done
// -----
} // lvr_fs__
