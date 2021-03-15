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

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include <Eigen/Dense>

/**  
 *  @brief Contains fundamental type defintions used by the project. 
 *  
 */

namespace py = pybind11;

namespace detail {

template <typename T> inline T sqr(T a) { return a*a; }

inline double id(size_t i, size_t j) { return i==j ? 1.0 : 0.0; }

inline bool is_odd(size_t i) { return i&1; }

inline bool is_even(size_t i) { return !is_odd(i); }

}

using RowVectorXd = Eigen::RowVectorXd;

using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
// Use RowMatrixXd instead of MatrixXd
// see https://pybind11.readthedocs.io/en/stable/advanced/cast/eigen.html#storage-orders for more details

// used to be
// void cosfft1(std::vector<double>& data, size_t n, bool inverse = false);

namespace FCT /// stands for Fast Cosine Transform
{

void cosfft1(size_t n, Eigen::Ref<Eigen::VectorXd> data, bool inverse = false);

void cft2(size_t m, size_t n,
          Eigen::Ref<RowMatrixXd> data,
          bool inverse = false);

/// this version of `cft2` might be beneficial for parallel computation
/// that parallel libraries often include tailored versions of `transpose`
/// besides row major matrix storage allows to slice vectors from the matrix efficiently
void cft2_with_transpose(size_t m, size_t n,
                         RowMatrixXd &data,     /// don't use `Eigen::Ref<RowMatrixXd>`
                         bool inverse = false); /// as you get assertion in `.transposeInPlace()`

}
