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
#include "parameters.hpp"
#include "math_consts.hpp"

template <typename T>
void
elm_line(int n, T ratio, T x1, T y1, T x2, T y2, T sinit, int isym, // input parameters
         vecg_t<T>& xe, vecg_t<T>& ye, vecg_t<T>& se, // output parameters
         vecg_t<T>& xm, vecg_t<T>& ym, vecg_t<T>& sm) // output parameters
{

    //-----------------------------------------
    // FDLIB
    //
    // Copyright by C. Pozrikidis, 1999
    // All rights reserved.
    //
    // This program is to be used only under the
    // stipulations of the licensing agreement.
    //----------------------------------------
 
    //-----------------------------------------------------
    // Disretization of a line segment into N elements
    //
    //  X1,Y1: coordinates of the first point
    //  X2,Y2: coordinates of the last point
    //
    //  ratio:
    //
    //     If Isym = 0, ratio of length of LAST to FIRST element
    //     If Isym = 1, ratio of length of MID  to FIRST element
    //
    //  alpha: geometric factor ratio
    //
    //  sinit: specified arc length at (X1, Y1)
    //
    //  se: arc length at the element end-nodes
    //  sm: arc length at the element mid-nodes
    //
    //  xe,ye: end nodes
    //  xm,ym: mid nodes
    //
    //-----------------------------------------------------

    //------------
    // one element
    //------------
      
    if (n == 1) {
        xe[0] = x1;
        ye[0] = y1;
        xe[1] = x2;
        ye[1] = y2;
        se[0] = sinit;
        T dx = x2 - x1, dy = y2 - y1;
        se[1] = se[0] + std::sqrt( dx*dx + dy*dy ); // std::hypot is more convenient but too slow
    } else {
        T alpha, factor, texp, deltax, deltay;
        if (isym == 0) { // biased distribution
            if (ratio == ONE<T>) {
                alpha  = ONE<T>;
                factor = ONE<T> / n;
            } else {
                texp   = ONE<T> / (n - ONE<T>);
                alpha  = std::pow(ratio, texp); // ratio ** texp
                factor = (ONE<T> - alpha) / (ONE<T> - std::pow(alpha, n));
            }

            deltax = (x2-x1) * factor;   // x length of first element
            deltay = (y2-y1) * factor;   // y length of first element

            xe[0] = x1;     // first point
            ye[0] = y1;
            se[0] = sinit;

            for (size_t i = 1; i <= n; ++i) {
                xe[i]  = xe[i-1]+deltax;
                ye[i]  = ye[i-1]+deltay;
                se[i]  = se[i-1]+std::sqrt(deltax*deltax+deltay*deltay);
                deltax = deltax * alpha;
                deltay = deltay * alpha;
            }

        } else if (n % 2 == 0) {        // symmetric distribution; even number of points

            T xh = HALF<T> * (x1+x2);   // mid-point
            T yh = HALF<T> * (y1+y2);

            if (n == 2) {
                xe[0] = x1;
                ye[0] = y1;
                xe[1] = xh;
                ye[1] = yh;
                xe[2] = x2;
                ye[2] = y2;
                se[0] = sinit;
                T dx1 = xh - x1, dy1 = yh - y1;
                T dx2 = x2 - xh, dy2 = y2 - yh;
                se[1] = se[0] + std::sqrt( dx1*dx1 + dy1*dy1 );
                se[2] = se[1] + std::sqrt( dx2*dx2 + dy2*dy2 );
            } else {                    // n > 2
                int nh  = n/2;
                int nh1 = nh+1;
                if (ratio == ONE<T>) {
                    alpha  = ONE<T>;
                    factor = ONE<T> / nh;
                } else {
                    texp   = ONE<T> / (nh - ONE<T>);
                    alpha  = std::pow(ratio, texp); // ratio ** texp
                    factor = (ONE<T> - alpha) / (ONE<T> - std::pow(alpha, nh));
                }
                deltax = (xh-x1) * factor;   // x length of first element
                deltay = (yh-y1) * factor;   // y length of first element

                xe[0] = x1;    // first point
                ye[0] = y1;
                se[0] = sinit;

                            
                for (size_t i = 1; i <= nh; ++i) {
                    xe[i]  = xe[i-1]+deltax;
                    ye[i]  = ye[i-1]+deltay;
                    se[i]  = se[i-1]+std::sqrt(deltax*deltax+deltay*deltay);
                    deltax = deltax * alpha;
                    deltay = deltay * alpha;
                }

                deltax = deltax/alpha;
                deltay = deltay/alpha;

                for (size_t i = nh1; i <= n; ++i) {
                    xe[i]  = xe[i-1]+deltax;
                    ye[i]  = ye[i-1]+deltay;
                    se[i]  = se[i-1]+std::sqrt(deltax*deltax+deltay*deltay);
                    deltax = deltax / alpha;
                    deltay = deltay / alpha;
                }
            }

        } else {                        // symmetric distribution; odd number of points

            if (ratio == ONE<T>) {
                alpha  = ONE<T>;
                factor = ONE<T> / (n + 1);
            } else {
                texp   = TWO<T> / (n - ONE<T>);
                alpha  = std::pow(ratio, texp); // ratio ** texp
                T tmp1 = HALF<T> * (n + ONE<T>);
                T tmp2 = HALF<T> * (n - ONE<T>);
                factor = (ONE<T> - alpha) / (TWO<T> - std::pow(alpha, tmp1) - std::pow(alpha, tmp2));
            }
      
            deltax = (x2-x1) * factor;   // x length of first element
            deltay = (y2-y1) * factor;   // y length of first element

            xe[0] = x1;    // first point
            ye[0] = y1;
            se[0] = sinit;

            for (size_t i = 1; i < (n+3)/2; ++i) {
                xe[i]  = xe[i-1]+deltax;
                ye[i]  = ye[i-1]+deltay;
                se[i]  = se[i-1]+std::sqrt(deltax*deltax+deltay*deltay);
                deltax = deltax * alpha;
                deltay = deltay * alpha;
            }

            deltax = deltax/(alpha*alpha);
            deltay = deltay/(alpha*alpha);

            for (size_t i = (n+5)/2-1; i <= n; ++i) {
                xe[i]  = xe[i-1]+deltax;
                ye[i]  = ye[i-1]+deltay;
                se[i]  = se[i-1]+std::sqrt(deltax*deltax+deltay*deltay);
                deltax = deltax / alpha;
                deltay = deltay / alpha;
            }

        }
        std::cout << "elm_line: Geometric ratio: " << alpha << std::endl;
    }

    //-------------------
    // compute mid-points
    //-------------------

    for (size_t i = 0; i < n; ++i) {
       xm[i] = HALF<T> * (xe[i]+xe[i+1]);
       ym[i] = HALF<T> * (ye[i]+ye[i+1]);
       sm[i] = HALF<T> * (se[i]+se[i+1]);
    }

    //-----
    // Done
    //-----

} // elm_line

