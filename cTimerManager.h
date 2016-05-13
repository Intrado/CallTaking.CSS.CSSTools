/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000.
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    olarouche    date created:    2000/03/15
 * %derived_by:   sroy %       %date_modified:  2001/02/05 11:24:38 %
 *
 * %name:         cTimerManager.h %
 * %version:      7 %
 *
 * References:
 *
 *     documents
 *
 * Description: interface for the cTimerManager class.
 *    Only one instance of a cTimerManager needs to be created.
 *
 *
 ***************************************************************************/

#ifndef CTIMERMANAGER_H
#define CTIMERMANAGER_H

#include <windows.h>
#include "cTimerHandler.h"
#include "cMutex.h"
#include "cCritSec.h"
#include "cThread.h"

class cTEvent;

class cTimerEntry;

#define INVALID_TIMER 0xFFFFFFFF

class CSSTOOLS_API cTimerManager : public cThread
{
public:
  cTimerManager(unsigned int nPrecisionMs = 100);
  virtual ~cTimerManager();

  bool GetEvent(unsigned int timerId, cTEvent* &timerEvent);

  unsigned int CreateTimer(cTimerHandler* argument, unsigned int timerVal, cTEvent* timerEvent = NULL);
  bool KillTimer(unsigned int timerId);
  bool IsValid(unsigned int timerId);
  bool Pause(unsigned int timerId);
  bool Resume(unsigned int timerId);
  bool Reset(unsigned int timerId);
  bool IsPaused(unsigned int timerId);
  bool ChangeTimeout (unsigned int timerId,unsigned int timerVal);
  void PauseAll ();
  void UnPauseAll ();

protected:

  // cTimerManager main loop.
  virtual long ThreadProc();

  void Unlock();
  void Lock();
  void UnLockPause();
  void LockPause();

private:

  void Pulse();
  void Grow();
  unsigned int GetSlot();

  cTimerEntry* mTimers;
  unsigned int mnElementCount;

  cMutex mMutex;
  cCritSec mPauseLock;

  unsigned int mnPrecisionMs;
  bool mPauseAll;
};

#endif

