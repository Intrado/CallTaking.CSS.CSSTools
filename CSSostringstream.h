/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2001.
 * ALL RIGHTS RESERVED.
 *
 *  created by:    fslight       date created:    2001/02/23
 * %derived_by:   fslight %       %date_modified:  2001/02/23 14:14:04 %
 *
 * %name:         CSSostringstream.h %
 * %version:      6 %
 *
 * Description:
 *
 * Object that adds an operator= to STL ostringstream to initialize the buffer with a value and position the
 * stream pointer to begin writing at the end of the buffer (instead of the beginning as str("xyz") does).
 * In the code, "<< hex" sets the mode to hexadecimal (for number inserted in the stream), and "<< dec"
 * sets the mode to decimal.
 *
 ***************************************************************************/

#ifndef __CSSostringstream_h_INCLUDED__
#define __CSSostringstream_h_INCLUDED__


#pragma warning(disable: 4786)
#include <sstream>

struct tCSSostringstream: public std::ostringstream
{
  tCSSostringstream()
  {
    setf(ios_base::showbase);
    str("");
  }

  std::basic_ostream<char>& operator=(const char *pInput)
  {
    str("");
    write(pInput, strlen(pInput));
    return *this;
  }

  std::basic_ostream<char>& operator=(std::string input)
  {
    str("");
    write(input.c_str(), input.size());
    return *this;
  }

  // This operator= copies the stream contained in src into this stream.
  void operator=(const tCSSostringstream &src) { operator=(src.str()); };
  bool operator==(const char *pRhs) { std::string s(pRhs); return str() == s; }
  bool operator==(const std::string &rhs) { return str() == rhs; }
  bool operator==(const tCSSostringstream &rhs) { return operator==(rhs.str()); }
  virtual bool empty() { return str().empty(); }
};


#endif
