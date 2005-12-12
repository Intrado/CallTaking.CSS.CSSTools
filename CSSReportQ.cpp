/*  This package manages a queue of events in the following way:
   The queue is created by the user. A thread is automaticaly started
   to monitor the contents of the Queue. Each time a event is found in
   the queue the previously registered thread is invoked with that event
   as parameter.

   The call "s.init" creates a new report queue and starts the thread that
   will monitor that queue. The caller registers the apply method that will
   be inovked when Events are found in the Queue.

   The call "s.report" enables the user to report an event. This event will
   be inserted in the queue.

   The call "s.terminate" terminates the thread that monitors the queue
   when the user does not want to use that queue anymore. */
#include <string>
#include <time.h>
using namespace std;
#include "CSSReportQ.h"
#include "CSSEvent.h"
#include "CSSReporter.h"
#include "NewAutoInitialize.h"
#include <algorithm>
#include <functional>
#include "cCritSec.h"
#include "cAutoLock.h"
#include "CSS.h"


namespace CSS
{
  const char *cCSSReportQ::moduleName = "CSSReportQ";
  cCritSec lockReport;
  int nbQueue = 0;
   
  cCSSReportQ::cCSSReportQ()
  {
    mpNewEvent = NULL;
    mpReportSuspended = NULL;
    nbevt = 0;
    id = 0;
    suspended = 0;
    mReportQueueName="RQ";
  };

  cCSSReportQ::~cCSSReportQ()
  {
    // Terminate the thread before deleting anything
    terminate();
    delete mpNewEvent;
    delete mpReportSuspended;
  }

  void cCSSReportQ::DoInit()
  {
    eventQueue.clear();
    /*  Create Event Queue */    
    mpNewEvent = new cCondition;
    mpReportSuspended = new cCondition;

    //  Now let's start the event dispatching thread
    eventThread = new EventThread(mReportQueueName);
    eventThread->init(((cCSSReportQ* )this));
    eventThread->Run(true);

    {
      cAutoLock _lock(&lockReport);
      id = nbQueue;
      nbQueue++;
    };
    isInFlushMode = false;
  }
  
  cCSSReportQ * cCSSReportQ::init()
  {
    CSSTrace(moduleName, "Init", "Starting");;
    DoInit();
    return ((cCSSReportQ* )this);
  }

  cCSSReportQ * cCSSReportQ::init(string reportQueueName)
  {
    mReportQueueName=reportQueueName;
    CSSTrace(moduleName, "Init","Queue Name : ", reportQueueName);;
    DoInit();
    return ((cCSSReportQ* )this);
  }
  
  bool cCSSReportQ::report(cCSSEvent * event)
  {
    CSSTrace(moduleName, "Report", mReportQueueName, " : Starting " );
    {
      cAutoLock _lock(&mLock);
      
      if (isInFlushMode == false)
      {
        time(&event->_timeStamp);
        eventQueue.push_back(event);
        if (nbevt < MAXLONG) 
          nbevt++;
        else  
          nbevt = 0;
        mpNewEvent->signal();
      }
      else
      {
        delete event;
        CSSTrace (moduleName, "Report", mReportQueueName, " : Event flushed ");
      }
    }
    CSSTrace(moduleName, "Report", mReportQueueName, " : Ending");
    return true;
  }
  
  void cCSSReportQ::start()
  {
		{
			cAutoLock _lock(&mLock);
			isInFlushMode = false;
		}
  }
  
  void cCSSReportQ::stop()
  {
		{
			cAutoLock _lock(&mLock);
			isInFlushMode = true;
		}
  }
  
  bool cCSSReportQ::terminate()
  {
    CSSTrace(moduleName, "Terminate", "Starting");;
    if (eventThread)
    {
      {
        cAutoLock _lock(&mLock);
        terminateThread = true;
        suspended = 0;
        mpReportSuspended->signal();
        mpNewEvent->signal();
      }
      cThread::Join(eventThread);
      eventThread = NULL;
    }
    CSSTrace(moduleName, "Terminate", "Ending");;
    return true;
  }
  
  void cCSSReportQ::suspend()
  {
    {
      cAutoLock _lock(&mLock);
      suspended++;
    };
  }
  
  void cCSSReportQ::resume()
  {
    {
      cAutoLock _lock(&mLock);
      if (suspended > 0)
      {
        suspended--;
      }
      if (suspended == 0)
      {
        mpReportSuspended->signal();
      }
    }
  }
  
  void cCSSReportQ::EventThread::init(cCSSReportQ * rep)
  {
    /* ****   ROUTINES NOT IN PUBLIC INTERFACE                 **** */
    /*  Initialization routine for the Event thread */
    report = dynamic_cast<cCSSReportQ*>(rep);
    {
      cAutoLock _lock(&report->mLock);
      report->terminateThread = false;
    };
  }
  
  long cCSSReportQ::EventThread::ThreadProc()
  {
    cCSSEvent * currEvent;
    /*  Routine that monitors the event queue and invokes the apply method
    * when a event is found in the queue */
    DiagThreadFork (report->mReportQueueName + " : CSSReportQ Thread" );
    while (1)
    {
      {
        cAutoLock _lock(&report->mLock);
        if (report->eventQueue.size() == 0)
        {
          CSSTrace(moduleName, "PerformMonitor",report->mReportQueueName, " : eventQ empty : Waiting ");
          if (report->terminateThread == true)
          {
            CSSTrace(moduleName, "PerformMonitor",report->mReportQueueName, " : Terminate");;
            break;
          };
          report->mpNewEvent->wait(report->mLock);
          CSSTrace(moduleName, "PerformMonitor", report->mReportQueueName, " : eventQ condition received");;
        };
        
        if (report->suspended > 0)
        {
          CSSTrace(moduleName, "PerformMonitor", report->mReportQueueName, " : report suspended ");
          report->mpReportSuspended->wait(report->mLock);
          CSSTrace(moduleName, "PerformMonitor",report->mReportQueueName, " : suspendCd received ");
          Sleep(100);
        };
        
        if (report->terminateThread == true)
        {
          CSSTrace(moduleName, "PerformMonitor", report->mReportQueueName, " : Terminate ");
          break;
        };
        currEvent = NULL;
        if (report->eventQueue.size() != 0)
        {
          currEvent = (dynamic_cast<cCSSEvent *>(report->eventQueue.front()));
          report->eventQueue.pop_front();
          if (report->nbevt > 0)
          {
            /*  Added by cc */
            report->nbevt--;
          };
        };
      };
      if (currEvent != NULL)
      {
        {
          cAutoLock _lock(currEvent->reporters);
          /* Invoke all the callback on the reporters list.  We keep the list locked so that if an object
             tries to unregister its reporter, it will be allowed to do so only after this event has been
             signaled. */
          report->signalEvent(currEvent->reporters, currEvent);
        }

        // The event will no longer be used
        delete currEvent;
      }
    }
    return NULL;
  }
  
  void cCSSReportQ::signalEvent(cCSSEvent::ReporterList *pReporters, cCSSEvent * event)
  {
    CSSTrace(moduleName, "SignalEvent",mReportQueueName, " : Starting");

    cCSSEvent::ReporterList::iterator it;
    if (event->mSignalLowPriorityOnly)
    {
      it = find_if(pReporters->begin(), pReporters->end(), 
                   bind2nd(mem_fun(&CSS::cCSSReporter::IsPriority), cCSSReporter::Low));
    }
    else
    {
      it = pReporters->begin();
    }

    // signal2() is simply a version of signal() that can be called through a STL algorithm.
    for_each(it, pReporters->end(), bind2nd(mem_fun(&cCSSReporter::signal2), event));

    CSSTrace(moduleName, "SignalEvent",mReportQueueName, " : Ending");
  }
  
  namespace CSS
  {
    class CSSReportQ_static
    {
    public:
      CSSReportQ_static()
      {
      };
    };
    static CSSReportQ_static CSSReportQ_static_instance;
  }
  
}
