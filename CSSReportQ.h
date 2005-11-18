#ifndef __CSSReportQ_INCLUDED__
#define __CSSReportQ_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include <deque>
#include "CSSObject.h"
#include "CSSTools.h"
#include "cThread.h"
#include "cCondition.h"

/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description. */
/*  Created by Claude Chausse,      21 March 1996 */
/*  Updated by Jacques Dagenais,    22 March 1996 */
/* *************************************************************************** */
/*  The ReportQ Interface */
/*   This interface defines a Reporter.T Derived class. It Can be used */
/*   Almost as a CSSReporter on the receiver side. */
/*   Internally a Thread will manage a queue and dispatch event */
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description. */
/*  Created by Claude Chausse,      21 March 1996 */
/*  Updated by Jacques Dagenais,    22 March 1996 */

#include "CSSEvent.h"

namespace CSS
{
  class CSSTOOLS_API cCSSReportQ : public virtual cCSSObject
  {
  public:

    static const char *moduleName;
    
    class EventThread : public cThread
    {
    public:
      EventThread(std::string aThreadName) : cThread(aThreadName.c_str()) {};
    protected:
      virtual long ThreadProc();
    private:
      cCSSReportQ * report;
      virtual void init (cCSSReportQ * report);
      friend class cCSSReportQ;
    };
    
    public:
      
      cCSSReportQ ();
      virtual ~cCSSReportQ ();
      virtual cCSSReportQ *init ();
      virtual cCSSReportQ *init (std::string reportQueueName);
      virtual bool report (cCSSEvent * event);
      virtual bool terminate ();
      virtual void suspend ();
      virtual void resume ();
      virtual void signalEvent (cCSSEvent::ReporterList *pReporters, cCSSEvent * event);
      virtual void start();
      virtual void stop();
      
    private:
      virtual void DoInit();
      std::string mReportQueueName;
      cCritSec mLock;
      cCondition *mpNewEvent;
      cCondition *mpReportSuspended;
      EventThread * eventThread;
      bool isInFlushMode;
      bool terminateThread;
      std::deque<cCSSObject*> eventQueue;
      int nbevt;
      int id;
      int suspended;
      friend class EventThread;
  };
};

#endif
