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

int main(int argc, char **argv)
{
    auto popt = parse_command_line(argc, argv);
    if (popt.verbose) {
        std::cout << popt;
    }

    mat_t phi(MAX_SEGMENTS, MAX_ELEMS);
    vec_t velt(MAX_DIM), veln(MAX_DIM), cp(MAX_DIM);

    int ngl = body_ax_geo(popt);
    if (popt.verbose) {
        std::cout << "-I- ngl: " << ngl << "\n";
    }

}

