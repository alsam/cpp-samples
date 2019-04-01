#ifndef __INCL_get_opt_H__
#define __INCL_get_opt_H__
// prevent multiple includes

// -*- C++ -*-

//! \file get_opt.h
//! \brief This header defines class \c get_opt.
//!
//! Detailed file description here.
//! \date Friday, January 22, 2010
//! \author Alexander Samoilov



// -------------- Forward declarations and includes -------------------

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for exit
#include "logger.h"

// --------------------------------------------------------------------
//! usage : parsing and storing command line parameters
//
//! implemented as the Meyers Singleton design pattern
//! 
// --------------------------------------------------------------------

// ------------------------ Class itself ------------------------------
struct get_opt
{

  // typedefs and inner classes

public:

  // ------------------------------------------------------------------
  //! @name Special methods
  // ------------------------------------------------------------------
  //! @{

private:

  //! Default constructor : hidden.
  get_opt () : verbose(false), log(std::cout, false) {}

  //! Copy constructor : hidden.
  get_opt (const get_opt&);

  //! Copy operator : hidden.
  get_opt& operator = (const get_opt&);

  //! @}
  // ------------------------------------------------------------------

  // Class interface methods

public:

  //! implementation: The Meyers Singleton
  static get_opt& instance() {
    static get_opt opt;
    return opt;
  }

  //! parse command line
  void parse_cmdline(int argc, char** argv) {

    if (argc < 2) {
      std::cout << "tlp input file is expected\n";
      std::exit(-1);
    }

    argc--; argv++;
    while (argc && **argv == '-') {
      switch (argv[0][1]) {

        case 'v':
          verbose = true;
          log.set_verbosity (verbose);
          break;

        case 'l':
          argc--; argv++;
          if (argv[0]) {
            log_name = argv[0];
            log_strm.open (argv[0]);
            if (log_strm.good()) {
              log.redirect (log_strm);
            }
          }
          break;

        case 'o':
          argc--; argv++;
          if (argv[0]) {
            out_name = argv[0];
            out_strm.open (argv[0]);
          }
          break;

        default:
          break;

      }

      argc--; argv++;

    }
    inp_name = argv[0];
  }


  // ------------------------------------------------------------------

  // Data members : public

  std::string inp_name, log_name, out_name;

  bool verbose;

  logger log;

  std::ofstream log_strm, out_strm;

  // ------------------------------------------------------------------

}; // class get_opt

inline logger& dlog()
{
  get_opt& _ = get_opt::instance();
  return _.log;
}

inline std::ofstream& out()
{
  get_opt& _ = get_opt::instance();
  return _.out_strm;
}

// prevent multiple includes
#endif
