#ifndef __INCL_vm_H__
#define __INCL_vm_H__
// prevent multiple includes

// -*- C++ -*-


//! \file vm.hpp
//! \brief This header defines class \c VM.
//!
//! Detailed file description here.
//! \date Tuesday, May 18, 2010
//! \author Alexander Samoilov



// -------------- Forward declarations and includes -------------------

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// --------------------------------------------------------------------
//! Brief class description here.
//
//! Detailed class description
//! here.
// --------------------------------------------------------------------

// ------------------------ Class itself ------------------------------
class VM
{

  // typedefs and inner classes

public:

  typedef          int  int32_t; // true for many (all?) 32 & 64 bit Linuxes
  typedef unsigned int uint32_t; // ditto

  // more portable way :
  // use int32_t, uint_32 defined in <cstdint> with -std=c++0x g++ flag


  //! unary operation codes
  enum UNARY_OP {
    POP         , //  0
    PUSH_CONST  , //  1
    PUSH_IP     , //  2
    PUSH_SP     , //  3
    LOAD        , //  4
    STORE       , //  5
    JMP         , //  6
    NOT         , //  7
    PUTC        , //  8
    GETC        , //  9
    HALT        , // 10
  };

  //! binary operation codes
  enum BINARY_OP {
    ADD         , //  0
    SUB         , //  1
    MUL         , //  2
    DIV         , //  3
    AND         , //  4
    OR          , //  5
    XOR         , //  6
    EQ          , //  7
    LT          , //  8
  };

  //! for decoding VM instructions packed in 32bit words
  //! WARNING: for little endian order - Intel's processors, doesn't work for SUN
  struct INSTR {

    unsigned int optional   : 24;

    // operation code, one of either UNARY_OP or BINARY_OP
    unsigned int opcode     : 7;

    // 0 : unary, 1 : binary
    unsigned int binop_flag : 1;
  };

  // ------------------------------------------------------------------
  //! @name Special methods
  // ------------------------------------------------------------------
  //! @{

public:

  //! Default constructor.
  //
  //! Provided to disable compiler from generating it automatically.
  //! Specify if it is useful or disallow it by not defining its body.
  VM ();

  // NOTE: Copy constructor and operator would be created by
  //       compiler unless they are not explicitly defined.
  //       Just make stuff below private if they should never be used.

private:

  //! Copy constructor.
  //
  //! Provided to disallow compiler generate it automatically.
  //! Specify if it is useful or disable it by not defining it elsewhere.
  VM (const VM&);

  //! Copy operator.
  //
  //! Provided to disallow compiler generate it automatically.
  //! Specify if it is useful or disable it by not defining it elsewhere.
  VM& operator = (const VM&);

public:

  //! Destructor.
  //
  //! Destructors are usually virtual.
  //!
  virtual ~VM ();


  //! @}
  // ------------------------------------------------------------------

  // Class interface methods

public:

  /**
   * reads VM image from file @param fname
   * fills data_ container
   * @return image_size
   */
  int32_t load_image (std::string const& fname);

  //!
  void interprete ();

  // ------------------------------------------------------------------

  // Internal use and other special methods

private:

  //! reads 32bit integer from image stream
  int32_t read_int32 (std::istream& is);

  //! fetch to stack; stack grows downward
  void f (int32_t v) { data_[--sp_] = v; }

  //! get from stack
  int32_t g() { return data_[sp_++]; }

  //! interpretes unary operation, @return true for halt intruction,
  //! false otherwise
  bool unary (char opcode, unsigned optional);

  //! interpretes binary operation
  void binary (char opcode, unsigned optional);

  // ------------------------------------------------------------------

  // Data members

private:

  //! this is a VM image
  std::vector<int32_t> data_;

  //! instruction pointer
  int32_t ip_;

  //! stack pointer
  int32_t sp_;

  // ------------------------------------------------------------------

}; // class VM


// ----------- Inline implementations for 'VM'

//
// inlines here
//

// prevent multiple includes
#endif
