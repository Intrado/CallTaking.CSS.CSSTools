#ifndef CCOMMANDLINE_H
#define CCOMMANDLINE_H

#include "CSSTools.h"

class CSSTOOLS_API cCommandLine
{
public:  
  cCommandLine(bool aExtractCommandLine = true);
  cCommandLine(char* apFromString);  
  virtual ~cCommandLine();

  // Extract the process command-line and generate an argv
  bool ExtractCommandLine();

  // Returns the unparsed command line
  char* GetOriginalCommandLine();

  // Generate an argv from a given string
  bool FromString(const char* apFromString);

  // Search for an item in the argv array, returns -1 if not found
  int FindSwitch(const char *apSwitch);

  // Returns the argc value
  int GetArgc();

  // Returns the argv array
  char** GetArgv();

private:
  char* mpCommandLine;
  void Free();
  int mArgc;
  char** mArgv;
  
};

#endif