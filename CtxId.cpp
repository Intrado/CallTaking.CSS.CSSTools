#include <windows.h>
#include "CtxId.h"
using namespace std;

/* ************************************************************** */
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description */
/* *************************************************************************** */
/*  The Objet CtxId Implementation */
/* *************************************************************************** */
/*  Methodes Surcharge */
/*  Constructeur */
namespace CSS
{
  cCtxId *cCtxId::init(int ContextID)
  {
    nCtxId = ContextID;
    bNewCtxID = true;
    return this;
  }
  
  int cCtxId::readCtxId()
  {
    return (nCtxId);
  }
  
  bool cCtxId::getCtxId(int &ContextID)
  {
    /*  Utiliser seulement par CSSRecorder */
    ContextID = nCtxId;
    if (bNewCtxID == true)
    {
      bNewCtxID = false;
      return true;
    }
    else
    {
      return false;
    };
  }
  
}
