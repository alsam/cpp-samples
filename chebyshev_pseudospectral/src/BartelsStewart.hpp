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
 *  @brief Bartels-Stewart algorithm for solving matrix equations of the form \f$A X + X B = C\f$ where \f$A,B,C,X\f$ are all matrices.
 *
 */

///  @details This algorithm consists of 4 parts:
///
/// \f{enumerate}{
///   \item Similarity transformation of the matrices $A$ and $B$ to the real Schur form with
/// 	orthogonal matrices $U$ and $Q$. Let us determine the order that
/// 	matrix $A$ transforms to low Schur form, $B$ --
/// 	to the upper one.
///   \begin{equation}
///         \tilde{A} = U^{-1} A U, \qquad \tilde{B} = Q^{-1} B Q
///   \end{equation}
///
///   \item RHS transformation
///
///   \begin{equation}
///         \tilde{b} = U^{-1} b Q
///   \end{equation}
///
///   \item Solution of the transformed system by matrix $Y$
///
///   \begin{equation}
///         \tilde{A} Y + Y \tilde{B} = \tilde{b}
///   \end{equation}
///
///     This is a linear system with block triangular form, diagonal blocks have order $1$, $2$ and $4$.
///     Thus we are to solve series af linear systems of orders $1$ (trivial case), $2$ and $4$.
///     All of these systems can be solved independently in parallel.
///
///   \item Back substitution
///
///   \begin{equation}
/// 	X = U Y Q^{-1}
///   \end{equation}
///
/// \f}

/* Legacy interface

void hshldr(double **a, int n);
void bckmlt(double **a, double **u, int n);
int schur(double **h, double **u, int nn, double eps);
void initau(int m, int n, double **a, double **b, double **u, double **v);
void sysslv(double a[5][5], double b[5], int n);
void shrslv(double **a, double **b, double **c, int m, int n);
void bs_solve(int m, int n, double **a, double **b,
    double **u, double **v, double **c, double **res);
*/

namespace BS /// BS stands for \f$\boldsymbol{B}\f$artels-\f$\boldsymbol{S}\f$tewart
{
    /// @brief reduces the matrix to upper Hessenberg form by Householder similarity transformations
    /// see [Reducing a Matrix to Hessenberg Form](https://www.ams.org/journals/mcom/1969-23-108/S0025-5718-1969-0258255-3/S0025-5718-1969-0258255-3.pdf)
    /// \param[in,out] a - the matrix `A`
    /// \param[in] n - the matrix dimension \f$(N + 1) \star (N + 1)\f$
    void hshldr(Eigen::Ref<RowMatrixXd> a, size_t n);

    /// @brief from matrix `A` from `hshldr` builds ortogonal matrix `U`
    /// that reduces the matrix `A` to upper Hessenberg form
    /// the matrices `A` and `U` can be aliased i.e. be the same
    /// \param[in]  a - the matrix `A` from `hshldr`
    /// \param[out] u - the ortogonal matrix `U` that reduces `A` to upper Hessenberg form
    /// \param[in]  n - the matrices dimension \f$(N + 1) \star (N + 1)\f$
    void bckmlt(Eigen::Ref<RowMatrixXd> a,
                Eigen::Ref<RowMatrixXd> u,
                size_t n);

    bool schur(Eigen::Ref<RowMatrixXd> h,
               Eigen::Ref<RowMatrixXd> u,
               size_t n, double eps = 1.0e-30);

    void init_au(size_t m,
                 Eigen::Ref<RowMatrixXd> a,
                 Eigen::Ref<RowMatrixXd> u);

    void init_abuv(size_t m, size_t n,
                   Eigen::Ref<RowMatrixXd> a,
                   Eigen::Ref<RowMatrixXd> b,
                   Eigen::Ref<RowMatrixXd> u,
                   Eigen::Ref<RowMatrixXd> v);

    void shrslv(size_t m, size_t n,
                Eigen::Ref<RowMatrixXd> a,
                Eigen::Ref<RowMatrixXd> b,
                Eigen::Ref<RowMatrixXd> c);

    void bs_solve(size_t m,
                  Eigen::Ref<RowMatrixXd> a,
                  Eigen::Ref<RowMatrixXd> u,
                  Eigen::Ref<RowMatrixXd> c,
                  Eigen::Ref<RowMatrixXd> res);

    void bs_solve(size_t m, size_t n,
                  Eigen::Ref<RowMatrixXd> a,
                  Eigen::Ref<RowMatrixXd> b,
                  Eigen::Ref<RowMatrixXd> u,
                  Eigen::Ref<RowMatrixXd> v,
                  Eigen::Ref<RowMatrixXd> c,
                  Eigen::Ref<RowMatrixXd> res);
};
