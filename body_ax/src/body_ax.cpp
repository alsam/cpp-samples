// -*- C++ -*-

// The MIT License (MIT)
//
// Copyright (c) 2017 Alexander Samoilov
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

// ==========================================
//  FDLIB, CFDLAB, BEMLIB
//
//  Copyright by C. Pozrikidis, 1999
//  All rights reserved.
//
//  This program is to be used only under the
//  stipulations of the licensing agreement.
// ==========================================
//
// ---------------------------------------------------
//  Axisymmetric streaming (uniform)
//  potential flow past a stationary body.
//
//  This program solves
//  an integral equation of the second kind
//  for the disturbance harmonic potential
//  over the body contour
//  and computes streamlines
//
//  The contour of the body in the xy upper half-plane
//  consists of a Number of SeGments (NSG)
//
//  The segments may be straight lines or circular arcs.
//
//  Each segment is discretized into a number of elements
//
//
//  Symbols:
//  --------
//
//  NSG: Number of segments defining the body contour
//
//  NE(i): Number of elements on the ith segment
//
//  RT(i): Stretch ratio of elements on ith segment
//
//  Itp(i): Index for shape of the ith segment:
//          1 for a straight segment
//          2 for a circular arc
//
//  (Xe, Ye):  end-nodes of elements on a segment
//  (Xm, Ym):  mid-nodes of elements on a segment
//  (Xw, Yw):  end-nodes of elements on all segments
//
//  (X0, Y0):  coordinates of collocation points
//
//  T0(i): angle subtended from the center of a circular element
//         at ith collocation point
//
//  arel(i):  axisymmetric surface area of ith element
//
//  phi: disturbance potential at collocation points
//
//  dphidn0: normal derivative of disturbance potential
//                  at collocation points
//
//  dphids0: tangential derivative of potential
//                  at collocation points
//
//  cp:      pressure coefficient at collocation points
//
//  NGL: Number of Gaussian points for integration
//       over each element
//
//  Icross: Index for stopping the computation of the streamlines
//          Default value is 0
//          If Icross = 1, computation stops when a streamline
//          crosses the yz plane
//
//  Notes:
//  ------
//
//  Normal vector points into the flow
//
// ----------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>
#include "parameters.hpp"
#include "program_options.hpp"
#include "lin_alg_types.hpp"
#include "body_ax_geo.hpp"
#include "body_ax_sdlp.tcc"
#include "body_ax_vel.tcc"

program_options parse_command_line(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc("allowed options");
    desc.add_options()
            ("help",                   "describe arguments")
            ("verbose",                "be verbose")
            ("flow_type",              po::value<int>(), "legacy Fortran constants for flow type, 50 for sphere, 51 for torus")
            ("input_data",             po::value<std::string>(), "name of the input file e.g. `sphere.dat`, `torus_trgl.dat`")
            ("asy_name",               po::value<std::string>(), "dump streamlines to vector `.asy` file if name is given");
    try
    {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        program_options popt;

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            std::exit(1);
        }

        popt.verbose = vm.count("verbose");
        if (vm.count("flow_type")) {
            popt.flow_type = vm["flow_type"].as<int>();
        }
        if (vm.count("input_data")) {
            popt.input_data = vm["input_data"].as<std::string>();
        }
        if (vm.count("asy_name")) {
            popt.asy_name = vm["asy_name"].as<std::string>();
        }

        return popt;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << desc << std::endl;
        std::exit(-1);
    }
}

using Time = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;
using fsec = std::chrono::duration<float>;

int main(int argc, char **argv)
{
    auto popt = parse_command_line(argc, argv);
    if (popt.verbose) {
        std::cout << popt;
    }

    matg_t<FLOATING_TYPE> phi;
    vecg_t<FLOATING_TYPE> velt, veln, cp;
    matg_t<FLOATING_TYPE> al;      // for the linear system
    vecg_t<FLOATING_TYPE> bl, sol; // ditto

    vecg_t<FLOATING_TYPE> xstr, ystr; // for streamlines

    auto const& run_params = body_ax_geo<FLOATING_TYPE>(popt);
    if (popt.verbose) {
        std::cout << "-I- ngl: " << run_params.ngl << "\n";
        std::cout << "-I- vx: " << run_params.vx << "\n";
        std::cout << "-I- cr: " << run_params.cr << "\n";
        std::cout << "-I- ne[0]: " << run_params.ne[0] << "\n";
        std::cout << "-I- xwmin: " << run_params.xwmin << "\n";
        std::cout << "-I- ywmin: " << run_params.ywmin << "\n";
    }

    // ...

    //---------------------------------------------
    // Generate the linear system
    // for the potential at the collocation points
    //
    // Generate the influence matrix
    // consisting of integrals of the
    // single-layer potential
    //
    // Compute the rhs by evaluating the dlp
    //-------------------------------------------------------

    // resizing
    al.resize(run_params.ncl, run_params.ncl);
    bl.resize(run_params.ncl);
    // stiffness matrix and rhs
    for (int i = 0; i < run_params.ncl; ++i) {           // loop over collocation points
        bl(i) = ZERO<FLOATING_TYPE>;
        int j = -1;
        for (int k = 0; k < run_params.nsg; ++k) {       // loop over segments
            FLOATING_TYPE rad = NaN<FLOATING_TYPE>, xcnt = NaN<FLOATING_TYPE>, ycnt = NaN<FLOATING_TYPE>;
            if (run_params.itp[k] == 2) {
                rad  = run_params.actis[k];
                xcnt = run_params.xcntr[k];
                ycnt = run_params.ycntr[k];
            }
            for (int l = 0; l < run_params.ne[k]; ++l) { // loop over elements
                FLOATING_TYPE x1 = run_params.xw(k,l);
                FLOATING_TYPE y1 = run_params.yw(k,l);
                FLOATING_TYPE t1 = run_params.tw(k,l);

                FLOATING_TYPE x2 = run_params.xw(k,l+1);
                FLOATING_TYPE y2 = run_params.yw(k,l+1);
                FLOATING_TYPE t2 = run_params.tw(k,l+1);

                j = j+1;
                bool ising = (i == j);
                FLOATING_TYPE qqq, www;
                body_ax_sdlp (run_params.x0[i], run_params.y0[i], run_params.t0[i],
                              x1, y1, t1, x2, y2, t2, run_params.gl, ising,
                              run_params.itp[k], rad,xcnt,ycnt,
                              qqq, www);

                al(i, j) = www;
                bl(i) += qqq * run_params.dphidn0[j];

            }

        }
        al(i, i) -= HALF<FLOATING_TYPE>;

    }

    auto t0 = Time::now();
    //------------------------
    // Solve the linear system
    //------------------------
    sol = al.lu().solve(bl.transpose());
    auto t1 = Time::now();
    fsec fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);
    std::cout << "linear system solution took " << fs.count() << "s\n";
    std::cout <<  "linear system solution took " << d.count() << "ms\n";

    if (popt.verbose) {
        for (size_t i = 0; i < run_params.ncl; ++i) {
            std::cout << "sol(" << i << ") : " << sol(i) << std::endl;
        }
    }

    //------------------------
    // Distribute the solution
    //------------------------

    size_t max_elems = run_params.ne.maxCoeff();
    phi.resize(run_params.nsg, max_elems);
    velt.resize(run_params.nsg * max_elems);
    veln.resize(run_params.nsg * max_elems);
    cp.resize(run_params.nsg * max_elems);

    size_t k = 0;        // counter
    for (size_t i = 0; i < run_params.nsg; ++i) {
        for (size_t j = 0; j < run_params.ne[i]; ++j) {
            phi(i, j) = sol(k++);
        }
    }

    //------------------------------------------------
    // Compute the tangential disturbance velocity
    //             by taking tangential derivative of \phi
    //
    // Compute the pressure coefficient cp
    //             drag force
    //------------------------------------------------
 
    FLOATING_TYPE forcex = ZERO<FLOATING_TYPE>, dphids0;
    k = 0;        // counter
    for (size_t i = 0; i < run_params.nsg; ++i) {
        for (size_t j = 0; j < run_params.ne[i]; ++j) {
            //--------------------------------
            // tangential disturbance velocity
            // computed by differentiating phi
            //--------------------------------
            if (j == 0) {                          // forward differencing
                dphids0 = (phi(i,j+1)-phi(i,j))/(run_params.s0(k+1)-run_params.s0(k));
            } else if(j == run_params.ne(i) - 1) { // backward differencing
                dphids0 = (phi(i,j)-phi(i,j-1))/(run_params.s0(k)-run_params.s0(k-1));
            } else {                               // second-order differencing
                FLOATING_TYPE g1 = phi(i,j-1);
                FLOATING_TYPE g2 = phi(i,j);
                FLOATING_TYPE g3 = phi(i,j+1);
                FLOATING_TYPE h1 = run_params.s0(k-1);
                FLOATING_TYPE h2 = run_params.s0(k);
                FLOATING_TYPE h3 = run_params.s0(k+1);
                FLOATING_TYPE aa = ((g3-g2)/(h3-h2)-(g1-g2)/(h1-h2))/(h3-h1);
                FLOATING_TYPE bb = (g3-g2)/(h3-h2)-aa*(h3-h2);
                dphids0 = bb;
            }
            //---------------
            // total velocity
            //---------------

            FLOATING_TYPE velx = run_params.dphidn0(k)*run_params.vnx0(k)+dphids0*run_params.tnx0(k);
            FLOATING_TYPE vely = run_params.dphidn0(k)*run_params.vny0(k)+dphids0*run_params.tny0(k);

            velx = velx + run_params.vx;        // add incident flow

            //---
            // add line vortex ring
            //---

            int iopt = 1;
            FLOATING_TYPE ulvr, vlvr, psi;
            lvr_fs<FLOATING_TYPE>(iopt, run_params.x0[k], run_params.y0[k], run_params.xlvr, run_params.ylvr, ulvr, vlvr, psi);
 
            velx = velx + run_params.cr*ulvr;
            vely = vely + run_params.cr*vlvr;

            //-------------------------------
            // tangential and normal velocity
            // and pressure coefficient
            //-------------------------------

            // ...
            velt(k) = velx*run_params.tnx0(k)+vely*run_params.tny0(k);
            veln(k) = velx*run_params.vnx0(k)+vely*run_params.vny0(k);

            //------------
            // axial force
            //------------

            cp(k) = ONE<FLOATING_TYPE> - (velt(k)*velt(k)) / (run_params.vx*run_params.vx);

            forcex = forcex + cp(k)*run_params.vnx0(k)*run_params.arel(k);
            if (popt.verbose) {
                std::cout << "velt(" << k << "): " << velt(k) << " veln(" << k << "): " << veln(k)
                          << " cp(" << k << "): " << cp(k) << std::endl;
            }

            ++k;
        }
    }

    std::cout << "Axial Force: " << forcex << std::endl;

    //-------------------
    // print the solution
    //-------------------
    std::ofstream sol_out("body_ax.out");
    if (sol_out) {
        k = 0;
        for (size_t i = 0; i < run_params.nsg; ++i) {
            sol_out << run_params.ne[i] << std::endl;
            for (size_t j = 0; j < run_params.ne[i]; ++j) {
                sol_out << j << " "
                        << run_params.x0(k) << " "
                        << run_params.y0(k) << " "
                        << run_params.s0(k) << " "
                        << phi(i, j) << " " << velt(k) << " " << veln(k) << " " << cp(k) << std::endl;
            
                ++k;
            }
        }
    }
    sol_out << 0 << std::endl;
    sol_out.close();

    bool detailed_iso = false;
    FLOATING_TYPE dl, ux1, uy1, ux2, uy2;
    int irk;

    if (detailed_iso) {
        dl    = static_cast<FLOATING_TYPE>(0.05) * static_cast<FLOATING_TYPE>(0.125);
        irk   = 4;   // Runge-Kutta 4th order
    } else {
        dl    = static_cast<FLOATING_TYPE>(0.05);
        irk   = 2;
    }

    //--------------------------
    // prepare for crossing test
    //--------------------------
    bool icross;
    if (popt.flow_type == to_underlying(FlowType::SPHERE)) {
        icross = true;
    } else if (popt.flow_type == to_underlying(FlowType::THORUS)) {
        icross = false;
    }

    //--------------------------
    // begin drawing streamlines
    //--------------------------
    size_t n_streamlines = run_params.x00.size(), l;
    constexpr size_t mstr = 1200;
    xstr.resize(mstr);
    ystr.resize(mstr);
    for (size_t i = 0; i < n_streamlines; ++i) {
        FLOATING_TYPE x00s = run_params.x00[i], y00s = run_params.y00[i];
        FLOATING_TYPE xcross = x00s;   // to be used for crossing check

        //------

        l = 0;     // local counter for inquiry
        for (size_t k = 0; k < mstr; ++k) {
            //k = 0;     // total counter

            xstr(l) = x00s;
            ystr(l) = y00s;

            //---------------
            // integrate ODEs
            //---------------

            velocity<FLOATING_TYPE>(run_params, phi, run_params.dphidn0, x00s,y00s, ux1,uy1);

            FLOATING_TYPE step = dl / std::sqrt(ux1*ux1 + uy1*uy1);     // set the frozen-time step

            FLOATING_TYPE xsv = x00s;  // save
            FLOATING_TYPE ysv = y00s;  // save

            //----------------------
            if (irk == 2) {
            //----------------------

                FLOATING_TYPE steph = HALF<FLOATING_TYPE> * step;

                x00s = xsv + step * ux1;
                y00s = ysv + step * uy1;

                velocity<FLOATING_TYPE>(run_params, phi, run_params.dphidn0, x00s,y00s, ux2,uy2);

                x00s = xsv + steph * (ux1 + ux2);
                y00s = ysv + steph * (uy1 + uy2);

            //---------------------------
            } else if (irk == 4) {
            //---------------------------

                FLOATING_TYPE steph = HALF<FLOATING_TYPE> * step, ux3, uy3, ux4, uy4;
                FLOATING_TYPE step6 = step / FLOATING_TYPE(6.0);

                x00s = xsv + steph * ux1;
                y00s = ysv + steph * uy1;

                velocity<FLOATING_TYPE>(run_params, phi, run_params.dphidn0, x00s,y00s, ux2,uy2);

                x00s = xsv + steph * ux2;
                y00s = ysv + steph * uy2;

                velocity<FLOATING_TYPE>(run_params, phi, run_params.dphidn0, x00s,y00s, ux3,uy3);

                x00s = xsv + step * ux3;
                y00s = ysv + step * uy3;

                velocity<FLOATING_TYPE>(run_params, phi, run_params.dphidn0, x00s,y00s, ux4,uy4);

                x00s = xsv + step/FLOATING_TYPE(6.0) * (ux1+FLOATING_TYPE(2.0)*ux2+FLOATING_TYPE(2.0)*ux3+ux4);
                y00s = ysv + step/FLOATING_TYPE(6.0) * (uy1+FLOATING_TYPE(2.0)*uy2+FLOATING_TYPE(2.0)*uy3+uy4);

            //-----------
            }
            //-----------

            //k = k+1;
            l = l+1;

            //---------------------------
            // test for x=0 plane crossing
            //---------------------------

            if (icross) {
                auto test = xcross*x00s;
                if (test < ZERO<FLOATING_TYPE>) {
                    std::cout << " Crossed the x=0 plane: I will stop\n";
                    break; // to the next streamline
                }
            }

            //-------------------------
            // test for sphere crossing
            //-------------------------
            if (icross) {
                FLOATING_TYPE crosss = std::hypot(x00s-run_params.xcntr[0], y00s-run_params.ycntr[0]);
                if (crosss < run_params.actis[0]) {
                    l = l - 1;
                    //break;
                    goto finish_streamline;
                }
            }

            //-----------------------
            // window crossing checks
            //-----------------------

            if (x00s < run_params.xwmin) break;
            if (x00s > run_params.xwmax) break;
            if (y00s < run_params.ywmin) break;
            if (y00s > run_params.ywmax) break;


        } // k loop

        xstr(l) = x00s;
        ystr(l) = y00s;

finish_streamline:;
        std::cout << " One streamline with " << l << " points completed\n";
        for (size_t i = 0; i < l; ++i) {
            std::cout << '(' << xstr(i) << ',' << ystr(i) << "),\n";
        }
    }



}

