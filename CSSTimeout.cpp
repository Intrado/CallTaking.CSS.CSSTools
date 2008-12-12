/*
Call to "s.init" initialize a new timer by registering the condition
to signal when the timeout expires. The calling thread should provide
a condition of type Multicondition.T .

Call to "s.start" starts a timer, The interval in seconds must be specified .

Call to "s.stop" disables the specified timer, this is automaticly done
when the timer expires (see TimeoutMgr ()) .
A call to "s.stop" does not remove a timer object from the list.
*/


#pragma warning(disable: 4786)
#include <string>
#include <time.h>
#include "CSSTimeout.h"
using namespace std;
#include "ToStr.h"
#include "CSSMulticondition.h"
#include "cCritSec.h"
#include "cAutoLock.h"
#include "CSSostringstream.h"
#include "CSS.h"


// Application-defined messages
#define WM_APP_START_TIMER WM_APP+1
#define WM_APP_STOP_TIMER WM_APP+2


namespace CSS
{
const int cCSSTimeout::kToMct = -1;
const char *cCSSTimeout::kModuleName = "CSSTimeout";
cCSSTimeout::cTimeoutThread *cCSSTimeout::gpTimeoutThread = 0;


cCSSTimeout::cCSSTimeout(string name)
{
  mName = name;
  mpMC = 0;
  mExpirationTime = 0;
  mStatus = Disable;

  /* Since there is one static cCSSTimeout instance somewhere in telephony, we know the first
     call to this constructor is made by the main thread before the program's main is called (in
     other words, before any other thread is created).  That's why we can test gpTimeoutThread
     without using a critical section. */
  if (gpTimeoutThread == 0)
  {
    gpTimeoutThread = new cTimeoutThread;
    gpTimeoutThread->Run(true);
  }
}


cCSSTimeout::~cCSSTimeout()
{
  tCSSostringstream trace;
  (trace = "Starting for timer <") << mName << "> (" << showbase << hex << this << ")";
  DiagTrace(kModuleName, "~cCSSTimeout", trace.str());

  DeleteTimeout(this);
}


cCSSTimeout* cCSSTimeout::init(cCSSMulticondition *pMC)
{
  mpMC = pMC;
  mStatus = Expired;
  return this;
}


void cCSSTimeout::start(double interval)
{
  if (Diag::TraceOn)
  {
    tCSSostringstream trace;
    if (mpMC)
    {
      (trace = "Start timer <") << mName << "> (" << showbase << hex << this << "), interval == " << dec << interval << ", MC == <" << mpMC->mName << ">";
      DiagTrace(kModuleName, "Start", trace.str());
    }
    else
    {
      (trace = "Start timer <") << mName << "> (" << showbase << hex << this << "), interval == " << dec << interval << ", no multicondition";
      DiagTrace(kModuleName, "Start", trace.str());
    }
  }

  mExpirationTime = time(0) + (time_t)interval;
  mStatus = Set; // Set here and not in cTimeoutThread to preserve old behavior.

  // Convert the interval to milliseconds.
  unsigned long duration = (unsigned long)(interval * 1000);
  cTimeoutThread::cStartTimerParam *pParam = new cTimeoutThread::cStartTimerParam(this, duration);
  PostThreadMessage(gpTimeoutThread->GetID(), WM_APP_START_TIMER, (WPARAM)pParam, 0);
}


void cCSSTimeout::stop()
{
  if (Diag::TraceOn)
  {
    tCSSostringstream trace;
    (trace = "Stop timer <") << mName << "> (" << showbase << hex << this << ")";
    DiagTrace(kModuleName, "Stop", trace.str());
  }
  Stop(false);
}


/* If synchronous is true, it means that we want to insure that 'this' won't
   be accessed after this function exits. */
void cCSSTimeout::Stop(bool synchronous)
{
  {
    cAutoLock a(mStatusCS);
    mStatus = Expired; // Set here and not in cTimeoutThread to preserve old behavior.
  }

  DWORD timeoutThreadID = gpTimeoutThread->GetID();
  DWORD currentThreadID = GetCurrentThreadId();

  cTimeoutThread::cStopTimerParam *pParam = new cTimeoutThread::cStopTimerParam(this, currentThreadID, synchronous);
  PostThreadMessage(timeoutThreadID, WM_APP_STOP_TIMER, (WPARAM)pParam, 0);

  if (synchronous)
  {
    if (currentThreadID != timeoutThreadID)
    {
      mStopEvt.Wait(INFINITE);
    }
    else
    {
      /* We don't wait for the timeout thread to process the WM_APP_STOP_TIMER message, because
         the thread would wait for itself indefinitely.  We will flag this timer as invalid, so that
         if the thread was to receive a WM_TIMER message for this timer before it received the
         WM_APP_STOP_TIMER we just posted, it won't try to access the timer and signal its multicondition. */
      gpTimeoutThread->VoidTimeout(this);
    }
  }
}


void cCSSTimeout::disable()
{
  if (Diag::TraceOn)
  {
    tCSSostringstream trace;
    (trace = "Starting for timer <") << mName << "> (" << showbase << hex << this << ")";
    DiagTrace(kModuleName, "Disable", trace.str());
  }
  {
    cAutoLock a(mStatusCS);
    mStatus = Disable;
  }
  stop();
}


cCSSTimeout::TimerStatus cCSSTimeout::getStatus()
{
  cAutoLock a(mStatusCS);
  return mStatus;
}


bool cCSSTimeout::OkToExpire() // Returns true if status was set to Expired.
{
  bool res = false;
  cAutoLock a(mStatusCS);
  // The status may have been set to Expired by stop() or to Disable by disable().
  if (mStatus == Set)
  {
    mStatus = Expired;
    res = true;
  }
  return res;
}


int cCSSTimeout::getValue() // Return the number of seconds left before expiration.
{
  int value = 0;

  TimerStatus st = Disable;
  {
    cAutoLock a(mStatusCS);
    st = mStatus;
  }

  if (st == Set)
  {
    value = (mExpirationTime - time(0));
    if (value < 0)
    {
      value = 0;
    }
  }

  DiagTrace(kModuleName,"GetValue","Actual Time", value);
  return value;
}


// Kept for backward compatibility.
void cCSSTimeout::Add(cCSSTimeout * newtm)
{
}


// Kept for backward compatibility.
void cCSSTimeout::DeleteTimeout(cCSSTimeout * oldtm)
{
  tCSSostringstream trace;
  (trace = "Remove timer <") << oldtm->mName << "> (" << showbase << hex << oldtm << ")";
  DiagTrace(kModuleName, "DeleteTimeout", trace.str());

  oldtm->Stop(true);
}


// cTimeoutThread
// ==============
void cCSSTimeout::cTimeoutThread::VoidTimeout(cCSSTimeout *pTO)
{
  mVoidedUserTimerSet.insert(pTO);
}


bool cCSSTimeout::cTimeoutThread::IsTimerVoided(cCSSTimeout *pTO)
{
  tUserTimerSet::iterator it = mVoidedUserTimerSet.find(pTO);
  bool res = (it != mVoidedUserTimerSet.end());
  return res;
}


UINT_PTR cCSSTimeout::cTimeoutThread::HandleStartTimer(cStartTimerParam *pTimerParam)
{
  cAutoLock a(mMapCS);

  UINT_PTR timer = SetTimer(0, 0, pTimerParam->mTimerDuration, 0);
  if (timer != 0)
  {
    // Kill any previous timer for pTimerParam->mpTO since we want to restart this timer with a new interval.
    tUserToSystemTimerMap::iterator it = mUserTimerMap.find(pTimerParam->mpTO);
    if (it != mUserTimerMap.end())
    {
      KillTimer(0, it->second);
    }

    mUserTimerMap[pTimerParam->mpTO] = timer;
    mSystemTimerMap[timer] = pTimerParam->mpTO;
  }
  else
  {
    DWORD err = GetLastError();
    tCSSostringstream trace;
    (trace = "Unable to set timer for user timer ") << showbase << hex << pTimerParam->mpTO << ": system error " << err;
    DiagWarning(kModuleName, "cTimeoutThread::HandleStartTimer", trace.str());
  }

  return timer;
}


bool cCSSTimeout::cTimeoutThread::HandleStopTimer(cCSSTimeout *pTO)
{
  cAutoLock a(mMapCS);

  UINT_PTR timer = 0;
  tUserToSystemTimerMap::iterator it = mUserTimerMap.find(pTO);
  bool found = (it != mUserTimerMap.end());
  if (found)
  {
    timer = it->second;
    if (KillTimer(0, timer) != TRUE)
    {
      DWORD err = GetLastError();
      if (err != ERROR_SUCCESS)
      {
        tCSSostringstream trace;
        (trace = "Unable to kill user timer ") << showbase << hex <<  pTO << " associated with system timer " << timer << ": system error " << err;
        DiagWarning(kModuleName, "cTimeoutThread::HandleStopTimer", trace.str());
        timer = 0;
      }
    }

    mUserTimerMap.erase(pTO);
    mSystemTimerMap.erase(timer);
  }
  return found;
}


long cCSSTimeout::cTimeoutThread::ThreadProc()
{
  tCSSostringstream trace;
  MSG msg;

  DiagThreadFork("CSSTimeout Thread");

  while (GetMessage(&msg, 0, 0, 0) != 0)
  {
    switch (msg.message)
    {
    case WM_APP_START_TIMER:
      {
        cAutoLock a(mMapCS);
        cStartTimerParam *pParam = (cStartTimerParam*)msg.wParam;
        HandleStartTimer(pParam);
        delete pParam;
      }
      break;

    case WM_APP_STOP_TIMER:
      {
        cAutoLock a(mMapCS);
        cStopTimerParam *pParam = (cStopTimerParam*)msg.wParam;
        cCSSTimeout *pTO = pParam->mpTO;
        DWORD requestorID = pParam->mRequestorThreadID;

        HandleStopTimer(pTO);

        if (pParam->mSynchronous)
        {
          if (requestorID != _threadId)
          {
            (trace = "Calling SignalStopProcessed for user timer <") << pTO->mName << "> (" << showbase << hex << pTO << ")";
            DiagTrace(kModuleName, "cTimeoutThread::ThreadProc", trace.str());

            pTO->SignalStopProcessed();
          }
          else
          {
            (trace = "Erasing voided user timer (") << showbase << hex << pTO << ")";
            DiagTrace(kModuleName, "cTimeoutThread::ThreadProc", trace.str());

            mVoidedUserTimerSet.erase(pTO);
          }
        }
        delete pParam;
      }
      break;

    case WM_TIMER:
      {
        cAutoLock a(mMapCS);
        UINT_PTR timer = (UINT_PTR)msg.wParam;
        tSystemToUserTimerMap::iterator it = mSystemTimerMap.find(timer);
        if (it != mSystemTimerMap.end())
        {
          cCSSTimeout *pTO = it->second.mpTO;
          string name = it->second.mName;
          bool voided = IsTimerVoided(pTO);

          /* Use the name that was stored in the map instead of accessing pTO to allow the trace to come
             through even if pTO has been deleted.  The crash will happen below but at least
             we will have a trace that will tell us what timer caused the crash. */
          (trace = "User timer <") << name << "> (" << showbase << hex << pTO << ") expired";
          DiagTrace(kModuleName, "cTimeoutThread::ThreadProc", trace.str());

          bool found = HandleStopTimer(pTO);

          if (!voided && found && pTO->OkToExpire())
          {
            if (pTO->mpMC)
            {
              (trace = "Signaling MC <") << pTO->mpMC->mName << ">";
              DiagTrace(kModuleName, "cTimeoutThread::ThreadProc", trace.str());

              pTO->mpMC->signal(kToMct);
            }
          }

          if (voided)
          {
            mVoidedUserTimerSet.erase(pTO);
          }
        }
        else
        {
          (trace = "WM_TIMER: No user timer associated with system timer ") << timer;
          DiagWarning(kModuleName, "cTimeoutThread::ThreadProc", trace.str());
          KillTimer(0, timer);
        }
      }
      break;
    }
  }

  DiagOut(kModuleName, "cTimeoutThread::ThreadProc", "Stopped");
  return 0;
}
}

