// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, September 11, 2013
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#include "elliptic_integral.hpp"

#if defined PRINT_ME

#include <iostream>
#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>

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

