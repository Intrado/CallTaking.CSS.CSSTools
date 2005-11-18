// cMappingTable.h: interface for the cMappingTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMAPPINGTABLE_H__B02AFD55_7D80_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CMAPPINGTABLE_H__B02AFD55_7D80_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "cCritSec.h"

#include "cNotifiableObject.h"
#include "cHiddenNotifiableObject.h"

typedef struct sMappingTableRec
{
	cNotifiableObject *pNotifiableObject;
	cHiddenNotifiableObject *pHiddenNotifiableObject;
}	__sMappingTableRec;


class cMappingTable  
{
public:
	cMappingTable();
	virtual ~cMappingTable();
	cHiddenNotifiableObject * Find(cNotifiableObject *pNotifiableObject);
	void Register(cNotifiableObject *pNotifiableObject, cHiddenNotifiableObject *pHiddenNotifiableObject);
	bool Unregister(cNotifiableObject *pNotifiableObject);
	bool Unregister(cHiddenNotifiableObject *pHiddenNotifiableObject);
private:

	cCritSec mLockList;	// Lock pour la Liste
  typedef std::list<sMappingTableRec> LISTMAP;
	    
	LISTMAP mMapList;   

};

#endif // !defined(AFX_CMAPPINGTABLE_H__B02AFD55_7D80_11D2_BFD5_00805F7DCE90__INCLUDED_)
