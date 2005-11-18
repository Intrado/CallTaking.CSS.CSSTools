#ifndef __CSSMulticondition_INCLUDED__
#define __CSSMulticondition_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include "CSSObject.h"
using namespace std;

/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description */
/* *********************************************************************** */
/*  The Multicondition Interface */
/*  A CSSMultiondition object is used to alert a thread that is waiting */
/*  for many possible events. The mutex m is used to make sure that only */
/*  one condition type can be set at the same time. */
/*  For example we can use a Multicondition after a message transmission */
/*  to wait for a reply. In this case, three condition can be generated. */
/*  One after an ACK timeout, one after reception of an ACK  and another */
/*  one after reception of a NAK.  The transmission thread has to test the */
/*  condition type to know what is the current event. All the events */
/*  objects types are  new types derived from Ttype object (see below). */
/*  Use positive non-zero signal types for contracts between signalers and */
/*  waiters. kTimeout is signaled by CSSTimeout. */
#include "CSSTools.h"
#include "cCritSec.h"
#include "cEvent.h"

namespace CSS
{
  class CSSTOOLS_API cCSSMulticondition : public cCSSObject, public cCritSec
  {
  public:
    static const int kNotSetMct;
    static const char *kModuleName;

    typedef int tSignalType;
    
  public:
    // Construction / destruction
    cCSSMulticondition();
    virtual ~cCSSMulticondition();    
    virtual cCSSMulticondition *init (std::string name);

    // Wait and signal functions
    virtual void signal (tSignalType type); 
    virtual tSignalType wait();    

    // Signal type handling
    virtual void setType (tSignalType type);
    virtual tSignalType getType ();
    virtual void resetType ();

    std::string mName;
  private:
    tSignalType mSignalType;
    cEvent *mpWaitEvent;
  };
};

#endif
