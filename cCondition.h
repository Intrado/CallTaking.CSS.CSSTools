#ifndef __CSSCondition_INCLUDED__
#define __CSSCondition_INCLUDED__
#pragma warning(disable:4786)
#include <string>
#include "CSSObject.h"
using namespace std;

#include "CSSTools.h"
#include "cCritSec.h"
#include "cEvent.h"

class CSSTOOLS_API cCondition : public cCSSObject
{
public:
  static const char *kModuleName;
public:
  // Construction / destruction
  cCondition();
  virtual ~cCondition();    

  // Wait and signal functions
  virtual void signal (); 
  virtual void wait(cCritSec *apLock);    
  virtual void wait(cCritSec &apLock);    
protected:
  cEvent *mpWaitEvent;
};

#endif
