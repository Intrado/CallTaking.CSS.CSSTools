// ThreadPool.h: interface for the cThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADPOOL_H__1A5E47C4_09F2_11D4_85A9_0008C7CB118E__INCLUDED_)
#define AFX_THREADPOOL_H__1A5E47C4_09F2_11D4_85A9_0008C7CB118E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class cAbstractPBLMessage;
class cThreadPoolHelper;
class cFiberProc;

class cThreadPool  
{
public:
  // minThreadCount = Minimum of thread to keep alive
  // maxThreadCount = Maximun of thread to keep alive
  // queueThreshold = Queue length before starting new threads
	cThreadPool(int minThreadCount = 10, int maxThreadCount = 10, int queueThreshold = 10);
	virtual ~cThreadPool();

  void Post(cAbstractPBLMessage* pMsg);  // enqueues a message in the exec queue
  bool RegisterFiber(cFiberProc* pFiber); // returns true, if the registration went ok
private:  
  cThreadPoolHelper* pHelper;
};

class cFiberProc {
public:
  cFiberProc() {};
  virtual void Execute(cAbstractPBLMessage* executeItem) = 0;
};
#endif // !defined(AFX_THREADPOOL_H__1A5E47C4_09F2_11D4_85A9_0008C7CB118E__INCLUDED_)
