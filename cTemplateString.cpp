// cTemplateString.cpp: implementation of the cTemplateString class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>

#include "cTemplateString.h"

#pragma warning(disable:4786)

#include <string>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if defined(_DEBUG)
// For debug purposes
string* last;
#define ACC(var) ((string*)var)
#else
#define ACC(var) ((string*)var)
#endif
const char* const strTrue = "true";
const char* const strFalse = "false";

cTemplateString::cTemplateString(const cTemplateString& copyFrom)
{
  data = new string;
  *this = copyFrom;
  lastValue = ACC(data)->c_str();
}
cTemplateString::cTemplateString()
{
  data = new string;
  lastValue = ACC(data)->c_str();   // Coverity 14937: initialize pointer field: lastValue
}
cTemplateString::cTemplateString(const bool   copyFrom)
{
  data = new string;
  operator=(copyFrom?strTrue:strFalse);
}
cTemplateString::cTemplateString(const int    copyFrom)
{
  data = new string;
  format("%d",copyFrom);
}
cTemplateString::cTemplateString(const double copyFrom)
{
  data = new string;
  format("%lf",copyFrom);
}


cTemplateString::cTemplateString(const char* const copyFrom)
{
  data = new string;
  *this = copyFrom;
  lastValue = ACC(data)->c_str();
}

cTemplateString::~cTemplateString()
{
  delete ACC(data);
}

int cTemplateString::size() const
{
  return ACC(data)->size();
}

cTemplateString& cTemplateString::operator=(const cTemplateString& copyFrom)
{
  *ACC(data) = *ACC(copyFrom.data);
  lastValue = ACC(data)->c_str();
  return *this;
}

cTemplateString& cTemplateString::operator= (const char* const copyFrom)
{
  *ACC(data) = copyFrom;
  lastValue = ACC(data)->c_str();
  return *this;
}

bool cTemplateString::operator== (const cTemplateString& compareTo) const
{
  return ACC(data)->compare(*ACC(compareTo.data)) == 0;
}

int cTemplateString::compare(const cTemplateString& compareTo) const
{
  return ACC(data)->compare(*ACC(compareTo.data));
}

int cTemplateString::compareNoCase(const cTemplateString& compareTo) const
{
  return _stricmp(lastValue, compareTo.lastValue);
}

int cTemplateString::compare(int startPos, int maxCompare, const cTemplateString& compareTo) const
{
  return ACC(data)->compare(startPos, maxCompare, *ACC(compareTo.data));
}

bool cTemplateString::empty() const 
{
  return ACC(data)->empty();
}

bool cTemplateString::operator !=(const cTemplateString& addTo) const
{
  return (*ACC(data) != *ACC(addTo.data));
}

int cTemplateString::find(const char* const stringToFind,int index) const
{
  return ACC(data)->find(stringToFind, index);
}

int cTemplateString::find(const char charToFind, int index) const
{
  return ACC(data)->find(charToFind, index);
}

int cTemplateString::rfind(const char* const stringToFind, int index) const
{
  return ACC(data)->rfind(stringToFind, index);
}

int cTemplateString::rfind(const char charToFind, int index) const
{
  return ACC(data)->rfind(charToFind, index);
}

cTemplateString& cTemplateString::insert(int index,const char* stringToInsert)
{
  ACC(data)->insert(index,stringToInsert);
  lastValue = ACC(data)->c_str();
  return *this;
}

cTemplateString& cTemplateString::insert(int index,const cTemplateString& stringToInsert)
{
  ACC(data)->insert(index,*ACC(stringToInsert.data));
  lastValue = ACC(data)->c_str();
  return *this;
}

const char*  cTemplateString::c_str() const 
{
  return ACC(data)->c_str();
}

cTemplateString cTemplateString::operator+ (const cTemplateString& addTo) const
{
  cTemplateString result((*ACC(data) +  *ACC(addTo.data)).c_str());
  return result;
}

cTemplateString cTemplateString::operator+(const char* const addTo) const
{
  cTemplateString result((*ACC(data) +  addTo).c_str());
  return result;
}

cTemplateString& cTemplateString::operator+=(const cTemplateString& addTo)
{
  *ACC(data)+= addTo.c_str();
  lastValue = ACC(data)->c_str();
  return *this;
}

cTemplateString cTemplateString::substr(int from, int to) const
{
  cTemplateString result(ACC(data)->substr(from,to).c_str());
  return result;
}

bool cTemplateString::operator<(const cTemplateString& compareTo) const
{
  return (*ACC(data) < *ACC(compareTo.data));
}

cTemplateString& cTemplateString::format(const char* format,...)
{
  int result = 0;
  const int bufSize = 1024;
  char tempBuffer[bufSize];
  va_list argList;
  va_start(argList, format);
  result = _vsnprintf(tempBuffer, bufSize, format, argList);
  va_end(argList);
  *this = tempBuffer;
  return *this;
}

cTemplateString& cTemplateString::erase(int From, int NberOfChar)
{
  ACC(data)->erase(From, NberOfChar);
  lastValue = ACC(data)->c_str();
  return *this;
}

cTemplateString& cTemplateString::append(const char* const str, int pos, int n)
{
  ACC(data)->append(str, pos, n);
  lastValue = ACC(data)->c_str();
  return *this;
}

cTemplateString& cTemplateString::append(int n, char c)
{
  ACC(data)->append(n, c);
  lastValue = ACC(data)->c_str();
  return *this;
}

cTemplateString::operator bool() const
{
  if(compareNoCase(strTrue) == 0) return true;
  if(compareNoCase(strFalse) == 0) return false;
  throw(BadCast);
  return false;
}

cTemplateString::operator int() const
{
  int res;
  if(sscanf(c_str(),"%d",&res) != 1) throw(BadCast);
  return res;
}

cTemplateString::operator double() const 
{
  double res;
  if(sscanf(c_str(),"%lf",&res) != 1) throw(BadCast);
  return res;
}

cTemplateString::operator const char* () const
{
  return c_str();
}

bool cTemplateString::operator== (const char* compareTo) const
{
  return operator==(cTemplateString(compareTo));
}

bool cTemplateString::operator !=(const char* isDiff) const
{
  return operator!=(cTemplateString(isDiff));
}

void cTemplateString::reserve(int minSize)
{
  ACC(data)->reserve(minSize);
}

cTemplateString cTemplateString::upper() const
{
  string sUpper = *ACC(data);

  for(unsigned int i = 0; i < sUpper.size(); i++)
  {
    sUpper[i] = toupper(sUpper[i]);
  }

  return sUpper.c_str();
}

cTemplateString cTemplateString::lower() const
{
  string sLower = *ACC(data);

  for(unsigned int i = 0; i < sLower.size(); i++)
  {
    sLower[i] = tolower(sLower[i]);
  }

  return sLower.c_str();
}

cTemplateString& cTemplateString::replace(int nFrom, int nUpTo, const char* pReplacement, int nReplacementPos, int nReplacementUpTo)
{
  ACC(data)->replace(nFrom, nUpTo, pReplacement, nReplacementPos, nReplacementUpTo);
  lastValue = ACC(data)->c_str();
  return *this;
}

cTemplateString& cTemplateString::replace(int nFrom, int nUpTo, const char cReplacement, int nNBCopies)
{
  ACC(data)->replace(nFrom, nUpTo, nNBCopies, cReplacement);
  lastValue = ACC(data)->c_str();
  return *this;
}

int cTemplateString::capacity()
{
  return ACC(data)->capacity();
}
