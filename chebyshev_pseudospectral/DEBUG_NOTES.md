```c++
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
// [?2004l#5  0x0000555555573128 in BS::bckmlt (a=..., u=..., n=30) at chebyshev_pseudospectral/src/BartelsStewart.cpp:80
// [?2004h(gdb) p l
// [?2004l$1 = 18446744073709551615
// [?2004h(gdb) p n
// [?2004l$2 = 30
// [?2004h(gdb) p (signed)l
// [?2004l$3 = -1
```

```c++
// 140│ bool BS::schur(Eigen::Ref<RowMatrixXd> h,
// 141│                Eigen::Ref<RowMatrixXd> u,
// 142│                size_t nn, double eps)
// 143│ {
// 144│     constexpr size_t MAXITER = 150;
// 145│     bool notlast,zero;
// 146│     size_t i,j,k,l,n=nn,na,jl,m,its;
// 147│     double rsum,test,hn=0.0,p,q,r,s,w,x,y,z;
// 148│
// 149│     for (i=0; i<=n; i++) {
// 150│         jl = (i>1) ? (i-1) : 0;
// 151│         for (rsum=0.0, j=jl;j<=n;j++)
// 152│             rsum += std::fabs(h(i, j));
// 153│         if (hn<rsum) hn = rsum;
// 154│     }
// 155│     test = eps*hn;
// 156│     if (hn==0.0) return true;
// 157│ nextw:
// 158│     if (n<=0) return true;
// 159│     its=0;
// 160│     na=n-1;
// 161│
// 162│     for (;;) {
// 163│         // iterations
// 164│         l=n+1;
// 165│         do {
// 166│             l--;
// 167├───────────> zero=(std::fabs(h(l, l-1)) <= test);
// 168│         } while (l!=1 && !zero);
// 169│         if (!zero) l=0;
// 170│         else h(l, l-1)=0.0;
// 
// [?2004l#5  0x00005555555741a3 in BS::schur (h=..., u=..., nn=2, eps=1.0000000000000001e-30) at chebyshev_pseudospectral/src/BartelsStewart.cpp:167
// [?2004h(gdb) p l
// [?2004l$3 = 18446744073709551615
// [?2004h(gdb) p (signed)l
// [?2004l$4 = -1
```
```c++
386│             a(i, m+1) = 0.0;
387│             for (k=0; k<=m; k++) {
388│                 a(i, m+1) -= u(k, i)*c(k, j);
389│             }
390│         }
391│         for (i=0; i<=m; i++) {
392│             rslt(i, j) = a(i, m+1);
393│         }
394│     }
395│     for (i=0; i<=m; i++) {
396│         for (j=0; j<=n; j++) {
397├───────────> b(n+1, j) = 0.0;
398│             for (k=0; k<=n; k++) {
399│                 b(n+1, j) += rslt(i, k)*v(k, j);
400│             }
401│         }
402│         for (j=0; j<=n; j++) {
403│             rslt(i, j) = b(n+1, j);
404│         }
405│     }
406│
407│     shrslv(m, n, a, b, rslt);
/home/spike/work/github/cpp-samples/chebyshev_pseudospectral/src/BartelsStewart.cpp                                                                                                  
a fields>}
[?2004h(gdb) p a
[?2004l$2 = {<Eigen::RefBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> > >> = {<Eigen::MapBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -
1>, 0, Eigen::OuterStride<-1> >, 1>> = {<Eigen::MapBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> >, 0>> = {<Eigen::MatrixBase<Eigen::Ref<Eigen:
:Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> > >> = {<Eigen::DenseBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> > >> = {<Eigen
::DenseCoeffsBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> >, 3>> = {<Eigen::DenseCoeffsBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1
>, 0, Eigen::OuterStride<-1> >, 1>> = {<Eigen::DenseCoeffsBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> >, 0>> = {<Eigen::EigenBase<Eigen::Ref<
Eigen::Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> > >> = {<No data fields>}, <No data fields>}, <No data fields>}, <No data fields>}, <No data fields>}, <No data f
ields>}, m_data = 0x55555560de60, m_rows = {m_value = 65}, m_cols = {m_value = 65}}, <No data fields>}, m_stride = {m_outer = {m_value = 65}, m_inner = {<No data fields>}}}, <No dat
a fields>}
[?2004h(gdb) where
[?2004l#0  0x00007ffff7a41ef5 in raise () from /usr/lib/libc.so.6
#1  0x00007ffff7a2b862 in abort () from /usr/lib/libc.so.6
#2  0x00007ffff7a2b747 in __assert_fail_base.cold () from /usr/lib/libc.so.6
#3  0x00007ffff7a3a646 in __assert_fail () from /usr/lib/libc.so.6
#4  0x000055555556f58b in Eigen::DenseCoeffsBase<Eigen::Ref<Eigen::Matrix<double, -1, -1, 1, -1, -1>, 0, Eigen::OuterStride<-1> >, 1>::operator() (this=0x7fffffffd460, row=63, col=0
) at /usr/include/eigen3/Eigen/src/Core/DenseCoeffsBase.h:364
#5  0x0000555555578b11 in BS::bs_solve (m=30, n=62, a=..., b=..., u=..., v=..., c=..., rslt=...) at /home/spike/work/github/cpp-samples/chebyshev_pseudospectral/src/BartelsStewart.c
pp:397
#6  0x000055555556d94f in PoissonProblem::solve (this=0x7fffffffd570) at /home/spike/work/github/cpp-samples/chebyshev_pseudospectral/src/PoissonProblem.cpp:139
#7  0x000055555555d80f in main (argc=5, argv=0x7fffffffd7e8) at /home/spike/work/github/cpp-samples/chebyshev_pseudospectral/src/main.cpp:116
(gdb) 
```
