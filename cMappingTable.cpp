// cMappingTable.cpp: implementation of the cMappingTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cMappingTable.h"
#include "cSafeLock.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cMappingTable::cMappingTable()
{
LISTMAP:: iterator k;
  k=mMapList.begin();
}

cMappingTable::~cMappingTable()
{

}


cHiddenNotifiableObject * cMappingTable::Find(cNotifiableObject *pNotifiableObject)
{
cSafeLock LockL(mLockList);
int nbEle;
sMappingTableRec TheMapListRec;
LISTMAP:: iterator k;

    
  LockL.Acquire();
  
  //Cherche dans la liste 
    nbEle=mMapList.size();

    // Recherche le thread concerne
    for (k = mMapList.begin(); k != mMapList.end(); ++k)
      {
      
      TheMapListRec= *k;
        //On a deja le groupe
        if (TheMapListRec.pNotifiableObject == pNotifiableObject)
        {
            LockL.Release();
          return TheMapListRec.pHiddenNotifiableObject;
        }

      }
  LockL.Release();
  return NULL;
}


void cMappingTable::Register(cNotifiableObject *pNotifiableObject, cHiddenNotifiableObject *pHiddenNotifiableObject)
{
cSafeLock LockL(mLockList);
sMappingTableRec TheMapListRec;
LISTMAP:: iterator k;

  LockL.Acquire();

  TheMapListRec.pHiddenNotifiableObject = pHiddenNotifiableObject;
  TheMapListRec.pNotifiableObject= pNotifiableObject;

  // Est-il present ??
  for (k = mMapList.begin(); k != mMapList.end(); ++k)
  {
    if (  ((*k).pNotifiableObject == pNotifiableObject) || 
          ((*k).pHiddenNotifiableObject == pHiddenNotifiableObject)
       )
    {
      LockL.Release();
      return;
    }

  }
  
  // On l'enregistre alors
  k = mMapList.end();
  mMapList.insert(k,TheMapListRec);
  LockL.Release();

}


bool cMappingTable::Unregister(cNotifiableObject *pNotifiableObject)
{
cSafeLock LockL(mLockList);
sMappingTableRec TheMapListRec;
LISTMAP:: iterator k;

  LockL.Acquire();

  for (k = mMapList.begin(); k != mMapList.end(); ++k)
    {

        TheMapListRec= *k;
    if (TheMapListRec.pNotifiableObject  == pNotifiableObject)  
     { 
       mMapList.erase(k);
       LockL.Release();
       return true;
     }
    }
  LockL.Release();
  return false;
}

bool cMappingTable::Unregister(cHiddenNotifiableObject *pHiddenNotifiableObject)
{
cSafeLock LockL(mLockList);
sMappingTableRec TheMapListRec;
LISTMAP:: iterator k;

  LockL.Acquire();

  for (k = mMapList.begin(); k != mMapList.end(); ++k)
    {

        TheMapListRec= *k;
    if (TheMapListRec.pHiddenNotifiableObject  == pHiddenNotifiableObject)  
     { 
      
         mMapList.erase(k);
       LockL.Release();
       return true;
     }
    }
  LockL.Release();
  return false;
}
