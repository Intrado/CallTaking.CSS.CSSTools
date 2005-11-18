// TimedItem.cpp: implementation of the cTimedItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TimedItem.h"
#include <windows.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cTimedItem::cTimedItem(void* item, unsigned long aliveDuration /* in milliseconds */)
{
  theItem = item;
  SetAliveDuration(aliveDuration);
}

cTimedItem::~cTimedItem()
{

}

unsigned long cTimedItem::ExpireAt()
{
  return expiresAt;
}

bool cTimedItem::HasExpired()
{
  return GetTickCount() > expiresAt;
}

void cTimedItem::Snooze(unsigned long millisecDelay)
{
  expiresAt = GetTickCount() + millisecDelay;
}

void* cTimedItem::GetItem()
{
  return theItem;
}

void cTimedItem::SetAliveDuration(unsigned long aliveDuration /* in milliseconds */)
{
  createdAt = GetTickCount();
  expiresAt = createdAt + aliveDuration;
}
