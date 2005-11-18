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
bool CSSToolsTraceOn = false;

cCSSObject::cCSSObject()
{
}

cCSSObject::~cCSSObject()
{
}

// Register to diag statically when the library is loaded
class CSSToolsDebugModuleRegistration
{
public:
  CSSToolsDebugModuleRegistration()
  {
    PDIAGDLLREGISTERDEBUGMODULE pDiagDllRegisterDebugModule = NULL;
    HINSTANCE DiagDll = LoadLibrary(gDiagDllName);

    if (DiagDll != NULL)
    {
      pDiagDllRegisterDebugModule = (PDIAGDLLREGISTERDEBUGMODULE) GetProcAddress(DiagDll,"DiagDllRegisterDebugModule");
      if (pDiagDllRegisterDebugModule != NULL)
      {
        // Register the CSS debug flag to Diag
        pDiagDllRegisterDebugModule("CSS", "CSSGENERIC", CSS::Diag::TraceOn);
        pDiagDllRegisterDebugModule("CSS", "CSSTOOLS", CSSToolsTraceOn);
      }
      FreeLibrary(DiagDll);
    }
  } 
} StaticObject;

 

