#include <string>
#include "cCondition.h"
using namespace std;
#include "cAutoLock.h"
#include "ToStr.h"
#include "CSS.h"

const char *cCondition::kModuleName = "CSSCondition";

cCondition::cCondition()
{
  mpWaitEvent = new cEvent(NULL, true);
}

cCondition::~cCondition()
{
  delete mpWaitEvent;
}

void cCondition::wait(cCritSec *apLock)
{
  mpWaitEvent->Reset();
  apLock->Release();

  if (CSSToolsTraceOn)
  {
    DiagTrace(kModuleName, "Wait", "Waiting for Condition");
  }

  mpWaitEvent->Wait(INFINITE);
  apLock->Acquire();
}

void cCondition::wait(cCritSec &aLock)
{
  wait(&aLock);
}

void cCondition::signal()
{
  if (CSSToolsTraceOn)
  {
    DiagTrace(kModuleName, "Signal", "Signaling condition");
  }
  mpWaitEvent->Signal();
}
