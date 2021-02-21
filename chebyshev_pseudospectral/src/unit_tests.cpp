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

template<unsigned N>
void sysslv(double a[N][N], double b[N])
{
    constexpr unsigned n = N;
    double biga,save;
    int i,j,k,imax;

    for (j=0;j<n;j++) {
        biga=0.0;
        for(i=j;i<n;i++)
        if (std::fabs(biga) < std::fabs(a[i][j])) {
            biga=a[i][j];
            imax=i;
        }
        if (std::fabs(biga) == 0.0) {
            throw std::logic_error("Very singular matrix...");
        }
        for (k=j;k<n;k++) {
            save=a[imax][k];
            a[imax][k]=a[j][k];
            a[j][k]=save/biga;
        }
        save=b[imax];
        b[imax]=b[j];
        b[j]=save/biga;
        for (i=j+1;i<n;i++) {
            for(k=j+1;k<n;k++)
                a[i][k] -= a[i][j]*a[j][k];
            b[i] -= b[j]*a[i][j];
        }
    }
    for (k=n-2;k>=0;k--)
        for(j=n-1;j>k;j--)
            b[k] -= a[k][j]*b[j];
}

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

TEST(bsSuite, test_gauss_elim)
{
    // https://mxncalc.com/gaussian-elimination-calculator
    double A[4][4] = {{1.,  2., 3.,  4.},
                      {5.,  6., 8.,  5.},
                      {4., 15., 2., 12.},
                      {3.,  2., 5.,  6.},};
    double b[4] = {11., 12., 3., 7.,};
    double x[4] = { -7.127'853'881'278'5,
                     1.237'442'922'374'4,
                     4.858'447'488'584'5,
                     0.269'406'392'694'06,
                  };
    Eigen::Vector4d bb(b), xx(x);
    Eigen::Matrix4d AA(reinterpret_cast<double*>(A));
    AA.transposeInPlace();
    // std::cout << "AA: " << AA << std::endl;
    sysslv(A, b);
    Eigen::Vector4d b2(b);

    //EXPECT_DOUBLE_EQ((b2 - xx).norm(), 0.0);
    // Expected equality of these values:
    //   (b2 - xx).norm()
    //     Which is: 5.3413613165043815e-14
    //   0.0
    //     Which is: 0
    EXPECT_NEAR((b2 - xx).norm(), 0.0, 1e-12);

    Eigen::Vector4d x2 = AA.lu().solve(bb);
    // std::cout << "x2: " << x2 << std::endl;
    // std::cout << "xx: " << xx << std::endl;
    EXPECT_NEAR((x2 - xx).norm(), 0.0, 1e-12);
}
