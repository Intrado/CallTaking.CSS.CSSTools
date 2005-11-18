#ifndef __CtxId_INCLUDED__
#define __CtxId_INCLUDED__
#pragma warning(disable:4786)
#pragma warning(disable:4275)

/* ************************************************************** */
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description */
/* *************************************************************************** */

#include <string>
#include "CSSObject.h"
#include "CSSTools.h"
#include "SmartPtr.h"

namespace CSS
{
  class CSSTOOLS_API cCtxId:public cCSSObject, public RefCount
  {
  public:
    cCtxId()
    {
      bNewCtxID = false;
    };
    
    virtual cCtxId *init (int ContextID);
    virtual int readCtxId ();
    virtual bool getCtxId (int &ContextID);
    
    
  protected:
    bool bNewCtxID;
    int nCtxId;
    
  private:
  };
  cCtxId *Init (cCtxId * self, int ContextID);
};

typedef SmartPtr<CSS::cCtxId> PCtxId;

#pragma warning(default:4275)

#endif
