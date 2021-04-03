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

/** @brief C++ implementation for a model Dirichlet task.
  *
  * @details The model Dirichlet task for Poisson equation
  *          in a square region with zero boundary conditions. 
  *
  * \f{alignat}{{2}
  *      \Delta \Psi(x,y) &= -\omega(x,y) &&\quad |x|,\,|y| < 1, \\
  *      \Psi(x,y) &= 0 &&\quad |x| = 1,\,|y| = 1
  * \f}
  *
  * for \f$\omega(x,y) = 32\pi^2\sin 4\pi x \sin 4\pi y\f$
  */
class PoissonProblem
{

public:

    /// @brief a constructor initializes the Posson solver
    /// 
    /// @param M      grid size along `x` direction
    /// @param N      grid size along `y` direction
    /// @param x_min  minimum `x` value, typically it is -1
    /// @param x_max  maximum `x` value, typically it is +1
    /// @param y_min  minimum `y` value, typically it is -1
    /// @param y_max  maximum `y` value, typically it is +1
    PoissonProblem(size_t M,     size_t N,
                   double x_min, double x_max,
                   double y_min, double y_max,
                   bool verbose = false);

    void solve();

    void compute_residual();

    struct Boundary { // SOA - Structure of Arrays
        RowVectorXd left_, down_, right_, up_;

        Boundary(size_t M, size_t N)
            : left_  (M + 1),
              down_  (N + 1),
              right_ (M + 1),
              up_    (N + 1)
        {}
    };

public:

    RowMatrixXd second_order_operator(size_t m);

private:

    void RHS(Eigen::Ref<RowMatrixXd> omega);

    // FIXME does it really needed? check it for different corner cases 
    void homogenize_bc(Eigen::Ref<RowMatrixXd> omega, Eigen::Ref<RowMatrixXd> spectral_operator);

private:

    /// be `verbose`
    bool verbose_;

    /// grid size along `x` direction
    size_t M_;

    /// grid size along `y` direction
    size_t N_;

    /// \f$x\f$ distributed as
    /// \f$x_i=\cos\pi i/M\qquad 0\le i\le M\f$
    RowVectorXd x_grid_;

    /// \f$y\f$ distributed as
    /// \f$y_j=\cos\pi j/N\qquad 0\le j\le N\f$
    RowVectorXd y_grid_;

    /// \f$\omega\f$ is RHS for the problem
    RowMatrixXd ome_;

    /// \f$\Psi\f$ is the unknown
    RowMatrixXd psi_;

    /// \f$\partial^2/\partial{x}^2\f$ second derivative
    RowMatrixXd second_derivative_respect_x_;

    /// \f$\partial^2/\partial{y}^2\f$ second derivative
    RowMatrixXd second_derivative_respect_y_;

    RowMatrixXd u_, v_;

    Boundary border_;
};
