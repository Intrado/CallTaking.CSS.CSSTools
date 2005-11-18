#include <string>
#include "util.h"
#ifdef _LEAK_DETECT
    #undef new
    #endif
    #include <fstream>
    #ifdef _LEAK_DETECT
    #define new new(detectLeak, __FILE__, __LINE__)
    #endif
#include <iostream>
using namespace std;
#include "ToStr.h"
#include "cTime.h"
  
extern const CSSTOOLS_API bool util::TraceOn = false;
extern const CSSTOOLS_API bool util::ERR = false;

/* **** Out_String **** */
void util::
Out_String (string msg)
{
  try
  {
    cout << msg << flush;
    if (ERR == true)
    {
		cerr << msg << flush;
    };
  }
  catch (...)
  {
  }
  ;
}

void util::
Out_Char (char c)
{
/* **** Out_Char **** */
	cout << c << flush;
  if (ERR == true)
  {
	  cerr << c << flush;
  };
}

void util::
Trace (string module, string function, string msg)
{
  cTime currTime;
  cTemplateString time;
  time = currTime.FormatLocal("%H:%M:%S");
  Out_String (string(time) + " " + module + " " + function + msg + "\n");
}

string util::Toupper (string TheText)
{
  string TheUpperText;
  char UpperChar;
  int Lg;
  TheUpperText = "";
  Lg = (TheText).length();
  for (int I = 0; I <= Lg - 1; I++)
  {
    UpperChar = toupper(TheText[I]);
    TheUpperText = (TheUpperText + UpperChar);
  };
  return TheUpperText;
}

namespace util
{
  class util_static
  {
    public:
      util_static ()
    {
    };
  };
  static util_static util_static_instance;
}
