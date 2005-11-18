// cThread.cpp: implementation of the cThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <crtdbg.h>

#include "cTEvent.h"
#include "cEvtThread.h"
#include "cHiddenNotifiableObject.h"
#include "cSafeLock.h"
#include "CSS.h"

#define TRACE_MODULE "cEvtThread"

int cEvtThread::inst=0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cEvtThread::cEvtThread()
{
  LPTHREAD_START_ROUTINE lpStartAddress;     
  
  
  pmHiddenNotifiableObjectListToPurge = new (LIST_TO_PURGE);
  
  pmCurrHiddenNotifiableObject=NULL;
  Id=++inst;
  mnbUses=0;
  
  // Le signaux d'amorcage et d'arret du thread
  mhEvents[0]=HANDLE(mEvtRequestToStart);
  mhEvents[1]=HANDLE(mEvtRequestToExit);
  
  mnCount=2;
  mfWaitAll=false;
  mdwMilliseconds=INFINITE;
  mlpHandles=mhEvents;
  
  lpStartAddress = (LPTHREAD_START_ROUTINE)&Func;
  
  //Creation du Thread de gestion de la queue des evenements
  mhThread = CreateThread (0,         // pointer to security attributes
    0,          // initial thread stack size            
    lpStartAddress,     // pointer to thread function
    this,       // argument for new thread
    0,          // creation flags            
    &midThread      // pointer to receive thread ID
    );
  if(mhThread ==NULL)
  {
    throw HandleInvalid();
  }   
  
  
} 

cEvtThread::~cEvtThread()
{
  cSafeLock LockT(mLockEvtThread);
  DWORD ExitCode; 
  
  LockT.Acquire();
  
  //Informe le thread d'arret d'executer
  mEvtRequestToExit.Signal();
  
  //Synchonisation sur terminaison du thread
  //Est-tu mord ou encore en vie ???
  WaitForSingleObject(mhThread, INFINITE);
  GetExitCodeThread(mhThread,&ExitCode);      // handle to the thread
  
  CloseHandle (mhThread);
  --inst;
  LockT.Release();
  
  
}

cEvtThread::Func(cEvtThread * pObj)
{
  DiagTrace(TRACE_MODULE, "Func", "Starting new thread");
  if (pObj!= NULL)
    pObj->Dispatch(); 
}


// Notification de l'evenement et enfilement dans la queue
void cEvtThread::Notify(sTEventRec Evt, bool inFront)
{
  cSafeLock LockQ(mLockQueue);
  int nbEle;
  
  LockQ.Acquire();
  
  nbEle= mEvtQueue.size();
  
  Evt.pEvt->AddRef();
  // Enfilement de la reference de l'objet 
  if(inFront)
    mEvtQueue.push_front(Evt);
  else
    mEvtQueue.push_back(Evt);
  
  nbEle= mEvtQueue.size();
  
  LockQ.Release();
  
  // Signale le thread de gestion de la queue des evenement
  mEvtRequestToStart.Signal();
  
}


void cEvtThread::Dispatch()
{
  cSafeLock LockQ(mLockQueue);
  struct sTEventRec OurNotifyEventRec;
  cHiddenNotifiableObject* pOurHiddenNotifiableObject;
  cTEvent* pOurTEvent;
  void* pToken;
  DWORD dwResult;
  int nbEle;
  bool bEmpty;
  
  while(true)
  {
    
    dwResult = WaitForMultipleObjects(
      mnCount,
      mhEvents,
      mfWaitAll,
      mdwMilliseconds);
    
    dwResult-=WAIT_OBJECT_0;
    
    // Signal d'arret 
    switch(dwResult)
    {
      
      // Requete d'amorcage du thread d'execution
    case 0:
      break;
      
      // Requete de terminaison du thread d'execution
    case 1:
      //      if(DiagTraceOn)
      //        printf("On arrete ce thread .... %d\n", Id);
      
      return;
      break;
      
    default:
      //      if(DiagTraceOn)
      //        printf("Unknown cEvent.... %d\n", Id);
      break;
      
    }
    
    LockQ.Acquire();
    bEmpty=mEvtQueue.empty();
    LockQ.Release();
    
    // Prend un element dans la queue
    while (!bEmpty)
    {
      
      LockQ.Acquire();
      bEmpty=mEvtQueue.empty();
      if(!bEmpty)
      {
        // On prent l'element
        OurNotifyEventRec=mEvtQueue.front();
        mEvtQueue.pop_front();
        
        nbEle= mEvtQueue.size();
      }
      LockQ.Release();
      
      if (!bEmpty)
      {
        pOurHiddenNotifiableObject=OurNotifyEventRec.pHiddenNotifiableObject;
        
        SetCurrHiddenNotifiableObject(pOurHiddenNotifiableObject);
        
        pOurTEvent=OurNotifyEventRec.pEvt;
        pToken=OurNotifyEventRec.pToken;
        
        // On le distribue 
        if (pOurTEvent!= NULL)
        {
          // Est-ce un evenement a purger 
          if(!bIsPresent(pOurHiddenNotifiableObject))
          {
            pOurHiddenNotifiableObject->OnEvent(pOurTEvent,pToken);
          }
          else
          {
            RemoveIt(pOurHiddenNotifiableObject);
          }
          
          OurNotifyEventRec.pEvt->ReleaseRef();
        }
        
        SetCurrHiddenNotifiableObject(NULL);
        
      };//if (!bEmpty)
      
    }// While (!bEmpty)
  }// While(true)
}


void cEvtThread:: Purge(cHiddenNotifiableObject *pHiddenNotifiableObject)
{
  cSafeLock LockT(mLockEvtThread);
  cSafeLock LockQ(mLockQueue);
  
  // Place la reference de l'objet a effacer dans une liste,
  // Necessaire quant l'object cEvtThread est commun a plus d'un objet
  // cHiddenNotifiableObject
  
  
  // Enleve toutes les evenements qui refere a l'objet cHiddenNotifiableObject
  LockT.Acquire();
  
  //PutInListToPurge(pHiddenNotifiableObject);
  
  LockQ.Acquire();
  list<sTEventRec>::iterator index = mEvtQueue.begin();
  while(index !=mEvtQueue.end())
  {
    if((*index).pHiddenNotifiableObject == pHiddenNotifiableObject)
    {
      list<sTEventRec>::iterator next = index; ++next ;
      mEvtQueue.erase(index);
      index = next;
    }
    else 
      ++index;
  }

  LockQ.Release();
  LockT.Release();    
}

cHiddenNotifiableObject* cEvtThread::GetCurrHiddenNotifiableObject(void)
{
  return pmCurrHiddenNotifiableObject;    
}

void cEvtThread::SetCurrHiddenNotifiableObject(cHiddenNotifiableObject* pCurrHiddenNotifiableObject)
{
  this->pmCurrHiddenNotifiableObject=pCurrHiddenNotifiableObject;   
}


//Une reference d'utilisation de plus
int cEvtThread::AddRef(void)
{
  cSafeLock LockT(mLockEvtThread);
  int nResult;
  
  LockT.Acquire();
  nResult= ++mnbUses;
  LockT.Release();
  return nResult;
}


//Une reference d'utilisation de plus
int cEvtThread::Release(void)
{
  cSafeLock LockT(mLockEvtThread);
  int nResult;
  // On devrait mettre la logique d'effacement du composant si
  // plus de reference disponible de cet objet
  LockT.Acquire();
  nResult=--mnbUses;
  LockT.Release();
  
  return nResult;
}


int cEvtThread::PutInListToPurge(cHiddenNotifiableObject *pHiddenNotifiableObject)
{
  cSafeLock LockP(mLockListToPurge);
  LIST_TO_PURGE *pHiddenNotifiableObjListToPurge;
  LIST_TO_PURGE ::iterator k;
  // To be purge and no notify or OnEvent
  
  int nbEle;
  
  //Place l'element a purger dans la liste
  LockP.Acquire();
  pHiddenNotifiableObjListToPurge= (LIST_TO_PURGE*)pmHiddenNotifiableObjectListToPurge;
  
  //Est-il deja present
  nbEle=pHiddenNotifiableObjListToPurge->size();
  
  for (k = pHiddenNotifiableObjListToPurge->begin(); k != pHiddenNotifiableObjListToPurge->end(); ++k)
  {
    if ((*k) == pHiddenNotifiableObject) 
    {       
      nbEle=pHiddenNotifiableObjListToPurge->size();
      LockP.Release();
      return nbEle;
    }
  }
  
  k = pHiddenNotifiableObjListToPurge->end();
  
  pHiddenNotifiableObjListToPurge->insert(k,pHiddenNotifiableObject);
  nbEle=pHiddenNotifiableObjListToPurge->size();
  LockP.Release();
  return nbEle;
}

bool cEvtThread::bIsPresent(cHiddenNotifiableObject *pHiddenNotifiableObject)
{
  cSafeLock LockP(mLockListToPurge);
  LIST_TO_PURGE *pHiddenNotifiableObjListToPurge;
  LIST_TO_PURGE ::iterator k;
  int nbEle;
  
  pHiddenNotifiableObjListToPurge= (LIST_TO_PURGE*)pmHiddenNotifiableObjectListToPurge;
  
  LockP.Acquire();
  nbEle=pHiddenNotifiableObjListToPurge->size();
  
  for (k = pHiddenNotifiableObjListToPurge->begin(); k != pHiddenNotifiableObjListToPurge->end();k++)
  {
    if ( ((*k) == pHiddenNotifiableObject))
    { 
      LockP.Release();
      return true;
    }
  }
  
  LockP.Release();
  
  return false; 
  
}


void cEvtThread::RemoveIt(cHiddenNotifiableObject *pHiddenNotifiableObject)
{
  cSafeLock LockP(mLockListToPurge);
  LIST_TO_PURGE *pHiddenNotifiableObjListToPurge;
  LIST_TO_PURGE ::iterator k;
  int nbEle;
  
  //Place l'element a purger dans la liste
  LockP.Acquire();
  pHiddenNotifiableObjListToPurge= (LIST_TO_PURGE*)pmHiddenNotifiableObjectListToPurge;
  
  //Est-il deja present  
  for (k = pHiddenNotifiableObjListToPurge->begin(); k != pHiddenNotifiableObjListToPurge->end();k++)
  {
    if ((*k) == pHiddenNotifiableObject)
    { 
      
      pHiddenNotifiableObjListToPurge->erase(k);
      LockP.Release();      
      return;
    }
  }
  
  nbEle=pHiddenNotifiableObjListToPurge->size();
  LockP.Release();
}

