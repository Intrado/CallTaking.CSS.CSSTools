/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2001.
 * ALL RIGHTS RESERVED.
 *
 *  created by:    fslight       date created:    2001/02/23
 * %derived_by:   fslight %       %date_modified:  2001/02/23 14:13:42 %
 *
 * %name:         cBooleanToggler.h %
 * %version:      3 %
 *
 * Description:
 *
 *    Object that toggles a boolean value when it is destroyed.
 *
 ***************************************************************************/

#ifndef __cBooleanToggler_h_INCLUDED__
#define __cBooleanToggler_h_INCLUDED__


class cBooleanToggler
{
  bool *mpVar;
  bool toggle;
public:
  cBooleanToggler(): mpVar(0) { toggle = true; }

  cBooleanToggler(bool *pVar): mpVar(pVar) { toggle = true; }

  cBooleanToggler(bool *pVar, bool initialValue): mpVar(pVar)
  {
    if (mpVar)
      *mpVar = initialValue;
    toggle = true;
  }

  virtual void SetVar(bool *pVar)
  {
    mpVar = pVar;
  }

  virtual ~cBooleanToggler() { ToggleNow(); }

  virtual void DontToggle() { toggle = false; }

  virtual void ToggleNow()
  {
    if (toggle)
    {
      if (mpVar)
        *mpVar = !(*mpVar);
      DontToggle();
    }
  }
};


#endif
