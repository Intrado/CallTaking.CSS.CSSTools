// cLock.cpp: implementation of the cLock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cAutoLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructors lock the cLockableObject using cSafeLock's Acquire() method.
cAutoLock::cAutoLock(cLockableObject& rLock,int mcount):
cSafeLock(rLock,mcount)
{
  Acquire();
}

cAutoLock::cAutoLock(cLockableObject* pLock,int mcount):
cSafeLock(pLock,mcount)
{
  Acquire();
}

/* Destructor unlock the cLockableObject (using cSafeLock's Release() method) as many
   times as needed to insure it doesn't stay locked after the cAutoLock is destroyed. */
cAutoLock::~cAutoLock()
{
  while (mcount > 0)
  {
    Release(); // Release() decrements 'mcount' each time it is called.
  }
}
