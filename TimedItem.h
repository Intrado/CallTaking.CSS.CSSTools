// TimedItem.h: interface for the cTimedItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEDITEM_H__6950BB7C_8635_11D2_BAA3_00805F7DF67A__INCLUDED_)
#define AFX_TIMEDITEM_H__6950BB7C_8635_11D2_BAA3_00805F7DF67A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CSSTools.h"

class CSSTOOLS_API cTimedItem  
{
public:
  // Creates a new timed item. theItem is a reference to an external object
  // aliveDuration says how much time this object will sleep
	cTimedItem(void* theItem, unsigned long aliveDuration /* in milliseconds */);
	virtual ~cTimedItem();

  // Returns the moment to which this object will expire
	virtual unsigned long ExpireAt();
  // Returns true if the object has expired
  virtual bool HasExpired();
  // Sets a new limit before the object expiration
  virtual void Snooze(unsigned long millisecDelay);
  // Returns the associated item
  virtual void* GetItem();

  virtual void SetAliveDuration(unsigned long aliveDuration /* in milliseconds */);

  virtual void Destroy() { delete this; };

private:
  unsigned long expiresAt;
	unsigned long createdAt;
	void* theItem;
};

#endif // !defined(AFX_TIMEDITEM_H__6950BB7C_8635_11D2_BAA3_00805F7DF67A__INCLUDED_)
