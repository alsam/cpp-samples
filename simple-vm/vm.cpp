// -*- C++ -*-


//! \file vm.cpp
//! \brief This file defines implementation for class \c VM.
//!
//! Detailed file description here.
//! \date Tuesday, May 18, 2010
//! \author Alexander Samoilov

#include <cassert>
#include <stdexcept>
#include "vm.hpp"
#include "get_opt.hpp"

using namespace std;


// Default constructor implementation.
// Delete it if default construction is not required in class VM.
// NOTE: Do not remove its declaration from header file. Otherwise compiler
//       will create it automatically.
VM :: VM ()
  // initialization list here
  : ip_(0)
{}

// Destructor implementation.
VM :: ~VM ()
{}

int32_t
VM::read_int32 (istream& is)
{
  string line_buf;
  getline (is, line_buf);
  istringstream iss (line_buf);
  uint32_t ret;
  iss >> hex >> ret;
  dlog() << "line: " << line_buf << " value: " << ret << endl;
  return int32_t(ret);
}

int32_t
VM::load_image (string const& image_name)
{
  ifstream is (image_name.c_str());
  if (!is)
    throw std::runtime_error(std::string("unable to open input file " + image_name));

  sp_ = read_int32 (is);
  dlog() << "data_size : " << sp_ << endl;
  data_.resize(sp_);
  int32_t image_size = read_int32 (is);
  dlog() << "image_size : " << image_size << endl;
  for (size_t i = 0; i < image_size; ++i) {
    int32_t value = read_int32 (is);
    dlog() << "value: " << value << std::endl;
    data_[i] = value;
  }
  return image_size;
}

void
VM::interprete ()
{
  assert (ip_ == 0);
  assert (sp_ == data_.size()); 

  for (;;) {

    assert (ip_ < data_.size());

    const INSTR *instr = reinterpret_cast<const INSTR*>(&data_[ip_]);
    dlog() << "ip: " << ip_ << " binop: " << instr->binop_flag
           << " op: " << instr->opcode << " opt: " << instr->optional << endl;

    if (instr->binop_flag) {
      binary(instr->opcode, instr->optional);
    } else {
      if (unary(instr->opcode, instr->optional)) { // halt
        return;
      }
    }

    ip_ = ip_ + 1;
  }
}

bool
VM::unary (char opcode, unsigned optional)
{
  int32_t addr, st_data, cond, x;
  switch (opcode) {
  case POP:
    dlog() << "pop\n";
    sp_ = sp_ + 1;
    return false;

  case PUSH_CONST:
    dlog() << "push <const> " << optional << endl;
    f(optional);
    return false;

  case PUSH_IP:
    dlog() << "push ip\n";
    f(ip_ + 1); // test05.bin : ip_ points to the next instruction
    return false;

  case PUSH_SP:
    dlog() << "push sp\n";
    f(sp_);
    return false;

  case LOAD:
    addr = g();
    dlog() << "load " << addr << endl;
    f(data_[addr]);
    return false;

  case STORE:
    dlog() << "store\n";
    st_data = g();
    addr = g();
    data_[addr] = st_data;
    return false;

  case JMP:
    cond = g();
    addr = g();
    dlog() << "jmp cond: " << cond << " addr: " << addr << endl;
    if (cond) ip_ = addr - 1; // test08.bin : ip_ would be incremented to correct value in interprete
    return false;

  case NOT:
    dlog() << "not\n";
    if (g() == 0) f(1); else f(0);
    return false;

  case PUTC:
    x = g();
    dlog() << "putc " << x << endl;
    //putchar (g() & 0xff);
    putchar (x & 0xff);
    return false;

  case GETC:
    dlog() << "getc\n";
    x = getchar ();
    f(x & 0xff);
    return false;

  case HALT:
    dlog() << "halt\n";
    return true;
  }
}

void
VM::binary(char opcode, unsigned optional)
{
  int32_t b = g(), a = g();
  int32_t r;

  switch (opcode) {
  case ADD: dlog() << "add\n"; r = a + b; break;
  case SUB: dlog() << "sub\n"; r = a - b; break;
  case MUL: dlog() << "mul\n"; r = a * b; break;
  case DIV: dlog() << "div\n"; r = a / b; break;
  case AND: dlog() << "and\n"; r = a & b; break;
  case OR:  dlog() << "or\n";  r = a | b; break;
  case XOR: dlog() << "xor\n"; r = a ^ b; break;
  case EQ:  dlog() << "eq\n";  r = !!(a == b); break;
  case LT:  dlog() << "lt\n";  r = !!(a < b); break;
  }

  f(r);
}


