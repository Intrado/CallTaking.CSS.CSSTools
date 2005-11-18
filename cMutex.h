// cMutex.h: interface for the cMutex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMUTEX_H__8997543D_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CMUTEX_H__8997543D_797B_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>
#include "CSSTools.h"

#include "cLockableObject.h"

class  CSSTOOLS_API cMutex : public cLockableObject  
{

public:
	cMutex(char *pName=0, bool bInitialState=false, LPSECURITY_ATTRIBUTES lpMutexAttributes=0);
	virtual ~cMutex();

	virtual bool Acquire(long timeout=INFINITE);
	virtual void Release();

	virtual operator HANDLE(){return handle;};

private:
	const char *HandleInvalid() const { return "Handle hMutex invalid"; }
  cMutex::cMutex(const cMutex&);
  cMutex& cMutex::operator=(const cMutex&);

};



#endif // !defined(AFX_CMUTEX_H__8997543D_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
