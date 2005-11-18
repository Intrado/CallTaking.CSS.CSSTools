/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 1998.
 * ALL RIGHTS RESERVED.
 *
 * File: cMonitorAbstractBase.h
 *
 * Project: Utility class
 *
 * Description:
 *
 *  This file contains the declaration of the base class for the class
 *  exported by a monitoring DLL.
 *
 ***************************************************************************/

#ifndef _CMONITORABSTRACTBASE_H
#define _CMONITORABSTRACTBASE_H


#include <windows.h>


// Define the CMONITOR_IMPLEMENTATION symbol only when implementing the
// cMonitorAbstractBase-derived class.
#ifdef CMONITOR_IMPLEMENTATION
#define CMONITOR_IMPL_API _declspec(dllexport)
#else
#define CMONITOR_IMPL_API _declspec(dllimport)
#endif


// This abstract class is provided to allow a monitoring DLL to be dynamically loaded
// and still be able to create an instance of the monitoring dialog.  A class in the
// monitoring DLL must derive from this class and implement the Show method.
// The Show() method MUST make sure the dialog is created before returning.
class cMonitorAbstractBase
{
public:
  // Shows the dialog window on the screen.
  virtual void Show() = 0;
};


// The monitoring DLL must also export and implement the following functions
// - GetMonitoringObject(): Create an instance of the cMonitorAbstractBase-derived class
//                          (if necessary) and return a pointer to it.
#ifdef __cplusplus
extern "C" {
#endif
CMONITOR_IMPL_API cMonitorAbstractBase* GetMonitoringObject();
#ifdef __cplusplus
}
#endif


// - DisableMonitoringObject(): Close the monitoring DLL's dialog (most likely with a call to
//                              CDialog::EndDialog().
#ifdef __cplusplus
extern "C" {
#endif
CMONITOR_IMPL_API void DisableMonitoringObject();
#ifdef __cplusplus
}
#endif


#endif
