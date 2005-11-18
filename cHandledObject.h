// cHandledObject.h: interface for the cHandledObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANDLEDOBJECT_H__FA55ADF3_8BA2_11D2_BFD8_00805F7DCE90__INCLUDED_)
#define AFX_CHANDLEDOBJECT_H__FA55ADF3_8BA2_11D2_BFD8_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CSSTools.h"

class CSSTOOLS_API cHandledObject  
{
public:
	cHandledObject();
  cHandledObject(const cHandledObject &src);
  cHandledObject& operator=(const cHandledObject &rhs);
	virtual ~cHandledObject();
protected:
	HANDLE handle;
};

#endif // !defined(AFX_CHANDLEDOBJECT_H__FA55ADF3_8BA2_11D2_BFD8_00805F7DCE90__INCLUDED_)
