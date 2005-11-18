#ifndef __CSSEvent_INCLUDED__
#define __CSSEvent_INCLUDED__
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description */
/* ************************************************************************** */
/*   The Event interface */
/*      This interface defines the root type for all events in the CSS */
/*      module. */
/*      All events have a timestamp */
#pragma warning(disable:4786)
#include <string>
#include "CSSObject.h"
#include "CSSEvent.h"
#include "CSSTools.h"
#include "cCritSec.h"
#include <list>
#include "cTime.h"

namespace CSS
{
  class cCSSReporter;
  class cCSSReportQ;
  class cCSSDualReportQ;


  class CSSTOOLS_API cCSSEvent:public virtual cCSSObject
  {
  public:
    static const char *moduleName;

#pragma warning(push)
#pragma warning(disable: 4275)
    class CSSTOOLS_API ReporterList: public std::list<cCSSReporter*>, public cCritSec
#pragma warning(pop)
    {
    public:
      ReporterList() {};
      virtual ~ReporterList() {};
      void add(cCSSReporter *reporter);
      void remove(cCSSReporter *reporter);
    };

    cCSSEvent() { mSignalLowPriorityOnly = false; }
    virtual ~cCSSEvent() {}
    ReporterList *reporters;
    std::string detail;
    cTime timeStamp;
	time_t _timeStamp;

  private:
    bool mSignalLowPriorityOnly;

    friend class cCSSReportQ;
    friend class cCSSDualReportQ;
  };
};

#endif
