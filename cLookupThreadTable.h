// cLookupThreadTable.h: interface for the cLookupThreadTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOOKUPTHREADTABLE_H__89975439_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CLOOKUPTHREADTABLE_H__89975439_797B_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "cEvtThread.h"
#include "cCritSec.h"

//Element 
typedef struct  sLookupThreadRec
{
	char* pGroupId;				//Identificateur de groupe
	cEvtThread * pEvtThread;	//Objet Thread de la gestion de la queue de notification
}	__sLookupThreadRec;


typedef std::list<sLookupThreadRec> LISTLOOKUP;

class cLookupThreadTable  
{
public:
	cEvtThread* Unregister(cEvtThread* pThreadRef);

	cEvtThread * GetThread(const char* pGroupId);
	cLookupThreadTable();
	virtual ~cLookupThreadTable();
private:
	cCritSec mLockList;	// Lock pour la Liste
	LISTLOOKUP mLookupThreadList;   
    
};

#endif // !defined(AFX_CLOOKUPTHREADTABLE_H__89975439_797B_11D2_BFD5_00805F7DCE90__INCLUDED_)
