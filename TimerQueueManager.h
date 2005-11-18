// TimerQueueManager.h: interface for the cTimerQueueManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERQUEUEMANAGER_H__959B1278_8A35_11D2_BAA7_00805F7DF67A__INCLUDED_)
#define AFX_TIMERQUEUEMANAGER_H__959B1278_8A35_11D2_BAA7_00805F7DF67A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable: 4251 4275)
#endif // _MSC_VER > 1000
#include <list>

#include "cThread.h"

class cTimedQueue;

class CSSTOOLS_API cTimerQueueManager  : public cThread
{
public:
	cTimerQueueManager();
	virtual ~cTimerQueueManager();
	void Requeue(cTimedQueue *aQueue);
	virtual long ThreadProc();
	virtual void Leave(cTimedQueue* aQueue);
	virtual void Join(cTimedQueue* aQueue);
private:
  std::list<cTimedQueue*> managedObjects;
  HANDLE mQueueEvent;
};

#endif // !defined(AFX_TIMERQUEUEMANAGER_H__959B1278_8A35_11D2_BAA7_00805F7DF67A__INCLUDED_)
