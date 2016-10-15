// -*- C++ -*-

// The MIT License (MIT)
//
// Copyright (c) 2016 Alexander Samoilov
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


//---------------------------------------------------------------------
//  Created:            Thursday, October 2, 2012
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

//
//    bitslice and other bit operations
//

#ifndef __BITOPS_HPP__
#define __BITOPS_HPP__

#include <bitset>
#include <stdexcept>

const unsigned long page_4k_mask  = 0xFFFFFFF000L;
const unsigned long page_64k_mask = 0xFFFFFF0000L;

template <ssize_t MSB, ssize_t LSB>
struct bit_slice {

    static const ssize_t msb  = MSB;
    static const ssize_t lsb  = LSB;
    static const ssize_t span = MSB-LSB+1;

    typedef std::bitset<span> type;

    type mask;

    /// a constructor, the mask is set here
    bit_slice() { mask.set(); }

    /// a simple C-like way for creating the mask
    type cmask()                             const { return type((1LL << span) - 1); }
    
    type slice(unsigned long val)            const { return type(val >> LSB) & mask; }

    unsigned long shl_lsb(unsigned long val) const { return slice(val).to_ulong() << lsb; }

    type operator()(unsigned long val)       const { return slice(val); }

    template <typename BITSET>
    type operator()(BITSET const& bset)      const { return slice(bset.to_ulong()); }
};

/// functions int ffs(int i) and int ffsl(long int) are implemented in glibc
/// #include <string.h> // for ffs

inline unsigned long Exp2(unsigned int x) { return (1L << x); }

inline unsigned long Mask(unsigned int x) { return Exp2(x) - 1L; }

inline unsigned Log2(unsigned int x) {
    if ((x & (x-1)) != 0) throw std::logic_error("x is not 2^n");
    unsigned n = 1;
    if ((x & 0x0000FFFF) == 0) {n +=16; x >>=16;}
    if ((x & 0x000000FF) == 0) {n += 8; x >>= 8;}
    if ((x & 0x0000000F) == 0) {n += 4; x >>= 4;}
    if ((x & 0x00000003) == 0) {n += 2; x >>= 2;}
    return (n - (x&1));
}

inline unsigned Log2(unsigned long x) {
    if ((x & (x-1)) != 0) throw std::logic_error("x is not 2^n");
    unsigned n = 1;
    if ((x & 0x00000000FFFFFFFFL) == 0) {n +=32; x >>=32;}
    if ((x & 0x000000000000FFFFL) == 0) {n +=16; x >>=16;}
    if ((x & 0x00000000000000FFL) == 0) {n += 8; x >>= 8;}
    if ((x & 0x000000000000000FL) == 0) {n += 4; x >>= 4;}
    if ((x & 0x0000000000000003L) == 0) {n += 2; x >>= 2;}
    return (n - (x&1));
}

inline uint16_t bitrev(uint16_t x) {
    x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1); // swap odd/even bits
    x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2); // swap bit pairs
    x = ((x >> 4) & 0x0F0F) | ((x & 0x0F0F) << 4); // swap nibbles
    x = ((x >> 8) & 0x00FF) | ((x & 0x00FF) << 8); // swap bytes
    return x;
}

inline uint32_t bitrev(uint32_t x) {
    x = (x & 0x55555555)  <<  1 | (x >>   1) & 0x55555555;
    x = (x & 0x33333333)  <<  2 | (x >>   2) & 0x33333333;
    x = (x & 0x0F0F0F0F)  <<  4 | (x >>   4) & 0x0F0F0F0F;
    x = (x << 24) | ((x & 0xFF00) << 8) |
        ((x >> 8) & 0xFF00) | (x >> 24);
    return x;
}

template <size_t N>
std::ostream& operator<<(std::ostream& out, std::bitset<N> const& b) {
    out << b.to_string();
    return out;
}

/// macro for emulating verilog-like (28:12) bit slice notation

#define bitslicer(range) bit_slice<true?range,false?range>

#define bitslicer_word(word,range) bit_slice<word*32+(true?range),word*32+(false?range)>

#endif // __BITOPS_HPP__
