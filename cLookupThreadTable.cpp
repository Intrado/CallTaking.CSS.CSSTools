// cLookupThreadTable.cpp: implementation of the cLookupThreadTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cLookupThreadTable.h"
#include "Windows.h"
#include "cSafeLock.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cLookupThreadTable::cLookupThreadTable()
{
LISTLOOKUP:: iterator k;

  k=mLookupThreadList.begin();
}

cLookupThreadTable::~cLookupThreadTable()
{

}

// Obtient la reference du thread de gestion de la queue de notification
// Si le groupe du thread n'est pas creer alors on le creera et on l'enregistrera

cEvtThread * cLookupThreadTable::GetThread(const char *pGroupId)
{
cSafeLock LockL(mLockList);
sLookupThreadRec TheLookupThreadRec;
cEvtThread *pTheNewEvtThread; 
int nbEle;
LISTLOOKUP:: iterator k;

  LockL.Acquire();

    nbEle=mLookupThreadList.size();

    // Recherche le thread concerne
    for (k = mLookupThreadList.begin(); k != mLookupThreadList.end(); ++k)
      {
      TheLookupThreadRec= *k;
        //On a deja le groupe
        if (strcmp(TheLookupThreadRec.pGroupId,pGroupId)==0) 
        {
          // Une reference d'utilisation de plus
          TheLookupThreadRec.pEvtThread->AddRef();
          LockL.Release();
          return TheLookupThreadRec.pEvtThread; 
        }

      }

    // On Cree un cEvtThread
    pTheNewEvtThread= new cEvtThread;

    // On Cree un sLookupThreadRec 
    // Attention il faudra detruire ulterieurement la chaine prise par GroupId....
#ifdef UNDER_CE
    TheLookupThreadRec.pGroupId = (char *)malloc(strlen(pGroupId) * sizeof(char));
    strcpy(TheLookupThreadRec.pGroupId, pGroupId);
#else // UNDER_CE
    TheLookupThreadRec.pGroupId = _strdup(pGroupId);
#endif // UNDER_CE    
    TheLookupThreadRec.pEvtThread = pTheNewEvtThread;

    // Et on l'enregistre
    k = mLookupThreadList.end();
    mLookupThreadList.insert(k,TheLookupThreadRec);
    // Une reference d'utilisation de plus
    TheLookupThreadRec.pEvtThread->AddRef();
    
  LockL.Release();
  return pTheNewEvtThread;
}



cEvtThread* cLookupThreadTable::Unregister(cEvtThread* pThreadRef)
{
cSafeLock LockL(mLockList);
sLookupThreadRec TheLookupThreadRec;
LISTLOOKUP:: iterator k;
cEvtThread* pEvtThread=pThreadRef;
int nbRef;

  LockL.Acquire();

  for (k = mLookupThreadList.begin(); k != mLookupThreadList.end(); ++k)
    {

        TheLookupThreadRec= *k;
    if (TheLookupThreadRec.pEvtThread== pThreadRef)   
     { 

           //Une reference d'utilisation de moins
      nbRef=TheLookupThreadRec.pEvtThread->Release();
   
         // Une reference d'utilisation de moins
        if (nbRef==0)
        {
        // On enleve seulement lorsqu'il n'y a plus de reference  
            mLookupThreadList.erase(k);

        free(TheLookupThreadRec.pGroupId);

        //Autodestruction de cEvtThread 
        
        delete(TheLookupThreadRec.pEvtThread);
        
        // Signifiant plus de reference d utilisation disponbile sur cet objet
        pEvtThread=NULL;
        }
       LockL.Release();
       return pEvtThread;
     }
    }

  LockL.Release();
  return pEvtThread;
}

