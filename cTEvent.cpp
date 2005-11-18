// cTEvent.cpp: implementation of the cTEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "cTEvent.h"
#include "cSafeLock.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int cTEvent::inst=0;
int cTEvent::instDestroy=0;

static bool DiagTraceOn=false;

cTEvent::cTEvent()
{
cSafeLock Lock(LockTEvent);

  Lock.Acquire();
  nbUses=1;   // Equivalent a un AddRef() 
  Id= ++inst;
  Lock.Release();
}

cTEvent::~cTEvent()
{
  ++instDestroy;
  
}

//Une reference d'utilisation de plus
int cTEvent::AddRef(void)
{
cSafeLock Lock(LockTEvent);
int nResult;

  Lock.Acquire();
  nResult=++nbUses;
  Lock.Release();
  return nResult;
}


//Une reference d'utilisation de plus
int cTEvent::ReleaseRef(void)
{
cSafeLock Lock(LockTEvent);
int nResult;

  Lock.Acquire();
  nResult= --nbUses;

  if(nResult==0)
    {
//    if(DiagTraceOn)
//      printf("Destruction cTEvent %d    %d\n",nbUses, Id);
    Lock.Release();
    delete this;
    return 0;
    }
  Lock.Release();

  return nResult;
}
