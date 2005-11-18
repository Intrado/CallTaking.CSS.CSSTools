/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    sroy              date created:    2000/03/13
 *
 * name:          CSSObject.h
 * version:       1
 *
 *
 * Description:
 *
 *     Ancestor object for all telephony objects.
 *
	Last change:  SR    7 Apr 2000    4:10 pm
 ***************************************************************************/

// Disable annoying warnings
#pragma warning(disable:4251)
#pragma warning(disable:4786)
#pragma warning(disable:4240)
#pragma warning(disable:4291)


#ifndef __CSSObject__
#define __CSSObject__

#include "CSSTools.h"

#define isType(ref,type) (dynamic_cast<type>(ref) != NULL)

class CSSTOOLS_API cCSSObject
{
public:
  cCSSObject();
  virtual ~cCSSObject();
};


// Ancestor object for all userInfo types (UI = UserInfo).
class CSSTOOLS_API cCSSObjectUI: public cCSSObject
{
public:
  // In a derived object, this function must allocate (new) a new object, copy itself into it and return it.
  virtual cCSSObjectUI* Copy() = 0;
};

// Boolean definition for CSS debug module registration
namespace CSS
{
  namespace Diag
  {
    extern CSSTOOLS_API bool TraceOn;
  };
};

// Diag flag for misc CSSTools stuff outside the CSS namespace
extern bool CSSToolsTraceOn;

#endif

#pragma warning( default : 4705 )

