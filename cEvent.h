// cEvent.h: interface for the cEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEVENT_H__847AFCD1_7990_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CEVENT_H__847AFCD1_7990_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include "CSSTools.h"
#include "cMutex.h"
#include "cHandledObject.h"


class CSSTOOLS_API cEvent : public cHandledObject 
{
public:
	cEvent(char *pName=0, bool Manual=false,bool bInitialState=false, LPSECURITY_ATTRIBUTES lpEventAttributes=0);
	virtual ~cEvent();

	virtual void Signal();
	virtual void Reset();

	virtual bool Wait(int TimeOut);
	virtual bool WaitnLock(int TimeOut,cMutex *pmx);

	virtual operator HANDLE(){return handle;};

private:

  	const char *HandleInvalid() const { return "Handle hEvent invalid"; }

};

#endif // !defined(AFX_CEVENT_H__847AFCD1_7990_11D2_BFD5_00805F7DCE90__INCLUDED_)
