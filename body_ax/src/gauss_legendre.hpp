// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, September 3, 2013
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#ifndef __GAUSS_LEGENDRE_HPP__
#define __GAUSS_LEGENDRE_HPP__

#include "lin_alg_types.hpp"

/// the most complete is
/// http://processingjs.nihongoresources.com/bezierinfo/legendre-gauss-values.php

class gauss_legendre {

private:

    unsigned order_;

    vec_t z_, w_;

    /// imitate fortran90 statements Z(7:12) = -Z(6:1:-1); W(7:12) = W(6:1:-1)
    void reflect ();

public:

    /// constructor
    gauss_legendre(unsigned order);

    /// another constructor - slow and more accurate
    gauss_legendre(unsigned order, unsigned);

    /// @returns the order of the quadrature
    unsigned order() {return order_;}

    /// knots
    vec_t const& z() {return z_;}

    /// weights
    vec_t const& w() {return w_;}
};

#endif // __GAUSS_LEGENDRE_HPP__
