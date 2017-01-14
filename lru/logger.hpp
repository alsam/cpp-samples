#pragma once
// -*- C++ -*-


//! \file logger.h
//! \brief This header defines class \c logger.
//!
//! Detailed file description here.
//! \date Friday, January 22, 2010
//! \author Alexander Samoilov



// -------------- Forward declarations and includes -------------------
#include <iostream>
#include <fstream>
#include <stdexcept>

// --------------------------------------------------------------------
//! Brief class description here.
//
//! Detailed class description
//! here.
// --------------------------------------------------------------------

// ------------------------ Class itself ------------------------------
class logger
{

  // typedefs and inner classes

public:

  // ------------------------------------------------------------------
  //! @name Special methods
  // ------------------------------------------------------------------
  //! @{

public:

  //! Default constructor.
  //
  //! Provided to disable compiler from generating it automatically.
  //! Specify if it is useful or disallow it by not defining its body.
  logger ();

  //! a real constructor
  logger (std::ostream& out, bool verbose = true) : verbose_(verbose) {
    out_ = new std::ostream (out.rdbuf());
  }

  // NOTE: Copy constructor and operator would be created by
  //       compiler unless they are not explicitly defined.
  //       Just make stuff below private if they should never be used.

private:

  //! Copy constructor.
  //
  //! Provided to disallow compiler generate it automatically.
  //! Specify if it is useful or disable it by not defining it elsewhere.
  logger (const logger&);

  //! Copy operator.
  //
  //! Provided to disallow compiler generate it automatically.
  //! Specify if it is useful or disable it by not defining it elsewhere.
  logger& operator = (const logger&);

public:

  //! Destructor.
  //
  //! Destructors are usually virtual.
  //!
  virtual ~logger () {close();}

  //! @}
  // ------------------------------------------------------------------

  // Class interface methods

public:

  //! set verbosity
  logger& set_verbosity (bool verbose) {
    verbose_ = verbose;
    return *this;
  }

  //! @return verbosity
  bool verbose () const {return verbose_;}

  //! redirect log stream
  void redirect (std::ostream& out) {out_->rdbuf(out.rdbuf());}

  void open (std::string const& name) {
    close();
    out_ = new std::ofstream(name.c_str());
    if (!out_->good()) {
      throw std::ios::failure("cannot open log file");
    }
  }

  void close () {
    delete out_;
  }

  //! @return log stream
  std::ostream& log () {return *out_;}

  //! @}
  // ------------------------------------------------------------------

  // Class interface methods

public:

  // ------------------------------------------------------------------

  // Internal use and other special methods

private:

  // ------------------------------------------------------------------

  // Data members

private:

  //! only two verbosity levels : true or false
  bool verbose_;

  //! logging stream
  std::ostream* out_; 

  // ------------------------------------------------------------------

}; // class logger


// a special fix for static const class data members
#define FIX_LOGGER_FOR_PRIMITIVE_TYPES(TYPE)  \
inline                                        \
logger& operator<< (logger &log, TYPE val) {  \
  if (log.verbose()) {                        \
    log.log() << val;                         \
  }                                           \
  return log;                                 \
}

FIX_LOGGER_FOR_PRIMITIVE_TYPES(int)
FIX_LOGGER_FOR_PRIMITIVE_TYPES(unsigned)
FIX_LOGGER_FOR_PRIMITIVE_TYPES(long)
FIX_LOGGER_FOR_PRIMITIVE_TYPES(unsigned long)
FIX_LOGGER_FOR_PRIMITIVE_TYPES(float)
FIX_LOGGER_FOR_PRIMITIVE_TYPES(double)

template <typename T>
inline
logger& operator<< (logger &log, T& val) {
  if (log.verbose()) {
    log.log() << val;
  }
  return log;
}

template <typename T>
inline
logger& operator<< (logger &log, T const& val) {
  if (log.verbose()) {
    log.log() << val;
  }
  return log;
}

//! override manipulators such as std::endl
inline
logger& operator<< (logger& log, std::ostream &(*pf)(std::ostream&)) {
  if (log.verbose()) {
    pf(log.log());
  }
  return log;
}

// prevent multiple includes
