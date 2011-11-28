/*  See the file CSSReportQ.i3 for instructions on how to use a CSSDualReportQ. */
#include <string>
#include "CSSDualReportQ.h"
#include "CSSEvent.h"
#include "CSSReportQ.h"
#include "CSSReporter.h"
#include "NewAutoInitialize.h"
#include <algorithm>
#include <functional>
#include <iterator>

using namespace std;
#include "CSS.h"

namespace CSS
{
  const char *cCSSDualReportQ::moduleName = "CSSDualReportQ";
  cCSSDualReportQ * cCSSDualReportQ::initDual ()
  {
    DiagTrace(moduleName, "Init", "Starting");
    mReportQ.init();
    {
      // Patch for Microsoft Visual C++ 5 Article ID: Q167749 
      cCSSReportQ * (cCSSReportQ::*fp) () = &CSS::cCSSReportQ::init;
      (((cCSSReportQ*)this)->*fp) ();
    }
    
    DiagTrace(moduleName, "Init", "Ending");
    return ((cCSSDualReportQ *)this);
  }
  
  
  bool cCSSDualReportQ::terminate ()
  {
    bool res;
    DiagTrace(moduleName, "Terminate", "Starting");
    res = mReportQ.terminate ();
    if (res == true)
    {
      {
        // Patch for Microsoft Visual C++ 5 Article ID: Q167749 
        bool (cCSSReportQ::*fp) () = &CSS::cCSSReportQ::terminate;
        (((cCSSReportQ* )this)->*fp) ();
      }
      
    };
    DiagTrace(moduleName, "Terminate", "Ending");
    return res;
  }

  void cCSSDualReportQ::signalEvent (cCSSEvent::ReporterList *pReporters, cCSSEvent * event)
  {
    bool deleteEvent = true;

    /* This function will first signal the event to the High and Normal priority reporters that are
       registered for it, and then report the event to the mReportQ member with a new reporters list
       containing only the Low priority reporters. */
    DiagTrace(moduleName, "SignalEvent", "Starting");

    int size = pReporters->size ();
    if (size > 0)
    {
      // Split the list in two parts: one contains the Low priority reporters, the other contains all others.
      cCSSEvent::ReporterList normalHighRep, lowRep;
      cCSSEvent::ReporterList::iterator it = find_if(pReporters->begin(), pReporters->end(),
                                                 bind2nd(mem_fun(&CSS::cCSSReporter::IsPriority), cCSSReporter::Low));

      copy(pReporters->begin(), it, inserter(normalHighRep, normalHighRep.end()));
      copy(it, pReporters->end(), inserter(lowRep, lowRep.end()));

      // First report the event to the High and Normal priority reporters
      // signal2() is simply a version of signal() that can be called through a STL algorithm.
      for_each(normalHighRep.begin(), normalHighRep.end(), bind2nd(mem_fun(&cCSSReporter::signal2), event));

      // Now, report to the Low priority reporters through the second queue.
      if (lowRep.size() > 0)
      {
        event->mSignalLowPriorityOnly = true;

        // mReportQ will delete the event when it is done.
        deleteEvent = false;
        mReportQ.report(event);
      }
    }

    if (deleteEvent)
    {
      delete event;
    }

    DiagTrace(moduleName, "SignalEvent", "Ending");
  }
  
  namespace CSS
  {
    class CSSDualReportQ_static
    {
    public:
      CSSDualReportQ_static ()
      {
      };
    };
    static CSSDualReportQ_static CSSDualReportQ_static_instance;
  }
  
}
