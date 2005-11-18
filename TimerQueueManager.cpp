// TimerQueueManager.cpp: implementation of the cTimerQueueManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TimerQueueManager.h"
#include "TimedQueue.h"

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


cTimerQueueManager::cTimerQueueManager() : cThread("Timed Queue Manager")
{
  mQueueEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
  Run();
}


cTimerQueueManager::~cTimerQueueManager()
{
  Stop();
  CloseHandle(mQueueEvent);
}

void cTimerQueueManager::Join(cTimedQueue *aQueue)
{
  managedObjects.push_back(aQueue);
}

void cTimerQueueManager::Leave(cTimedQueue *aQueue)
{
  managedObjects.remove(aQueue);
}

void cTimerQueueManager::Requeue(cTimedQueue *aQueue)
{
  SetEvent(mQueueEvent);
}

long cTimerQueueManager::ThreadProc()
{
  while(1)
  {
    // Create the event list to wait on
    int count = managedObjects.size()+1;
    long nextWakeup = INFINITE;    
    int iIndex = 1;
    list<cTimedQueue*>::iterator index;

    for(index = managedObjects.begin();    index!=managedObjects.end();    ++index)
    {
      nextWakeup = min(nextWakeup,(*index)->NextWakeup());
    }
    
    WaitForSingleObject(mQueueEvent,nextWakeup -GetTickCount());
  } 
  return 0;
}
