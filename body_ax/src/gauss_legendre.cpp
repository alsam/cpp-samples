// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, September 3, 2013
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#include "gauss_legendre.hpp"

#if defined PRINT_ME

using namespace std;

void print_gl_coefs(unsigned n)
{
    gauss_legendre<double> gl(n);
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

void TestReduction(unsigned len) {
    gauss_legendre<double> gl(len);
    auto const& z = gl.z();
    auto const& w = gl.w();
    auto reduction = w.dot(z.cwiseProduct(z));
    double expected = 2.0 / 3.0;
    double absolute_range = 1e-10;
    EXPECT_NEAR (expected, reduction, absolute_range);
}

TEST(Reduction, Len2) {
    TestReduction(2);
}

TEST(Reduction, Len3) {
    TestReduction(3);
}

TEST(Reduction, Len4) {
    TestReduction(4);
}

TEST(Reduction, Len5) {
    TestReduction(5);
}

TEST(Reduction, Len6) {
    TestReduction(6);
}

TEST(Reduction, Len8) {
    TestReduction(8);
}

TEST(Reduction, Len12) {
    TestReduction(12);
}

TEST(Reduction, Len20) {
    TestReduction(20);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
