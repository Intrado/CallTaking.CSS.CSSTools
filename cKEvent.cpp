// cKEvent.cpp: implementation of the cKEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cKEvent.h"
#include "cSafeLock.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cKEvent::~cKEvent()
{

}

cKEvent::cKEvent(char *pName, bool Manual,bool bInitialState, LPSECURITY_ATTRIBUTES lpEventAttributes):
cEvent(pName, Manual,bInitialState,lpEventAttributes)
{
  Kind = 0;   // Coverity #15239: initialize scalar filed
}

int cKEvent::GetKind()
{
cSafeLock LockE(Lock);
int nResult;

  LockE.Acquire();
  nResult=Kind;
  LockE.Release();
  return nResult;
}

void cKEvent::SetKind(int Kind)
{
cSafeLock LockE(Lock);
  LockE.Acquire();
  this->Kind=Kind;
  LockE.Release();  
}

void cKEvent::Signal(int Kind)
{
cSafeLock LockE(Lock);

  LockE.Acquire();
  this->Kind=Kind;
  cEvent::Signal();
  LockE.Release();  
}


int  cKEvent::WaitFor(int TimeOut)
{
cSafeLock LockE(Lock);
int nResult;
bool retVal;

  LockE.Acquire();
  retVal=cEvent::WaitnLock(TimeOut,&Lock);
  if (retVal==false) nResult = WAIT_TIMEOUT;
  else (nResult= Kind);
  LockE.Release();
  return nResult;
}

bool cKEvent::WaitnLock(int TimeOut,cMutex *pmx)
{
int nResult;

  pmx->Release();
  nResult=WaitFor(TimeOut);
  pmx->Acquire();
  if (nResult>=0)
    return true;
  else
    return false;

}

