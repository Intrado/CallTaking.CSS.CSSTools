/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 1998.
 * ALL RIGHTS RESERVED.
 *
 * File: cMonitor.h
 *
 * Project: Utility class
 *
 * Description:
 *
 *  This file contains the declaration of the class instanciated by someone
 *  who wants to use one or many monitoring DLLs.
 *
 ***************************************************************************/

#ifndef _CMONITOR_H
#define _CMONITOR_H

#include <windows.h>
#pragma warning(disable:4786 4503 4251)
#include <string>
#include <map>
#include "CSSTools.h"
#include "cMonitorAbstractBase.h"

class CSSTOOLS_API cMonitor
{
public:
  // Destructor.
  ~cMonitor();

  // Shows the dialog window on the screen for the given monitoring DLL.
  void Show(char *pDllName);

  // Dismiss the dialog window for the given monitoring DLL.
  void Dismiss(char *pDllName);

private:
  struct tMonitoringDllMapEntry
  {
    HMODULE dllHandle;
    cMonitorAbstractBase *pMonitorObject;
  };
  typedef std::map<std::string, tMonitoringDllMapEntry*> tMonitoringDllMap;

  tMonitoringDllMap mMonitoringMap; // Map of DLL names to info structures
};


#endif
