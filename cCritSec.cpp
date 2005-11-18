// cCritSec.cpp: implementation of the cCritSec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cCritSec.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cCritSec::cCritSec()
{
  InitializeCriticalSection(&mTheCriticalSection);   
}

cCritSec::cCritSec(const cCritSec &sec)
{
  InitializeCriticalSection(&mTheCriticalSection);   
}

cCritSec& cCritSec::operator=(const cCritSec &sec)
{
  /* Must not re-initialize mTheCriticalSection (it must be initialized only once, and it has been
     done in one of the constructors above).  Re-initializing it would cause leaks, because
     DeleteCriticalSection is called only once for a given cCritSec, in its destructor.
     However, the operator= must be defined to do nothing, because the default one would do
     mTheCriticalSection = sec.mTheCriticalSection, which gives undesirable results (Win32 says
     a CRITICAL_SECTION must not be directly manipulated). */
  return *this;
}


cCritSec::~cCritSec()
{
  DeleteCriticalSection(&mTheCriticalSection);
}

bool cCritSec::Acquire(long timeout)
{
  EnterCriticalSection(&mTheCriticalSection);
    return true;
  
}

void cCritSec::Release()
{
  LeaveCriticalSection(&mTheCriticalSection);

}
