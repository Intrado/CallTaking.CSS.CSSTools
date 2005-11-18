// cHiddenNotifiableObject.h: interface for the cHiddenNotifiableObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHiddenNotifiableOBJECT_H__89975438_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CHiddenNotifiableOBJECT_H__89975438_797B_11D2_BFD5_00805F7DCE90__INCLUDED_

#include <list>

#include "cTEvent.h"
#include "cCritSec.h"

#include "CSSTools.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Refence des classes 
class  cNotifierObject;
class  cNotifiableObject;
class  cHiddenNotifiableObject;  
class  cLookupThreadTable;
class  cMappingTable;
class  cEvtThread;


typedef struct sNotifierObjWithTokenRec
{
	cNotifierObject *pNotifierObject;
	void* pToken;
}__sNotifierObjWithTokenRec;


typedef std::list<sNotifierObjWithTokenRec> LISTNOTIFIER;


class cHiddenNotifiableObject
{

public:
		
	cHiddenNotifiableObject(cNotifiableObject *pNotifiableObject, const char*  pGroupId);
	virtual void Notify( cTEvent* pEvt, void* pToken);
	virtual void OnEvent(cTEvent* pEvt, void* pToken);
	virtual ~cHiddenNotifiableObject();
  virtual void PurgeEvents();
  virtual void RepostEvent(cTEvent *pEvt, const void* pToken, bool inFront);
  virtual cNotifiableObject* GetNotifiableObject();
	int Id;

	void Disable(void);
	bool PopEvent(cTEvent **pEvt,void** pToken);
  int  GetNotificationQueueSize();

	static cHiddenNotifiableObject * Expand(cNotifiableObject *pNotifiableObject);
	
private:

	cCritSec mLockHiddenNotifiable;
	cCritSec mLockList;

	// Objet HiddenNotifiable en traitement actuel
	cHiddenNotifiableObject *pCurrHiddenNotifiableObject;
	
	cEvtThread* pmEvtThread;		// Reference du thread de gestion de la queue des evenements
	bool mbObsolete;

	cNotifiableObject* pmMapNotifiableObject;	// Objet de correspondance de cette instance
	
	// Liste des objets NotifierObject ou on est enregistre
	void* pmNotifierObjectList;  
		
	// Variable membre static 

	static int nNbInstance;
	static cLookupThreadTable LookupThreadTable;
	static cMappingTable MappingTable;

private:

	void UnregisterAll(void);
private:
	friend class cNotifierObject;
	void Register(cNotifierObject* pNotifierObject,void* pToken);
	void Unregister(cNotifierObject* pNotifierObject,void* pToken);

};



#endif // !defined(AFX_CHiddenNotifiableOBJECT_H__89975438_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
