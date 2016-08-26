// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, September 3, 2013
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#include <iostream>
#include "gauss_legendre.hpp"

// [Legendre-Gauss Quadrature](http://mathworld.wolfram.com/Legendre-GaussQuadrature.html)

using namespace Eigen;
using namespace std;

/// imitate fortran90 statements Z(7:12) = -Z(6:1:-1); W(7:12) = W(6:1:-1)
void
gauss_legendre::reflect ()
{
    unsigned len = order_ / 2, start = (order_ + 1) / 2;
    z_.conservativeResize(order_);
    w_.conservativeResize(order_);
    z_.segment(start, len) = -z_.segment(0, len).reverse();
    w_.segment(start, len) =  w_.segment(0, len).reverse();
}

/// constructor
gauss_legendre::gauss_legendre(unsigned order)
: order_(order)
{
    unsigned half = (order + 1) / 2;
    z_.resize(half);
    w_.resize(half);
    switch (order) {
    case 1:
        z_ << 0.0;
        w_ << 2.0;
        break;
    case 2:
        z_ << -0.57735026918962576450;
        w_ << 1.0;
        break;
    case 3:
        z_ << -0.77459666924148337703,  0.0;
        w_ <<  0.555555555555555555555, 0.888888888888888888888;
        break;
    case 4:
        z_ << -0.86113631159405257522, -0.33998104358485626480;
        w_ <<  0.34785484513745385737,  0.65214515486254614262;
        break;
    case 5:
        z_ << -0.90617984593866399279, -0.53846931010568309103,                    0.0;
        w_ <<  0.23692688505618908751,  0.47862867049936646804, 0.56888888888888888889;
        break;
    case 6:
        z_ << -0.932469514203152,      -0.661209386466265,          -0.238619186083197;
        w_ <<  0.171324492379170,       0.360761573048139,           0.467913934572691;
        break;
    case 8:
        z_ << -0.96028985649753623168, -0.79666647741362673959,-0.52553240991632898581, -0.18343464249564980493;
        w_ <<  0.10122853629037625915,  0.22238103445337447054, 0.31370664587788728733,  0.36268378337836198296;
        break;
    case 12:
        z_ << -0.981560634246719, -0.904117256370475, -0.769902674194305, -0.587317954286617, -0.367831498998180, -0.125233408511469;
        w_ << 0.047175336386511,   0.106939325995318,  0.160078328543346,  0.203167426723066,  0.233492536538355,  0.249147045813403;
        break;
    default:
        cerr << " Gauss_Legendre: Chosen number of Gaussian points is not available; will take N=20" << endl;
    case 20:
        z_ << -0.993128599185094924786, -0.963971927277913791268, -0.912234428251325905868, -0.839116971822218823395, -0.746331906460150792614,
              -0.636053680726515025453, -0.510867001950827098004, -0.373706088715419560673, -0.227785851141645078080, -0.076526521133497333755;
        w_ <<  0.017614007139152118312,  0.040601429800386941331,  0.062672048334109063570,  0.083276741576704748725,  0.101930119817240435037,
               0.118194531961518417312,  0.131688638449176626898,  0.142096109318382051329,  0.149172986472603746788,  0.152753387130725850698;
    }
    reflect();
}

// quick check with Julia
/**
 *
```julia
julia> z = [ -0.981561 -0.904117 -0.769903 -0.587318 -0.367831 -0.125233  0.125233  0.367831  0.587318  0.769903  0.904117  0.981561 ]
1×12 Array{Float64,2}:
 -0.981561  -0.904117  -0.769903  -0.587318  -0.367831  -0.125233  0.125233  0.367831  0.587318  0.769903  0.904117  0.981561

julia> w = [ 0.0471753  0.106939  0.160078  0.203167  0.233493  0.249147  0.249147  0.233493  0.203167  0.160078  0.106939 0.0471753 ]
1×12 Array{Float64,2}:
 0.0471753  0.106939  0.160078  0.203167  0.233493  0.249147  0.249147  0.233493  0.203167  0.160078  0.106939  0.0471753

julia> dot(w, z.*z)
0.6666654323126298
```
 */

/// another constructor - slow, more accurate and for different quadrature orders
gauss_legendre::gauss_legendre(unsigned order, unsigned)
: order_(order)
{
  const double eps = 1e-15;

  // TODO add the implementation
}

#if defined PRINT_ME

void print_gl_coefs(unsigned n)
{
    gauss_legendre gl(n);
    auto const& z = gl.z();
    auto const& w = gl.w();

    // dot(w, z.*z) should be 2/3
    auto reduction = w.dot(z.cwiseProduct(z));

    cout << "n = " << n << ":\nz = [ " << z << " ]\nw = [ " << w << " ]\ndot(w, z.*z) = " << reduction << endl;
}

main()
{
    for (auto n : {1, 2, 3, 4, 5, 6, 8, 12, 20,} ) print_gl_coefs(n);
}

#endif

#if defined UNIT_TEST

#include "gtest/gtest.h"

TEST(Reduction, Len5) {
    unsigned len = 5;
    gauss_legendre gl(len);
    auto const& z = gl.z();
    auto const& w = gl.w();
    auto reduction = w.dot(z.cwiseProduct(z));
    double expected = 2.0 / 3.0;
    double absolute_range = 1e-10;
    EXPECT_NEAR (expected, reduction, absolute_range);
}

TEST(Reduction, Len12) {
    unsigned len = 12;
    gauss_legendre gl(len);
    auto const& z = gl.z();
    auto const& w = gl.w();
    auto reduction = w.dot(z.cwiseProduct(z));
    double expected = 2.0 / 3.0;
    double absolute_range = 1e-10;
    EXPECT_NEAR (expected, reduction, absolute_range);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
