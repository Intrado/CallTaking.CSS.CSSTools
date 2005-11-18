/* 
Call to "s.init" allocates memory for the mutex and return the new
multicondition object

Call to "s.settype" sets the type of the multicondition.

Call to "s.gettype" returns the type of the multicondition.

Call to "s.resettype" resets the multiconditon type to NIL.

Call to "s.signal" sends the signal with the setting condition.

The waiting thread has to call "s.gettype" and do a TYPECASE on the
muticondition. And the sending thread has to call "s.signal" with
the given condition. "s.signal" call first "settype" with the
given condition before sending the signal. */

#include <string>
#include "CSSMulticondition.h"
using namespace std;
#include "cAutoLock.h"
#include "ToStr.h"
#include "CSS.h"

namespace CSS
{
  const char *cCSSMulticondition::kModuleName = "CSSMulticondition";
  const int cCSSMulticondition::kNotSetMct = 0;

  
  cCSSMulticondition::cCSSMulticondition()
  {
    mSignalType = kNotSetMct;
    mpWaitEvent = new cEvent(NULL, true);
  }

  cCSSMulticondition::~cCSSMulticondition()
  {
    delete mpWaitEvent;
  }
  
  cCSSMulticondition * cCSSMulticondition::init(string aName)
  {
    mName = aName;
    return (this);
  }

  cCSSMulticondition::tSignalType cCSSMulticondition::wait()
  {
    CSSTrace(kModuleName, "Wait", "Waiting for MC : ", mName);
    mSignalType = kNotSetMct;
    mpWaitEvent->Reset();
    this->Release();
    mpWaitEvent->Wait(INFINITE);    
    this->Acquire();
    return mSignalType;
  }
  
  void cCSSMulticondition::signal(tSignalType aType)
  {
    {
      cAutoLock _lock(this);
      if (mSignalType == kNotSetMct)
      {
        mSignalType = aType;
        CSSTrace(kModuleName, "Signal", "  type setted, MC=", mName);
        mpWaitEvent->Signal();
      }
      else
      {
        CSSTrace(kModuleName, "Signal", "type <> NIL MC = ", mName);
      }
    }
  }

  void cCSSMulticondition::setType(tSignalType aType)
  {
    cAutoLock _Lock(this);
    CSSTrace(kModuleName, "SetType", "MC : ", mName, "Locking mutex");
    mSignalType = aType;
  }
  
  cCSSMulticondition::tSignalType cCSSMulticondition::getType()
  {
    return mSignalType;
  }
  
  void cCSSMulticondition::resetType()
  {
    cAutoLock _Lock(this);
    mSignalType = kNotSetMct;
  }
};
