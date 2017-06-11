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

#include "lin_alg_types.hpp"

/// the most complete is
/// http://processingjs.nihongoresources.com/bezierinfo/legendre-gauss-values.php

template <typename T>
class gauss_legendre {

private:

    unsigned order_;

    vecg_t<T> z_, w_;

    /// imitate fortran90 statements Z(7:12) = -Z(6:1:-1); W(7:12) = W(6:1:-1)
    void reflect ();

public:

    /// constructor
    gauss_legendre(unsigned order = 6);

    /// another constructor - slow and more accurate
    gauss_legendre(unsigned order, unsigned);

    /// @returns the order of the quadrature
    unsigned order() const { return order_; }

    /// knots
    vecg_t<T> const& z() const { return z_; }

    /// weights
    vecg_t<T> const& w() const { return w_; }
};

#include "gauss_legendre.tcc"
