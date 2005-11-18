#include <string>
#include "CSSEvent.h"
using namespace std;
#include "CSSReporter.h"
#include "cAutoLock.h"
#include <algorithm>
#include <functional>
#include "CSS.h"

/* ************************************************************** */
/*  Module name: CSSEvent.m3 */
/*  Description: */
/*              The Module Implement the CSSEvent.i3 interface */
/*              It will be the base class for all Event generated */
/*              internally or to the Upper layer modules */
/* ************************************************************** */

namespace CSS
{
  const char *cCSSEvent::moduleName = "CSSEvent";

  cCSSReporter::eReporterPriority NextPriority(cCSSReporter::eReporterPriority p)
  {
    return (p == cCSSReporter::High ? cCSSReporter::Normal : cCSSReporter::Low);
  }


  void cCSSEvent::ReporterList::add(cCSSReporter *reporter)
  {
    cAutoLock _lock(this);

    // The list is always kept sorted by priority.  Elements must be unique.
    iterator it = find(begin(), end(), reporter);
    if (it == end())
    {
      // Insert the reporter in the list.
      it = find_if(begin(), end(), bind2nd(mem_fun(cCSSReporter::IsPriority), NextPriority(reporter->GetPriority())));
      insert(it, reporter);
    }
  }

  void cCSSEvent::ReporterList::remove(cCSSReporter *reporter)
  {
    cAutoLock _lock(this);
    if (Diag::TraceOn)
    {
      DiagTrace(moduleName, "", "Unregistering a reporter");
    }

    // The list is always kept sorted by priority.  Elements are unique.
    iterator it = find(begin(), end(), reporter);
    if (it != end())
    {
      erase(it);
    }
  }

  namespace CSS
  {
    class CSSEvent_static
    {
    public:
      CSSEvent_static () {}
    };
    static CSSEvent_static CSSEvent_static_instance;
  }
}
