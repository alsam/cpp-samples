#pragma once

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

#include <array>
#include <type_traits>

constexpr unsigned MAX_SEGMENTS = 10;
constexpr unsigned MAX_ELEMS = 512;
constexpr unsigned MAX_QUADRATURE_POINTS = 20;
constexpr unsigned MAX_DIM = MAX_SEGMENTS * MAX_ELEMS;

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

enum class FlowType : unsigned char {
    SPHERE = 50,
    THORUS = 51,
};

template <typename T>
struct parameters {
    int ngl;
    std::array<int, MAX_SEGMENTS> ne, itp;
    T vx; // velocity of incident flow
    T cr; // line vortex ring strength
    T xlvr,  ylvr;
    T xwmin, xwmax;
    T ywmin, ywmax;
    union {
        struct {
            T rad;
            T xcenter;
            // ...
        } sphere;
        struct {
            T xfirst,  yfirst;  // 1st vertex
            T xsecond, ysecond; // 2nd vertex
            T xthird,  ythird;  // 3d vertex
            // ..
        } thorus;
    };
};
