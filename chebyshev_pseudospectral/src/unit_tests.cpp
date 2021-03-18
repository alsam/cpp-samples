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
#include "ChebyshevDifferentiate.hpp"
#include "PoissonProblem.hpp"
#include "FastCosineTransform.hpp"

template<size_t N>
void sysslv(double a[N][N], double b[N])
{
    constexpr size_t n = N;
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
    constexpr size_t M = 2;

    RowVectorXd x(M + 1), x_inv(M + 1);

    x     << 1.,  2., 3.;
    x_inv << 6., -2., 2.;

    FCT::cosfft1(M, x);

    EXPECT_DOUBLE_EQ((x - x_inv).norm(), 0.0);
}

TEST(cftSuite, test_cosfft12)
{
    constexpr size_t M = 32;
    auto y = [](double x) {return x*x*x*x - 1;}; /// \f$y = x^4 - 1\f$
    double x_min = -1., x_max = 1.;
    double xa = 0.5*(x_min-x_max);
    double xb = 0.5*(x_min+x_max);
    RowVectorXd x_grid(M + 1), f_vals1(M + 1), f_vals2(M + 1);
    for (size_t i = 0; i <= M; i++) {
        x_grid[i] = xa*std::cos(M_PI*i/(double)M)+xb;
    }

    for (size_t i = 0; i <= M; i++) {
        f_vals2[i] = f_vals1[i] = y(x_grid[i]);
    }

    FCT::cosfft1(M, f_vals1);
    FCT::cosfft1(M, f_vals1, FCT::TransformType::Inverse);

    // EXPECT_DOUBLE_EQ((f_vals1 - f_vals2).norm(), 0.0);
    constexpr double EPS = 1e-14;
    EXPECT_NEAR((f_vals1 - f_vals2).norm(), 0.0, EPS);
}

/// check that `.transpose()` and `.transposeInPlace()`
/// surely work with non-square matrices
TEST(eigenSuite, test_transpose)
{
    constexpr size_t M = 2;
    constexpr size_t N = 3;
    RowMatrixXd ma(M, N), maT;
    ma << 1., 2., 3.,
          4., 5., 6.;
    maT = ma.transpose();

    // std::cout << ma << std::endl;
    // std::cout << maT << std::endl;

    EXPECT_EQ(ma.rows(), maT.cols());
    EXPECT_EQ(maT.rows(), ma.cols());

    ma.transposeInPlace();

    EXPECT_EQ(ma.rows(), maT.rows());
    EXPECT_EQ(ma.cols(), maT.cols());

    // assertion failure if use
    // `Eigen::Ref<RowMatrixXd>` instead of plain `RowMatrixXd&` 
    auto transpose = [](RowMatrixXd& ma) {
        ma.transposeInPlace();
    };

    transpose(ma);

    EXPECT_EQ(ma.rows(), maT.cols());
    EXPECT_EQ(maT.rows(), ma.cols());
}

TEST(eigenSuite, test_unity)
{
    constexpr size_t M = 2;
    constexpr size_t N = 3;
    RowMatrixXd ma;

    ma = Eigen::MatrixXd::Identity(M + 1, N + 1);

    // std::cout << "ma:\n" << ma << std::endl;

    EXPECT_EQ(ma.rows(), M + 1);
    EXPECT_EQ(ma.cols(), N + 1);

    ma.transposeInPlace();

    EXPECT_EQ(ma.rows(), N + 1);
    EXPECT_EQ(ma.cols(), M + 1);

    EXPECT_EQ(ma(0, 0), 1.0);
    EXPECT_EQ(ma(1, 1), 1.0);
    EXPECT_EQ(ma(2, 2), 1.0);
    EXPECT_EQ(ma(1, 2), 0.0);
    EXPECT_EQ(ma(2, 1), 0.0);
}

TEST(cftSuite, test_cf2)
{
    constexpr size_t M = 2;
    constexpr size_t N = 4;

    RowMatrixXd m(M + 1, N + 1), m_inv(M + 1, N + 1), m1;

    m << 1., 2., 3., 1., 2.,
         4., 5., 6., 4., 5.,
         7., 8., 9., 7., 8.;
    m_inv <<  72.,    -0.87867965644035673,   9.,     -5.1213203435596428,  18.,
             -30., -4.4408920985006262e-16,  -6.,  4.4408920985006262e-16,  -6.,
              24.,    -0.29289321881345254,   3.,     -1.7071067811865479,   6.;

    m1 = m;

    FCT::cft2(M, N, m);
    FCT::cft2_with_transpose(M, N, m1);

    // std::cout << std::setprecision(17) << m << std::endl;

    constexpr double EPS = 1e-14;
    EXPECT_NEAR((m - m_inv).norm(), 0.0, EPS);
    EXPECT_NEAR((m - m1).norm(), 0.0, EPS);
}

TEST(bsSuite, test_gauss_elim)
{
    constexpr double EPS = 1e-12;

    constexpr size_t M = 4;

    // https://mxncalc.com/gaussian-elimination-calculator
    double A[M][M] = {{1.,  2., 3.,  4.},
                      {5.,  6., 8.,  5.},
                      {4., 15., 2., 12.},
                      {3.,  2., 5.,  6.},};
    double b[M] = {11., 12., 3., 7.,};
    double x[M] = { -7.127'853'881'278'5,
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
    EXPECT_NEAR((b2 - xx).norm(), 0.0, EPS);

    Eigen::Vector4d x2 = AA.lu().solve(bb);
    // std::cout << "x2: " << x2 << std::endl;
    // std::cout << "xx: " << xx << std::endl;
    EXPECT_NEAR((x2 - xx).norm(), 0.0, EPS);
}

TEST(ChebyshevDifferentiate, test_deriv1)
{
    constexpr size_t M = 32;

    auto y        = [](double x) {return x*x*x*x + sin(x);}; /// \f$y = x^4 + \sin x\f$
    auto y_deriv1 = [](double x) {return 4*x*x*x + cos(x);}; /// \f$y = 4x^3 + \cos x\f$

    RowVectorXd x_grid(M + 1), f_vals(M + 1), f_deriv_vals(M + 1);

    double x_min = -1., x_max = 1.;
    double xa = 0.5*(x_max-x_min);
    double xb = 0.5*(x_min+x_max);

    // std::cout << "xa: " << xa << " xb: " << xb << std::endl;

    for (size_t i = 0; i <= M; i++) {
        x_grid[i] = xa*std::cos(M_PI*i/(double)M)+xb;
    }

    // std::cout << "x_grid: " << x_grid << std::endl;

    for (size_t i = 0; i <= M; i++) {
        f_vals[i]       = y        (x_grid[i]);
        f_deriv_vals[i] = y_deriv1 (x_grid[i]);
    }

    // std::cout << "f_vals: [" << f_vals << "]\n";
    // std::cout << "f_deriv_vals: [" << f_deriv_vals << "]\n";

    FCT::cosfft1(M, f_vals, FCT::TransformType::Inverse);
    CS::spectral_differentiate(M, f_vals, f_vals, 2.0 / (x_max - x_min));
    FCT::cosfft1(M, f_vals);

    // std::cout << "f_vals: [" << f_vals << "]\n";
    // std::cout << "f_vals - f_deriv_vals: [" << f_vals - f_deriv_vals << "]\n";

    //EXPECT_DOUBLE_EQ((f_vals - f_deriv_vals).norm(), 0.0);

    constexpr double EPS = 1e-10;
    EXPECT_NEAR((f_vals - f_deriv_vals).norm(), 0.0, EPS);
}

TEST(PoissonProblem, test_homogeneous_boundary)
{
    auto sums_evens_odds = [](auto const& begin, auto const& end, size_t stride = 1)
                           -> std::pair<double, double> {
        double evens = 0.0, odds = 0.0;
        size_t counter = 0;
        for (auto it = begin; ;std::advance(it, stride)) {
            if (detail::is_even(counter++)) {
                evens += *it;
            } else {
                odds += *it;
            }
            if (it == end)
                break;
        }
        return {evens, odds};
    };

    auto check_evens_odds = [sums_evens_odds](auto const& ma, size_t row1, size_t col1, size_t row2, size_t col2, size_t stride = 1)
    {
        auto [evens, odds] = sums_evens_odds(&ma(row1, col1), &ma(row2, col2), stride);
        EXPECT_EQ(evens, 0);
        EXPECT_EQ(odds, 0);
    };

    constexpr size_t M = 4;

    double A[M + 1][M + 1] = { {1.,  2., 3.,  4.,  8.,},
                               {5.,  6., 8.,  5.,  7.,},
                               {3.,  5., 7.,  2.,  1.,},
                               {4., 15., 2., 12., 11.,},
                               {3.,  2., 5.,  6.,  5.,}, };

    RowMatrixXd AA(M + 1, M + 1), BB = RowMatrixXd::Zero(M + 1, M + 1);

    for (size_t i = 0; i <= M; ++i) {
        for (size_t j = 0; j <= M; ++j) {
            AA(i, j) = A[i][j];
        }
    }

    // std::cout << "AA: [\n" << AA << "]\n";
    CS::homogeneous_boundary(M, M, AA, BB);
    // std::cout << "BB: [\n" << BB << "]\n";
    // std::cout << "AA: [\n" << AA << "]\n";
    CS::homogeneous_boundary(M, M, AA, AA);
    // std::cout << "AA: [\n" << AA << "]\n";
    EXPECT_DOUBLE_EQ((AA - BB).norm(), 0.0);

    constexpr size_t N = 2*M;
    RowMatrixXd CC(M + 1, N + 1);
    for (size_t i = 0; i <= M; ++i) {
        for (size_t j = 0; j <= N; ++j) {
            CC(i, j) = A[i][j % 2];
        }
    }
    // std::cout << "CC: [\n" << CC << "]\n";
    CS::homogeneous_boundary(M, N, CC, CC);

    // std::cout << "CC: [\n" << CC << "]\n";
    auto [evens, odds] = sums_evens_odds(&CC(2, 0), &CC(2, 8));
    EXPECT_EQ(evens, 0);
    EXPECT_EQ(odds, 0);
    check_evens_odds(CC, 2, 0, 2, 8);
    // std::cout << "evens: " << evens << " odds: " << odds << std::endl;
    auto [evens1, odds1] = sums_evens_odds(&CC(0, 3), &CC(4, 3), N + 1);
    check_evens_odds(CC, 0, 3, 4, 3, N + 1);
    // std::cout << "evens: " << evens1 << " odds: " << odds1 << std::endl;
    EXPECT_EQ(evens1, 0);
    EXPECT_EQ(odds1, 0);
}
