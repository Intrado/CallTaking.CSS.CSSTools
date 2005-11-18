/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2001.
 * ALL RIGHTS RESERVED.
 *
 *  created by:    fslight       date created:    2001/02/23
 * %derived_by:   sroy %       %date_modified:  2001/05/16 15:13:10 %
 *
 * %name:         cLockableInfo.h %
 * %version:      3 %
 *
 * Description:
 *
 * Template used to have a status or flag of a built-in type (int, bool, etc) or of a user-defined type,
 * which can be locked and can still be used as the original type can.
 * 'I' must have an operator==, operator=, operator!=, operator< (if those operators are to be used)
 * and a copy constructor.
 *
 ***************************************************************************/

#ifndef __cLockableInfo_h_INCLUDED__
#define __cLockableInfo_h_INCLUDED__


template <class I> class cLockableInfo: public cCritSec
{
  I mInfo;
public:
  cLockableInfo(I val) { operator=(val); }
  operator I() { return mInfo; }
  bool operator==(const I &rhs) { return mInfo == rhs; }
  bool operator==(const cLockableInfo &rhs) { return operator==(rhs.mInfo); }
  bool operator!=(const I &rhs) { return mInfo != rhs; }
  bool operator!=(const cLockableInfo &rhs) { return operator!=(rhs.mInfo); }
  bool operator<(const I &rhs) { return mInfo < rhs; }
  bool operator<(const cLockableInfo &rhs) { return operator<(rhs.mInfo); }
  cLockableInfo& operator=(const I &src) { mInfo = src; return *this; }
  cLockableInfo& operator=(const cLockableInfo &src) { mInfo = src.mInfo; return *this; }
};


#endif
