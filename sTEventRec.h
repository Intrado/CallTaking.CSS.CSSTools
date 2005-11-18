// cTNotifiableEvent.h: interface for the cTNotifiableEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_cTNotifiableEvent_H__6816C845_7A3A_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_cTNotifiableEvent_H__6816C845_7A3A_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

	
//#include "sEvtToNotif.h"
#include "cTEvent.h"
#include "cHiddenNotifiableObject.h"	

typedef struct  sTEventRec
{
		cTEvent * pEvt;
		cHiddenNotifiableObject *pHiddenNotifiableObject;
		void* pToken;
}		__sTEventRec;


#endif // !defined(AFX_cTNotifiableEvent_H__6816C845_7A3A_11D2_BFD5_00805F7DCE90__INCLUDED_)
