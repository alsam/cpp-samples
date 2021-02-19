#!/bin/sh

LC_ALL="en_US.UTF-8" g++ -g $(pkg-config --cflags --libs python3) -I/usr/include/eigen3 src/cft.cpp src/PoissonProblem.cpp src/main.cpp -o poisolve
