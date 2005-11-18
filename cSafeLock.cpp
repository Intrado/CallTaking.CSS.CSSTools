// cSafeLock.cpp: implementation of the cSafeLock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSafeLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cSafeLock:: cSafeLock(cLockableObject& rlock, int mcount):mcount(mcount),mpLock(&rlock)
{

}

cSafeLock:: cSafeLock(cLockableObject* plock,int mcount):mcount(mcount),mpLock(plock)
{

}
bool cSafeLock::Acquire(long timeout)
{ 
bool bRes;

  bRes=mpLock->Acquire(timeout);
  mcount++;
  return bRes;

}


void cSafeLock::Release()
{ 
  if (mcount > 0) 
    {
    mpLock->Release();
    mcount--;
    }
}


cSafeLock::~cSafeLock()
{
  if (mcount!=0)
  {
    //Leve une exception
    throw(this);
  }

}
