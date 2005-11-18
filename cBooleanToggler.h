/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2001.
 * ALL RIGHTS RESERVED.
 *
 *  created by:    fslight       date created:    2001/02/23
 * %derived_by:   sroy %       %date_modified:  2001/02/23 14:13:42 %
 *
 * %name:         cBooleanToggler.h %
 * %version:      2 %
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
  cBooleanToggler(bool *pVar): mpVar(pVar) { toggle = true; }
  cBooleanToggler(bool *pVar, bool initialValue): mpVar(pVar) { *mpVar = initialValue; toggle = true; }
  virtual ~cBooleanToggler() { if (toggle) { *mpVar = !(*mpVar); } }
  virtual void DontToggle() { toggle = false; }
};


#endif
