#pragma once

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
#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>
#include "math_consts.hpp"

/*! \file elliptic_integral.h
    \brief computation of elliptic integrals of the first and second kinds by iteration method

    Details.
*/

/**
 * \f$ F\left(k\right)\equiv\int_0^{\pi/2}\frac{d\eta}{\sqrt{1-k^2\cos^2\eta}} \f$
 */
template <typename T>
void elliptic_integral (T const& rk2, T& f, T& e)
{
    constexpr T ACCURACY = T(1.0e-6);
    T rk = std::sqrt(rk2), g = ONE<T>, b = rk, c, d;

    f = HALF<T> * PI<T>;
    e = ONE<T>;
    do {
        c = std::sqrt(ONE<T> - b*b);
        b = (ONE<T> - c) / (ONE<T> + c);
        d = f*b;
        f += d;
        g = HALF<T>*g*b;
        e += g;
    } while (std::fabs(d) > ACCURACY);
    e = f * (ONE<T> - HALF<T>*rk2*e);
}

template <typename T>
void elliptic_integral_new (T const& rk2, T& f, T& e)
{
    T srk = std::sqrt(rk2);
    f = boost::math::ellint_1(srk);
    e = boost::math::ellint_2(srk);
    // `std::comp_ellint_1` and `std::comp_ellint_2` are too slow
    // f = std::comp_ellint_1(srk);
    // e = std::comp_ellint_2(srk);
}
