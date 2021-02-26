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

PoissonProblem::PoissonProblem(unsigned M,   unsigned N,
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

    for (unsigned i = 0; i <= M_; ++i) {
        x_grid_[i] = xa*std::cos(M_PI*i/(double)M_)+xb;
    }

    for (unsigned i = 0; i <= N_; ++i) {
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

    // fill right hand function
    for (unsigned i = 0; i <= M_; ++i) {
        for (unsigned j = 0; j <= N_; ++j) {
            ome_(i, j) = 32.*M_PI*M_PI * std::sin(4.*M_PI*x_grid_[i]) * std::sin(4.*M_PI*y_grid_[j]);
        }
    }
}
