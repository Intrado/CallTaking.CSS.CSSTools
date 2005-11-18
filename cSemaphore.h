// cSemaphore.h: interface for the cSemaphore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSEMAPHORE_H__23729FD3_7B06_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CSEMAPHORE_H__23729FD3_7B06_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "CSSTools.h"
#include "cLockableObject.h"

class CSSTOOLS_API cSemaphore : public cLockableObject 
{
public:
	cSemaphore(long  InitialCount,long MaximumCount,char* pName=0,LPSECURITY_ATTRIBUTES lpAttributes=0);
	virtual ~cSemaphore();

	virtual bool Acquire(long timeout=INFINITE);
	virtual void Release();
  virtual void Release(BOOL & result);

	virtual operator HANDLE(){return handle;};

private:
  const char *HandleInvalid() const { return "Handle hSemaphore invalid"; }
  cSemaphore::cSemaphore(const cSemaphore& s);
  cSemaphore& cSemaphore::operator=(const cSemaphore& s);
};



#endif // !defined(AFX_CSEMAPHORE_H__23729FD3_7B06_11D2_BFD5_00805F7DCE90__INCLUDED_)
