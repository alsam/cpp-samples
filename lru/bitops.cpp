// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, October 2, 2012
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

//
//   Brief file description here
//

#include <iostream>
#include <iomanip>
#include <cassert>
#include "bitops.hpp"
#include <string.h> // for ffs

using namespace std;

#if defined BIT_SLICE_TEST

#define TRIPLET(a) true ? true ? a , true ? false ? a , false ? true ? a

template <int a, int b, int c>
void test_triple()
{
  cout << "a: " << a << " b: " << b << " c: " << c << endl;
}

template <typename T>
void test_log2(string const& descr, T val)
{
    cout << descr << ": "  << val << " " << Log2(val) << endl;
}

int main()
{
    cout << "test ffs:\n";
    cout << "ffs(0x80): " << ffs(0x80) << " ffsl(0xFEE0DDF000): " << ffsl(0xFEE0DDF000) << endl;

    cout << "test_triple:\n";
    test_triple<TRIPLET(1:2:3)>();

    cout << "bit slice unit test" << endl;

    unsigned long a = 0xFEE0DDF00D;
    bit_slice<28,12> slicer;
    cout << "slicer.msb: " << slicer.msb << endl;
    //                          
    cout << "addr           : " << bitset<40>(a) << endl;
    cout << " 28:12 slice   :            " << slicer(a) << endl;
    cout << hex << "addr in hex : " << a << " 28:12 slice : " << slicer(a).to_ulong() << endl;

    bitslicer(28:12) sl;

    cout << "addr           : " << bitset<40>(a) << endl;
    cout << " 28:12 slice   :            " << sl(a) << endl;
    cout << hex << "addr in hex : " << a << " 28:12 slice : " << sl(a).to_ulong() << endl;

    cout << "sl.shl_lsb(a): " << sl.shl_lsb(a) << endl;
    cout << "sl.mask:    " << sl.mask << endl;
    cout << "sl.cmask(): " << sl.cmask() << endl;

    cout << dec;
    for (int i = 0; i < 32; i++) {
      test_log2(string("1U<<")+to_string(i), 1U<<i);
    }
    for (int i = 32; i < 64; i++) {
      test_log2(string("1L<<")+to_string(i), 1UL<<i);
    }
    
    unsigned x = (1<<7) + 2; // ??
    cerr << "x: "  << x  << " x & (x-1): " << (x & (x-1)) << endl;
    cerr << Log2(x) << endl;
}

/* output from the test:

test_triple:
a: 1 b: 2 c: 3
bit slice unit test
addr           : 1111111011100000110111011111000000001101
 28:12 slice   :            00000110111011111
addr in hex : fee0ddf00d 28:12 slice : ddf
addr           : 1111111011100000110111011111000000001101
 28:12 slice   :            00000110111011111
addr in hex : fee0ddf00d 28:12 slice : ddf
sl.shl_lsb(a): ddf000
sl.mask:    11111111111111111
sl.cmask(): 11111111111111111
1U<<0: 1 0
1U<<1: 2 1
1U<<2: 4 2
1U<<3: 8 3
1U<<4: 16 4
1U<<5: 32 5
1U<<6: 64 6
1U<<7: 128 7
1U<<8: 256 8
1U<<9: 512 9
1U<<10: 1024 10
1U<<11: 2048 11
1U<<12: 4096 12
1U<<13: 8192 13
1U<<14: 16384 14
1U<<15: 32768 15
1U<<16: 65536 16
1U<<17: 131072 17
1U<<18: 262144 18
1U<<19: 524288 19
1U<<20: 1048576 20
1U<<21: 2097152 21
1U<<22: 4194304 22
1U<<23: 8388608 23
1U<<24: 16777216 24
1U<<25: 33554432 25
1U<<26: 67108864 26
1U<<27: 134217728 27
1U<<28: 268435456 28
1U<<29: 536870912 29
1U<<30: 1073741824 30
1U<<31: 2147483648 31
1L<<32: 4294967296 32
1L<<33: 8589934592 33
1L<<34: 17179869184 34
1L<<35: 34359738368 35
1L<<36: 68719476736 36
1L<<37: 137438953472 37
1L<<38: 274877906944 38
1L<<39: 549755813888 39
1L<<40: 1099511627776 40
1L<<41: 2199023255552 41
1L<<42: 4398046511104 42
1L<<43: 8796093022208 43
1L<<44: 17592186044416 44
1L<<45: 35184372088832 45
1L<<46: 70368744177664 46
1L<<47: 140737488355328 47
1L<<48: 281474976710656 48
1L<<49: 562949953421312 49
1L<<50: 1125899906842624 50
1L<<51: 2251799813685248 51
1L<<52: 4503599627370496 52
1L<<53: 9007199254740992 53
1L<<54: 18014398509481984 54
1L<<55: 36028797018963968 55
1L<<56: 72057594037927936 56
1L<<57: 144115188075855872 57
1L<<58: 288230376151711744 58
1L<<59: 576460752303423488 59
1L<<60: 1152921504606846976 60
1L<<61: 2305843009213693952 61
1L<<62: 4611686018427387904 62
1L<<63: 9223372036854775808 63
x: 130 x & (x-1): 128
terminate called after throwing an instance of 'std::logic_error'
  what():  x is not 2^n
make: *** [bunit] Aborted (core dumped)

*/

#endif
