// cNotifierObject.h: interface for the cNotifierObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CNOTIFIEROBJECT_H__89975437_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CNOTIFIEROBJECT_H__89975437_797B_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "cHiddenNotifiableObject.h"
#include "cNotifiableObject.h"
#include "cTEvent.h"
#include "cCritSec.h"

#include "CSSTools.h"


typedef struct sHiddenNotifiableObjWithTokenRec
{
	cHiddenNotifiableObject *pHiddenNotifiableObject;
	// On evitera une operation d'expand pour la destruction
	cNotifiableObject *pNotifiableObject;
	void* pToken;
}__sHiddenNotifiableObjWithTokenRec;


typedef std::list<sHiddenNotifiableObjWithTokenRec> LISTOBJ;

class CSSTOOLS_API cNotifierObject  
{

public:
	virtual void Register(cNotifiableObject* pNotifiableObject,void* pToken);
	virtual void Unregister(cNotifiableObject *pNotifiableObject,void *pToken);
	virtual void Notify(cTEvent *pEvt);
  virtual void DuplicateRegistration(const cNotifierObject* pSource);
	
	cNotifierObject();
	virtual ~cNotifierObject();
	int Id;
private:
	    
	void UnregisterAll(void);
	void * pHiddenNotifiableObjectList;
	cCritSec mLockNotifier;
	static int nNbInstance;
private:

	friend class cHiddenNotifiableObject;
	virtual void UnregisterMe(cNotifiableObject *pNotifiableObject,void *pToken);

};

#endif // !defined(AFX_CNOTIFIEROBJECT_H__89975437_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
