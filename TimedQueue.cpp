// TimedQueue.cpp: implementation of the cTimedQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TimedQueue.h"
#include "TimedItem.h"
#include "TimerQueueManager.h"
#include "cEvent.h"
#include "cCritSec.h"

using namespace std;

cTimerQueueManager* theTimerQueueManager =0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cTimedQueueEvent::cTimedQueueEvent(cTimedItem* ringingObject, cTimedQueue* originator) :
mRingingObject(ringingObject), 
mOriginator(originator)
{ 
}

cTimedQueue::cTimedQueue() 
{
  newStatus = NoRing;
  if(!theTimerQueueManager) theTimerQueueManager = new cTimerQueueManager;
  theTimerQueueManager->Join(this);
}

cTimedQueue::~cTimedQueue()
{
  theTimerQueueManager->Leave(this);  
}

bool cTimedQueue::StillExists(cTimedItem* anItem)
{
  iterator index;
  for(index = begin(); index != end();++index)
  {
    if((*index) == anItem)
      return true;
  }
  return false;
}


void cTimedQueue::ReevaluateWakeup()
{
  // There could be some optimizations that could be done at this point  
  theTimerQueueManager->Requeue(this);
  if(size()>0)
    queueNotEmpty.Signal();
}


long cTimedQueue::NextWakeup()
{
  // Evaluate next wakeup
  iterator index;
  list<cTimedQueueEvent*> signals;
  unsigned long nextWakeup = INFINITE;
  bool oneExpiration;
  Acquire();
  for(index = begin();index != end();++index)
  {
    if((*index)->HasExpired())
    {
      cTimedQueueEvent *theEvent = new cTimedQueueEvent(*index,this);
      signals.push_back(theEvent);
    }
    else
    {
      nextWakeup = min(nextWakeup ,(*index)->ExpireAt());
    }
  }
  Release();
  
  list<cTimedQueueEvent*>::iterator indexSignals;
  for(indexSignals = signals.begin();indexSignals != signals.end();++indexSignals)
  {
    Notify(*indexSignals);
    (*indexSignals)->ReleaseRef();
    oneExpiration = true;
  }
  
  if(oneExpiration)
  {
    if(newStatus == NoRing)
    {
      newStatus = ItemRinging;
      cTimedQueueStatusChange* theStatusChange = new cTimedQueueStatusChange(this,ItemRinging);
      Notify(theStatusChange);
      theStatusChange->ReleaseRef();
    }
  }
  else
  {
    if(newStatus == ItemRinging)
    {
      newStatus = NoRing;
      cTimedQueueStatusChange* theStatusChange = new cTimedQueueStatusChange(this,NoRing);
      Notify(theStatusChange);
      theStatusChange->ReleaseRef();
    }
  }      
  return nextWakeup;
}


void cTimedQueue::WaitIfQueueEmpty()
{
  queueNotEmpty.Wait(INFINITE);
}


void cTimedQueue::PushBack(cTimedItem* anItem)
{
  Acquire();
  push_back(anItem);
  ReevaluateWakeup();
  Release();
}

cTimedItem* cTimedQueue::PopFront()
{
  Acquire();
  cTimedItem* retVal  = 0;
  if(size()>0)
  {
    retVal = *begin();
    pop_front();
  }
  ReevaluateWakeup();
  Release();
  return retVal;
}

int cTimedQueue::Size()
{
  Acquire();
  int retVal  = size();
  Release();
  return retVal;
}

cTimedItem* cTimedQueue::GetAt(int index)
{
  Acquire();
  int indexCopy = index;
  cTimedItem* retVal  = 0;
  iterator listIndex = begin();
  if(index < (int)size())
  {
    while((indexCopy--)!=0) ++listIndex;
    retVal = *listIndex;
  }
  Release();
  return retVal;
}

cTimedItem* cTimedQueue::RemoveAt(int index)
{
  Acquire();
  int indexCopy = index;
  cTimedItem* retVal  = 0;
  iterator listIndex = begin();
  if(index < (int)size())
  {
    while((indexCopy--)!=0) ++listIndex;
    retVal = *listIndex;
    erase(listIndex);
  }
  ReevaluateWakeup();
  Release();
  return retVal;
}

void cTimedQueue::Clear()
{
  Acquire();
  while(Size() != 0)
    delete PopFront();
  ReevaluateWakeup();
  Release();
}

void cTimedQueue::Remove(cTimedItem* anItem)
{
  Acquire();
  remove(anItem);
  anItem->Destroy();
  Release();
}
