// cHiddenNotifiableObject.cpp: implementation of the cHiddenNotifiableObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//#include <crtdbg.h>

#include "cHiddenNotifiableObject.h"
#include "cNotifierObject.h"
#include "cLookupThreadTable.h"
#include "cMappingTable.h"
#include "cEvtThread.h"
#include "cSafeLock.h"
#include "cAutoLock.h"

static bool DiagTraceOn=false;

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int    cHiddenNotifiableObject::nNbInstance=0;

//Instance variable membre static permanent necessaire
cLookupThreadTable cHiddenNotifiableObject::LookupThreadTable;
cMappingTable cHiddenNotifiableObject::MappingTable;


cHiddenNotifiableObject::cHiddenNotifiableObject(cNotifiableObject *pNotifiableObject,const char* pGroupId)
{
  
  //  _ASSERT(pNotifiableObject!=NULL);
  //  _ASSERT(pGroupId);
  
  //Creation de la liste des notifiers
  pmNotifierObjectList  = new LISTNOTIFIER;
  
  mbObsolete=false;
  
  pmMapNotifiableObject=pNotifiableObject;  
  
  // Demande la reference du thread de notification qui s'occupera de ses evenements
  
  Id=++nNbInstance;
  
  //Enregistrement des correspondances cNotifiableObject, cHiddenNotifiableObject
  MappingTable.Register(pNotifiableObject,this);
  
  // L'enregistrement se fait automatiquement pour cette fonction
  // Creation de cEvtThread s'il n'y a pas de thread associe a ce groupe
  pmEvtThread= LookupThreadTable.GetThread(pGroupId);
  
  pCurrHiddenNotifiableObject = 0;  // Coverity 14983: initialzie the pointer field
}

cHiddenNotifiableObject::~cHiddenNotifiableObject()
{
  LISTNOTIFIER *pNotifierObjList;
  int nbElem;
  
  
  
  pNotifierObjList= (LISTNOTIFIER*)pmNotifierObjectList;
  nbElem=pNotifierObjList->size();
  //_ASSERT(nbElem==0);
  
  if (pNotifierObjList!= NULL)
  {
    delete pNotifierObjList;
  }
  
  --nNbInstance;
  
}

cHiddenNotifiableObject* cHiddenNotifiableObject::Expand(cNotifiableObject *pNotifiableObject)
{
  cHiddenNotifiableObject* pTheHiddenNotifiableObject;
  
  //  _ASSERT(pNotifiableObject!=NULL);
  
  pTheHiddenNotifiableObject=MappingTable.Find(pNotifiableObject);
  return pTheHiddenNotifiableObject;  
}

void cHiddenNotifiableObject::Disable(void)
{
  cSafeLock Lock(mLockHiddenNotifiable);
  
  Lock.Acquire();
  
  // Desactive cet objet, plus de traitement fait par cet objet
  mbObsolete=true;
  
  //Des-enregistrement aupres de tous les notificateur 
  UnregisterAll();
  
  // Demande une purge des evenements qui lui conserne dans son thread
  // de queue de notification
  pmEvtThread->Purge(this);
  
  
  Lock.Release();
  
  // S'assure que l'evenement en cours ne soit pas affecte a lui 
  while((pmEvtThread->GetCurrHiddenNotifiableObject())==this)
  {
    //    if (DiagTraceOn)
    //      printf("(cHiddenNotifiableObject::Unregister)   We dont need more event !!!!!!!!!!\n");
    Sleep(1);
  };
  
  Lock.Acquire();
  
  //De-enregistrement de de cEvtThread aupres de cLookupThreadTable
  // Si NULL signifie qu'il y a plus de reference sur cet objet cEvtThread
  pmEvtThread=LookupThreadTable.Unregister(pmEvtThread);
  
  //Desenregistrement aupres du MappingTable
  MappingTable.Unregister(this);
  
  Lock.Release();
  
  
}

void cHiddenNotifiableObject::Notify(cTEvent *pEvt, void* pToken)
{
  sTEventRec EvtRec;
  
  //Notifie l'evenement a l'objet associe cEvtThread
  
  //  _ASSERT(pEvt!=NULL);
  
  // On ne plus rien recevoir desole....
  if (mbObsolete==true)
  {
    
    if(DiagTraceOn)
    {
      //        printf("cHiddenNotifiableObject::Notify   We dont need more event !!!!!!!!!!\n");
      //        printf("Notify(): No more notify (evt)....\n");
    }
    return;
  }
  
  if (pmEvtThread != NULL)
  {
    EvtRec.pEvt = pEvt;
    EvtRec.pHiddenNotifiableObject= this;
    EvtRec.pToken=pToken;
    EvtRec.pEvt->AddRef();
    pmEvtThread->Notify(EvtRec);
    EvtRec.pEvt->ReleaseRef();
  }
  
}

void cHiddenNotifiableObject::OnEvent(cTEvent *pEvt, void* pToken)
{
  
  if (mbObsolete==false)
  {
    // On retransmet l'evenement a son correspondant evidemment
    pmMapNotifiableObject->OnEvent(pEvt,pToken);
  }
  
  return;
  
}

void cHiddenNotifiableObject::PurgeEvents()
{
  pmEvtThread->Purge(this);
}


void cHiddenNotifiableObject::Register(cNotifierObject* pNotifierObject,void* pToken)
{
  cSafeLock Lock(mLockHiddenNotifiable);
  LISTNOTIFIER *pNotifierObjList;
  LISTNOTIFIER::iterator k;
  sNotifierObjWithTokenRec TheNotifierObjWithTokenRec;
  int nbEle;
  
  //  _ASSERT(pNotifierObject!=NULL);
  
  
  Lock.Acquire();
  
  TheNotifierObjWithTokenRec.pNotifierObject=pNotifierObject;
  TheNotifierObjWithTokenRec.pToken=pToken;
  
  pNotifierObjList= (LISTNOTIFIER*)pmNotifierObjectList;
  
  // Est-il present ??
  for (k = pNotifierObjList->begin(); k != pNotifierObjList->end(); ++k)
  {
    
    if (
      ((*k).pNotifierObject == pNotifierObject) 
      && ((*k).pToken==pToken)
      )
    {
      nbEle=pNotifierObjList->size();
      Lock.Release();
      return;
    }
  }
  
  // On l'enregistre alors
  k = pNotifierObjList->end();
  pNotifierObjList->insert(k,TheNotifierObjWithTokenRec);
  nbEle=pNotifierObjList->size();
  Lock.Release();
  
}

void cHiddenNotifiableObject::Unregister(cNotifierObject* pNotifierObject,void* pToken)
{
  cSafeLock Lock(mLockHiddenNotifiable);
  LISTNOTIFIER *pNotifierObjList;
  LISTNOTIFIER::iterator k;
  int nbEle;
  
  
  //_ASSERT(pNotifierObject!=NULL);
  Lock.Acquire();
  pNotifierObjList= (LISTNOTIFIER*)pmNotifierObjectList;
  
  // Est-il present ??
  for (k = pNotifierObjList->begin(); k != pNotifierObjList->end(); ++k)
  {
    if (
      ((*k).pNotifierObject == pNotifierObject) 
      && ((*k).pToken==pToken)
      )
      
    {
      nbEle=pNotifierObjList->size();
      
      pNotifierObjList->erase(k);
      Lock.Release();
      return;
    }
  }
  
  nbEle=pNotifierObjList->size();
  Lock.Release();
}


void cHiddenNotifiableObject::UnregisterAll(void)
{
  cSafeLock Lock(mLockHiddenNotifiable);
  LISTNOTIFIER *pNotifierObjList;
  LISTNOTIFIER::iterator k;
  int nbEle;
  
  
  Lock.Acquire(); 
  pNotifierObjList= (LISTNOTIFIER*)pmNotifierObjectList;
  
  nbEle=pNotifierObjList->size();
  
  for (k = pNotifierObjList->begin(); k != pNotifierObjList->end();)
  {
    {
      //Des-enregistrement aupres des notificateur si prealableement enregistree
      (*k).pNotifierObject->UnregisterMe(pmMapNotifiableObject,(*k).pToken);
      pNotifierObjList->erase(k++);
    } 
  }
  
  nbEle=pNotifierObjList->size();
  //_ASSERT(nbEle==0);
  Lock.Release();
  
}


void cHiddenNotifiableObject::RepostEvent(cTEvent *pEvt, const void* pToken, bool inFront)
{
  cSafeLock Lock(mLockHiddenNotifiable);
  sTEventRec EvtRec;
  EvtRec.pEvt = pEvt;
  EvtRec.pHiddenNotifiableObject= this;
  EvtRec.pToken=(void*)pToken;
  pmEvtThread->Notify(EvtRec, inFront);
}

cNotifiableObject* cHiddenNotifiableObject::GetNotifiableObject()
{
  return pmMapNotifiableObject;
}

bool cHiddenNotifiableObject::PopEvent(cTEvent **pEvt,void** pToken)
{
  if(pmEvtThread->GetmidThread() != GetCurrentThreadId()) // We are NOT in the notification thread !
  {
    return false;
  }

  // We can safely search the next element of the list for me
  cAutoLock pt(pmEvtThread->GetmLockQueue());

  for(list<sTEventRec>::iterator index = pmEvtThread->GetmEvtQueue().begin() ; index != pmEvtThread->GetmEvtQueue().end() ; ++index)
  {
    if(index->pHiddenNotifiableObject == this) // next item
    {
      *pEvt = index->pEvt;
      *pToken = index->pToken;
      pmEvtThread->GetmEvtQueue().erase(index);
      return true;
    }
  }

  return false;
}

int cHiddenNotifiableObject::GetNotificationQueueSize()
{
  return pmEvtThread->GetmEvtQueue().size();
}
