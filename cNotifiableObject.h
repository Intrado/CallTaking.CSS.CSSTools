// cNotifObj.h: interface for the cNotifObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CNOTIFOBJ_H__B02AFD54_7D80_11D2_BFD5_00805F7DCE90__INCLUDED_)
#define AFX_CNOTIFOBJ_H__B02AFD54_7D80_11D2_BFD5_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "cTEvent.h"

#include "CSSTools.h"

class CSSTOOLS_API cNotifiableObject  
{
public:
	cNotifiableObject(const char *pGroupId);
	virtual ~cNotifiableObject();
	virtual void OnEvent(cTEvent *pEvt,void* pToken);
	bool PopEvent(cTEvent **pEvt,void** pToken);
  int  GetNotificationQueueSize();

  // Non virtual methods
  void PurgeEvents(); 
  void RepostEventWithToken(cTEvent *pEvt, const void* pToken, bool inFront = false);
private:
};

#endif // !defined(AFX_CNOTIFOBJ_H__B02AFD54_7D80_11D2_BFD5_00805F7DCE90__INCLUDED_)
