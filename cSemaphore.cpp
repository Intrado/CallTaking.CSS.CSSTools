// cSemaphore.cpp: implementation of the cSemaphore class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSemaphore.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cSemaphore::cSemaphore(long InitialCount,long MaximumCount,char* pName,LPSECURITY_ATTRIBUTES lpAttributes)
{
  handle= CreateSemaphore( lpAttributes,  // pointer to security attributes
               InitialCount,    // initial count  
               MaximumCount,    // maximum count
               pName);        // pointer to semaphore-object name);
  if(handle ==NULL)
  {
    HandleInvalid();
    throw this;
  }   

}

cSemaphore::cSemaphore(const cSemaphore& s)
{
}

cSemaphore& cSemaphore::operator=(const cSemaphore& s)
{
  return *this;
}

cSemaphore::~cSemaphore()
{
}


void cSemaphore::Release()
{
  ReleaseSemaphore(handle,1,NULL);
}

void cSemaphore::Release(BOOL & result)
{
  result=ReleaseSemaphore(handle,1,NULL);
}



bool cSemaphore::Acquire(long timeout)
{
 DWORD dwWaitResult; 

 dwWaitResult = WaitForSingleObject(handle,timeout); 

 switch (dwWaitResult)     
  {        
   // The thread got mutex ownership.
   case WAIT_OBJECT_0:             
   return true;
   // Cannot get mutex ownership due to time-out.
   case WAIT_TIMEOUT:             
   return false;
   // Got ownership of the abandoned mutex object.
   case WAIT_ABANDONED:             
   return true;     
 }   

 return true;
  
}

