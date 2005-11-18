// cSharedList.cpp: implementation of the cSharedList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSharedList.h"

enum 
	{_iw=0,
	 _ir,
	 _NbRec,
	 _RecSize,
	 _NbClients
	};


CONST  FRAMESIZE = (_NbClients+1)*2;   	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cSharedList::~cSharedList()
{
	if(plock!=NULL)
	   delete plock;

	if(psema!=NULL)
	   delete psema;
}

bool cSharedList::RegisterClient (const char *pclient)
{
WORD *w;
w = (WORD*)Buffer();
 
// Aucune validation d'enregistrement du client
if (plock->Acquire(INFINITE))
  {
   w[_NbClients]+=1;
   plock->Release();
   return true;
 }
 return false;
}

bool cSharedList::UnRegisterClient (const char *pclient)
{
WORD *w;
w = (WORD*)Buffer();

// Aucune validation de des-enregistrement du client

 if (plock->Acquire(INFINITE))
  {
  if(w[_NbClients]!= 0)
	   w[_NbClients]-=1;
  plock->Release();
  return true;
  }
 return false;
}



cSharedList::cSharedList(const char *pname,WORD NbRec, WORD recSize):
cSharedMemory(pname,(recSize*NbRec)+FRAMESIZE)

{
WORD *w1;
WORD *w;

  w = (WORD*)Buffer();
  
  plock = new cMutex("SMMutex",false);
  psema = new cSemaphore(0,2048,"SMSemaphore");

  if (IsCreated())
	 {
     w = (WORD*)Buffer();
	 w[_iw] = 0;
	 w[_ir] = 0;
	 w[_NbRec] = NbRec;		// Nb de record permis
	 w[_RecSize]= recSize;	// Taille des record
	 w[_NbClients]=0;
	 }

  w1=  (WORD*)Buffer()+ FRAMESIZE;
  FillMemory(w1,NbRec*recSize,0);

}

bool cSharedList::put( void * evPtr)
{
  WORD tmpIndexPtr;
  int s;

  if (plock->Acquire(INFINITE))
  {
    if (getNbClient() == 0)
	{
      plock->Release();
      return false;
    }
 	//Prochain index d'ecriture

    tmpIndexPtr = getIndexPtrWr() +1;

    // A ton atteint la fin, alors on redebute (Tampon Circulaire)
	if (tmpIndexPtr == getNbRec()) 
		tmpIndexPtr = 0;

	// On ne permet pas d'overwriter sur les record non encore lu
    if (getIndexPtrRd() != tmpIndexPtr) 
	{
      SetIndexPtrWr(tmpIndexPtr);
      s = (int)Buffer();
 	  s = s +(tmpIndexPtr*getRecSize())+FRAMESIZE;
      CopyMemory ((void*)s,evPtr,getRecSize());
      psema->Release();
	  plock->Release();
      return true;

    }
	else
	{
	  plock->Release();
  	  return false;
	}
 
  }
 
  else
  {
    plock->Release();
    return false;
  }
	
}


void* cSharedList::get(void)
{
  WORD tmpIndexPtr;
  int s;

  if (psema->Acquire(INFINITE))
  {
    if(plock->Acquire(INFINITE))
	{
	  // Prochain index de lecture
      tmpIndexPtr = getIndexPtrRd() +1;
      if (tmpIndexPtr == getNbRec())
		  tmpIndexPtr = 0;

      SetIndexPtrRd(tmpIndexPtr);
      s = (int)Buffer();
      s = s + (tmpIndexPtr*getRecSize())+FRAMESIZE;
      plock->Release();
	  return (void*)s;
	}
	else
	{
  	  plock->Release();
	  return NULL;
	}

  }
  else
  {
	 return NULL;
  }
}
 

void cSharedList::flush (void)
{
  WORD tmpIndexPtr;

  if (plock->Acquire(INFINITE))
  {
    while (psema->Acquire (INFINITE)) 
 		{
	     tmpIndexPtr = getIndexPtrRd() +1;
		 if (tmpIndexPtr == getNbRec())
		    tmpIndexPtr = 0;
         SetIndexPtrRd(tmpIndexPtr);
		}
   	plock->Release();
  }

}


WORD cSharedList::getIndexPtrWr(void)
{
 WORD *w;
 w = (WORD*)Buffer();

  return w[_iw];
}


cSharedList::SetIndexPtrWr (WORD value)
{
WORD *w;
 w = (WORD*)Buffer();
 w[_iw] = value;
}

WORD cSharedList::getIndexPtrRd(void)
{
WORD *w;
w = (WORD*)Buffer();
  return w[_ir];
}


cSharedList::SetIndexPtrRd (WORD value)
{
WORD *w;
w = (WORD*)Buffer();
  w[_ir]= value;
}
			
WORD cSharedList::getNbRec(void)
{
WORD *w;
w = (WORD*)Buffer();
  return w[_NbRec];
}


WORD cSharedList::getRecSize(void)
{  
WORD *w;
w = (WORD*)Buffer();
  return w[_RecSize];	
}

WORD cSharedList::getNbClient(void)
{
WORD *w;
w = (WORD*)Buffer();
   return w[_NbClients];
}

