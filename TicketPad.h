#ifndef __TicketPad_INCLUDED__
#define __TicketPad_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include "CSSObject.h"
#include "cCritSec.h"
#include "CSSTools.h"
/* ************************************************************** */
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description */
/* *************************************************************************** */
/*  The "Objet" interface */
/*  Author: Rejean Belanger,9 Septembre 1998 */
/* *************************************************************************** */
namespace CSS
{
  class CSSTOOLS_API cTicketPad:public cCSSObject
  {
    static const char *moduleName;
    static const char *COUNTER;
    static const char *KEY;
    static const char *SECTION_COUNTER;
    static const char *KEY_VALUE;
    
    public:
      cTicketPad ();
      virtual cTicketPad *init (std::string TicketPadName);
      virtual int getTicket ();                 

    protected:
      cCritSec lockTicketPad;
      std::string TicketPadName;
      std::string TicketFileName;
      std::string PathTicketFileName;
      int TicketNumber;
      std::string TicketPath;
      
    private:
      bool bFirstReading;
      int backupFileNumber;
      virtual bool getConfigKey (std::string section, std::string key, int &cValue);
      virtual bool setConfigKey (std::string section, std::string key, int Value);
      static bool VerifyAndCreate(std::string FileName);
      cTicketPad *Init (cTicketPad * self, std::string TicketPadName);
  };

  CSSTOOLS_API cTicketPad *GetTicketPad (std::string TicketPadName);
};
#endif
