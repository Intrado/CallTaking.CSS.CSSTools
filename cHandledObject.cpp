// cHandledObject.cpp: implementation of the cHandledObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cHandledObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cHandledObject::cHandledObject()
{
  handle=NULL;
}

cHandledObject::cHandledObject(const cHandledObject &src)
{
  operator=(src);
}

cHandledObject& cHandledObject::operator=(const cHandledObject &rhs)
{
  DuplicateHandle(GetCurrentProcess(), rhs.handle, GetCurrentProcess(), &handle,
                  0, TRUE, DUPLICATE_SAME_ACCESS);
  return *this;
}

cHandledObject::~cHandledObject()
{
  if (handle!=NULL)
    CloseHandle(handle);
}
