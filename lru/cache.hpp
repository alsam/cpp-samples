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
//  Created:            Thursday, October 3, 2012
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include <array>
#include <unordered_map>
#include <functional>
#include <string>
#include "bitops.hpp"
//#include "pte.hpp"
//#include "util.hpp"


template <unsigned NSETS, unsigned NWAYS, typename TAG, typename DATA>
class CACHE {

public:

    typedef TAG tagtype;
    typedef DATA datatype;

    struct CACHE_LINE {
        typename TAG::type tag;
        DATA data;
        bool valid;
        CACHE_LINE(typename TAG::type const& t, DATA const& d): tag(t), data(d), valid(true) {}
        CACHE_LINE(): tag(typename TAG::type()), data(DATA()), valid(false) {}
    };

    struct SET {
        std::array<CACHE_LINE, NWAYS> line;

        CACHE_LINE const& operator[](unsigned way) const { return line[way]; }
        CACHE_LINE      & operator[](unsigned way)       { return line[way]; }

        void fill(unsigned way, typename TAG::type const& tag, DATA const& data) {
            CACHE_LINE cache_line(tag, data);
            line[way] = cache_line;
        }

        std::pair<bool,unsigned> find_way(unsigned long addr) {
            unsigned way;
            typename TAG::type tag = TAG()(addr);
            for (way = 0; way < nways; way++) {
                CACHE_LINE const& ln = line[way];
                if (ln.valid && ln.tag == tag) { // hit
                    set_mru(way);
                    return std::make_pair(true,way);
                }
            }
            way = get_lru();
            line[way].valid = false; // evict this way immedly or defer?
            return std::make_pair(false,way);
        }

        void set_mru(unsigned way) {
            lru_[way].set();
            for (unsigned i = 0; i < NWAYS; i++)
                lru_[i].reset(way);
        }

        unsigned get_lru() {
            for (unsigned i = 0; i < NWAYS; i++) {
                if (!lru_[i].any()) // bitstring consisting of zeros => this line is lru
                    return i;
            }
            return 0;
        }

        std::string lru_to_string() const {
            std::string s = " : ";
            for (int i = lru_.size() - 1; i >= 0; i--) {
                s += std::to_string(i);
            }
            s += "\n";
            for (unsigned i = 0; i < lru_.size(); i++) {
                s += std::to_string(i) + ": " + lru_[i].to_string() + "\n";
            }
            return s;
        }

    private:
        /// lru policy square bit matrix
        std::array<std::bitset<NWAYS>, NWAYS> lru_;
    };

    static const unsigned nsets = NSETS;
    static const unsigned nways = NWAYS;
    static const unsigned total_lines = NSETS * NWAYS;
    static const unsigned long tag_bits = TAG::span;

    typedef std::hash<typename TAG::type> tag_hash_fn_t;

    typedef unsigned (CACHE::*get_set_idx_fn_t) (unsigned long);

    CACHE();

    unsigned get_set_idx_simple(unsigned long addr);

    unsigned get_set_idx_bitset(unsigned long addr);

    /// how it is implemented in rtl
    unsigned get_set_idx_rtl(unsigned long addr);

    /// chooses a real get_set_idx via pointer to member fn, set in constructor
    unsigned get_set_idx(unsigned long addr) { return (this->*get_set_idx_fn_)(addr); }

    bool enabled() const { return nsets > 0; }

    /// fills the cache line with data
    /// @param[in] addr - virtual address
    void fill(unsigned long addr, DATA const& data);

    /// set-associative cache lookup
    /// @returns true if hit and enabled, false otherwise
    /// @param[in]  addr - virtual address
    /// @param[out] way - on miss the returned way is LRU - to be evicted
    /// @param[out] set - on miss - the tried set, the LRU way on this set is to be evicted
    bool lookup(unsigned long addr, unsigned& way, unsigned& set);

    /// invalidate cache line
    void evict(unsigned& way, unsigned& set) {
        sets_[set][way].valid = false;
    }

private:

    ///
    tag_hash_fn_t tag_hash_fn_;

    ///
    get_set_idx_fn_t get_set_idx_fn_;

    /// sliced bits for tagging
    TAG tag_bit_slicer_;

    /// number of bits for storing set number == Log2(nsets)
    unsigned set_bits_;

private:

    //CACHE_LINE lines_[NSETS][NWAYS];
    std::array<SET, NSETS> sets_;
};

#endif // __CACHE_HPP__
