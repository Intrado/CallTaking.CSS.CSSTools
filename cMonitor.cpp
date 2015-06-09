/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 1998.
 * ALL RIGHTS RESERVED.
 *
 * File: cMonitor.cpp
 *
 * Project: Utility class
 *
 * Description:
 *
 *  This file contains the implementation of the class instanciated by someone
 *  who wants to use one or many monitoring DLLs.
 *
 ***************************************************************************/

#include "stdafx.h"
#include "cMonitor.h"

using namespace std;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name: ~cMonitor
 *
 * Arguments: None
 *
 * Returns: None
 *
 * Description: Class destructor.
 *
 *-------------------------------------------------------------------------*/
cMonitor::~cMonitor()
{
  int nb = mMonitoringMap.size();

  for (int i = 0; i < nb; i++)
  {
    tMonitoringDllMap::iterator im;

    im = mMonitoringMap.begin();

    if (im != mMonitoringMap.end())
    {
      delete im->second;
      mMonitoringMap.erase(im);
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name: Show
 *
 * Arguments: monitorDllName [in]: Name of the monitoring DLL.
 *
 * Returns: None
 *
 * Description: Shows the dialog window on the screen for the given monitoring DLL.
 *
 *-------------------------------------------------------------------------*/
void cMonitor::Show(char *pDllName)
{
  string monitorDllName = pDllName;
  cMonitorAbstractBase* (*pEnableFunction)();
  tMonitoringDllMapEntry *pInfo;

  pInfo = new tMonitoringDllMapEntry;

  pInfo->dllHandle = LoadLibrary(monitorDllName.c_str());

  pEnableFunction = (cMonitorAbstractBase* (*)()) GetProcAddress(pInfo->dllHandle,
                                                                 "GetMonitoringObject");
  if (pEnableFunction == 0)
  {
    //DWORD lastError = GetLastError();
    delete pInfo;   // Coverity #14656: free pInfo to avoid resource leak
    return;
  }

  pInfo->pMonitorObject = pEnableFunction();

  pInfo->pMonitorObject->Show();

  mMonitoringMap.insert(tMonitoringDllMap::value_type(monitorDllName, pInfo));
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name: Dismiss
 *
 * Arguments: monitorDllName [in]: Name of the monitoring DLL.
 *
 * Returns: None
 *
 * Description: Dismiss the dialog window for the given monitoring DLL.
 *
 *-------------------------------------------------------------------------*/
void cMonitor::Dismiss(char *pDllName)
{
  string monitorDllName = pDllName;
  void (*pDisableFunction)();

  // Find the info associated with the given dll name.
  tMonitoringDllMap::iterator im;
  im = mMonitoringMap.find(monitorDllName);

  if (im == mMonitoringMap.end())
  {
    return;
  }

  tMonitoringDllMapEntry *pInfo = im->second;

  pDisableFunction = (void (*)()) GetProcAddress(pInfo->dllHandle, "DisableMonitoringObject");
  if (pDisableFunction == 0)
  {
    DWORD lastError = GetLastError();
    return;
  }

  pDisableFunction();

  BOOL res = FreeLibrary(pInfo->dllHandle);
  if (res == 0)
  {
    DWORD lastError = GetLastError();
    return;
  }

  mMonitoringMap.erase(im);
}
