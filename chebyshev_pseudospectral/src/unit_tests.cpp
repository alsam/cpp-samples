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

#include <gtest/gtest.h>
#include "math.hpp"

TEST(cftSuite, test_cosfft1)
{
    RowVectorXd x(3), x_inv(3);

    x     << 1.,  2., 3.;
    x_inv << 6., -2., 2.;

    cosfft1(x, 2);

    EXPECT_DOUBLE_EQ((x - x_inv).norm(), 0.0);
}

TEST(cftSuite, test_cf2)
{
    RowMatrixXd m(3, 3), m_inv(3, 3);

    m << 1., 2., 3.,
         4., 5., 6.,
         7., 8., 9.;
    m_inv << 6.,  4., 10.,
            18., 10., 22.,
            30., 16., 34.;

    cft2(m, 2);

    EXPECT_DOUBLE_EQ((m - m_inv).norm(), 0.0);
}
