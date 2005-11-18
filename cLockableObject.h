// cLockableObject.h: interface for the cLockableObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOCKABLEOBJECT_H__1C3F99B3_8B87_11D2_BFD8_00805F7DCE90__INCLUDED_)
#define AFX_CLOCKABLEOBJECT_H__1C3F99B3_8B87_11D2_BFD8_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CSSTools.h"
#include "cHandledObject.h"

class CSSTOOLS_API cLockableObject :public cHandledObject
{
public:
	cLockableObject();
	virtual bool Acquire(long timeout=INFINITE)=0;
	virtual void Release()=0;
	virtual ~cLockableObject()=0;
	
};

#endif // !defined(AFX_CLOCKABLEOBJECT_H__1C3F99B3_8B87_11D2_BFD8_00805F7DCE90__INCLUDED_)
