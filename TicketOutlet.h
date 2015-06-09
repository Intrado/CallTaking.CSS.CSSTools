#ifndef __TicketOutlet_INCLUDED__
#define __TicketOutlet_INCLUDED__
#pragma warning(disable:4786)
/* ************************************************************** */
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description */
/* *************************************************************************** */
/*  The "Objet" interface */
/*  Author: Rejean Belanger,9 Septembre 1998 */
/* *************************************************************************** */
#include <string>
#include <deque>
#include "CSSObject.h"
#include "TicketPad.h"
#include "CSSTools.h"
#include "cCritSec.h"

namespace CSS
{
  class CSSTOOLS_API cTicketOutlet : public cCSSObject
  {
    static const char *moduleName;
  public:
    virtual cTicketOutlet *init ();
    virtual ~cTicketOutlet();
    virtual CSS::cTicketPad * getTicketPad (std::string TicketPadName);
    
  protected:
    cCritSec lockTicketOutlet;
    std::deque<cCSSObject*> instanceList;
    
  private:
    virtual bool registerInstance (std::string NameInstance, cCSSObject * RefInstance);
    virtual bool unregisterInstance (cCSSObject * RefInstance);
    virtual cCSSObject *getInstance (std::string Name);

    class CSSTOOLS_API InstanceRec:public cCSSObject
    {
    public:
      InstanceRec(){ RefInstance = 0; };  // Coverity 14980: initialize pointer field
      
    private:
      std::string NameInstance;
      cCSSObject *RefInstance;
      friend cTicketOutlet;
    };    
  };
  CSSTOOLS_API CSS::cTicketPad* GetTicketPad (cTicketOutlet * self, std::string TicketPadName);
};
#endif
