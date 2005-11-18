/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000.
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    olarouche     date created:    2000/05/02
 * %derived_by:   fslight %       %date_modified:  2000/11/02 16:32:28 %
 *
 * %name:         cTime.h %
 * %version:      13 %
 *
 * References:
 *
 *
 * Description:
 *
 *  The time is considered as a GMT time. (No time saving adjustement)
 *  Precision is in milliseconds.
 *  The time range is from January 1, 1970, to January 18, 19:14:07, 2038.
 *
 *  If the time is not within this range, it is represented as the minimum
 *  time (January 1, 1970 00:00:00) and the
 *  Method Seconds() will return the minimum value (i.e. LONG_MIN).
 *  Methods Milliseconds() will return the minimum value (i.e. DBL_MIN).
 *
 ***************************************************************************/

#ifndef CTIME_H
#define CTIME_H

#include "CSSTools.h"
#include "cTemplateString.h"
#include <time.h>
#include <sys/timeb.h>
#ifdef __BCPLUSPLUS__
#define _timeb timeb
#define _ftime ftime
#endif
class CSSTOOLS_API cTime
{
public:

  typedef enum {InvalidTime} exception;

  // Default Constructor: Initialize at the current time.
  cTime();                                              // May throw InvalidTime in debug
  cTime(const cTime& rhs);                              // May throw InvalidTime in debug
  cTime(const time_t NewTime, bool bLocalTime = false); // May throw InvalidTime in debug
  cTime(const struct _timeb& NewTime);                  // May throw InvalidTime in debug

  // YYYYMMDDHHMMSSsss
  cTime(const cTemplateString& sDateTime,
        bool bLocalTime);                               // May throw InvalidTime in debug

  // YYYY/MM/DD HH:MM:SS:sss
  cTime(const cTemplateString& sDateTime,
        char DateSeperator = '/', char DateTimeSeparator = ' ',
        char TimeSeparator = ':',
        bool bLocalTime = false);                      // May throw InvalidTime in debug

  virtual ~cTime();

  // Time in seconds.
  long Second() const;

  // Time in milliseconds.
  double Millisecond() const;
  virtual operator double() const;
  double operator-(const cTime& rhs);

  virtual cTime& operator=(const cTime& rhs);
  virtual cTime& operator=(const cTemplateString& sTimeStamp);
  virtual bool operator==(const cTime& rhs) const;
  virtual bool operator!=(const cTime& rhs) const;
  virtual bool operator>=(const cTime& rhs) const;
  virtual bool operator<=(const cTime& rhs) const;
  virtual bool operator>(const cTime& rhs) const;
  virtual bool operator<(const cTime& rhs) const;

  // %s for milliseconds
  cTemplateString FormatGmt(const char* pFormat = "%Y/%m/%d %H:%M:%S:%s") const;
  cTemplateString FormatLocal(const char* pFormat = "%Y/%m/%d %H:%M:%S:%s") const;

  // Call FormatGmt()
  virtual operator const char* (); 
  virtual const char* c_str();

protected:

  bool Init(const time_t NewTime, bool bLocalTime = false);
  bool Init(const cTemplateString& sDateTime, bool bLocalTime);
  bool Init(const cTemplateString& sDateTime, char DateSeperator,
            char DateTimeSeparator, char TimeSeparator, bool bLocalTime);

  struct _timeb StrToTimeb(const cTemplateString& sDateTime, bool bLocalTime) const;

  bool VerifyTimeValidity();

  cTemplateString FormatMs(const char* pFormat) const;

protected:

  struct _timeb mGMTTime;

private:

  // Use for temporary buffer in method operator const char*() and c_str().
  cTemplateString msLastTimeValue;

};

#endif
