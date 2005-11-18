// cTemplateString.h: interface for the cTemplateString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTEMPLATESTRING_H__20A66CF3_1E80_11D3_8557_0008C7CB118E__INCLUDED_)
#define AFX_CTEMPLATESTRING_H__20A66CF3_1E80_11D3_8557_0008C7CB118E__INCLUDED_

#include "CSSTools.h"

class CSSTOOLS_API cTemplateString  
{
public:
	cTemplateString();
	cTemplateString(const bool   copyFrom);
	cTemplateString(const int    copyFrom);
	cTemplateString(const double copyFrom);
	cTemplateString(const char* const copyFrom);
	cTemplateString(const cTemplateString& copyFrom);
	virtual ~cTemplateString();
  virtual int size() const;
  virtual cTemplateString& operator= (const cTemplateString& copyFrom);
  virtual cTemplateString& operator= (const char* const copyFrom);
  virtual bool operator== (const char* compareTo) const;
  virtual bool operator== (const cTemplateString& compareTo) const;
  virtual int compare(const cTemplateString& compareTo) const;
  int compareNoCase(const cTemplateString& compareTo) const;
  virtual int compare(int startPos, int maxCompare, const cTemplateString& compareTo) const;
  virtual cTemplateString operator+ (const cTemplateString& addTo) const ;
  virtual cTemplateString operator+ (const char* const addTo) const ;
  virtual cTemplateString& operator+=(const cTemplateString& addTo);
  virtual bool operator<(const cTemplateString& compareTo) const;
  virtual const char*  c_str() const;
  virtual bool empty() const;
  virtual int  find(const char* const stringToFind, int index = 0) const;
  virtual int  find(const char charToFind, int index = 0) const;
  virtual int  rfind(const char* const stringToFind, int index = -1) const;
  virtual int  rfind(const char charToFind, int index = -1) const;
  virtual cTemplateString& insert(int index,const char* stringToInsert);
  virtual cTemplateString& insert(int index,const cTemplateString& stringToInsert);
  virtual bool operator !=(const cTemplateString& isDiff) const;
  virtual bool operator !=(const char* isDiff) const;
  virtual cTemplateString substr(int from, int to = -1) const;
  virtual cTemplateString& format(const char* format,...);

  cTemplateString& erase(int From = 0, int NberOfChar = -1);
  cTemplateString& append(const char* const str, int pos, int n);
  cTemplateString& append(int n, char c);
  void reserve(int minSize);
  int capacity();
  cTemplateString upper() const;
  cTemplateString lower() const;

  cTemplateString& replace(int nFrom, int nUpTo, const char* pReplacement, int nReplacementPos = 0, int nReplacementUpTo = -1);
  cTemplateString& replace(int nFrom, int nUpTo, const char cReplacement, int nNBCopies = 1);

  typedef enum {BadCast} exception;
  virtual operator const char* () const; 
  virtual operator bool() const;         // may throw BadCast
  virtual operator int() const;          // may throw BadCast
  virtual operator double() const ;      // may throw BadCast
public:
  void* data;
  const char* lastValue;
};

inline cTemplateString operator +(const char* left, const cTemplateString& right) {
  cTemplateString retVal = left; retVal+=right;
  return retVal;
}

#endif // !defined(AFX_CTEMPLATESTRING_H__20A66CF3_1E80_11D3_8557_0008C7CB118E__INCLUDED_)
