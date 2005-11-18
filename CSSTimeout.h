#ifndef __CSSTimeout_INCLUDED__
#define __CSSTimeout_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include <set>
#include "CSSObject.h"
#include "cThread.h"
#include "CSSMulticondition.h"

namespace CSS
{
  class CSSTOOLS_API cCSSTimeout : public cCSSObject
  {
  public:
    enum TimerStatus
    {
      Set, Expired, Disable
    };

    static void Add(cCSSTimeout * newtm); // Kept for backward compatibility.  Does nothing.
    static void DeleteTimeout(cCSSTimeout * oldtm); // Kept for backward compatibility.  Only calls stop().

    static const int kToMct;
    static const char *kModuleName;

    cCSSTimeout(std::string name = "");
    virtual ~cCSSTimeout();
    virtual cCSSTimeout *init(cCSSMulticondition *pMC);
    virtual void start(double interval);
    virtual void stop();
    virtual void disable();
    virtual TimerStatus getStatus();
    virtual int getValue();

  private:
    class cTimeoutThread : public cThread
    {
    public:
      struct cStartTimerParam
      {
        cStartTimerParam(cCSSTimeout *pTO, unsigned long timerDuration): mpTO(pTO), mTimerDuration(timerDuration) {}

        cCSSTimeout *mpTO;
        unsigned long mTimerDuration;
      };

      struct cStopTimerParam
      {
        cStopTimerParam(cCSSTimeout *pTO, DWORD requestorThreadID, bool synchronous): mpTO(pTO), mRequestorThreadID(requestorThreadID), mSynchronous(synchronous) {}

        cCSSTimeout *mpTO;
        DWORD mRequestorThreadID;
        bool mSynchronous;
      };

      struct cTimerData
      {
        cTimerData(): mpTO(0) {}
        cTimerData(cCSSTimeout *pTO): mpTO(pTO) { mName = mpTO->mName; }
        operator cCSSTimeout*() { return mpTO; }

        cCSSTimeout *mpTO;
        std::string mName;
      };

      cTimeoutThread(): cThread("TimeoutThread") {}
      ~cTimeoutThread() {}
      virtual void VoidTimeout(cCSSTimeout *pTO); // This function must only be called by the timeout thread itself.
      virtual DWORD GetID() { return _threadId; }
      virtual HANDLE GetHandle() { return _threadHandle; }

    private:
      typedef std::map<UINT_PTR, cTimerData> tSystemToUserTimerMap;
      typedef std::map<cCSSTimeout*, UINT_PTR> tUserToSystemTimerMap;
      typedef std::set<cCSSTimeout*> tUserTimerSet;

      virtual long ThreadProc();
      virtual UINT_PTR HandleStartTimer(cStartTimerParam *pTimerParam);
      virtual bool HandleStopTimer(cCSSTimeout *pTO);
      virtual bool IsTimerVoided(cCSSTimeout *pTO);

      tSystemToUserTimerMap mSystemTimerMap;
      tUserToSystemTimerMap mUserTimerMap;
      cCritSec mMapCS; // Lock when accessing mSystemTimerMap or mUserTimerMap.
      tUserTimerSet mVoidedUserTimerSet;
    };

    static cTimeoutThread *gpTimeoutThread;

    virtual void Stop(bool synchronous);
    virtual bool OkToExpire();
    virtual void SignalStopProcessed() { mStopEvt.Signal(); }

    cCSSMulticondition *mpMC;
    time_t mExpirationTime;
    TimerStatus mStatus;
    cCritSec mStatusCS;
    cEvent mStopEvt;
    std::string mName;

    friend class cTimeoutThread;
    friend struct cTimeoutThread::cTimerData;
  };
};

#endif
