/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2001.
 * ALL RIGHTS RESERVED.
 *
 *  created by:    fslight       date created:    2001/02/23
 * %derived_by:   sroy %       %date_modified:  2001/02/23 14:12:22 %
 *
 * %name:         cSuspendReporting.h %
 * %version:      2 %
 *
 * Description:
 *
 *    Object that suspends reporting when it is constructed and automatically
 *    resumes reporting when it is destroyed.
 *
 ***************************************************************************/

#ifndef __cSuspendReporting_h_INCLUDED__
#define __cSuspendReporting_h_INCLUDED__


template <class T> class cSuspendReporting
{
  T *mpObj;
public:
  cSuspendReporting(T *pObj): mpObj(pObj) { mpObj->suspendReport(); }
  virtual ~cSuspendReporting() { mpObj->resumeReport(); }
};


#endif
