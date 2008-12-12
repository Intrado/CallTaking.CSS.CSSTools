#include <string>
#include "DLLIF.h"
#include "assert.h"
#include "NewAutoInitialize.h"
#include "CSS.h"

using namespace std;

#pragma warning(disable:4018)

namespace CSS
{
  const char *cDLLIF::moduleName = "cDLLIF";

  LPCSTR dllModuleName;
  const int REPORTDIAG_ENABLE = 128;
  const int DIAGEVENTNUMBER = -1;

  typedef void (__stdcall *DiagDllCB)(DiagDllEventType eventType, const char *module);

  void callbackFunction(void* objPtr)
  {
    cDLLIF * dllif = (cDLLIF*) objPtr;
    if (dllif->cbDataREC->evtType == DIAGEVENTNUMBER)
    {
      dllif->processDiagEvent();
    }
    else
    {
      dllif->exec();
    }
  }

/*  Initializes DLLIF by creating instances of required types and getting
the addresses of DLL procedures */
cDLLIF *cDLLIF::init(HINSTANCE dllHandle, string baseName, bool useDiag, string iniFileName)
{

  // Initialize function pointers
  lpfnRegisterCallback = NULL;
  lpfnUnRegisterCallback = NULL;
  lpfnEnableDiagTrace = NULL;
  lpfnDisableDiagTrace = NULL;
  lpfnInitDiag = NULL;
  lpfnInitDiagEx = NULL;
  lpfnAddDiagReportType = NULL;
  lpfnRemoveDiagReportType = NULL;
  lpfnEndDiag = NULL;
  lpfnAddDiagFileToList = NULL;
  lpfnRemoveDiagFileFromList = NULL;
  lpfnStartReportAllFile = NULL;
  lpfnStopReportAllFile = NULL;
  lpfnCreateRegIni = NULL;
  lpfnRegisterCallbackc = NULL;
  lpfnUnRegisterCallbackc = NULL;
  lpfnEnableDiagTracec = NULL;
  lpfnDisableDiagTracec = NULL;
  lpfnInitDiagc = NULL;
  lpfnInitDiagExc = NULL;
  lpfnAddDiagReportTypec = NULL;
  lpfnRemoveDiagReportTypec = NULL;
  lpfnEndDiagc = NULL;
  lpfnAddDiagFileToListc = NULL;
  lpfnRemoveDiagFileFromListc = NULL;
  lpfnStartReportAllFilec = NULL;
  lpfnStopReportAllFilec = NULL;
  lpfnCreateRegInic = NULL;

  isDiagOn = useDiag;
  dllBaseName = baseName;
  hLibrary = dllHandle;
  cbDataREC = (NEW0 DllCBData);
  if (initProcAddress() == false)
  {
    if (Diag::TraceOn)
    {
      DiagTrace(moduleName, "InitProcAddress", " failed.");;
    };
    return NULL;
  };
  if (Diag::TraceOn)
  {
    DiagTrace(moduleName, "InitProcAddress", " finished ok.");;
  };
  if (!iniFileName.empty())
//	if (!iniFileName != NULL)
  {
    /*  Creation du RegIni de la dll */
    createDllRegIni(iniFileName);
  };
  
  if ((isDiagOn))
  {
    if (Diag::TraceOn)
    {
      DiagTrace(moduleName, "InitProcAddress", "DLL diagnostics are wanted ");;
    };
    bool IsDebug = false;

    // IsDebug is true if we are debugging
    #ifdef _DEBUG
        IsDebug = true;
    #endif

    if (lpfnInitDiagEx != NULL)
      (*lpfnInitDiagEx) (REPORTDIAG_ENABLE, baseName.c_str(), WrNew, NULL, IsDebug); //T2S
    else if (lpfnInitDiagExc != NULL)
      (*lpfnInitDiagExc) (REPORTDIAG_ENABLE, baseName.c_str(), WrNew, NULL, IsDebug); //T2S
    else if (lpfnInitDiag != NULL)
      (*lpfnInitDiag) (REPORTDIAG_ENABLE, baseName.c_str(), WrNew, NULL); //T2S
    else if (lpfnInitDiagc != NULL)
      (*lpfnInitDiagc) (REPORTDIAG_ENABLE, baseName.c_str(), WrNew, NULL); //T2S
    else
      assert(0);

    if (lpfnStartReportAllFile != 0)
    {
      (*lpfnStartReportAllFile) (Error);
      (*lpfnStartReportAllFile) (Warning);
    }
    else if (lpfnStartReportAllFilec != 0)
    {
      (*lpfnStartReportAllFilec) (Error);
      (*lpfnStartReportAllFilec) (Warning);
    }
    
    // Temp closure before the new user data member is added in diag
    DiagDllCBEx CallBackFunction = &apply;
  };
	/* Do Register call back after diag is nitialized */
	((DllCBFunctionData*)cbDataREC)->cbFunc = (void*) callbackFunction;
	((DllCBFunctionData*)cbDataREC)->objPtr = (void*) this; 

  if (lpfnRegisterCallback != 0)
	  (*lpfnRegisterCallback) (cbDataREC, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE);
  else if (lpfnRegisterCallbackc != 0)
	  (*lpfnRegisterCallbackc) (cbDataREC, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE);
  else
    assert(0);

  return this;
}

void cDLLIF::destroy()
{
  if (lpfnUnRegisterCallback != 0)
	  (*lpfnUnRegisterCallback) (cbDataREC);
  else if (lpfnUnRegisterCallbackc != 0)
	  (*lpfnUnRegisterCallbackc) (cbDataREC);
  else
    assert(0);

  delete cbDataREC;
  closure = NULL;
}

void cDLLIF::createDllRegIni(string iniFileName)
{
  /*  Creation du registreur de la dll */
  if (!iniFileName.empty()) //T2S
//  if (iniFileName != NULL)
  {
    DiagTrace(moduleName, "Init", "CreateRegIni in Dll", iniFileName);;
    if (lpfnCreateRegIni != NULL)
    {
      /*  Pour une raison de compatibilite */
      (*lpfnCreateRegIni) (iniFileName.c_str()); //T2S
    }
    else if (lpfnCreateRegInic != NULL)
    {
      /*  Pour une raison de compatibilite */
      (*lpfnCreateRegInic) (iniFileName.c_str()); // T2S
    };
    /* It is possible that there is no CreateRegIni exported */
  };
}

void cDLLIF::processDiagEvent()
{
  DiagRecord *dllDiagREC;
  /*  Extracts the Diag message from the Diag record and sends it to Diag */
  string modName;
  string fctName;
  string msg;
  dllDiagREC = (DiagRecord *) ((*cbDataREC).data);
  if (dllDiagREC != NULL)
  {
    modName = ((string)((*dllDiagREC).ModuleName)).c_str(); //T2S
    fctName = ((string)((*dllDiagREC).FunctionName)).c_str();//T2S
    msg = ((string)((*dllDiagREC).msg)).c_str();//T2S
    if ((*dllDiagREC).DiagMsg == Error)
    {
      DiagError((dllBaseName + "." + modName).c_str(), fctName.c_str(), msg);
    }
    else if ((*dllDiagREC).DiagMsg == Warning)
    {
      DiagWarning((dllBaseName + "." + modName).c_str(), fctName.c_str(), msg);
    }
    else if ((*dllDiagREC).DiagMsg == Trace)
    {
      if (Diag::TraceOn)
      {
        DiagTrace((dllBaseName + "." + modName).c_str(), fctName.c_str(), msg);;
      };
    };
  };
}

bool cDLLIF::initProcAddress()
{
  FARPROC getprocvar;
  /*  Gets procedure addresses from DLL */
  if (Diag::TraceOn)
  {
    DiagTrace(moduleName, "InitProcAddress", " starting...");;
  };

  /*  registercallback */
  getprocvar = GetProcAddress(hLibrary, ((string)"registercallback").c_str()); //T2S
  lpfnRegisterCallback = (PFNREGISTERCALLBACK *) (getprocvar);
  if (lpfnRegisterCallback == NULL)
  {
    getprocvar = GetProcAddress(hLibrary, ((string)"_RegisterCallbackc").c_str()); //T2S
    lpfnRegisterCallbackc = (PFNREGISTERCALLBACKC *) (getprocvar);
  };
  if (lpfnRegisterCallback == NULL && lpfnRegisterCallbackc == NULL)
  {
    DiagWarning(moduleName, "InitProcAddress", "Fail to get registerCallback function address from dll ",dllBaseName);
    return false;
  };

  /*  unregistercallback */
  getprocvar = GetProcAddress(hLibrary, ((string)"unregistercallback").c_str()); //T2S
  lpfnUnRegisterCallback = (PFNUNREGISTERCALLBACK *) (getprocvar);
  if (lpfnUnRegisterCallback == NULL)
  {
    getprocvar = GetProcAddress(hLibrary, ((string)"_UnregisterCallbackc").c_str()); //T2S
    lpfnUnRegisterCallbackc = (PFNUNREGISTERCALLBACKC *) (getprocvar);
  }
  if (lpfnUnRegisterCallback == NULL && lpfnUnRegisterCallbackc == NULL)
  {
    DiagWarning(moduleName, "InitProcAddress", "Fail to get unregisterCallback function address from dll ",dllBaseName);
    return false;
  };

  /*  createregini */
  getprocvar = GetProcAddress(hLibrary, ((string)"CreateRegIni").c_str()); //T2S
  lpfnCreateRegIni = (PFNCREATEREGINI *) (getprocvar);
  if (lpfnCreateRegIni == NULL)
  {
    getprocvar = GetProcAddress(hLibrary, ((string)"_CreateRegInic").c_str()); //T2S
    lpfnCreateRegInic = (PFNCREATEREGINIC *) (getprocvar);
  };
  if (lpfnCreateRegIni == NULL && lpfnCreateRegInic == NULL)
  {
    DiagWarning(moduleName, "InitProcAddress", "Fail to get CreateRegIni function address from dll ",dllBaseName);
  };

  if (isDiagOn)
  {
    /*  On ne retourne pas une erreur, pour question de compatibilite avec les anciennes dll */
    /*  RETURN FALSE; */
    /*  Get addresses of Diag procedures if Diag is to be enabled */

    /*  initdiag */
    getprocvar = GetProcAddress(hLibrary, ((string)"InitDiag").c_str()); //T2S
    lpfnInitDiag = (PFNINITDIAG *) (getprocvar);
    if (lpfnInitDiag == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_InitDiagc").c_str()); //T2S
      lpfnInitDiagc = (PFNINITDIAGC *) (getprocvar);
    };

    if (lpfnInitDiag == NULL && lpfnInitDiagc == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get InitDiag function address from dll ",dllBaseName);
      return false;
    };

    /*  initdiagEx without verification */
    getprocvar = GetProcAddress(hLibrary, ((string)"InitDiagEx").c_str()); //T2S
    lpfnInitDiagEx = (PFNINITDIAGEX *) (getprocvar);
    if (lpfnInitDiagEx == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_InitDiagExc").c_str()); //T2S
      lpfnInitDiagExc = (PFNINITDIAGEXC *) (getprocvar);
    }
    if (lpfnInitDiagEx == NULL && lpfnInitDiagExc == NULL)
      DiagWarning("DllIf", "InitProcAddress", "InitDiagEx function not found in DLL" , dllBaseName , ". No diag traces will be available");

    /*  enablediagtrace */
    getprocvar = GetProcAddress(hLibrary, ((string)"EnableDiagTrace").c_str()); //T2S
    lpfnEnableDiagTrace = (PFNENABLEDIAGTRACE *) (getprocvar);
    if (lpfnEnableDiagTrace == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_EnableDiagTracec").c_str()); //T2S
      lpfnEnableDiagTracec = (PFNENABLEDIAGTRACEC *) (getprocvar);
    };
    if (lpfnEnableDiagTrace == NULL && lpfnEnableDiagTracec == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get EnableDiagTrace function address from dll ",dllBaseName);;
      return false;
    };

    /*  disablediagtrace */
    getprocvar = GetProcAddress(hLibrary, ((string)"DisableDiagTrace").c_str()); //T2S
    lpfnDisableDiagTrace = (PFNDISABLEDIAGTRACE *) (getprocvar);
    if (lpfnDisableDiagTrace == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_DisableDiagTracec").c_str());//T2S
      lpfnDisableDiagTracec = (PFNDISABLEDIAGTRACEC *) (getprocvar);
    };
    if (lpfnDisableDiagTrace == NULL && lpfnDisableDiagTracec == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get DisableDiagTrace function address from dll ",dllBaseName);
      return false;
    };

    /*  adddiagreporttype */
    getprocvar = GetProcAddress(hLibrary, ((string)"AddDiagReportType").c_str());//T2S
    lpfnAddDiagReportType = (PFNADDDIAGREPORTTYPE *) (getprocvar);
    if (lpfnAddDiagReportType == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_AddDiagReportTypec").c_str());//T2S
      lpfnAddDiagReportTypec = (PFNADDDIAGREPORTTYPEC *) (getprocvar);
    };
    if (lpfnAddDiagReportType == NULL && lpfnAddDiagReportTypec == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get AddDiagReportType function address from dll ",dllBaseName);
      return false;
    };

    /*  removediagreporttype */
    getprocvar = GetProcAddress(hLibrary, ((string)"RemoveDiagReportType").c_str());//T2S
    lpfnRemoveDiagReportType = (PFNREMOVEDIAGREPORTTYPE *) (getprocvar);
    if (lpfnRemoveDiagReportType == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_RemoveDiagReportTypec").c_str());//T2S
      lpfnRemoveDiagReportTypec = (PFNREMOVEDIAGREPORTTYPEC *) (getprocvar);
    };
    if (lpfnRemoveDiagReportType == NULL && lpfnRemoveDiagReportTypec == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get RemoveDiagReportType function address from dll ",dllBaseName);;
      return false;
    };

    /*  enddiag */
    getprocvar = GetProcAddress(hLibrary, ((string)"EndDiag").c_str());//T2S
    lpfnEndDiag = (PFNENDDIAG *) (getprocvar);
    if (lpfnEndDiag == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_EndDiagc").c_str()); //T2S
      lpfnEndDiagc = (PFNENDDIAGC *) (getprocvar);
    };
    if (lpfnEndDiag == NULL && lpfnEndDiagc == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get EndDiag function address from dll ",dllBaseName);;
      return false;
    };

    /*  adddiagfiletolist */
    getprocvar = GetProcAddress(hLibrary, ((string)"AddDiagFileToList").c_str()); //T2S
    lpfnAddDiagFileToList = (PFNADDDIAGFILETOLIST *) (getprocvar);
    if (lpfnAddDiagFileToList == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_AddDiagFileToListc").c_str());//T2S
      lpfnAddDiagFileToListc = (PFNADDDIAGFILETOLISTC *) (getprocvar);
    };
    if (lpfnAddDiagFileToList == NULL && lpfnAddDiagFileToListc == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get AddDiagFileToList function address from dll ",dllBaseName);;
      return false;
    };

    /*  removediagfiletolist */
    getprocvar = GetProcAddress(hLibrary, ((string)"RemoveDiagFileFromList").c_str()); //T2S
    lpfnRemoveDiagFileFromList = (PFNREMOVEDIAGFILEFROMLIST *) (getprocvar);
    if (lpfnRemoveDiagFileFromList == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_RemoveDiagFileFromListc").c_str());//T2S
      lpfnRemoveDiagFileFromListc = (PFNREMOVEDIAGFILEFROMLISTC *) (getprocvar);
    };
    if (lpfnRemoveDiagFileFromList == NULL && lpfnRemoveDiagFileFromListc == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get RemoveDiagFileFromList function address from dll ",dllBaseName);;
      return false;
    };

    /*  startreportallfile */
    getprocvar = GetProcAddress(hLibrary, ((string)"StartReportAllFile").c_str()); //T2S
    lpfnStartReportAllFile = (PFNSTARTREPORTALLFILE *) (getprocvar);
    if (lpfnStartReportAllFile == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_StartReportAllFilec").c_str()); //T2S
      lpfnStartReportAllFilec = (PFNSTARTREPORTALLFILEC *) (getprocvar);
    };
    
    if (lpfnStartReportAllFile == NULL && lpfnStartReportAllFilec == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get StartReportAllFile function address from dll ",dllBaseName);;
      return false;
    };

    /*  stopreportallfile */
    getprocvar = GetProcAddress(hLibrary, ((string)"StopReportAllFile").c_str()); //T2S
    lpfnStopReportAllFile = (PFNSTOPREPORTALLFILE *) (getprocvar);
    if (lpfnStopReportAllFile == NULL)
    {
      getprocvar = GetProcAddress(hLibrary, ((string)"_StopReportAllFilec").c_str()); //T2S
      lpfnStopReportAllFilec = (PFNSTOPREPORTALLFILEC *) (getprocvar);
    };
    if (lpfnStopReportAllFile == NULL && lpfnStopReportAllFilec == NULL)
    {
      DiagWarning(moduleName, "InitProcAddress", "Fail to get StopReportAllFile function address from dll ",dllBaseName);
      return false;
    };
  };
  return true;
}

void __stdcall apply(DiagDllEventType eventType, const char* enabledModuleName, void *data)
{
  ((cDLLIF::DLLIFClosure*)data)->MemberApply (eventType, enabledModuleName);
}

void cDLLIF::DLLIFClosure::MemberApply (DiagDllEventType eventType, const char *module)
{
  string enabledModuleName = module;
  string dllName;
  /*  Called by Diag when a Diag event occurs */
  string modName;
  int lg;
  DiagTrace(moduleName, "Apply", "Receiving Diag event with : ", enabledModuleName);;
  if ((eventType == DiagEnableAllModules))
  {
    DiagTrace(moduleName, "Apply", "Enabling all ", dllIF->dllBaseName, ".dll modules");;
    if (*dllIF->lpfnStartReportAllFile != NULL)
      (*dllIF->lpfnStartReportAllFile) (Trace);
    else if (*dllIF->lpfnStartReportAllFilec != NULL)
      (*dllIF->lpfnStartReportAllFilec) (Trace);
    else
      assert(0);
  }
  
  else if ((eventType == DiagDisableAllModules))
  {
    DiagTrace(moduleName, "Apply", "Disabling all ", dllIF->dllBaseName, ".dll modules");;

    if (*dllIF->lpfnStopReportAllFile != NULL)
      (*dllIF->lpfnStopReportAllFile) (Trace);
    else if (*dllIF->lpfnStopReportAllFilec != NULL)
      (*dllIF->lpfnStopReportAllFilec) (Trace);
    else
      assert(0);
  }
  
  else
  {
    lg = (dllIF->dllBaseName).length();
    dllName = (enabledModuleName).substr(0, lg + 1 );
    if (enabledModuleName.length() > lg + 1)
    {
      modName = enabledModuleName.substr(lg + 1, enabledModuleName.length() - lg + 1);
      if (((dllName == (dllIF->dllBaseName + "."))))
      {
        DiagTrace(moduleName, "Apply", "Module of ", dllIF->dllBaseName, ".DLL");;
        if ((eventType == DiagEnableModule))
        {
          DiagTrace(moduleName, "Apply", "Enabling ", dllIF->dllBaseName, ("." + modName).c_str());;
          dllModuleName = new char[modName.length() + 1]; //T2S
          dllModuleName = ((string)modName).c_str(); //T2S
          
          if (*dllIF->lpfnAddDiagFileToList != NULL)
            (*dllIF->lpfnAddDiagFileToList) (dllModuleName, Trace);
          else if (*dllIF->lpfnAddDiagFileToListc != NULL)
            (*dllIF->lpfnAddDiagFileToListc) (dllModuleName, Trace);
          else
            assert(0);

	  if( dllModuleName != NULL ) { delete (void *)dllModuleName; }; //T2S

        }
        
        else if ((eventType == DiagDisableModule))
        {
          DiagTrace(moduleName, "Apply", "Disabling ", dllIF->dllBaseName, ("." + modName).c_str());;
	  dllModuleName = new char[modName.length() + 1]; //T2S
	  dllModuleName = ((string)modName).c_str(); //T2S
          if (*dllIF->lpfnRemoveDiagFileFromList != NULL)
            (*dllIF->lpfnRemoveDiagFileFromList) (dllModuleName);
          else if (*dllIF->lpfnRemoveDiagFileFromListc != NULL)
            (*dllIF->lpfnRemoveDiagFileFromListc) (dllModuleName);
          else
            assert(0);

	  if( dllModuleName != NULL ) { delete (void *)dllModuleName; }; //T2S

        }
        
        else
        {
        };
      };
    };
  };
}

void cDLLIF::exec()
{
  assert(false);
}

}