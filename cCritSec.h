// cCritSec.h: interface for the cCritSec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCRITSEC_H__85841F21_8946_11D2_BFD8_00805F7DCE90__INCLUDED_)
#define AFX_CCRITSEC_H__85841F21_8946_11D2_BFD8_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include "CSSTools.h"

#include "cLockableObject.h"

class CSSTOOLS_API cCritSec : public cLockableObject    
{
public:

	virtual bool Acquire(long timeout=INFINITE);
	virtual void Release();

	cCritSec();
  cCritSec(const cCritSec &sec);
  cCritSec& cCritSec::operator=(const cCritSec &sec);
	virtual ~cCritSec();


private:
	
	CRITICAL_SECTION mTheCriticalSection;
};

#endif // !defined(AFX_CCRITSEC_H__85841F21_8946_11D2_BFD8_00805F7DCE90__INCLUDED_)
