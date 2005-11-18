// cKEvent.h: interface for the cKEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CKEVENT_H__1128EE03_7FDE_11D2_BFD6_00805F7DCE90__INCLUDED_)
#define AFX_CKEVENT_H__1128EE03_7FDE_11D2_BFD6_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cEvent.h"
#include "CSSTOOLS.h"

class CSSTOOLS_API cKEvent : public cEvent  
{
public:
	virtual ~cKEvent();

	cKEvent(char *pName=0, bool Manual=false, bool bInitialState=false, LPSECURITY_ATTRIBUTES lpEventAttributes=0);
	int GetKind();
    void SetKind(int Kind);
	void Signal(int Kind);
	int  WaitFor(int TimeOut);
	bool WaitnLock(int TimeOut,cMutex *pmx);

private:
	cMutex Lock;
	int Kind;// Must be negative value Positive vlue reserve by Win32

};

#endif // !defined(AFX_CKEVENT_H__1128EE03_7FDE_11D2_BFD6_00805F7DCE90__INCLUDED_)
