#!/bin/sh

LC_ALL="en_US.UTF-8" g++ $(pkg-config --cflags --libs python3) -I/usr/include/eigen3 src/cft.cpp src/poisson_solver.cpp -o poisolve
