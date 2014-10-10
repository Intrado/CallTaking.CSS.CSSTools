/***************************************************************************
* Copyright (C) Positron Public Safety Systems Inc., 2000.
* ALL RIGHTS RESERVED.
* 
*  created by:    olarouche    date created:    2000/03/15
* %derived_by:   sroy %       %date_modified:  2001/02/05 11:24:33 %
*
* %name:         cTimerManager.cpp %
* %version:      8 %
*
* References:
*
*     documents
*
* Description: implementation of the CTimerManager class.
*
*
***************************************************************************/

#include "stdafx.h"
#include "cTimerManager.h"
#include <assert.h>

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTimerEntry
 *
 * Description: Contains informations for one timer.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

class cTimerEntry
{
public:
  cTimerHandler* argument;
  unsigned int timerVal;
  unsigned int originalTimerVal;
  bool running;
  cTEvent* eventInfo;
};

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTimerManager
 *
 * Arguments: unsigned int nPrecisionMs [in], timer precision in milliseconds.
 *
 * Returns: void
 *
 * Exceptions:
 *
 * Description: Constructor
 *    Only one timer manager needs to be created for all timers.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTimerManager::cTimerManager(unsigned int nPrecisionMs): cThread("TheTimerManager")
{
  mPauseAll=false;
  mnPrecisionMs = nPrecisionMs;
  mTimers = NULL;

  // Init internal arrays
  mnElementCount=0;
  Grow();

  Run();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  ~cTimerManager
 *
 * Arguments: void
 *
 * Returns: void
 *
 * Exceptions:
 *
 * Description: Destructor
 *    Kill the timer thread loop.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTimerManager::~cTimerManager()
{
  Lock();
  mnElementCount = 0;

  if(mTimers != NULL)
  {
    delete[] mTimers;
  }

  Stop();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Reset
 *
 * Arguments: int timerId [in],
 *
 * Returns: True:   Success
 *          False:  Invalid Timer ID
 *
 * Exceptions:
 *
 * Description: Restart the specified timer.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTimerManager::Reset(unsigned int timerId)
{
  bool bResult = false;

  Lock();
  if(timerId >= 0 && timerId < mnElementCount && mTimers[timerId].argument != NULL)
  {
    mTimers[timerId].timerVal = mTimers[timerId].originalTimerVal;
    mTimers[timerId].running = true;
    bResult = true;
  }
  Unlock();

  return bResult;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  GetEvent
 *
 * Arguments: int timerId[in],
 *            cTEvent* &timerEvent [out],
 *
 * Returns: True:   Success
 *          False:  Invalid Timer ID
 *
 * Exceptions:
 *
 * Description: Get the event associated with the specified timer.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTimerManager::GetEvent(unsigned int timerId, cTEvent* &timerEvent)
{
  bool bResult = false;

  Lock();
  if(timerId >= 0 && timerId < mnElementCount && mTimers[timerId].argument != NULL)
  {
    timerEvent = mTimers[timerId].eventInfo;
    bResult = true;
  }
  Unlock();

  return bResult;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  CreateTimer
 *
 * Arguments: cTimerHandler* argument[in], Callback reference.
 *            int timerVal [in], timer interval in milliseconds.
 *            cTEvent* timerEvent [in], addtionnal timer information.
 *
 * Returns: True:   Success
 *          False:  Invalid Timer ID
 *
 * Exceptions:
 *
 * Description: When the timer expires, call cTimerHandler->OnTimerEvent(),
 *    and restart the timer. Timers are cyclics.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

unsigned int cTimerManager::CreateTimer(cTimerHandler* argument, unsigned int timerVal, cTEvent* timerEvent)
{
  unsigned int mySlot;
  Lock();

  mySlot = GetSlot();
  mTimers[mySlot].argument			= argument;
  mTimers[mySlot].timerVal			= timerVal;
  mTimers[mySlot].originalTimerVal	= timerVal;
  mTimers[mySlot].running				= true;
  mTimers[mySlot].eventInfo			= timerEvent;
  Unlock();
  return mySlot;  
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  KillTimer
 *
 * Arguments: int timerId[in],
 *
 * Returns: True:   Success
 *          False:  Invalid Timer ID
 *
 * Exceptions:
 *
 * Description: Stop the timer.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTimerManager::KillTimer(unsigned int timerId)
{
  bool bResult = false;

  Lock();
  if((timerId >= 0) && (timerId < mnElementCount) && (mTimers[timerId].argument != NULL))
  {
    bResult = true;
    mTimers[timerId].argument			= NULL;
    mTimers[timerId].timerVal			= 0;
    mTimers[timerId].running			= false;
    mTimers[timerId].originalTimerVal	= 0;

    // Delete associated event
    if(mTimers[timerId].eventInfo != NULL)
    {
      mTimers[timerId].eventInfo->ReleaseRef();
      mTimers[timerId].eventInfo = NULL;
    }
  }
  Unlock();
  return bResult;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Pause
 *
 * Arguments: int timerId [in],
 *
 * Returns: True:   Success
 *          False:  Invalid Timer ID
 *
 * Exceptions:
 *
 * Description: Suspend the timer.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTimerManager::Pause(unsigned int timerId)
{
  bool bResult = false;

  Lock();
  if(timerId >= 0 && timerId < mnElementCount && mTimers[timerId].argument != NULL)
  {
    if(mTimers[timerId].running == true)
    {
      mTimers[timerId].running = false;
      bResult = true;
    }
  }
  Unlock();

  return bResult;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Resume
 *
 * Arguments: int timerId [in],
 *
 * Returns: True:   Success
 *          False:  Invalid Timer ID or timer not paused.
 *
 * Exceptions:
 *
 * Description: Resume timer activity (after a call to Pause())
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTimerManager::Resume(unsigned int timerId)
{
  bool bResult = false;

  Lock();
  if(timerId >= 0 && timerId < mnElementCount && mTimers[timerId].argument != NULL)
  {
    if(mTimers[timerId].running == false)
    {
      mTimers[timerId].running = true;
      bResult = true;
    }
  }
  Unlock();

  return bResult;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  IsValid
 *
 * Arguments: int timerId [in],
 *
 * Returns: True:   Timer ID is valid.
 *          False:  Invalid Timer ID
 *
 * Exceptions:
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTimerManager::IsValid(unsigned int timerId)
{
  bool bResult = false;
  Lock();
  if(timerId >= 0 && timerId < mnElementCount && mTimers[timerId].argument != NULL)
  {
    bResult = true;
  }
  Unlock();

  return bResult;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  IsPaused
 *
 * Arguments: int timerId [in],
 *
 * Returns: True:   Timer is paused.
 *          False:  Timer is NOT paused.
 *
 * Exceptions:
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTimerManager::IsPaused(unsigned int timerId)
{
  bool bResult = false;

  Lock();
  if(timerId >= 0 && timerId < mnElementCount && mTimers[timerId].argument != NULL)
  {
    if(mTimers[timerId].running == false)
    {
      bResult = true;
    }
  }
  Unlock();

  return bResult;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Grow
 *
 * Arguments: void
 *
 * Returns: void
 *
 * Exceptions:
 *
 * Description: Create more timers.
 *
 * Notes: Caller should protect by calling Lock().
 *
 *-------------------------------------------------------------------------*/

void cTimerManager::Grow()
{
  cTimerEntry* newTimerList;
  unsigned int oldElementCount = mnElementCount;

  mnElementCount += 10;
  newTimerList = new cTimerEntry[mnElementCount];

  memset(newTimerList, 0 , sizeof(cTimerEntry)*mnElementCount);

  if(mTimers != NULL)
  {
    memcpy(newTimerList, mTimers , sizeof(cTimerEntry)*oldElementCount);
    delete[] mTimers;
  }

  mTimers = newTimerList;  
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  GetSlot
 *
 * Arguments: void
 *
 * Returns: An empty timer space.
 *
 * Exceptions:
 *
 * Description:
 *
 * Notes: Caller should protect by calling Lock().
 *
 *-------------------------------------------------------------------------*/

unsigned int cTimerManager::GetSlot()
{
  unsigned int i;

  for(i=0;i<mnElementCount;i++)
  {
    if(mTimers[i].argument == NULL)
    {
      return i;
    }
  }

  // No free element found - increase array size
  Grow();

  // and take the new last element (that should be free)
  return mnElementCount-1;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Lock-UnLock
 *
 * Arguments: void
 *
 * Returns: void
 *
 * Exceptions:
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

void cTimerManager::Lock()
{
  if(!mMutex.Acquire(INFINITE))
  {
    assert(FALSE);
  }
}

void cTimerManager::Unlock()
{
  mMutex.Release();
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Pulse
 *
 * Arguments: void
 *
 * Returns: void
 *
 * Exceptions:
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

void cTimerManager::Pulse()
{
  unsigned int index;
  // a tick just happened - see if a timer elapsed
  Lock();

  for(index = 0; index < mnElementCount; index++)
  {
    if(mTimers[index].argument != NULL && mTimers[index].running == true)
    {
      if(mTimers[index].timerVal > mnPrecisionMs)
      {
        mTimers[index].timerVal -= mnPrecisionMs;
      }
      else // Timer ringing !
      {
        cTimerHandler* thisItem = mTimers[index].argument;
        Unlock();
        LockPause();
        if (mPauseAll==false)
          thisItem->OnTimerEvent(index, mTimers[index].eventInfo);
        UnLockPause();
        mTimers[index].timerVal = mTimers[index].originalTimerVal;
        Lock();
      }
    }
  }  
  Unlock();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  StartTimerMgr
 *
 * Arguments: void
 *
 * Returns: void
 *
 * Exceptions:
 *
 * Description: Thread main loop.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

long cTimerManager::ThreadProc()
{
  // Set overflow to true for first pass.
  bool bOverflow = false;

  DWORD ticker = GetTickCount() + (DWORD) (mnPrecisionMs);
  DWORD currTick;

  while(_isRunning) // infinite loop
  {
    // Determine if an overflow condition occured.
    if((ticker - mnPrecisionMs) >  ticker)
    {
      bOverflow = true;
    }

    while((ticker > (currTick = GetTickCount())) || bOverflow)
    {
      unsigned int nSleepTime;

      if(bOverflow)
      {
        bOverflow = false;
        nSleepTime = mnPrecisionMs;
      }
      else
      {
        nSleepTime = ticker - currTick;
      }

      Sleep(nSleepTime);
    }

    // reinit the counter at now + TICKTIMER sec
    ticker += (DWORD)(mnPrecisionMs);
    Pulse();      
  }

  return 0;
}

void cTimerManager::PauseAll ()
{
   LockPause();
   mPauseAll=true;
   UnLockPause();
}

void cTimerManager::UnPauseAll ()
{
   LockPause();
   mPauseAll=false;
   UnLockPause();
}

void cTimerManager::LockPause()
{
  if(!mPauseLock.Acquire(INFINITE))
  {
    assert(FALSE);
  }
}


void cTimerManager::UnLockPause()
{
  mPauseLock.Release();
}

bool cTimerManager::ChangeTimeout (unsigned int timerId,unsigned int timerVal)
{
 bool bResult = false;
 Lock();
 if((timerId >= 0 && timerId < mnElementCount) && (mTimers[timerId].argument != NULL))
 {
  mTimers[timerId].timerVal			= timerVal;
  mTimers[timerId].originalTimerVal	= timerVal;
  bResult=true;
 }
 Unlock();
 return bResult;
}