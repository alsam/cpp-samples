// -*- C++ -*-

// The MIT License (MIT)
//
// Copyright (c) 2019 Alexander Samoilov
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

#include <cmath>
#include <algorithm>
#include <iostream>
#include "math.hpp"

void cosfft1(size_t n,
             Eigen::Ref<Eigen::RowVectorXd> data,
             bool inverse)
{
    size_t  m;
    size_t  mmax, istep, i1, i2, i3, i4;
    double  wr, wi, wpr, wpi, wtemp, theta,
            y1, y2, tempr, tempi, h1r, h1i, h2r, h2i,
            temp, sum, first, last;

    temp = data[n];
    if (inverse) {
        first=0.5*data[0];
        last=0.5*temp;
    }
    sum = 0.0;
    for (size_t j=0; j<=n; j+=2) sum += data[j];
    for (size_t j=1; j<=n; j+=2) sum -= data[j];
    data[n] = sum;

    wtemp = std::sin(0.5*(theta=M_PI/n));
    wpr = -2.0*wtemp*wtemp;
    wpi = std::sin(theta); m=n >> 1;
    wr = 1.0;
    wi = 0.0;
    sum = data[0];
    for (size_t j=1; j<=m; j++) {
        wr = (wtemp=wr)*wpr-wi*wpi+wr;
        wi = wi*wpr+wtemp*wpi+wi;
        y1 = 0.5*(data[j]+data[n-j]);
        y2 = data[j]-data[n-j];
        data[j] = y1-wi*y2;
        data[n-j] = y1+wi*y2;
        sum += wr*y2;
    }
    size_t j = 1;
    for (size_t i=1; i<=n; i+=2) {
        if (j > i) {
            std::swap(data[j-1],data[i-1]);
            std::swap(data[j],data[i]);
        }
        m=n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax) {
        istep = mmax << 1;
        theta = 2.0*M_PI/mmax;
        wtemp = std::sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi = std::sin(theta);
        wr = 1.0; wi = 0.0;
        for (size_t m=1; m<=mmax; m+=2) {
            for (size_t i=m;i<=n;i+=istep) {
                j = i+mmax;
                tempr = wr*data[j-1]-wi*data[j];
                tempi = wr*data[j]+wi*data[j-1];
                data[j-1] = data[i-1]-tempr;
                data[j] = data[i]-tempi;
                data[i-1] += tempr;
                data[i] += tempi;
            }
            wr = (wtemp=wr)*wpr-wi*wpi+wr;
            wi = wi*wpr+wtemp*wpi+wi;
        }
        mmax = istep;
    }
    theta = M_PI/(n >> 1);
    wtemp = std::sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp; wi=wpi=sin(theta); wr=wpr+1;

    data[0] += data[1];
    m=n >> 2;

    for (size_t i=1; i<=m; i++) {
        i2=(i1=i << 1)+1; i4=(i3=n-i2+1)+1;
        h1r=0.5*(data[i1]+data[i3]);
        h1i=0.5*(data[i2]-data[i4]);
        h2r=0.5*(data[i2]+data[i4]);
        h2i=0.5*(data[i3]-data[i1]);
        data[i1]=h1r+wr*h2r-wi*h2i;
        data[i2]=h1i+wr*h2i+wi*h2r;
        data[i3]=h1r-wr*h2r+wi*h2i;
        data[i4] = -h1i+wr*h2i+wi*h2r;
        wr=(wtemp=wr)*wpr-wi*wpi+wr;
        wi=wi*wpr+wtemp*wpi+wi;
    }
    data[1] = sum;
    for (size_t j=3; j<=n; j+=2) {
        sum += data[j];
        data[j] = sum;
    }

    for (size_t j=0; j<n; j+=2) {
        data[j    ] += temp;
        data[j + 1] -= temp;
    }

    if (inverse) {
        for (size_t j=0;j<n;j+=2) {
            data[j    ] = (data[j    ]-(first+last))*2.0/n;
            data[j + 1] = (data[j + 1]-(first-last))*2.0/n;
        }
        data[n] = (data[n]-(first+last))*2.0/n;
        data[0] *= 0.5;
        data[n] *= 0.5;
    }
}

void cft2(size_t m, size_t n,
          Eigen::Ref<RowMatrixXd> data,
          bool inverse)
{
    // data is (M+1)x(N+1) matrix
    for (size_t i=0; i<=m; ++i) {
        cosfft1(n, data.row(i), inverse);
    }

    data.transposeInPlace();

    // data is (N+1)x(M+1) matrix
    // after `.transposeInPlace()`
    for (size_t i=0; i<=n; ++i) {
        cosfft1(m, data.row(i), inverse);
    }
}
