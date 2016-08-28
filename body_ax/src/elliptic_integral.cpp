// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, September 11, 2013
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#include <cmath>
#include "elliptic_integral.hpp"

using namespace std;

/// \f$F\left(k\right)\equiv\int_0^{\pi/2}\frac{d\eta}{\sqrt{1-k^2\cos^2\eta}}\f$
void elliptic_integral (double rk2, double& f, double& e)
{
    const double accuracy = 1.0e-12;
    double rk = sqrt(rk2), g = 1.0, b = rk, c, d;

    f = 0.5 * M_PI;
    e = 1.0;
    do {
        c = sqrt(1.0 - b*b);
        b = (1.0 - c) / (1.0 + c);
        d = f*b;
        f += d;
        g = 0.5*g*b;
        e += g;
    } while (fabs(d) > accuracy);
    e = f * (1.0 - 0.5*rk2*e);
}

#if defined PRINT_ME

#include <iostream>
#include "boost/math/special_functions/ellint_1.hpp"
#include "boost/math/special_functions/ellint_2.hpp"

int main()
{
    unsigned nsteps = 42;
    double dx = 1.0 / nsteps;
    for (unsigned i = 1; i < nsteps; ++i) {
        double rk = i * dx;
        double f, e;
        elliptic_integral(rk, f, e);
        double bf = boost::math::ellint_1(sqrt(rk));
        double be = boost::math::ellint_2(sqrt(rk));
        std::cout << "rk: " << rk << " f: " << f << " bf: " << bf << " f/bf: " << f/bf << " e: " << e << " be: " << be << std::endl;
    }
}

#endif

