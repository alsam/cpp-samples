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

#include "bs2.hpp"

/**  
 *  @brief Bartels-Stewart algorithm for solving matrix equations of the form \f$A X + X B = C\f$ where \f$A,B,C,X\f$ are all matrices. 
 *  
 */

void BS::hshldr(Eigen::Ref<RowMatrixXd> a, unsigned n)
{
    unsigned i,j,l;
    double max, sum, s, p;

    for (l=0; l<=n-2; l++) {
        for (max=0.0, i=l+1; i<=n; i++)
            if (max < (s=fabs(a(i, l)))) max = s;
        if (max == 0) a(l, n+1) = a(n+1, l) = 0.0;
        else {
            for (sum=0.0, i=l+1; i<=n; i++) {
                s = (a(i, l) /= max);
                sum += s*s;
            }
            s = std::sqrt(sum);
            s = (a(l+1, l) < 0.0) ? -s : s; // ???
            a(l, n+1) = -max*s;
            p = (a(l+1, l) += s);
            a(n+1, l) = p*s;
            for (j=l+1; j<=n; j++) {
                for (sum=0.0, i=l+1; i<=n; i++)
                    sum += a(i, l)*a(i, j);
                p = sum / a(n+1, l);
                for (i=l+1; i<=n; i++)
                    a(i, j) -= a(i, l)*p;
            }
            for (i=0; i<=n; i++) {
                for (sum=0.0, j=l+1; j<=n; j++)
                    sum += a(i, j)*a(j, l);
                p = sum / a(n+1, l);
                for (j=l+1; j<=n; j++)
                    a(i, j) -= p*a(j, l);
            }
        }
    }
    a(n-1, n+1) = a(n, n-1);
}

