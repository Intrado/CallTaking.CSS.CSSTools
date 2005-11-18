/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    fslight              date created:    2004/02/24
 *
 * name:          CSS.h
 * version:       1
 *
 *
 * Description:
 *
 *     Things global to the CSS namespace.
 *
 ***************************************************************************/

#ifndef __CSS_H_INCLUDED__
#define __CSS_H_INCLUDED__

#include "CSSDiagEx.h"
// Boolean definition for CSS debug module registration
namespace CSS
{
  namespace Diag
  {
    extern CSSTOOLS_API bool TraceOn;
  };

// Must be included only here so that the functions contained in CSSTrace.h use the right Diag::TraceOn variable.
#include "CSSTrace.h"
};

#endif
