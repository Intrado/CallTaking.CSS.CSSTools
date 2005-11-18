// cMutex.cpp: implementation of the cMutex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cMutex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


cMutex::cMutex(char *pName, bool bInitialState, LPSECURITY_ATTRIBUTES lpMutexAttributes)
{
  handle = CreateMutex(
    lpMutexAttributes,       // no security attributes
    bInitialState,            // initially not owned
    pName);           // name of mutex
  if (handle == NULL) 
  {
    HandleInvalid();
    throw this;
  }

}

cMutex::cMutex(const cMutex&)
{
}

cMutex& cMutex::operator=(const cMutex&)
{
  return *this;
}

cMutex::~cMutex()
{
}

bool cMutex::Acquire(long TimeOut)
{
  DWORD dwWaitResult;
  dwWaitResult = WaitForSingleObject(handle,TimeOut);

  switch (dwWaitResult)
  {
  case WAIT_OBJECT_0:
    // The thread got mutex ownership.
    return true;

  case WAIT_TIMEOUT:
    // Cannot get mutex ownership due to time-out.
    return false;

  case WAIT_ABANDONED:
    // Got ownership of the abandoned mutex object.
    return true;
  }

  return true;
}

void cMutex::Release()
{
  ReleaseMutex(handle);

}






