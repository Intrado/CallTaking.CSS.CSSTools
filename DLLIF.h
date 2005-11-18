/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description. */
/*  Created  by Claude Chausse,     03 May 1997 */
/*  Modified by Ashraf Ballan,      13 Feb 1998 */

#ifndef __DLLIF_INCLUDED__
#define __DLLIF_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include "CSSObject.h"
#include "DiagDll.h"
#include "WinDef.h"
#include "CSSTools.h"
namespace CSS
{
  class CSSTOOLS_API cDLLIF : public cCSSObject
  {
  public:
    enum DiagType
    {
      Skip, Trace, Warning, Error
    };
    enum TypeOfWriting
    {
      WrAdd, WrNew
    };
    typedef void (__stdcall PFNREGISTERCALLBACK) (void *data, HANDLE event, HANDLE semaphore);
    typedef void (__stdcall PFNUNREGISTERCALLBACK) (void *data);
    typedef void (__stdcall PFNENABLEDIAGTRACE) ();
    typedef void (__stdcall PFNDISABLEDIAGTRACE) ();
    typedef void (__stdcall PFNINITDIAG) (int Flags, LPCSTR filename, int WriteType, void *Proc);
    typedef void (__stdcall PFNINITDIAGEX) (int Flags, LPCSTR filename, int WriteType, void *Proc, bool IsDebug);
    typedef void (__stdcall PFNADDDIAGREPORTTYPE) (int Flags, LPCSTR filename, int WriteType, void *Proc);
    typedef void (__stdcall PFNREMOVEDIAGREPORTTYPE) (int Flags);
    typedef void (__stdcall PFNENDDIAG) ();
    typedef void (__stdcall PFNADDDIAGFILETOLIST) (LPCSTR DiagFileP, int DiagLevelP);
    typedef void (__stdcall PFNREMOVEDIAGFILEFROMLIST) (LPCSTR DiagFileP);
    typedef void (__stdcall PFNSTARTREPORTALLFILE) (int level);
    typedef void (__stdcall PFNSTOPREPORTALLFILE) (int level);
    typedef void (__stdcall PFNCREATEREGINI) (LPCSTR IniFileWithPath);
    
    typedef void (__cdecl PFNREGISTERCALLBACKC) (void *data, HANDLE event, HANDLE semaphore);
    typedef void (__cdecl PFNUNREGISTERCALLBACKC) (void *data);
    typedef void (__cdecl PFNENABLEDIAGTRACEC) ();
    typedef void (__cdecl PFNDISABLEDIAGTRACEC) ();
    typedef void (__cdecl PFNINITDIAGC) (int Flags, LPCSTR filename, int WriteType, void *Proc);
    typedef void (__cdecl PFNINITDIAGEXC) (int Flags, LPCSTR filename, int WriteType, void *Proc, bool IsDebug);
    typedef void (__cdecl PFNADDDIAGREPORTTYPEC) (int Flags, LPCSTR filename, int WriteType, void *Proc);
    typedef void (__cdecl PFNREMOVEDIAGREPORTTYPEC) (int Flags);
    typedef void (__cdecl PFNENDDIAGC) ();
    typedef void (__cdecl PFNADDDIAGFILETOLISTC) (LPCSTR DiagFileP, int DiagLevelP);
    typedef void (__cdecl PFNREMOVEDIAGFILEFROMLISTC) (LPCSTR DiagFileP);
    typedef void (__cdecl PFNSTARTREPORTALLFILEC) (int level);
    typedef void (__cdecl PFNSTOPREPORTALLFILEC) (int level);
    typedef void (__cdecl PFNCREATEREGINIC) (LPCSTR IniFileWithPath);
    
    static const char *moduleName;
    
    struct DllCBData
    {
      int dllSignature;
      int evtType;
      int objectSrcID;
      int objectDstID;
      void *data;
    };
    
    struct DllCBFunctionData // this struct is used to pass the cb function to dll
    {												 // It's used "over" the DllCBData struct passed when registering
      void * cbFunc;
      void * objPtr;
    };
    
    class CSSTOOLS_API DLLIFClosure
    {
    public:
      DLLIFClosure () {};
      void MemberApply (DiagDllEventType event, const char *module);
    protected:
      
    private:
      cDLLIF * dllIF;
    };
    
    class CSSTOOLS_API DiagRecord:public cCSSObject
    {
    public:
      DiagRecord (){};
    private:
      int DiagMsg;
      /*  Diag record type */
      LPCSTR ModuleName;
      LPCSTR FunctionName;
      LPCSTR msg;
      
      friend class cDLLIF;
    };
    
    public:
      cDLLIF() {};
      DllCBData *cbDataREC;
      virtual cDLLIF *init (HINSTANCE dllHandle, std::string dllBaseName, bool useDiag, std::string iniFileName = "");
      virtual void createDllRegIni (std::string iniFileName);
      virtual void exec ();
      virtual void destroy ();
    protected:
      
    private:
      std::string dllBaseName;
      DLLIFClosure *closure;
      HINSTANCE hLibrary;
      bool isDiagOn;
      
      // stdcall function pointers
      PFNREGISTERCALLBACK *lpfnRegisterCallback;
      PFNUNREGISTERCALLBACK *lpfnUnRegisterCallback;
      PFNENABLEDIAGTRACE *lpfnEnableDiagTrace;
      PFNDISABLEDIAGTRACE *lpfnDisableDiagTrace;
      PFNINITDIAG *lpfnInitDiag;
      PFNINITDIAGEX *lpfnInitDiagEx;
      PFNADDDIAGREPORTTYPE *lpfnAddDiagReportType;
      PFNREMOVEDIAGREPORTTYPE *lpfnRemoveDiagReportType;
      PFNENDDIAG *lpfnEndDiag;
      PFNADDDIAGFILETOLIST *lpfnAddDiagFileToList;
      PFNREMOVEDIAGFILEFROMLIST *lpfnRemoveDiagFileFromList;
      PFNSTARTREPORTALLFILE *lpfnStartReportAllFile;
      PFNSTOPREPORTALLFILE *lpfnStopReportAllFile;
      PFNCREATEREGINI *lpfnCreateRegIni;
      
      // cdecl Functions types
      PFNREGISTERCALLBACKC *lpfnRegisterCallbackc;
      PFNUNREGISTERCALLBACKC *lpfnUnRegisterCallbackc;
      PFNENABLEDIAGTRACEC *lpfnEnableDiagTracec;
      PFNDISABLEDIAGTRACEC *lpfnDisableDiagTracec;
      PFNINITDIAGC *lpfnInitDiagc;
      PFNINITDIAGEXC *lpfnInitDiagExc;
      PFNADDDIAGREPORTTYPEC *lpfnAddDiagReportTypec;
      PFNREMOVEDIAGREPORTTYPEC *lpfnRemoveDiagReportTypec;
      PFNENDDIAGC *lpfnEndDiagc;
      PFNADDDIAGFILETOLISTC *lpfnAddDiagFileToListc;
      PFNREMOVEDIAGFILEFROMLISTC *lpfnRemoveDiagFileFromListc;
      PFNSTARTREPORTALLFILEC *lpfnStartReportAllFilec;
      PFNSTOPREPORTALLFILEC *lpfnStopReportAllFilec;
      PFNCREATEREGINIC *lpfnCreateRegInic;
      virtual bool initProcAddress ();
      virtual void processDiagEvent ();
      
      friend void callbackFunction(void* objPtr);
      friend class DLLIFClosure;
  };
  void __stdcall apply (DiagDllEventType event, const char *module, void *data);
}
#endif
