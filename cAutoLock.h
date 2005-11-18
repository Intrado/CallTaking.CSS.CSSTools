// cLock.h: interface for the cLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOCK_H__4991D283_8AE0_11D2_BFD8_00805F7DCE90__INCLUDED_)
#define AFX_CLOCK_H__4991D283_8AE0_11D2_BFD8_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CSSTOOLS.h"
#include "cSafeLock.h"

class CSSTOOLS_API cAutoLock  :public cSafeLock
{
public:
	cAutoLock(cLockableObject& rLock,int mcount=0);
	cAutoLock(cLockableObject* pLock,int mcount=0);

	virtual ~cAutoLock();

};

#endif // !defined(AFX_CLOCK_H__4991D283_8AE0_11D2_BFD8_00805F7DCE90__INCLUDED_)
