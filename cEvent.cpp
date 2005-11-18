// cEvent.cpp: implementation of the cEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cEvent::cEvent(char *pName, bool Manual,bool bInitialState, LPSECURITY_ATTRIBUTES lpEventAttributes)
{
  handle = CreateEvent( 
    lpEventAttributes,        // no security attributes
        Manual,             // manual-reset event
        bInitialState,          // initial state is signaled
        pName);             // object name        
    if (handle == NULL) 
    {         
    HandleInvalid();
    throw this; 
    }
}

cEvent::~cEvent()
{
}

void cEvent::Reset()
{
  ResetEvent(handle);
}

void cEvent::Signal()
{
  SetEvent(handle);
}


bool cEvent::Wait(int TimeOut)
{
  if (WaitForSingleObject(handle,TimeOut) == WAIT_OBJECT_0)
    return true;
  else
    return false;
}


bool cEvent::WaitnLock(int TimeOut,cMutex *pmx)
{
DWORD dwResult;
  pmx->Release();
  dwResult= WaitForSingleObject(handle,TimeOut);
  pmx->Acquire(INFINITE);
  if(dwResult==WAIT_OBJECT_0)
    return true;
  else
    return false;
}
