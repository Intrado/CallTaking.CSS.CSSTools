// cLimitedTimeQueue.h: interface for the cLimitedTimeQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIMITEDTIMEQUEUE_H__4AE62A0B_80D7_11D3_8578_0008C7CB118E__INCLUDED_)
#define AFX_CLIMITEDTIMEQUEUE_H__4AE62A0B_80D7_11D3_8578_0008C7CB118E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TimedQueue.h"
#include "TimedItem.h"
#include "cNotifiableObject.h"

class CSSTOOLS_API cLimitedTimeQueue : public cTimedQueue  , private cNotifiableObject
{
public:
	cLimitedTimeQueue(int MaximumTimeInQueue);
	virtual ~cLimitedTimeQueue();

  class cLimitedQueueItem : public cTimedItem {
  public:
    cLimitedQueueItem() : cTimedItem(0,0) {};
  };

  virtual void PushBack(cLimitedQueueItem* anItem);
private:
 	virtual void OnEvent(cTEvent *pEvt,void* pToken);
  int mMaximumTimeInQueue;
};

#endif // !defined(AFX_CLIMITEDTIMEQUEUE_H__4AE62A0B_80D7_11D3_8578_0008C7CB118E__INCLUDED_)
