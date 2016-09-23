// -*- C++ -*-


//---------------------------------------------------------------------
//  Created:            Thursday, September 28, 2012
//  Original author:    Alexander Samoilov
//---------------------------------------------------------------------

//
//   Brief file description here
//

#ifndef __OPTS_HPP__
#define __OPTS_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for exit
#include "logger.hpp"

// --------------------------------------------------------------------
//! usage : parsing and storing command line parameters
//
//! implemented as the Meyers Singleton design pattern
//! 
// --------------------------------------------------------------------

template <typename T> struct conv              { T           operator()(const char* s) {return T(s);          } };
template<>            struct conv<char>        { int         operator()(const char* s) {return s[0];          } };
template<>            struct conv<int>         { int         operator()(const char* s) {return std::atoi(s);  } };
template<>            struct conv<unsigned>    { unsigned    operator()(const char* s) {return std::atoi(s);  } };
template<>            struct conv<size_t>      { size_t      operator()(const char* s) {return std::atol(s);  } };
template<>            struct conv<float>       { double      operator()(const char* s) {return std::atof(s);  } };
template<>            struct conv<double>      { double      operator()(const char* s) {return std::atof(s);  } };
template<>            struct conv<std::string> { std::string operator()(const char* s) {return std::string(s);} };

// ------------------------ Class itself ------------------------------
struct opts
{

  // typedefs and inner classes

public:

    // ------------------------------------------------------------------
    //! @name Special methods
    // ------------------------------------------------------------------
    //! @{

private:

    //! Default constructor : hidden.
    opts ()
    : argc_(0)
    , argv_(0)
    , verbose             (false)
    , eval                (false)
    , simple_mem_trace    (false)
    , yml_mem_trace       (false)
    , log                 (std::cout, false)
    , verbosity_level     (0)
    , verbosity_threshold (0)
    , full_screen         (false)
    , fp32                (false)
    , clmap               (false)
    , subw_x              (64)
    , subw_y              (64)
    {}
  
    //! Copy constructor : hidden.
    opts (const opts&);
  
    //! Copy operator : hidden.
    opts& operator = (const opts&);
  
    //! @}
    // ------------------------------------------------------------------
  
    // Class interface methods

public:

    void set_verbosity(bool v) { verbose = v; log.set_verbosity(v); }

    //! implementation: The Meyers' Singleton
    static opts& instance() {
        static opts opt;
        return opt;
    }
  
    //! parse command line
    void parse_cmdline(int argc, char** argv) {
        argc_ = argc;
        argv_ = argv;
        argc_--; argv_++;
        while (argc_ && **argv_ == '-') {
            switch (argv_[0][1]) {

            case 'v':
                set_verbosity (true);
                break;

            case 'l':
                to<std::string>(log_name);
                log_strm.open(log_name);
                if (log_strm.good()) {
                    log.redirect (log_strm);
                }
                break;

            case 's': // read simple memtrace
                simple_mem_trace = true;
		to<std::string>(inp_name);
                break;

            case 'i': // yml memtrace
                yml_mem_trace = true;
		to<std::string>(inp_name);
                break;

            case 'c':
                to<std::string>(inp_name);
                break;

            case 'm':
                clmap = true;
                break;

            case 'a':
                to<std::string>(inp_cache_amap_name);
                break;

            case 'w':
                to<std::string>(inp_world_amap_name);
                break;

            // an experiment for evaluating # of pages
            case 'e':
                eval = true;
                break;

            case 'd':
                to<int>(verbosity_level);
                break;

            case 'k':
                to<int>(verbosity_threshold);
                break;

            case 'f':
                full_screen = true;
                break;

            case 'p':
                do {
                    int ival;
                    to<int>(ival);
                    fp32 = ival == 32;
                } while( 0 );
                break;

            case 'x':
                to<int>(subw_x);
                break;

            case 'y':
                to<int>(subw_y);
                break;

            default:
                break;

            }
            advance();
        }
        if (argc_ > 0) inp_name =  std::string(argv_[0]);
        log.set_verbosity(verbose);
    }

    // ------------------------------------------------------------------

    // Data members : private
private:

    int    argc_;
    char** argv_;

    void advance() { argc_--; argv_++; }

    template <typename T>
    void to(T& ret) {
        conv<T> cv;
        if (argv_[0][2] != 0) {
            ret = cv(&argv_[0][2]);
        } else {
            advance();
            if (argv_[0]) {
                ret = cv(argv_[0]);
            }
        }
    }

    // ------------------------------------------------------------------

    // Data members : public
public:

    bool          verbose;
    bool          eval;
    bool          simple_mem_trace;
    bool          yml_mem_trace;
    logger        log;
    int           verbosity_level;
    int           verbosity_threshold;
    bool          full_screen;
    bool          fp32;
    bool          clmap;
    int           subw_x, subw_y;
    std::string   inp_name, inp_cache_amap_name, inp_world_amap_name, log_name;
    std::ofstream log_strm, out_strm;

    // ------------------------------------------------------------------

}; // class opts

inline logger& dlog()
{
  opts& _ = opts::instance();
  return _.log;
}

#endif // __OPTS_HPP__
