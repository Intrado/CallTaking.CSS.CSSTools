/* ************************************************************** */
/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    sroy              date created:    2000/04/04
 *
 * name:          CSSObject.h
 * version:       1
 *
 *
 * Description:
 *
 *     Ancestor object for all telephony objects.
 *
	Last change:  SR   4 April 2000
 ***************************************************************************/
#include "CSSObject.h"
#include "CSS.h"

#ifdef _DEBUG
  #ifdef _LEAK_DETECT
    const char* gDiagDllName = "Diagdld.dll";
  #else
    const char* gDiagDllName = "Diagd.dll";
  #endif
#else
  const char* gDiagDllName = "Diag.dll";
#endif


typedef void (_cdecl *PDIAGDLLREGISTERDEBUGMODULE)(const char *module, const char *nameState, bool &state);

bool CSS::Diag::TraceOn = false;
bool CSSToolsTraceOn = true;

cCSSObject::cCSSObject()
{
}

cCSSObject::~cCSSObject()
{
}