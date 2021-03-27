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
 *  @brief Contains Fast Cosine Transform routines. 
 *  
 */

namespace FCT /// stands for Fast Cosine Transform
{

enum class TransformType {Forward, Inverse};


/// @return Chebyshev type grid more dense at both ends
/// that spans [min .. max] inclusively
Eigen::VectorXd grid_span(size_t nodes,
                          double min = -1.,
                          double max =  1.);

void cosfft1(size_t n,
             Eigen::Ref<Eigen::VectorXd> data,
             TransformType transform_type = TransformType::Forward);

void cft2(size_t m, size_t n,
          Eigen::Ref<RowMatrixXd> data,
          TransformType transform_type = TransformType::Forward);

/// this version of `cft2` might be beneficial for parallel computation
/// that parallel libraries often include tailored versions of `transpose`
/// besides row major matrix storage allows to slice vectors from the matrix efficiently
void cft2_with_transpose(size_t m, size_t n,
                         RowMatrixXd &data, /// don't use `Eigen::Ref<RowMatrixXd>`
                         TransformType transform_type = TransformType::Forward); /// as you get assertion in `.transposeInPlace()`

}
