// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, September 11, 2013
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#ifndef __ELLIPTIC_INTEGRAL_HPP__
#define __ELLIPTIC_LEGENDRE_HPP__

/*! \file elliptic_integral.h
    \brief computation of elliptic integrals of the first and second kinds by iteration method

    Details.
*/

/**
 * \f$ F\left(k\right)\equiv\int_0^{\pi/2}\frac{d\eta}{\sqrt{1-k^2\cos^2\eta}} \f$
 */
void elliptic_integral (double rk2, double& f, double& e);

#endif // __ELLIPTIC_INTEGRAL_HPP__

