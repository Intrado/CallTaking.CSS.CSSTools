// TimedQueue.h: interface for the cTimedQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEDQUEUE_H__6950BB7D_8635_11D2_BAA3_00805F7DF67A__INCLUDED_)
#define AFX_TIMEDQUEUE_H__6950BB7D_8635_11D2_BAA3_00805F7DF67A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable: 4251 4275)
#endif // _MSC_VER > 1000

// Includes
#include "cNotifierObject.h"
#include "cTEvent.h"
#include "cEvent.h"
#include "cCritSec.h"
#include <list>

class cTimedItem;

class CSSTOOLS_API cTimedQueue : private std::list<cTimedItem*> , public cNotifierObject , public cCritSec
{
public:
	cTimedQueue();
	virtual ~cTimedQueue();
	virtual long NextWakeup();
  virtual void Clear();
  virtual bool StillExists(cTimedItem* anItem);

  virtual void PushBack(cTimedItem* anItem);
  virtual cTimedItem* PopFront();
  virtual cTimedItem* GetAt(int index);
  virtual cTimedItem* RemoveAt(int index);
	virtual void WaitIfQueueEmpty();
  virtual void Remove(cTimedItem* anItem);
  virtual int Size();

  typedef enum { NoRing, ItemRinging } QueueStatus;

private:
	virtual void ReevaluateWakeup();
  QueueStatus newStatus;
	cEvent queueNotEmpty;
  cEvent wakeupEvent;
};

// As a Notifier, a cTimedQueue object produces the following objects
class cTimedQueueEvent : public cTEvent
{
public:
  cTimedQueueEvent(cTimedItem* ringingObject, cTimedQueue* originator);
  cTimedItem* mRingingObject;
  cTimedQueue* mOriginator;
};

// As a Notifier, a cTimedQueue object produces the following objects
class cTimedQueueStatusChange : public cTEvent
{
public:
  cTimedQueueStatusChange(cTimedQueue* originator, cTimedQueue::QueueStatus newState) : mOriginator(originator), mState(newState) {}
  cTimedQueue::QueueStatus mState;
  cTimedQueue* mOriginator;
};

#endif // !defined(AFX_TIMEDQUEUE_H__6950BB7D_8635_11D2_BAA3_00805F7DF67A__INCLUDED_)
