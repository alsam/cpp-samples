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

template <typename T>
void
lgf_ax_fs (int iopt, T x, T s, T x0, T s0,
           T &g, T &gx, T &gs)
{
// =========================================
// FDLIB, CFDLAB, BEMLIB
//
// Copyright by C. Pozrikidis, 1999
// All rights reserved.
//
// This program is to be used only under the
// stipulations of the licensing agreement.
// =========================================
// -----------------------------------------
// Free-space axisymmetric Green's function.
// of Laplace's equation
//
//  Iopt =  1 compute only the Green's function
//       ne 1 compute Green's function and gradient
// -------------------------------------------

// --------
// prepare
// --------
    T dx = x - x0;
    T dxs = dx * dx;
    T ss0s = (s+s0) * (s+s0);
    T rks = FOUR<T> * s * s0 / (dxs + ss0s);
    T f, e;
    elliptic_integral(rks, f, e);
// -----------------
// Green's function
// -----------------
    T rj10 = f;
    T den = std::sqrt(dxs + ss0s);
    T ri10 = FOUR<T> * rj10 / den;
    g = ri10 / FOUR_PI<T>;
    if (iopt == 1) {
      return;
    }
// ---------------------
// compute: I30 and I31
// ---------------------
    T rksc = ONE<T> - rks;
    T rj30 = e / rksc;
    T rj31 = (-TWO<T> * f + (TWO<T> - rks) * e / rksc) / rks;
    T cf = FOUR<T> / (den * den * den);
    T ri30 = cf * rj30;
    T ri31 = cf * rj31;
// ---------
// gradient
// ---------
    gx = -dx * ri30;
    gs = -s * ri30 + s0 * ri31;
    gx /= FOUR_PI<T>;
    gs /= FOUR_PI<T>;
// -----
// Done
// -----
} // lgf_ax_fs

