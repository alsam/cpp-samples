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

#include "BartelsStewart.hpp"

/// @brief reduces the matrix to upper Hessenberg form by Householder similarity transformations
/// on @return the matrix `a` contains: the upper triangle and `(n+1)` column contains Hessenberg form
/// the low triangle and `(n+1)` row contains the history of transformations.
void BS::hshldr(Eigen::Ref<RowMatrixXd> a, size_t n)
{
    size_t i,j,l;
    double max, sum, s, p;

    for (l=0; l<=n-2; l++) {
        for (max=0.0, i=l+1; i<=n; i++)
            if (max < (s=std::fabs(a(i, l)))) max = s;
        if (max == 0) a(l, n+1) = a(n+1, l) = 0.0;
        else {
            for (sum=0.0, i=l+1; i<=n; i++) {
                s = (a(i, l) /= max);
                sum += s*s;
            }
            s = std::copysign(std::sqrt(sum), a(l+1, l));
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


// 67│ /// @brief from matrix `A` from `hshldr` builds ortogonal matrix `U`
// 68│ /// that reduces the matrix `A` to upper Hessenberg form
// 69│ /// the matrices `A` and `U` can be aliased i.e. be the same
// 70│ void BS::bckmlt(Eigen::Ref<RowMatrixXd> a,
// 71│                 Eigen::Ref<RowMatrixXd> u,
// 72│                 size_t n)
// 73│ {
// 74│     size_t i,j,l;
// 75│     double sum,p;
// 76│
// 77│     u(n,   n) = u(n-1, n-1) = 1.0;
// 78│     u(n-1, n) = u(n,   n-1) = 0.0;
// 79│     for (l=n-2; l>=0; l--) {
// 80├───────> if (a(n+1, l) != 0.0)
// 81│             for (j=l+1; j<=n; j++) {
// 82│                 for (sum=0.0, i=l+1; i<=n; i++)
// 83│                     sum += a(i, l)*u(i, j);
// 84│                 p = sum / a(n+1, l);
// 85│                 for (i=l+1; i<=n; i++)
// 86│                     u(i, j) -= a(i, l)*p;
// 87│             }
// 88│         for (i=l+1; i<=n; i++)
// 89│             u(i, l) = u(l, i) = 0.0;
// 90│         u(l, l) = 1.0;
// 91│     }
// 92│ }
// [?2004l#1  0x00007ffff7a3b862 in abort () from /usr/lib/libc.so.6
// [?2004h(gdb) up
// [?2004l#2  0x00007ffff7a3b747 in __assert_fail_base.cold () from /usr/lib/libc.so.6
// [?2004h(gdb) up
// [?2004l#3  0x00007ffff7a4a646 in __assert_fail () from /usr/lib/libc.so.6
// [?2004h(gdb) up
// [?2004l#4  0x000055555556f753 in Eigen::DenseCoeffsBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1
// >, 0, Eigen::OuterStride<-1> >, 1>::operator() (this=0x7fffffffe200, row=31, col=-1) at /usr/include/eigen
// 3/Eigen/src/Core/DenseCoeffsBase.h:364
// [?2004h(gdb) up
// [?2004l#5  0x0000555555573128 in BS::bckmlt (a=..., u=..., n=30) at /home/alsam/work/github/cpp-samples/ch
// ebyshev_pseudospectral/src/BartelsStewart.cpp:80
// [?2004h(gdb) p l
// [?2004l$1 = 18446744073709551615
// [?2004h(gdb) p n
// [?2004l$2 = 30
// [?2004h(gdb) p (signed)l
// [?2004l$3 = -1


/// @brief from matrix `A` from `hshldr` builds ortogonal matrix `U`
/// that reduces the matrix `A` to upper Hessenberg form
/// the matrices `A` and `U` can be aliased i.e. be the same
void BS::bckmlt(Eigen::Ref<RowMatrixXd> a,
                Eigen::Ref<RowMatrixXd> u,
                size_t n)
{
    size_t i,j;
    double sum,p;

    u(n,   n) = u(n-1, n-1) = 1.0;
    u(n-1, n) = u(n,   n-1) = 0.0;
    for (ssize_t l=n-2; l>=0; l--) {
        if (a(n+1, l) != 0.0)
            for (j=l+1; j<=n; j++) {
                for (sum=0.0, i=l+1; i<=n; i++)
                    sum += a(i, l)*u(i, j);
                p = sum / a(n+1, l);
                for (i=l+1; i<=n; i++)
                    u(i, j) -= a(i, l)*p;
            }
        for (i=l+1; i<=n; i++)
            u(i, l) = u(l, i) = 0.0;
        u(l, l) = 1.0;
    }
}

bool BS::schur(Eigen::Ref<RowMatrixXd> h,
               Eigen::Ref<RowMatrixXd> u,
               size_t nn, double eps)
{
    constexpr size_t MAXITER = 150;
    bool notlast,zero;
    size_t i,j,k,l,n=nn,na,jl,m,its;
    double rsum,test,hn=0.0,p,q,r,s,w,x,y,z;

    for (i=0; i<=n; i++) {
        jl = (i>1) ? (i-1) : 0;
        for (rsum=0.0, j=jl;j<=n;j++)
            rsum += std::fabs(h(i, j));
        if (hn<rsum) hn = rsum;
    }
    test = eps*hn;
    if (hn==0.0) return true;
nextw:
    if (n<=0) return true;
    its=0;
    na=n-1;

    for (;;) {
        // iterations
        l=n+1;
        do {
            l--;
            zero=(std::fabs(h(l, l-1)) <= test);
        } while (l!=1 && !zero);
        if (!zero) l=0;
        else h(l, l-1)=0.0;
        if (l>=na) {
            n=l-1;
            goto nextw;
        }
        x=h(n, n)/hn;
        y=h(na, na)/hn;
        r=(h(n, na)/hn)*(h(na, n)/hn);
        if (its>=MAXITER) return false;
        if (its%10 == 0 && its!=0) {
            y=(std::fabs(h(n, na))+std::fabs(h(na, na-1)))/hn;
            s=1.5*y; y *= y;
        }
        else { s=x+y; y=x*y-r; }
        its++;
        for (m=na-1;m>=l;m--) {
            x=h(m, m)/hn;
            r=h(m+1, m)/hn;
            z=h(m+1, m+1)/hn;
            p=x*(x-s)+y+r*(h(m, m+1)/hn);
            q=r*(x+z-s);
            r *= h(m+2, m+1)/hn;
            w=std::fabs(p)+std::fabs(q)+std::fabs(r);
            p /= w; q /= w; r /= w;
            if (m==l || std::fabs(h(m, m-1))*(std::fabs(q)+std::fabs(r))
                   <= std::fabs(p)*test)
                   break;
        }
        for (i=m+2; i<=n; i++) h(i, i-2)=0.0;
        for (i=m+3; i<=n; i++) h(i, i-3)=0.0;
        for (k=m; k<=na; k++) {
            notlast = (k!=na);
            if (k!=m) {
                p=h(k, k-1);
                q=h(k+1, k-1);
                if (notlast) r=h(k+2, k-1); else r=0.0;
                x=std::fabs(p)+std::fabs(q)+std::fabs(r);
                if (x==0.0) goto contin;
                p /= x; q /= x; r /= x;
            }
            s=sqrt(p*p+q*q+r*r);
            if (p<0.0) s = -s;
            if (k!=m) h(k, k-1) = -s*x;
            else if (l!=m) h(k, k-1) = -h(k, k-1);
            p += s; x = p/s; y = q/s; z = r/s; q /= p; r /= p;
            for (j=k;j<=nn;j++) {
                p=h(k, j)+q*h(k+1, j);
                if (notlast) {
                    p += r*h(k+2, j);
                    h(k+2, j) -= p*z;
                }
                h(k+1, j) -= p*y;
                h(k, j) -= p*x;
            }
            j = (k+3<n) ? k+3 : n;
            for (i=0;i<=j;i++) {
                p=x*h(i, k)+y*h(i, k+1);
                if (notlast) {
                    p += z*h(i, k+2);
                    h(i, k+2) -= p*r;
                }
                h(i, k+1) -= p*q;
                h(i, k) -= p;
            }
            for (i=0;i<=nn;i++) {
                p=x*u(i, k)+y*u(i, k+1);
                if (notlast) {
                    p += z*u(i, k+2);
                    u(i, k+2) -= p*r;
                }
                u(i, k+1) -= p*q;
                u(i, k) -= p;
            }
            contin:;
        }
    }
}

void BS::init_au(size_t m,
                   Eigen::Ref<RowMatrixXd> a,
                   Eigen::Ref<RowMatrixXd> u)
{
    hshldr(a, m);
    bckmlt(a, u, m);
    for (size_t i=0; i<m; i++) {
        a(i+1, i) = a(i, m+1);
    }

    if (!schur(a,u,m)) {
        throw std::logic_error("** SCHUR FAILED!");
    }
}

void BS::init_abuv(size_t m, size_t n,
                   Eigen::Ref<RowMatrixXd> a,
                   Eigen::Ref<RowMatrixXd> b,
                   Eigen::Ref<RowMatrixXd> u,
                   Eigen::Ref<RowMatrixXd> v)
{
    hshldr(a, m);
    bckmlt(a, u, m);
    for (size_t i=0; i<m; i++) {
        a(i+1, i) = a(i, m+1);
    }

    if (!schur(a,u,m)) {
        throw std::logic_error("** SCHUR FAILED!");
    }

    for (size_t i=0; i<=m; i++) {
        for (size_t j=i+1; j<=m; j++) {
            std::swap(a(i, j), a(j, i));
        }
    }

    hshldr(b,n);
    bckmlt(b,v,n);
    for (size_t i=0; i<n; i++) {
        b(i+1, i) = b(i, n+1);
    }

    if (!schur(b,v,n)) {
        throw std::logic_error("** SCHUR FAILED!");
    }
}

void BS::shrslv(size_t m, size_t n,
                Eigen::Ref<RowMatrixXd> a,
                Eigen::Ref<RowMatrixXd> b,
                Eigen::Ref<RowMatrixXd> c)
{
    size_t i,j,ib,ja,k,l,dk,dl,kk,ll;

    l=0;
    do {
        dl=1;
        if (l!=n && b(l+1, l)!=0.0) dl=2;
        ll=l+dl-1;
        if (l!=0) {
            for (j=l;j<=ll;j++)
                for (i=0;i<=m;i++)
                    for (ib=0;ib<l;ib++)
                        c(i, j) -= c(i, ib)*b(ib, j);
        }
        k=0;
        do {
            dk=1;
            if (k!=m && a(k, k+1)!=0.0) dk=2;
            kk=k+dk-1;
            if (k!=0) {
                for (i=k;i<=kk;i++)
                    for (j=l;j<=ll;j++)
                        for (ja=0;ja<k;ja++)
                            c(i, j) -= a(i, ja)*c(ja, j);
            }
            if (dk==2) {
                if (dl==2) {
                    Eigen::Matrix4d tt;
                    Eigen::Vector4d pp;
                    tt(0, 0) = a(k, k) + b(l, l);
                    tt(0, 1) = a(k, kk);
                    tt(0, 2) = b(ll, l);
                    tt(0, 3) = 0.0;
                    tt(1, 0) = a(kk, k);
                    tt(1, 1) = a(kk, kk) + b(l, l);
                    tt(1, 2) = 0.0;
                    tt(1, 3) = tt(0, 2);
                    tt(2, 0) = b(l, ll);
                    tt(2, 1) = 0.0;
                    tt(2, 2) = a(k, k) + b(ll, ll);
                    tt(2, 3) = tt(0, 1);
                    tt(3, 0) = 0.0;
                    tt(3, 1) = tt(2, 0);
                    tt(3, 2) = tt(1, 0);
                    tt(3, 3) = a(kk, kk) + b(ll, ll);

                    pp(0) = c(k, l);
                    pp(1) = c(kk, l);
                    pp(2) = c(k, ll);
                    pp(3) = c(kk, ll);

                    // sysslv(t,p,4);
                    pp = tt.lu().solve(pp);

                    c(k, l)   = pp(0);
                    c(kk, l)  = pp(1);
                    c(k, ll)  = pp(2);
                    c(kk, ll) = pp(3);

                } else {
                    Eigen::Matrix2d tt;
                    Eigen::Vector2d pp;
                    tt(0, 0) = a(k, k) + b(l, l);
                    tt(0, 1) = a(k, kk);
                    tt(1, 0) = a(kk, k);
                    tt(1, 1) = a(kk, kk) + b(l, l);

                    pp(0) = c(k, l);
                    pp(1) = c(kk, l);

                    // sysslv(t,p,2);
                    pp = tt.lu().solve(pp);

                    c(k, l)  = pp(0);
                    c(kk, l) = pp(1);
                }
            } else if (dl==2) {
                Eigen::Matrix2d tt;
                Eigen::Vector2d pp;
                tt(0, 0) = a(k, k)+b(l, l);
                tt(0, 1) = b(ll, l);
                tt(1, 0) = b(l, ll);
                tt(1, 1) = a(k, k)+b(ll, ll);

                pp(0) = c(k, l);
                pp(1) = c(k, ll);

                // sysslv(t,p,2);
                pp = tt.lu().solve(pp);

                c(k, l)  = pp(0);
                c(k, ll) = pp(1);
            } else {
                double t = a(k, k) + b(l, l);
                if (t==0.0) {
                    throw std::logic_error("** SHRSLV:unable to solve");
                }
                c(k, l) /= t;
            }
            k += dk;
        } while (k<=n);
        l += dl;
    } while (l<=n);
}

void BS::bs_solve(size_t m,
                  Eigen::Ref<RowMatrixXd> a,
                  Eigen::Ref<RowMatrixXd> u,
                  Eigen::Ref<RowMatrixXd> c,
                  Eigen::Ref<RowMatrixXd> rslt)
{
    bs_solve(m, m, a, a, u, u, c, rslt);
}

void BS::bs_solve(size_t m, size_t n,
                  Eigen::Ref<RowMatrixXd> a,
                  Eigen::Ref<RowMatrixXd> b,
                  Eigen::Ref<RowMatrixXd> u,
                  Eigen::Ref<RowMatrixXd> v,
                  Eigen::Ref<RowMatrixXd> c,
                  Eigen::Ref<RowMatrixXd> rslt)
{
    size_t i,j,k;

    for (j=0; j<=n; j++) {
        for (i=0; i<=m; i++) {
            a(i, m+1) = 0.0;
            for (k=0; k<=m; k++) {
                a(i, m+1) -= u(k, i)*c(k, j);
            }
        }
        for (i=0; i<=m; i++) {
            rslt(i, j) = a(i, m+1);
        }
    }
    for (i=0; i<=m; i++) {
        for (j=0; j<=n; j++) {
            b(n+1, j) = 0.0;
            for (k=0; k<=n; k++) {
                b(n+1, j) += rslt(i, k)*v(k, j);
            }
        }
        for (j=0; j<=n; j++) {
            rslt(i, j) = b(n+1, j);
        }
    }

    shrslv(m, n, a, b, rslt);

    for (j=0; j<=n; j++) {
        for (i=0; i<=m; i++) {
            a(i, m+1) = 0.0;
            for (k=0; k<=m; k++) {
                a(i, m+1) += u(i, k)*rslt(k, j);
            }
        }
        for (i=0; i<=m; i++) {
            rslt(i, j) = a(i, m+1);
        }
    }
    for (i=0; i<=m; i++) {
        for (j=0; j<=n; j++) {
            b(n+1, j) = 0.0;
            for (k=0; k<=n; k++) {
                b(n+1, j) += rslt(i, k)*v(j, k);
            }
        }
        for (j=0; j<=n; j++) {
            rslt(i, j) = b(n+1, j);
        }
    }
}
