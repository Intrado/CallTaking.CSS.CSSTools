// cTEvent.h: interface for the cTEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTEVENT_H__8997543C_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CTEVENT_H__8997543C_797B_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CSSTools.h"
#include "cMutex.h"

class CSSTOOLS_API  cTEvent  
{
public:
	cTEvent();
	virtual int AddRef(void);
	virtual int ReleaseRef(void);

	int Id;
	static int instDestroy;

protected:
	virtual ~cTEvent();
	cTEvent(class cTEvent const &){};
	void operator=(class cTEvent const &){};

private:
	static int inst;
	int nbUses;	// Nombre de reference sur cet objet
	cMutex LockTEvent;
};

#endif // !defined(AFX_CTEVENT_H__8997543C_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
