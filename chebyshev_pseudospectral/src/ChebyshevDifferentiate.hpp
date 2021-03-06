// -*- C++ -*-

// The MIT License (MIT)
//
// Copyright (c) 2021 Alexander Samoilov
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

#include "math.hpp"

/** 
 *  @brief Contains recurrent formulas for Chebyshev coefficients for function derivative
 *
 */

namespace CS /// CS stands for \f$\boldsymbol{C}\f$ebyshev-\f$\boldsymbol{S}\f$pectral
{

void spectral_differentiate(size_t n,
                            Eigen::Ref<RowVectorXd> in,
                            Eigen::Ref<RowVectorXd> out,
                            double span = 1.);

inline void spectral_differentiate(size_t n,
                                   Eigen::Ref<RowVectorXd> inout)
{
    spectral_differentiate(n, inout, inout, 1.);
}

void homogeneous_boundary(size_t m, size_t n,
                          Eigen::Ref<RowMatrixXd> in,
                          Eigen::Ref<RowMatrixXd> out);

inline void homogeneous_boundary(size_t m, size_t n,
                                 Eigen::Ref<RowMatrixXd> inout)
{
    homogeneous_boundary(m, n, inout, inout);
}

void second_derivative(size_t m, size_t n,
                       Eigen::Ref<RowMatrixXd> in,
                       Eigen::Ref<RowMatrixXd> out);

inline void second_derivative(size_t m, size_t n,
                              Eigen::Ref<RowMatrixXd> inout)
{
    second_derivative(m, n, inout, inout);
}

} // namespace CS
