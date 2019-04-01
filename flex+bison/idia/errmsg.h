/*
 * =====================================================================================
 * 
 *       Filename:  errmsg.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  02/06/2010 08:55:08 PM MSK
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  Alexander Samoilov (Mr)
 * 
 * =====================================================================================
 */

#ifndef  __ERRMSG_FILE_HEADER_INC__
#define  __ERRMSG_FILE_HEADER_INC__

#include <string>
#include <iostream>

class fileinfo {
  std::string filename_;
  size_t lineNum_;

public:
  fileinfo(std::string filename, size_t lineNum=1)
    : filename_(filename), lineNum_(lineNum) {}

  size_t line() const { return lineNum_; }
  
  std::string const& name() const { return filename_; }
 
  // Specifies a newline symbol at the character position given.
  void newline() { ++lineNum_; }
  
};

inline bool operator == (const fileinfo& a, const fileinfo& b)
{
  return a.line() == b.line() && a.name() == b.name();
}


class position {
  fileinfo *file_;
  size_t line_;
  size_t column_;

public:
  void init(fileinfo *f, int p) {
    if (file_ = f) {
      line_ = file_->line();
      column_ = p;
    } else {
      line_ = column_ = 0;
    }
  }

  std::string filename() const { return file_ ? file_->name() : ""; }
  
  size_t line() const { return line_; }
  
  size_t column() const { return column_; }
  
  bool match(std::string const& s) { return file_ && file_->name() == s; }
  
  bool match(size_t l) { return line_ == l; }
  
  bool matchColumn(size_t c) { return column_ == c; }
  
  bool operator! () const { return (file_ == 0); }
  
  //friend ostream& operator << (ostream& out, const position& pos);

};



#endif   /* ----- #ifndef __ERRMSG_FILE_HEADER_INC__  ----- */

