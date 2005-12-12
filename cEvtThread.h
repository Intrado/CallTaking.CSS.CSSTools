// cThread.h: interface for the cThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTHREAD_H__847AFCD7_7990_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CTHREAD_H__847AFCD7_7990_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <queue>
#include <list>

#include "sTEventRec.h"
#include "cSemaphore.h"
#include "cCritSec.h"
#include "cMutex.h"
#include "cEvent.h"

typedef std::list<cHiddenNotifiableObject*> LIST_TO_PURGE;

class cEvtThread  
{

public:
	
	// Purge touts les evenements associes a l'objet cHiddenNotifiableObject
	virtual void Purge(cHiddenNotifiableObject *pHiddenNotifiableObject);
	virtual void RemoveIt(cHiddenNotifiableObject *pHiddenNotifiableObject);
	
	// Enfile l'evenement dans la queue de notification
	virtual void Notify(sTEventRec Evt, bool inFront = false);
	cEvtThread();
	virtual ~cEvtThread();
	
	virtual int AddRef(void);
	virtual int Release(void);
	cHiddenNotifiableObject* GetCurrHiddenNotifiableObject(void);
	void SetCurrHiddenNotifiableObject(cHiddenNotifiableObject* pCurrHiddenNotifiableObject);

  DWORD             GetmidThread() { return midThread;} 
  std::list<sTEventRec>& GetmEvtQueue() { return mEvtQueue; }
  cCritSec&         GetmLockQueue() { return mLockQueue; }

	int Id;

private:
	static void Func(cEvtThread* pObj);
	// Dispatch l'evenement 
	virtual void Dispatch();	
	// Vide la queue pour nettoyage de ressource
	const char *HandleInvalid() const { return "Handle hThread invalid"; }
	int  PutInListToPurge(cHiddenNotifiableObject *pHiddenNotifiableObject);
	bool bIsPresent(cHiddenNotifiableObject *pHiddenNotifiableObject);



private:

	cCritSec mLockQueue;
	cCritSec mLockEvtThread;
	cCritSec mLockListToPurge;

	//Handle du Thread
	HANDLE  mhThread;
	// Identificateur du thread
	DWORD midThread;
	
	HANDLE mhEvents[2];
	DWORD mnCount;
	CONST HANDLE *mlpHandles;
	BOOL mfWaitAll;
	DWORD mdwMilliseconds;

	// Event (Automatic-Reset) de requete pour reveiller (amorcer) le thread qui s'endord, qui s'ennuit
	cEvent mEvtRequestToStart;
	// Event (Automatic-Reset) de requete pour terminer le thread afin de detruire cet objet
	cEvent mEvtRequestToExit;
	
	// Queue de notification des evenements
  std::list<sTEventRec> mEvtQueue;

	int mnbUses;	// Nombre de reference sur cet objet

	static int inst;	// Nombre d'instance 

	// Objet cHiddenNotifiableObject courant (en utilisation pour propagation de l'evenement deja defile 
	cHiddenNotifiableObject* pmCurrHiddenNotifiableObject;
	
	void * pmHiddenNotifiableObjectListToPurge;	
	
};

#endif // !defined(AFX_CTHREAD_H__847AFCD7_7990_11D2_BFD5_00805F7DCE90__INCLUDED_)
