// Thread.h: interface for the cThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADCLOSURE_H__959B1279_8A35_11D2_BAA7_00805F7DF67A__INCLUDED_)
#define AFX_THREADCLOSURE_H__959B1279_8A35_11D2_BAA7_00805F7DF67A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(push)
#pragma warning(disable:4251)

#include "cTemplateString.h"
#include <windows.h>
#include "LockableStlContainers.h"

class CSSTOOLS_API cThread  
{
public:
  cThread();
	cThread(cTemplateString threadName);
	virtual ~cThread();

  virtual bool Stop();
	virtual bool IsRunning();

  inline virtual cTemplateString& GetName() { return _threadName; }
  inline virtual DWORD GetThreadID() { return _threadId; }
  virtual bool Run(bool selfDestruct = false);
  static DWORD Join(cThread* th);
  static DWORD Join(cThread* th, long wait, bool *threadTerminated);
protected:

	virtual long ThreadProc() = 0;
	bool _isRunning;
	HANDLE _threadHandle;
	DWORD _threadId;
  bool mSelfDestruct;

private:
  cTemplateString _threadName;
  static DWORD _callback(void* theObject);
};

#pragma warning(pop)

#endif // !defined(AFX_THREADCLOSURE_H__959B1279_8A35_11D2_BAA7_00805F7DF67A__INCLUDED_)
