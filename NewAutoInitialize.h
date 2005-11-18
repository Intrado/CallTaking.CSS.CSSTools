
#ifndef _NEWAUTOINITIALIZE
#define _NEWAUTOINITIALIZE

#include "assert.h"

enum OperatorNewMode
{
  NewAutoInitialize = 1
};

#ifndef _LEAK_DETECT

inline void* operator new( size_t size, OperatorNewMode NewMode ) 
{
  if  (NewMode == NewAutoInitialize)
  {
    void *p = ::operator new(size);
    memset(p, 0, size);
    return p;
  }
  else
  {
    // Invalid new mode
    assert(0);
    return NULL;
  }
}
#define NEW0 new(NewAutoInitialize)

#else

#define NEW0 new

#endif

#endif