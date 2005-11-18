#ifndef __CSSDualReportQ_INCLUDED__
#define __CSSDualReportQ_INCLUDED__
#pragma warning(disable:4786)
#pragma warning(disable:4250)
#include <string>
/*  Copyright (C) 1996, Positron Industries, Public Safety Division.
* All rights reserved.
* See the file COPYRIGHT for a full description.
*
* Created by Frederic Slight,     14 May 1998
*
****************************************************************************
* */
#include "CSSReportQ.h"

namespace CSS
{
  /* A DualQueue signals its reporters using two queues.  Reporters of priority Normal and higher are signaled
     by the primary queue, and other reporters are signaled by the secondary queue. */
  class CSSTOOLS_API cCSSDualReportQ:public virtual cCSSReportQ
  {
  public:
    
    static const char *moduleName;
    cCSSDualReportQ() {};
    virtual ~cCSSDualReportQ() {};
    virtual cCSSDualReportQ *initDual ();
    virtual bool terminate ();
    virtual void signalEvent (cCSSEvent::ReporterList *pReporters, cCSSEvent * event);

  private:
    cCSSReportQ mReportQ;
  }; 
};

#endif
