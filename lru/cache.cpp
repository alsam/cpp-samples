// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, October 3, 2012
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

//
//   Brief file description here
//

#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <numeric> // for accumulate
#include "opts.hpp"
#include "cache.hpp"

using namespace std;

/// a constructor
template <unsigned S, unsigned W, typename TAG, typename DATA>
CACHE<S,W,TAG,DATA>::CACHE()
: set_bits_(Log2(S))
, tag_bit_slicer_(TAG())
, get_set_idx_fn_(0)
{
    dlog() << "-D- cons: set_bits_: " << set_bits_ << endl;
    dlog() << "-D- cons: nsets: " << nsets << endl;

    //get_set_idx_fn_ = &CACHE::get_set_idx_simple;
    get_set_idx_fn_ = &CACHE::get_set_idx_rtl;
}

template <unsigned S, unsigned W, typename TAG, typename DATA>
unsigned
CACHE<S,W,TAG,DATA>::get_set_idx_simple(unsigned long addr)
{
    // a specialization for dummy SA case and FA case to avoid warning: division by zero [-Wdiv-by-zero]
    // PS unfortunately the compiler is dump - this warning is unavoidable
    // creating a partial specialization is an overkill for this simple case
    if (nsets == 0 || nsets == 1) {
        return 0;
    } else {
        // a very bad hash fn but if number of sets is prime, say 7 = 2^3 - 1,
        // than we can get a fine 7/8 set distribution
        // set[ 0 ] = 141
        // set[ 1 ] = 130
        // set[ 2 ] = 137
        // set[ 3 ] = 138
        // set[ 4 ] = 132
        // set[ 5 ] = 141
        // set[ 6 ] = 149
        // set[ 7 ] = 0
        unsigned h = addr;

        return h % (nsets - 1);
    }
}

template <unsigned S, unsigned W, typename TAG, typename DATA>
unsigned
CACHE<S,W,TAG,DATA>::get_set_idx_bitset(unsigned long addr)
{
    // a specialization for dummy SA case and FA case to avoid warning: division by zero [-Wdiv-by-zero]
    // PS unfortunately the compiler is dump - this warning is unavoidable
    // creating a partial specialization is an overkill for this simple case
    if (nsets == 0 || nsets == 1) {
        return 0;
    } else {
        typename TAG::type bslice = tag_bit_slicer_(addr);
        size_t h = tag_hash_fn_(bslice);

        // experimentally got set distribution
        // set[ 0 ] = 108
        // set[ 1 ] = 52
        // set[ 2 ] = 373
        // set[ 3 ] = 24
        // set[ 4 ] = 235
        // set[ 5 ] = 88
        // set[ 6 ] = 88
        // set[ 7 ] = 0
        //dlog() << " addr: " << addr << " hash val: " << h << endl;
        //dlog() << " set #: " << h % nsets << endl;

        return h % nsets;
    }
}

template <unsigned S, unsigned W, typename TAG, typename DATA>
unsigned
CACHE<S,W,TAG,DATA>::get_set_idx_rtl(unsigned long addr)
{

    return addr % nsets;
}

template <unsigned S, unsigned W, typename TAG, typename DATA>
void
CACHE<S,W,TAG,DATA>::fill(unsigned long addr, DATA const& data)
{
    unsigned set = nsets > 1 ? get_set_idx(addr) : 0;
    SET& s = sets_[set];
    auto w = s.find_way(addr);
    if (w.first) {
        cout << "-I- the line for addr: " << hex << addr << dec << " already filled\n";
        return;
    }
    unsigned way = w.second;
    s.fill(way, TAG()(addr), data);
}

template <unsigned S, unsigned W, typename TAG, typename DATA>
bool
CACHE<S,W,TAG,DATA>::lookup(unsigned long addr, unsigned& way, unsigned& set)
{
    if (!enabled()) return false;

    set = nsets > 1
        ? get_set_idx(addr) // set-associative
        : 0;                // fully-associative

    SET& s = sets_[set];
    auto w = s.find_way(addr);

    return w.first;
}


#if defined CACHE_TEST

template <typename T>
void touch_way(T& set, unsigned way)
{
    cout << "touch way " << way << ":\n";
    set.set_mru(way);
    cout << " lru:\n" << set.lru_to_string();
}

template <typename C>
void test_lru_algo()
{
    typename C::SET set;

    cout << "init lru:\n" << set.lru_to_string();
    touch_way(set, 3);
    touch_way(set, 7);
    touch_way(set, 4);
    touch_way(set, 5);
    touch_way(set, 6);
    touch_way(set, 2);
    touch_way(set, 1);
    touch_way(set, 4);
    touch_way(set, 0);
    cout << " lru ind: " << set.get_lru() << endl;
}

template <typename C, typename ADR>
void test_lookup(C& cache, ADR const& adrs)
{
    dlog() << "test cache lookup\n";
    // 1. fill cache foreach even addr
    typedef typename C::datatype dtype;
    dtype some_data;
    for (unsigned i = 0; i < adrs.size(); i+=2) {
        unsigned adr = adrs[i];
        cache.fill(adr, some_data);
    }

    for (auto const& addr: adrs) {
        unsigned way, set;
        bool hit = cache.lookup(addr, way, set);
        dlog() << boolalpha << "hit: " << hit << " way: " << way << " set: " << set << endl;
    }
}

int main()
{
    cout << "cache unit test" << endl;
    opts& _ = opts::instance();
    _.set_verbosity(true);

    typedef CACHE<8, 8, bitslicer(39:12), bitslicer(11:0)> L1TLB_t;
    L1TLB_t L1TLB;
    dlog() << "-D- cons: nsets: " << L1TLB.nsets << endl;

    test_lru_algo<L1TLB_t>();

    vector<unsigned long> small_addresses = {  0xfee0de104d
                                             , 0xfee0de024d
                                             , 0xfee0de124d
                                             , 0xfee0ddf00d
                                             , 0xfee0de000d
                                             , 0xfee0ddf20d
                                             , 0xfee0de020d
                                             , 0xfee0ddf02d
                                             , 0xfee0de020d 
                                             , 0xfee0ddf02d };
    test_lookup(L1TLB, small_addresses);

    for (auto const& addr: small_addresses) {
        cout << " addr: " << hex << addr << endl;
        unsigned set_no = L1TLB.get_set_idx(addr);
    }

    // read addresses from the file
    ifstream ifs("some_addresses");
    vector<unsigned> thrushed_sets(L1TLB.nsets);
    vector<unsigned long> addresses;
    unordered_set<unsigned long> uniq_addresses, uniq_4k_pages, uniq_64k_pages;
    if (ifs.good()) {
        cout << "-D- reading addresses from file\n";
        for (string line; getline(ifs, line); ) {
            uint64_t addr = stoul(line, nullptr, 16);
            addresses.push_back(addr);
            uniq_addresses.insert(addr);
            uniq_4k_pages.insert(addr >> 12);
            uniq_64k_pages.insert(addr >> 16);
            //cout << "-D- read str: " << line << " conv: " << hex << addr << endl;
            unsigned set_no = L1TLB.get_set_idx(addr);
            thrushed_sets[set_no] ++;
        }
    }
    cout << dec << "-D- " << addresses.size() << " addresses were read " << uniq_addresses.size() << " were uniq\n";
    for (unsigned i = 0; i < thrushed_sets.size(); i++) {
        cout << "set[ " << i << " ] = " << dec << thrushed_sets[i] << endl;
    }
    fill(thrushed_sets.begin(), thrushed_sets.end(), 0);
    for (auto addr : uniq_addresses) {
        unsigned set_no = L1TLB.get_set_idx(addr);
        thrushed_sets[set_no] ++;
    }
    for (unsigned i = 0; i < thrushed_sets.size(); i++) {
        cout << "uniq_set[ " << i << " ] = " << dec << thrushed_sets[i] << endl;
    }
    // self check - sum of elements in thrushed_sets via accumulate
    cout << "-D- " << uniq_addresses.size() << " uniq addresses " << accumulate(thrushed_sets.begin(), thrushed_sets.end(), 0) << endl;
    cout << "-D- " << uniq_4k_pages.size() << " uniq 4K pages " << uniq_64k_pages.size() << " uniq 64K pages" << endl;
}                                                     
                                                      
#endif                                                

