// Thread.cpp: implementation of the cThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define SHAREDCOMP_CLASSDEF _declspec(dllexport)

#include "cThread.h"
#include "CSSDiagEx.h"
#include "cAutoLock.h"
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct cThreadInfo
{
  cThreadInfo(): mHandle(0), mJoinCount(0) {}
  cThreadInfo(HANDLE h): mHandle(h), mJoinCount(0) {}
  HANDLE mHandle;
  int mJoinCount;
};


typedef cLockableMap<cThread*, cThreadInfo> tThreadInfoMap;
tThreadInfoMap gThreadInstances;


cThread::cThread()
{
  mSelfDestruct = false;
  _isRunning= false;
  _threadName = "";
  _threadHandle = 0;
  _threadId = 0;
}

cThread::cThread(cTemplateString threadName)
{
  mSelfDestruct = false;
  _isRunning= false;
  _threadName = threadName;
  _threadHandle = 0;
  _threadId = 0;
}

cThread::~cThread()
{
  if(IsRunning())
  {
    Stop();
  }

  {
    cAutoLock a(gThreadInstances);
    tThreadInfoMap::iterator it = gThreadInstances.find(this);
    if (it != gThreadInstances.end())
    {
      int joinCount = it->second.mJoinCount;
      if (joinCount == 0)
      {
        // Nobody's joining this thread: close handle and remove it from gThreadInstances.
        CloseHandle(_threadHandle);
        gThreadInstances.erase(it);
      }
    }
    else
    {
      // "this" was removed from gThreadInstances by a call to Join(): nothing to do.
    }
  }
}

// Callback thread startup code
DWORD cThread::_callback(void* theObject)
{
  cThread* pThreadObject = ((cThread*)theObject);
  DWORD result;
  DiagDllThreadFork(pThreadObject->GetName().c_str());
  pThreadObject->_isRunning = true;
  result = pThreadObject->ThreadProc();
  pThreadObject->_isRunning = false;
  if (pThreadObject->mSelfDestruct)
  {
    delete pThreadObject;  // adios thread object
  }
  return result;
}

bool cThread::Run(bool selfDestruct)
{
  if(!_isRunning)
  {
    mSelfDestruct = selfDestruct;
    {
      cAutoLock a(gThreadInstances);

      // A cThread object can be run multiple times.
      tThreadInfoMap::iterator it = gThreadInstances.find(this);
      if (it != gThreadInstances.end())
      {
        /* Thread hasn't been joined last time: close its previous handle.  No need to remove it from
           gThreadInstances, its entry will be replaced below. */
        HANDLE h = it->second.mHandle;
        CloseHandle(h);
      }
      _threadHandle = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)_callback,this,NULL,&_threadId);

      cThreadInfo info(_threadHandle);
      gThreadInstances[this] = info;
    }
  }
  return _threadHandle != 0;
}

bool cThread::IsRunning()
{
  return _isRunning;
}

bool cThread::Stop() // calling this function will cause memory leaks
{
  bool result = false;
  if(_isRunning)
  {
    result = true;
    TerminateThread(_threadHandle,0);
    _isRunning = 0;
  }
  return result;
}

DWORD cThread::Join(cThread* th)
{
  return Join(th, INFINITE,0);
}

DWORD cThread::Join(cThread* th, long wait, bool* threadTerminated)
{
  HANDLE h = 0;

  {
    cAutoLock a(gThreadInstances);
    tThreadInfoMap::iterator it = gThreadInstances.find(th);
    if (it != gThreadInstances.end())
    {
      h = it->second.mHandle;
      int &joinCount = it->second.mJoinCount;
      ++joinCount;
    }
    else
    {
      // Thread has already finished.
      if (threadTerminated)
        *threadTerminated = true;
      return 0;
    }
  }

  DWORD wres = WaitForSingleObject(h, wait);
  DWORD exitCode = 0;
  if (threadTerminated != 0)
  {
    if (wres == WAIT_TIMEOUT) // the thread never returned
    {
      *threadTerminated = false;
    }
    else
    {
      *threadTerminated = true;
      GetExitCodeThread(h, &exitCode);
    }
  }
  else if (wres != WAIT_TIMEOUT)
  {
    GetExitCodeThread(h, &exitCode);
  }
  
  {
    cAutoLock a(gThreadInstances);
    tThreadInfoMap::iterator it = gThreadInstances.find(th);
    if (it != gThreadInstances.end())
    {
      h = it->second.mHandle;
      int &joinCount = it->second.mJoinCount;
      if (--joinCount == 0)
      {
        // Last call to Join() for this thread.
        CloseHandle(h);
        gThreadInstances.erase(it);
      }
    }
  }

  return exitCode;
}
