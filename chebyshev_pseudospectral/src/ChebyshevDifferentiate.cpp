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

#include "ChebyshevDifferentiate.hpp"

/// 
/// as \f$T_n(\cos\theta)=\cos n\theta\f$, then
/// 
/// \f[
/// 	\frac{T'_{n+1}(x)}{n+1}-\frac{T'_{n-1}(x)}{n-1}=\frac2{c_n}T_n(x)
/// 	\quad(n>=0)
/// \f]
/// 
/// where \f$c_0=2,\,c_n=1\,(n\le1)\f$ and \f$T'_0=T'_{-1}=0\f$, implies if
/// 
/// \f[
/// 	\frac d{dx}\sum^N_{n=0}a_nT_n(x)=\sum^N_{n=0}b_nT_n(x)
/// \f]
///
/// then
/// 
/// \f{align*}{
/// 	\sum^N_{n=0}a_nT'_n(x)
/// 		&=\sum^N_{n=0}c_nb_n\left[T_n(x)
/// 		\frac{T'_{n+1}(x)}{n+1}-\frac{T'_{n-1}(x)}{n-1}\right]\\
/// 		&=\sum^{N+1}_{n=0}[c_{n-1}b_{n-1}-b_{n+1}]T'_n(x)/n
/// \f}
/// 
/// equating \f$T'_n(x)\f$ for \f$n=1,\dots,N+1\f$
/// we derive recurrent equations for Chebyshev polynomials derivatives
/// 
/// \f{alignat*}{{2}
///        c_{n-1}b_{n-1}-b_{n+1} &= 2na_n &&\quad(1\le n\le N)\\
///                           b_n &=0      &&\quad(n\ge N)
/// \f}
///
/// if approximate solution is at each collocation point \f$(x_j)\f$
///
/// \f[
///     u(x,t) = \sum_{k=1}^{N+1}a_{k}T_{k-1}(x)
/// \f]
///
/// then, in particular, spatial derivatives can be defined directly in terms
/// of undifferentiated Chebyshev polynomials, i.e.
///
/// \f[
///     \frac{\partial{u}}{\partial{x}} = \sum_{k=1}^{N}a_{k}^{(1)}T_{k-1}(x)
/// \f]
///
/// and
///
/// \f[
///     \frac{\partial^2{u}}{\partial{x^2}} = \sum_{k=1}^{N-1}a_{k}^{(2)}T_{k-1}(x)
/// \f]
///
/// Specifically, the following recurrence relations permit all
/// the \f$a_k^{(1)}, a_k^{(2)}\f$ coefficients to be obtained in \f$O(N)\f$ operations
///
/// \f{alignat*}{{2}
///     a_k^{(1)} &= a_{k+2}^{(1)} + 2k a_{k+1}       &&\quad(2\le k\le N)\\
///     a_k^{(2)} &= a_{k+2}^{(2)} + 2k a_{k+1}^{(1)} &&\quad(2\le k\le N-1)\\
/// \f}
///
/// and \f$\qquad a_1^{(1)} = 0.5 a_3^{(1)} + 2k a_2, \quad a_1^{(2)} = 0.5 a_3^{(2)} + 2k a_2^{(1)}\f$
///
/// and \f$\qquad a_{N+1}^{(1)} = a_{N+2}^{(1)} = 0, \quad a_{N}^{(2)} = a_{N+1}^{(2)} = 0\f$

void CS::spectral_differentiate(size_t n,
                                Eigen::Ref<RowVectorXd> in,
                                Eigen::Ref<RowVectorXd> out,
                                double span)
{
    double temp1 = in[n-1],
           temp2 = in[n-2];
    
    out[n-1] = 2.0 *  n    * in[n] * span;
    out[n-2] = 2.0 * (n-1) * temp1 * span;
    for (size_t j = n-3; j>0; j--) {
        temp1  = in[j];
        out[j] = out[j+2] + 2.0 * (j+1) * temp2 * span;
        temp2  = temp1;
    }
    out[0] = 0.5 * out[2] + temp2 * span;
    out[n] = 0.0;
}

/// as \f$T_n(\cos\theta)=\cos n\theta\f$, then
///
/// \f[
/// 	\frac{T'_{n+1}(x)}{n+1}-\frac{T'_{n-1}(x)}{n-1}=\frac2{c_n}T_n(x)
/// 	\quad(n>=0)
/// \f]
///
///  From boundary conditions and Chebyshev polynomials traits \f$T_n(\pm1)=(\pm1)^n\f$ we derive
/// \f{alignat}{{3}
/// 	\sum^N_{p=0 \quad p\equiv0\pmod2} a_{mp} &=
/// 	\sum^N_{p=1 \quad p\equiv1\pmod2} a_{mp} &= 0 &&
/// 	\qquad 0\le m\le M\\
/// 	\sum^N_{q=0 \quad q\equiv0\pmod2} a_{qn} &=
/// 	\sum^N_{q=1 \quad q\equiv1\pmod2} a_{qn} &= 0 &&
/// 	\qquad 0\le n\le N
/// \f}
void CS::homogeneous_boundary(size_t m, size_t n,
                              Eigen::Ref<RowMatrixXd> in,
                              Eigen::Ref<RowMatrixXd> out)
{
    if (out != in) {
        out = in;
    }

    double evens, odds;
    for (size_t i=0; i<=m-2; ++i) {
        evens = odds = 0.0;
        for (size_t j=1; j<n-2; j+=2) {
            odds  -= out(i, j);
            evens -= out(i, j+1);
        }
        out(i, n-1) = odds;
        out(i, n)   = evens - out(i, 0);
    }

    for (size_t i=0; i<=m-2; ++i) {
        evens = odds = 0.0;
        for (size_t j=1; j<n-2; j+=2) {
            odds  -= out(j,   i);
            evens -= out(j+1, i);
        }
        out(n-1, i) = odds;
        out(n,   i) = evens - out(0, i);
    }

    evens = odds = 0.0;
    for (size_t j=1; j<n-2; j+=2) {
        odds  -= out(n-1, j);
        evens -= out(n-1, j+1);
    }
    out(n-1, n-1) = odds;
    out(n-1, n)   = evens - out(n-1, 0);

    evens = odds = 0.0;
    for (size_t i=0; i<m; i+=2) {
        odds  -= out(i, n-1);
        evens -= out(i, n);
    }
    out(n, n-1) = odds;
    out(n, n)   = evens;
}
