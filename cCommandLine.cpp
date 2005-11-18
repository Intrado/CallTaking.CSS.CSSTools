#include "cCommandLine.h"  
#include "windows.h"

#pragma warning (disable:4786)
#include <deque>
#include <string>

using namespace std;

cCommandLine::cCommandLine(bool aExtractCommandLine)
{
  mArgc = 0;
  mArgv = NULL;
  mpCommandLine = NULL;
  if (aExtractCommandLine)
    ExtractCommandLine();
}

cCommandLine::cCommandLine(char* apFromString)
{
  mpCommandLine = NULL;
  cCommandLine::FromString(apFromString);
}

cCommandLine::~cCommandLine()
{ 
  Free();
}

bool cCommandLine::ExtractCommandLine()
{
  Free();

  wchar_t* pCommandLine = GetCommandLineW();
  wchar_t** unicodeArgv;

  int commandLineSize = wcslen(pCommandLine) + 1;
  mpCommandLine = new char[commandLineSize];
  WideCharToMultiByte(CP_ACP, 0, pCommandLine, -1, mpCommandLine, commandLineSize, NULL, NULL);		
	
	unicodeArgv = CommandLineToArgvW(pCommandLine, &mArgc);

  if (mArgc != 0)
  {
    mArgv = new char*[mArgc];
    for(int i = 0; i < mArgc; i++)
	  {
      int stringSize = wcslen(unicodeArgv[i]);
      mArgv[i] = new char[stringSize + 1];
      WideCharToMultiByte(CP_ACP, 0, unicodeArgv[i], -1, mArgv[i], stringSize + 1, NULL, NULL);		
	  }
  }
  else
  {
    mArgv = 0;
  }
  GlobalFree(unicodeArgv);
  return true;
}

// Generate an argv from a given string
bool cCommandLine::FromString(const char* apString)
{ 
  if (!apString)
    return false;

  Free();

  mpCommandLine = new char[strlen(apString) + 1];
  strcpy(mpCommandLine, apString);
  
  deque<string> tokens;
  mArgc = 0;
  char* token = strtok(mpCommandLine, " ");
  while( token != NULL )
  {
    tokens.push_back(token);
    mArgc++;
    token = strtok(NULL, " ");
  }

  // Create a proper sized argv and copy the data
  if (mArgc != 0)
  {
    mArgv = new char*[mArgc];
    for (int i = 0 ; i < mArgc ; i++)
    {
      mArgv[i] = new char[tokens.at(i).size() + 1];
      strcpy(mArgv[i], tokens.at(i).c_str());
    }
  }
  else
  {
    mArgv = 0;
  }

  strcpy(mpCommandLine, apString);
  return true;
}

void cCommandLine::Free()
{
  delete [] mpCommandLine;
  if (mArgv != NULL)
  {
    for (int i = 0 ; i < mArgc ; i++)
    {
      delete [] mArgv[i];
    }
    delete [] mArgv;
  }
  mArgv = 0;
  mArgc = 0;
}

int cCommandLine::GetArgc()
{
  return mArgc;
}

char** cCommandLine::GetArgv()
{
  return mArgv;
}

int cCommandLine::FindSwitch(const char *apSwitch)
{
  for (int i = 0 ; i < mArgc ; i++)
  {
    if (stricmp(mArgv[i], apSwitch) == 0)
    {
      return i;
    }
  }
  return -1;
}

// Returns the unparsed command line
char* cCommandLine::GetOriginalCommandLine()
{
  return mpCommandLine;
}
