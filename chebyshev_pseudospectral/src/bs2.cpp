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
            if (max < (s=std::fabs(a(i, l)))) max = s;
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

void BS::bckmlt(Eigen::Ref<RowMatrixXd> a, Eigen::Ref<RowMatrixXd> u, unsigned n)
{
    unsigned i,j,l;
    double sum,p;

    u(n,   n) = u(n-1, n-1) = 1.0;
    u(n-1, n) = u(n,   n-1) = 0.0;
    for (l=n-2; l>=0; l--) {
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

bool BS::schur(Eigen::Ref<RowMatrixXd> h, Eigen::Ref<RowMatrixXd> u, unsigned nn, double eps)
{
    constexpr unsigned MAXITER = 150;
    bool notlast,zero;
    unsigned i,j,k,l,n=nn,na,jl,m,its;
    double rsum,test,hn=0.0,p,q,r,s,w,x,y,z;

    for (i=0;i<=n;i++) {
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
        for (i=m+2;i<=n;i++) h(i, i-2)=0.0;
        for (i=m+3;i<=n;i++) h(i, i-3)=0.0;
        for (k=m;k<=na;k++) {
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

void BS::initau(unsigned m, unsigned n,
                Eigen::Ref<RowMatrixXd> a,
                Eigen::Ref<RowMatrixXd> b,
                Eigen::Ref<RowMatrixXd> u,
                Eigen::Ref<RowMatrixXd> v)
{
    hshldr(a, m);
    bckmlt(a, u, m);
    for (unsigned i=0; i<m; i++) {
        a(i+1, i) = a(i, m+1);
    }

    if (!schur(a,u,m)) {
        throw std::logic_error("** SCHUR FAILED!");
    }

    for (unsigned i=0; i<=m; i++) {
        for (unsigned j=i+1; j<=m; j++) {
            std::swap(a(i, j), a(j, i));
        }
    }

    hshldr(b,n);
    bckmlt(b,v,n);
    for (unsigned i=0; i<n; i++) {
        b(i+1, i) = b(i, n+1);
    }

    if (!schur(b,v,n)) {
        throw std::logic_error("** SCHUR FAILED!");
    }
}

void BS::sysslv(double a[5][5], double b[5], unsigned n)
{
    double biga,save; int i,j,k,imax;

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

void BS::shrslv(Eigen::Ref<RowMatrixXd> a, Eigen::Ref<RowMatrixXd> b, Eigen::Ref<RowMatrixXd> c, unsigned m, unsigned n)
{
    unsigned i,j,ib,ja,k,l,dk,dl,kk,ll;
    double t[5][5],p[5];

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
                    t[0][0]=a(k, k)+b(l, l);
                    t[0][1]=a(k, kk);
                    t[0][2]=b(ll, l);
                    t[0][3]=0.0;
                    t[1][0]=a(kk, k);
                    t[1][1]=a(kk, kk)+b(l, l);
                    t[1][2]=0.0;
                    t[1][3]=t[0][2];
                    t[2][0]=b(l, ll);
                    t[2][1]=0.0;
                    t[2][2]=a(k, k)+b(ll, ll);
                    t[2][3]=t[0][1];
                    t[3][0]=0.0;
                    t[3][1]=t[2][0];
                    t[3][2]=t[1][0];
                    t[3][3]=a(kk, kk)+b(ll, ll);

                    p[0]=c(k, l);
                    p[1]=c(kk, l);
                    p[2]=c(k, ll);
                    p[3]=c(kk, ll);

                    sysslv(t,p,4);

                    c(k, l)   = p[0];
                    c(kk, l)  = p[1];
                    c(k, ll)  = p[2];
                    c(kk, ll) = p[3];

                } else {
                    Eigen::Matrix2d tt;
                    Eigen::Vector2d pp;
                    t[0][0]=a(k, k)+b(l, l);
                    t[0][1]=a(k, kk);
                    t[1][0]=a(kk, k);
                    t[1][1]=a(kk, kk)+b(l, l);

                    p[0]=c(k, l);
                    p[1]=c(kk, l);

                    sysslv(t,p,2);

                    c(k, l)  = p[0];
                    c(kk, l) = p[1];
                }
            } else if (dl==2) {
                t[0][0]=a(k, k)+b(l, l);
                t[0][1]=b(ll, l);
                t[1][0]=b(l, ll);
                t[1][1]=a(k, k)+b(ll, ll);

                p[0]=c(k, l);
                p[1]=c(k, ll);

                sysslv(t,p,2);

                c(k, l)  = p[0];
                c(k, ll) = p[1];
            } else {
                t[0][0]=a(k, k)+b(l, l);
                if (t[0][0]==0.0) {
                    throw std::logic_error("** SHRSLV:unable to solve");
                }
                c(k, l) /= t[0][0];
            }
            k += dk;
        } while (k<=n);
        l += dl;
    } while (l<=n);
}

void BS::bs_solve(unsigned m, unsigned n,
                  Eigen::Ref<RowMatrixXd> a,
                  Eigen::Ref<RowMatrixXd> b,
                  Eigen::Ref<RowMatrixXd> u,
                  Eigen::Ref<RowMatrixXd> v,
                  Eigen::Ref<RowMatrixXd> c,
                  Eigen::Ref<RowMatrixXd> rslt)
{
    unsigned i,j,k;

    for (j=0;j<=n;j++) {
        for (i=0;i<=m;i++) {
            a(i, m+1)=0.0;
            for (k=0;k<=m;k++) {
                a(i, m+1) -= u(k, i)*c(k, j);
            }
        }
        for (i=0;i<=m;i++) {
            rslt(i, j)=a(i, m+1);
        }
    }
    for (i=0;i<=m;i++) {
        for (j=0;j<=n;j++) {
            b(n+1, j)=0.0;
            for (k=0;k<=n;k++) {
                b(n+1, j) += rslt(i, k)*v(k, j);
            }
        }
        for (j=0;j<=n;j++) {
            rslt(i, j)=b(n+1, j);
        }
    }

    shrslv(a,b,rslt,m,n);

    for (j=0;j<=n;j++) {
        for (i=0;i<=m;i++) {
            a(i, m+1)=0.0;
            for (k=0;k<=m;k++) {
                a(i, m+1) += u(i, k)*rslt(k, j);
            }
        }
        for (i=0;i<=m;i++) {
            rslt(i, j)=a(i, m+1);
        }
    }
    for (i=0;i<=m;i++) {
        for (j=0;j<=n;j++) {
            b(n+1, j)=0.0;
            for (k=0;k<=n;k++) {
                b(n+1, j) += rslt(i, k)*v(j, k);
            }
        }
        for (j=0;j<=n;j++) {
            rslt(i, j)=b(n+1, j);
        }
    }
}
