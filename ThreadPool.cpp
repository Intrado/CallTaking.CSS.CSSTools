// ThreadPool.cpp: implementation of the cThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThreadPool.h"
#include "cThread.h"
#include <list>
using namespace std;
typedef list<cThread*> threadSet;
/*
class cThreadPoolHelper {
public:
  cThreadPoolHelper();
  threadSet readyThreads;
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cThreadPool::cThreadPool(int minThreadCount, int maxThreadCount, int queueThreshold)
{
  pHelper = 0;    // Coverity #15094: initialize the pointer field
}

cThreadPool::~cThreadPool()
{

}

void cThreadPool::Post(cAbstractPBLMessage* pMsg)  // enqueues a message in the exec queue
{

}

bool cThreadPool::RegisterFiber(cFiberProc* pFiber) // returns true, if the registration went ok
{
  return true;
}
