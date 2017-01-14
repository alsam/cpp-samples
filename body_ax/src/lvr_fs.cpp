// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Tuesday, August 7, 2012
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#include <cmath>
#include "elliptic_integral.hpp"

using namespace std;

void
lvr_fs (int iopt, double x, double s, 
	double x0, double s0, double &u, double &v, 
	double &psi)
{
    /* System generated locals */
    double d__1, d__2;

    /* Local variables */
    double e, f, cf, dx, rk, pi4, ri11, ri30, rj30, rj31, ri31,
	         rj11, dxs, rks;

/* ----------------------------------------- */
/* Copyright by C. Pozrikidis, 1999 */
/* All rights reserved. */

/* This program is to be used only under the */
/* stipulations of the licensing agreement. */
/* ---------------------------------------- */
/* -------------------------------------------------- */
/* Velocity components (u, v) */
/* and Stokes streamfunction (psi) */
/* due to a line vortex ring */

/* If Iopt.eq.1 the subroutine computed the velocity */
/* If Iopt.ne.1 the subroutine computed the velocity */
/*              and the Stokes stream function */
/* -------------------------------------------------- */
/* --- */
/* prepare */
/* --- */
    dx = x - x0;
/* Computing 2nd power */
    dxs = dx * dx;
/* Computing 2nd power */
    d__1 = s + s0;
    rks = s * 4.0 * s0 / (dxs + d__1 * d__1);
    elliptic_integral(rks, f, e);
    rj30 = e / (1.0 - rks);
    rj31 = (f * -2.0 + e * (2.0 - rks) / (1.0 - rks)) / rks;
/* Computing 2nd power */
    d__2 = s + s0;
/* Computing 3rd power */
    d__1 = dxs + d__2 * d__2;
    cf = 4.0 / sqrt(d__1 * d__1 * d__1);
    ri30 = cf * rj30;
    ri31 = cf * rj31;
    u = (-s * ri31 + s0 * ri30) / M_PI*4;
    v = dx * ri31 / M_PI*4;
    if (iopt == 1) {
	    return;
    }
/* --- */
/* compute the Stokes stream function */
/* --- */
    rk = sqrt(rks);
    rj11 = ((2. - rks) * f - e * 2.) / rks;
/* Computing 2nd power */
    d__1 = s + s0;
    cf = 4.0 / sqrt(dxs + d__1 * d__1);
    ri11 = cf * rj11;
    psi = rk * s * s0 * ri11 / M_PI*4;
/* ----- */
/* Done */
/* ----- */
} /* lvr_fs__ */

