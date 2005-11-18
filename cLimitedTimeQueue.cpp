// cLimitedTimeQueue.cpp: implementation of the cLimitedTimeQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cLimitedTimeQueue.h"

const char* const NotifiableObjectGroupName = "LimitedTimeQueue";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cLimitedTimeQueue::cLimitedTimeQueue(int MaximumTimeInQueue) : mMaximumTimeInQueue(MaximumTimeInQueue) , cNotifiableObject(NotifiableObjectGroupName)
{
  Register(this,0);
}

cLimitedTimeQueue::~cLimitedTimeQueue()
{
  Unregister(this,0);
}

void cLimitedTimeQueue::OnEvent(cTEvent *pEvt,void*)
{
  if(cTimedQueueEvent* asTimedQueueEvent = dynamic_cast<cTimedQueueEvent*>(pEvt))
  {
    Remove(asTimedQueueEvent->mRingingObject);
  }
}

void cLimitedTimeQueue::PushBack(cLimitedQueueItem* anItem)
{
  anItem->SetAliveDuration(mMaximumTimeInQueue);
  cTimedQueue::PushBack(anItem);
}

