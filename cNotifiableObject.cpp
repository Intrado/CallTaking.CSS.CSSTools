
#include "stdafx.h"
#include "cNotifiableObject.h"
#include "cHiddenNotifiableObject.h"
#include "cNotifierObject.h"
#include "cMappingTable.h"
#include <crtdbg.h>

cNotifiableObject::cNotifiableObject(const char *pGroupId)
{
  new cHiddenNotifiableObject(this,pGroupId);

}

cNotifiableObject::~cNotifiableObject()
{
cHiddenNotifiableObject *pMapHiddenNotifiableObj;

  //Enleve son objet correspondant
  pMapHiddenNotifiableObj=cHiddenNotifiableObject::Expand(this);
  pMapHiddenNotifiableObj->Disable();
  delete pMapHiddenNotifiableObj;
}

void cNotifiableObject::OnEvent(cTEvent *pEvt,void* pToken)
{
  // Ne doit en aucun cas etre appele, doit etre absolument override
//  _ASSERT(false);
}


void cNotifiableObject::PurgeEvents()
{  
  cHiddenNotifiableObject *pMapHiddenNotifiableObj;
  pMapHiddenNotifiableObj=cHiddenNotifiableObject::Expand(this);
  pMapHiddenNotifiableObj->PurgeEvents();
}

void cNotifiableObject::RepostEventWithToken(cTEvent *pEvt, const void* pToken, bool inFront)
{
  cHiddenNotifiableObject *pMapHiddenNotifiableObj;
  pMapHiddenNotifiableObj=cHiddenNotifiableObject::Expand(this);
  pMapHiddenNotifiableObj->RepostEvent(pEvt, pToken, inFront);
}

bool cNotifiableObject::PopEvent(cTEvent **pEvt,void** pToken)
{
  cHiddenNotifiableObject *pMapHiddenNotifiableObj;
  pMapHiddenNotifiableObj=cHiddenNotifiableObject::Expand(this);
  return pMapHiddenNotifiableObj->PopEvent(pEvt, pToken);
}

int  cNotifiableObject::GetNotificationQueueSize()
{
  cHiddenNotifiableObject *pMapHiddenNotifiableObj;
  pMapHiddenNotifiableObj=cHiddenNotifiableObject::Expand(this);
  return pMapHiddenNotifiableObj->GetNotificationQueueSize();
}

