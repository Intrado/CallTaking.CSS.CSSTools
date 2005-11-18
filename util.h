#ifndef __util_INCLUDED__
#define __util_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include "CSSTools.h"

namespace util
{
  extern CSSTOOLS_API const bool TraceOn;
  extern CSSTOOLS_API const bool ERR;
  CSSTOOLS_API std::string Toupper (std::string TheText);
  CSSTOOLS_API void Out_Char (char c);
  CSSTOOLS_API void Out_String (std::string msg);
  CSSTOOLS_API void Trace (std::string module, std::string function, std::string msg = "  Starting");
};
#endif
