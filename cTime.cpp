/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000.
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    olarouche     date created:    2000/05/23
 * %derived_by:   sroy %       %date_modified:  2000/11/02 16:32:38 %
 *
 * %name:         cTime.cpp %
 * %version:      11 %
 *
 * References:
 *
 *     list of relevant design documents
 *
 * Description:
 *
 *     description
 *
 ***************************************************************************/

#include "cTime.h"
#include "float.h"
#include "CSS.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTime
 *
 * Arguments:
 *
 * Returns: void
 *
 * Exceptions: May throw InvalidTime
 *
 * Description: Constructor
 *    Time stamp to current time.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::cTime()
{
  _ftime(&mGMTTime);
  VerifyTimeValidity();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTime
 *
 * Arguments:
 *
 * Returns: void
 *
 * Exceptions: May throw InvalidTime
 *
 * Description: Copy Constructor
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::cTime(const cTime& rhs)
{
  mGMTTime = rhs.mGMTTime;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTime
 *
 * Arguments:
 *    const time_t NewTime [in],
 *    bool bLocalTime [in], if false, NewTime is assumed to be GMT.
 *                          if true, NewTime is assumed to be local time.
 *
 * Returns: void
 *
 * Exceptions: May throw InvalidTime
 *
 * Description: 
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::cTime(const time_t NewTime, bool bLocalTime)
{
  Init(NewTime, bLocalTime);
  VerifyTimeValidity();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTime
 *
 * Arguments:
 *
 * Returns: void
 *
 * Exceptions: May throw InvalidTime
 *
 * Description: Copy Constructor
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::cTime(const struct _timeb& NewTime)
{
  mGMTTime = NewTime;
  VerifyTimeValidity();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTime
 *
 * Arguments:
 *    const cTemplateString& sDateTime [in], YYYYMMDDHHMMSSmmm
 *                                           01234567890123456
 *      YYYY  1970 to 2038
 *      MM    1 to 12
 *      DD    1 to 31
 *      HH    0 to 23
 *      MM    0 to 59
 *      SS    0 to 59
 *      MM    0 to 999
 *
 *    bool bLocalTime [in], if false, NewTime is assumed to be GMT.
 *                          if true, NewTime is assumed to be local time.
 *
 * Returns: void
 *
 * Exceptions: May throw InvalidTime
 *
 * Description: Constructor
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::cTime(const cTemplateString& sDateTime, bool bLocalTime)
{
  Init(sDateTime, bLocalTime);
  VerifyTimeValidity();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  cTime
 *
 * Arguments:
 *    const cTemplateString& sDateTime [in],  YYYY/MM/DD HH:MM:SS:mmm
 *                                            01234567890123456789012
 *    char DateSeperator [in],
 *    char DateTimeSeparator [in],
 *    char TimeSeparator [in],
 *    bool bLocalTime [in], if false, NewTime is assumed to be GMT.
 *                          if true, NewTime is assumed to be local time.
 *
 * Returns: void
 *
 * Exceptions: May throw InvalidTime
 *
 * Description: Constructor
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::cTime(const cTemplateString& sDateTime, char DateSeperator,
             char DateTimeSeparator, char TimeSeparator,
             bool bLocalTime)
{
  Init(sDateTime, DateSeperator, DateTimeSeparator, TimeSeparator, bLocalTime);
  VerifyTimeValidity();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  ~cTime
 *
 * Arguments: void
 *
 * Returns: void
 *
 * Exceptions:
 *
 * Description: Destructor
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::~cTime()
{
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Second
 *
 * Arguments: void
 *
 * Returns: Number of seconds since January 1, 1970, up to January 18, 19:14:07, 2038.
 *
 * Exceptions:
 *
 * Description: 
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

long cTime::Second() const
{
  if(mGMTTime.time == -1)
  {
    DiagError("cTime", "Second", "Time is invalid. Limit is exceeded.");
    return LONG_MIN;
  }

  return mGMTTime.time;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  Millisecond
 *
 * Arguments: void
 *
 * Returns: Number of milliseconds. If the time is too big to expressed in
 *    millseconds, returns DBL_MAX.
 *
 * Exceptions:
 *
 * Description: 
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

double cTime::Millisecond() const
{
  double dMs;
 
  if((mGMTTime.time == -1) || (mGMTTime.time >= DBL_MAX / 1000))
  {
    // Difference too big.
    // Set it to the max;
    dMs = DBL_MIN;
    DiagError("cTime", " Millisecond", "Time is invalid. Limit is exceeded.");
  }
  else
  {
    dMs = mGMTTime.time;
    dMs *= 1000;
    dMs += mGMTTime.millitm;
  }

  return dMs;
}

bool cTime::Init(const time_t NewTime, bool bLocalTime)
{
  _ftime(&mGMTTime);

  if(bLocalTime)
  {
    // Adjust local time to GMT time.
    mGMTTime.time = NewTime - ((mGMTTime.timezone - (mGMTTime.dstflag * 60)) * 60);
  }
  else
  {
    // Input time is GMT.
    mGMTTime.time = NewTime;
  }

  mGMTTime.millitm = 0;

  return true;
}

bool cTime::Init(const cTemplateString& sDateTime, bool bLocalTime)
{
  mGMTTime = StrToTimeb(sDateTime, bLocalTime);

  return (mGMTTime.time != -1);
}

bool cTime::Init(const cTemplateString& sDateTime, char DateSeperator,
                 char DateTimeSeparator, char TimeSeparator, bool bLocalTime)
{
  cTemplateString sDateTimeRaw = "";

  if(sDateTime.size() >= 23)
  {
    sDateTimeRaw = sDateTime.substr(0, 4);
    sDateTimeRaw += sDateTime.substr(5, 2);
    sDateTimeRaw += sDateTime.substr(8, 2);

    sDateTimeRaw += sDateTime.substr(11, 2);
    sDateTimeRaw += sDateTime.substr(14, 2);
    sDateTimeRaw += sDateTime.substr(17, 2);
    sDateTimeRaw += sDateTime.substr(20, 3);
  }

  return Init(sDateTimeRaw, bLocalTime);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  StrToTimeb
 *
 * Arguments:
 *    const cTemplateString& sDateTime [in],  YYYYMMDDHHMMSSmmm
 *                                            01234567890123456
 *    bool bLocalTime [in], if false, NewTime is assumed to be GMT.
 *                          if true, NewTime is assumed to be local time.
 *
 * Returns: struct _timeb
 *
 * Exceptions:
 *
 * Description: Convert sDateTime to _timeb.
 *              Milliseconds are optionnal.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

struct _timeb cTime::StrToTimeb(const cTemplateString& sDateTime,
                                bool bLocalTime) const
{
  struct tm TempTime;
  struct _timeb TimeB;
  struct _timeb LocalInfo;

  TimeB.time = -1;
  TimeB.millitm = 0;
  TimeB.timezone = 0;
  TimeB.dstflag = 0;

  if(sDateTime.size() >= 14)
  {
    TempTime.tm_year = atoi(sDateTime.substr(0, 4)) - 1900; // years since 1900
    TempTime.tm_mon = atoi(sDateTime.substr(4, 2)) - 1;     // Convert from 1-12 to 0-11
    TempTime.tm_mday = atoi(sDateTime.substr(6, 2));
    TempTime.tm_hour = atoi(sDateTime.substr(8, 2));
    TempTime.tm_min = atoi(sDateTime.substr(10, 2));
    TempTime.tm_sec = atoi(sDateTime.substr(12, 2));
    TempTime.tm_wday = -1;  // Not used // days since Sunday - [0,6] 
    TempTime.tm_yday = -1;  // Not used // days since January 1 - [0,365] 

    if(bLocalTime)
    {
      TempTime.tm_isdst = -1; // Automatically compute daylight savings.
    }
    else
    {
      TempTime.tm_isdst = 0; // No daylight savings.
    }

    TimeB.time = mktime(&TempTime);
    if(TimeB.time == -1)
    {
      DiagError("cTime", "StrToTimeb()", "Time is invalid. Limit is exceeded.");
    }
    else if(!bLocalTime)
    {
      // Adjust local time returned by mktime() to GMT.
      _ftime(&LocalInfo);
      TimeB.time -= (LocalInfo.timezone * 60);
    }

    if(sDateTime.size() >= 17)
    {
      TimeB.millitm = static_cast<unsigned short>(atoi(sDateTime.substr(14, 3)));
    }
  }

  return TimeB;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  operator=
 *
 * Arguments: const cTime& rhs [in],
 *
 * Returns: 
 *
 * Exceptions:
 *
 * Description: 
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime& cTime::operator=(const cTime& rhs)
{
  mGMTTime = rhs.mGMTTime;

  return *this;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  FormatGmt
 *
 * Arguments: format string see in MSDN strftime().
 *            The only addition is for milliseconds (000 to 999): %s.
 *
 * Returns: 
 *
 * Exceptions:
 *
 * Description: Use GMT time
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTemplateString cTime::FormatGmt(const char* pFormat) const
{
  cTemplateString sTime = FormatMs(pFormat);

	char szBuffer[1024];
	struct tm* ptmTemp = gmtime(&(mGMTTime.time));
	if (ptmTemp == NULL ||
		! strftime(szBuffer, sizeof(szBuffer), sTime.c_str(), ptmTemp))
  {
    strcpy(szBuffer, "Error: Invalid Time!");
  }
	return (cTemplateString)szBuffer;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  FormatLocal
 *
 * Arguments: format string see in MSDN strftime().
 *            The only addition is for milliseconds (000 to 999): %s.
 *
 * Returns: 
 *
 * Exceptions:
 *
 * Description: Use local time
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTemplateString cTime::FormatLocal(const char* pFormat) const
{
  cTemplateString sTime = FormatMs(pFormat);

	char szBuffer[1024];
	struct tm* ptmTemp = localtime(&(mGMTTime.time));
	if (ptmTemp == NULL ||
		! strftime(szBuffer, sizeof(szBuffer), sTime.c_str(), ptmTemp))
  {
    strcpy(szBuffer, "Error: Invalid Time!");
  }
	return (cTemplateString)szBuffer;
}

cTemplateString cTime::FormatMs(const char* pFormat) const
{
  const char* const kpMilliSecondFormat = "%s";
  cTemplateString sTime = pFormat;

  // Replace %s by the millisecond value.
  int nPosition = sTime.find(kpMilliSecondFormat);
  if(nPosition != -1)
  {
    cTemplateString sBuffer;
    sTime = sTime.erase(nPosition, strlen(kpMilliSecondFormat));
    sTime.insert(nPosition, sBuffer.format("%03d", mGMTTime.millitm));
  }

  return sTime;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  operator double
 *
 * Arguments: 
 *
 * Returns: The time in milliseconds.
 *
 * Exceptions:
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime::operator double() const 
{
  return Millisecond();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  operator-
 *
 * Arguments: 
 *
 * Returns: The time difference in milliseconds.
 *
 * Exceptions:
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

double cTime::operator-(const cTime& rhs)
{
  return Millisecond() - rhs.Millisecond();
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  VerifyTimeValidity
 *
 * Arguments: void
 *
 * Returns: 
 *
 * Exceptions: May throw InvalidTime
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

bool cTime::VerifyTimeValidity()
{
  if(mGMTTime.time == -1)
  {
    #ifdef _DEBUG
      throw(InvalidTime);
    #else
      DiagError("cTime", "VerifyTimeValidity", "Time is invalid.");
    #endif

  }
  return true;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  operator=
 *
 * Arguments: const cTemplateString& sTimeStamp [in], GMT time
 *
 * Returns: 
 *
 * Exceptions: May throw InvalidTime
 *
 * Description:
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

cTime& cTime::operator=(const cTemplateString& sTimeStamp)
{
  Init(sTimeStamp, '/', ' ', ':', false);
  VerifyTimeValidity();

  return *this;
}

bool cTime::operator==(const cTime& rhs) const
{
  return ((mGMTTime.time == rhs.mGMTTime.time) &&
    (mGMTTime.millitm == rhs.mGMTTime.millitm));
}

bool cTime::operator!=(const cTime& rhs) const
{
  return ! (*this == rhs);
}

bool cTime::operator>=(const cTime& rhs) const
{
  return ((*this == rhs) || (*this > rhs));
}

bool cTime::operator<=(const cTime& rhs) const
{
  return ((*this == rhs) || (*this < rhs));
}

bool cTime::operator>(const cTime& rhs) const
{
  bool bGreaterThan = false;

  if(mGMTTime.time > rhs.mGMTTime.time)
  {
    bGreaterThan = true;
  }
  else if(mGMTTime.time == rhs.mGMTTime.time)
  {
    if(mGMTTime.millitm > rhs.mGMTTime.millitm)
    {
      bGreaterThan = true;
    }
  }

  return bGreaterThan;
}

bool cTime::operator<(const cTime& rhs) const
{
  bool bLessThan = false;

  if(mGMTTime.time < rhs.mGMTTime.time)
  {
    bLessThan = true;
  }
  else if(mGMTTime.time == rhs.mGMTTime.time)
  {
    if(mGMTTime.millitm < rhs.mGMTTime.millitm)
    {
      bLessThan = true;
    }
  }

  return bLessThan;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:  c_str()
 *
 * Arguments: 
 *
 * Returns: Same as calling FormatGmt().
 *
 * Exceptions:
 *
 * Description: To be compatible with cTemplateString.
 *
 * Notes: 
 *
 *-------------------------------------------------------------------------*/

const char* cTime::c_str()
{
  msLastTimeValue = FormatGmt();
  return msLastTimeValue.c_str();
}

cTime::operator const char* ()
{
  return c_str();
}
