// cNotifierObject.cpp: implementation of the cNotifierObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <crtdbg.h>
#include "cNotifierObject.h"
#include "cSafeLock.h"

int    cNotifierObject::nNbInstance=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


cNotifierObject::cNotifierObject()
{
  LISTOBJ:: iterator k;
  LISTOBJ* pHidden;
  pHiddenNotifiableObjectList=new LISTOBJ;
  pHidden= (LISTOBJ*)(pHiddenNotifiableObjectList);
  k = pHidden->begin();
  Id=++nNbInstance;
  
}

cNotifierObject::~cNotifierObject()
{
  LISTOBJ* pHidden;
  
  pHidden= (LISTOBJ*)(pHiddenNotifiableObjectList);
  
  // Auto -desenregistrement
  UnregisterAll();
  
  if(pHidden!=NULL)
  {
    delete pHidden;
  }
  
}


void cNotifierObject::Register(cNotifiableObject *pNotifiableObject,void* pToken)
{
  cSafeLock Lock(mLockNotifier);
  cHiddenNotifiableObject* pHiddenNotifiableObj;
  sHiddenNotifiableObjWithTokenRec ObjToRegister;
  LISTOBJ:: iterator k;
  LISTOBJ* pHidden;
  int nbEle;
  
  // Expansion 
  //_ASSERT(pNotifiableObject!=NULL);
  
  pHiddenNotifiableObj= cHiddenNotifiableObject::Expand(pNotifiableObject);
  
  Lock.Acquire();
  
  //Conserve la reference du notifier ou il est enregistre pour auto-desenregistrement
  pHiddenNotifiableObj->Register(this,pToken);
  
  ObjToRegister.pHiddenNotifiableObject= pHiddenNotifiableObj;
  ObjToRegister.pNotifiableObject =pNotifiableObject;
  ObjToRegister.pToken= pToken;
  
  //Est-il deja present
  
  pHidden= (LISTOBJ*)(pHiddenNotifiableObjectList);
  nbEle=pHidden->size();
  
  // On peut ajouter la meme reference maintenant
  
  for (k = pHidden->begin(); k != pHidden->end(); ++k)
  {
    if (    ((*k).pHiddenNotifiableObject == pHiddenNotifiableObj) 
      && ((*k).pToken == pToken)
      )
    { 
      
      nbEle=pHidden->size();
      Lock.Release();
      return;
    }
  }
  
  k = pHidden->end();
  
  pHidden->insert(k,ObjToRegister);
  nbEle=pHidden->size();
  
  Lock.Release();
  
}

void cNotifierObject::UnregisterMe(cNotifiableObject *pNotifiableObject, void* pToken)
{
  cSafeLock Lock(mLockNotifier);
  sHiddenNotifiableObjWithTokenRec ObjFromRegister;
  cHiddenNotifiableObject* pHiddenNotifiableObj;
  LISTOBJ:: iterator k;
  LISTOBJ* pHidden;
  int nbEle;
  
  //_ASSERT(pNotifiableObject!=NULL);
  pHiddenNotifiableObj= cHiddenNotifiableObject::Expand(pNotifiableObject);
  
  Lock.Acquire();
  
  // Expansion 
  pHidden= (LISTOBJ*)(pHiddenNotifiableObjectList);
  nbEle=pHidden->size();
  for (k = pHidden->begin(); k != pHidden->end(); ++k)
  {
    ObjFromRegister=*k;
    
    if  (
      (ObjFromRegister.pNotifiableObject == pNotifiableObject) 
      && (ObjFromRegister.pToken == pToken)
      )
    { 
      
      pHidden->erase(k);
      nbEle=pHidden->size();
      Lock.Release();
      return;
    }
  }
  nbEle=pHidden->size();
  Lock.Release();
  
  
}

void cNotifierObject::Unregister(cNotifiableObject *pNotifiableObject, void* pToken)
{
  cSafeLock Lock(mLockNotifier);
  sHiddenNotifiableObjWithTokenRec ObjFromRegister;
  cHiddenNotifiableObject* pHiddenNotifiableObj;
  LISTOBJ:: iterator k;
  LISTOBJ* pHidden;
  int nbEle;
  
  //_ASSERT(pNotifiableObject!=NULL);
  pHiddenNotifiableObj= cHiddenNotifiableObject::Expand(pNotifiableObject);
  
  Lock.Acquire();
  
  // Expansion 
  pHidden= (LISTOBJ*)(pHiddenNotifiableObjectList);
  nbEle=pHidden->size();
  for (k = pHidden->begin(); k != pHidden->end(); ++k)
  {
    ObjFromRegister=*k;
    
    if  (
      (ObjFromRegister.pNotifiableObject == pNotifiableObject) 
      && (ObjFromRegister.pToken == pToken)
      )
    { 
      
      (*k).pHiddenNotifiableObject->Unregister(this,(*k).pToken);
      pHidden->erase(k);
      nbEle=pHidden->size();
      Lock.Release();
      return;
    }
  }
  nbEle=pHidden->size();
  Lock.Release();
  
  
}

void cNotifierObject::UnregisterAll(void)
{
  cSafeLock Lock(mLockNotifier);
  LISTOBJ:: iterator k;
  LISTOBJ* pHidden;
  int nbEle;
  
  Lock.Acquire();
  
  // Expansion 
  pHidden= (LISTOBJ*)(pHiddenNotifiableObjectList);
  nbEle=pHidden->size();
  
  for (k = pHidden->begin(); k != pHidden->end();)
  {
    //Des-enregistrement aupres des notificateur si prealableement enregistree
    (*k).pHiddenNotifiableObject->Unregister(this,(*k).pToken);
    pHidden->erase(k++);
  }
  
  nbEle=pHidden->size();
  //_ASSERT(nbEle==0);
  
  Lock.Release();
  
}

void cNotifierObject::Notify(cTEvent *pEvt)
{
  cSafeLock Lock(mLockNotifier);
  sHiddenNotifiableObjWithTokenRec ObjToRegister;
  cHiddenNotifiableObject* pHiddenNotifiableObject;
  void* pToken;
  LISTOBJ:: iterator k;
  int nbEle;
  int n;
  LISTOBJ* pHidden;
  
  //_ASSERT(pEvt!=NULL);
  
  Lock.Acquire();
  
  pHidden= (LISTOBJ*)(pHiddenNotifiableObjectList);
  nbEle=pHidden->size();
  
  // Appel tous les objets HiddenNotifiableObject enregistree avec l'evenement 
  
  for (k = pHidden->begin(); k != pHidden->end(); ++k)
  {
    ObjToRegister=(*k);
    
    pHiddenNotifiableObject= ObjToRegister.pHiddenNotifiableObject;
    pToken=ObjToRegister.pToken;   
    n=pHiddenNotifiableObject->Id;
    
    pEvt->AddRef();
    pHiddenNotifiableObject->Notify(pEvt,pToken);
    pEvt->ReleaseRef();
    
  }
  nbEle=pHidden->size();
  Lock.Release();
}

void cNotifierObject::DuplicateRegistration(const cNotifierObject* pSource)
{
  cSafeLock Lock(mLockNotifier);
  sHiddenNotifiableObjWithTokenRec ObjToRegister;
  cHiddenNotifiableObject* pHiddenNotifiableObject;
  void* pToken;
  LISTOBJ::iterator k;
  LISTOBJ* pHidden;
  
  Lock.Acquire();
  pHidden= (LISTOBJ*)(pSource->pHiddenNotifiableObjectList);
  
  // Copy all registration
  for (k = pHidden->begin(); k != pHidden->end(); ++k)
  {
    ObjToRegister = (*k);
    pHiddenNotifiableObject = ObjToRegister.pHiddenNotifiableObject;
    pToken = ObjToRegister.pToken;   
    Register(pHiddenNotifiableObject->GetNotifiableObject(), pToken);
  }
  
  Lock.Release();
}
