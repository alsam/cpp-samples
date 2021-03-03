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

#include "PoissonProblem.hpp"
#include "ChebyshevDifferentiate.hpp"

PoissonProblem::PoissonProblem(size_t M,     size_t N,
                               double x_min, double x_max,
                               double y_min, double y_max,
                               bool verbose)
: verbose_(verbose),
  M_(M), N_(N),
  x_grid_(M + 1),
  y_grid_(N + 1),
  ome_   (M + 1, N + 1),
  psi_   (M + 1, N + 1),
  border_(M, N)
{
    double xa = 0.5*(x_min-x_max);
    double xb = 0.5*(x_min+x_max);
    double ya = 0.5*(y_min-y_max);
    double yb = 0.5*(y_min+y_max);

    for (size_t i = 0; i <= M_; ++i) {
        x_grid_[i] = xa*std::cos(M_PI*i/(double)M_)+xb;
    }

    for (size_t i = 0; i <= N_; ++i) {
        y_grid_[i] = ya*std::cos(M_PI*i/(double)N_)+yb;
    }

    if (verbose_) {
        std::cout << "x_grid: [" << x_grid_ << "]\n";
        std::cout << "y_grid: [" << y_grid_ << "]\n";
    }

    // zero boundary conditions
    border_.left_  = RowVectorXd::Zero(M_ + 1);
    border_.down_  = RowVectorXd::Zero(N_ + 1);
    border_.right_ = RowVectorXd::Zero(M_ + 1);
    border_.up_    = RowVectorXd::Zero(N_ + 1);

    cosfft1(M_, border_.left_,  false);
    cosfft1(N_, border_.down_,  false);
    cosfft1(M_, border_.right_, false);
    cosfft1(N_, border_.up_,    false);

    // fill right hand function
    for (size_t i = 0; i <= M_; ++i) {
        for (size_t j = 0; j <= N_; ++j) {
            ome_(i, j) = 32.*M_PI*M_PI * std::sin(4.*M_PI*x_grid_[i])
                                       * std::sin(4.*M_PI*y_grid_[j]);
        }
    }
}

void PoissonProblem::generate_matrix(size_t n, Eigen::Ref<RowMatrixXd> ma)
{
    // TODO use MatrixXd::Identity(rows,cols)
    for (size_t i=0; i<=n; ++i) {
        for (size_t j=0; j<=n; ++j) {
            ma(i, j) = detail::id(i, j);
        }
    }
    laplacian(n, ma, ma);
}

void PoissonProblem::homogeneous_boundary(size_t n,
                                          Eigen::Ref<RowMatrixXd> in,
                                          Eigen::Ref<RowMatrixXd> out)
{
    for (size_t i=0; i<=n; i++) {
        double evens = 0.0, odds = 0.0;
        for (size_t j=1; j<=n-2; j+=2) {
            odds  -= in(i, j);
            evens -= in(i, j+1);
        }

        if (out != in) {
            for (size_t j=0; j<=n-2; j++) {
                out(i, j) = in(i, j);
            }
        }

        out(i, n-1) = odds;
        out(i, n)   = evens-in(i, 0);
/*
        out(i, 0) = out(i, 1) = 0.0;
*/
    }
}

void PoissonProblem::laplacian(size_t n,
                               Eigen::Ref<RowMatrixXd> in,
                               Eigen::Ref<RowMatrixXd> out)
{
    homogeneous_boundary(n, in, out);
    for (size_t i = 0; i <= n; ++i) {
        spectral_differentiate(n, out.row(i), out.row(i));
        spectral_differentiate(n, out.row(i), out.row(i));
    }
}

void PoissonProblem::RHS(size_t n,
                         Eigen::Ref<RowMatrixXd> ma,
                         Boundary const& boundary)
{
}
