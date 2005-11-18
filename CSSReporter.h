#ifndef __CSSReporter_INCLUDED__
#define __CSSReporter_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include "CSSObject.h"
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description. */
/*  Created Claude Chausse,          21 March 1996 */
/*  Updated by Jacques Dagenais,     22 March 1996 */
/* *************************************************************************** */
/*   The Reporter Interface */
/*   This interface defines the base class for every modules that wants */
/*    CSSEvents to be reported to him from a lower layer. */
/*   The Event Receiver just derived directly from CSSReporter.T or from */
/*   a Derived class then Override the Apply method that is going to be */
/*   called by the Event transmitter with the Appropriate Derived */
/*   CSSEvent Object.  The Receiver must be aware of the CSSEvent.T */
/*   derived class in order to understand fully the meaning of the Event */
/*       This class is used either internally and by the Application layer */
/* *************************************************************************** */
/*  This interface  is not really used, it will be derived from by the users */
/*  to allow them to register the desired Apply method to be invoked when */
/*  when processing the events found in the Event Queue */
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description. */
/*  Created Claude Chausse,          21 March 1996 */
/*  Updated by Jacques Dagenais,     22 March 1996 */
#include "CSSEvent.h"
#include "CSSTools.h"

namespace CSS
{
  class CSSTOOLS_API cCSSReporter:public virtual cCSSObject
  {
  public:
    // The order of elements in this enum is important.
    enum eReporterPriority
    {
      Low = 0, Normal = 1, High = 2
    };

    cCSSReporter(eReporterPriority pr = Normal): mPriority(pr) {}
    virtual eReporterPriority GetPriority() const { return mPriority; }
    virtual bool IsPriority(eReporterPriority p) { return mPriority == p; }
    // Use signal2() only to call signal() through a STL algorithm.
    virtual bool signal2(cCSSEvent *pEvent) { signal(pEvent); return true; }

    virtual void signal (cCSSEvent * event)=0;

  private:
    eReporterPriority mPriority;
  };
}
#endif
