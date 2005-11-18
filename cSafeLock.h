// cSafeLock.h: interface for the cSafeLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSAFELOCK_H__1C3F99B4_8B87_11D2_BFD8_00805F7DCE90__INCLUDED_)
#define AFX_CSAFELOCK_H__1C3F99B4_8B87_11D2_BFD8_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cLockableObject.h"
#include "CSSTOOLS.h"

class CSSTOOLS_API cSafeLock : public cLockableObject 
{
public:
	cSafeLock(cLockableObject& rlock, int mcount=0);
	cSafeLock(cLockableObject* plock, int mcount=0);
	virtual ~cSafeLock();
	virtual bool Acquire(long timeout=INFINITE);
	virtual void Release();

protected:
	cLockableObject* mpLock;
	int mcount;
};

#endif // !defined(AFX_CSAFELOCK_H__1C3F99B4_8B87_11D2_BFD8_00805F7DCE90__INCLUDED_)
