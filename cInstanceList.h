/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 1998.
 * ALL RIGHTS RESERVED.
 *
 * File: cInstanceList.h
 *
 * Project: Utility class
 *
 * Description:
 *
 *  This file contains the declaration of the cInstanceList template
 *  (list of instance for an object).
 *
 ***************************************************************************/

#ifndef _CINSTANCELIST_H
#define _CINSTANCELIST_H

#pragma warning(disable:4786 4503)

#include <list>
#include "cCritSec.h"

template<class cObjType> class cInstanceList
{
public:
  // Add an instance in the list.
  inline void Add(cObjType *pInst);

  // Remove an instance from the list.
  inline bool Remove(cObjType *pInst);

  // Get the first instance in the list.
  inline cObjType* GetFirst();

  // Get the instance following the given instance.
  inline cObjType* GetNext(cObjType *pNextTo);

  // Get the number of items in the instance list.
  inline int GetSize();

  bool operator==(const cInstanceList<cObjType>& rhs);

private:
  typedef std::list<cObjType*> tInstList;
  tInstList mList; // The list of instances.
  cCritSec mListCritSec; // Critical section used to control simultaneous access to the list.
};


template<class cObjType> inline void cInstanceList<cObjType>::Add(cObjType *pInst)
{
  mListCritSec.Acquire();

  if (pInst != 0)
  {
    mList.insert(mList.end(), pInst);
  }

  mListCritSec.Release();
}


template<class cObjType> inline bool cInstanceList<cObjType>::Remove(cObjType *pInst)
{
  bool bFound = false;

  tInstList::iterator is;

  mListCritSec.Acquire();

  for (is = mList.begin(); is != mList.end(); ++is)
  {
    if (*is == pInst)
    {
      mList.erase(is);
      bFound = true;
      break;
    }
  }

  mListCritSec.Release();
  return bFound;
}


template<class cObjType> inline cObjType* cInstanceList<cObjType>::GetFirst()
{
  cObjType *pRes;
  tInstList::iterator is;

  mListCritSec.Acquire();

  is = mList.begin();

  if (is == mList.end())
  {
    pRes = 0;
  }
  else
  {
    pRes = *is;
  }

  mListCritSec.Release();

  return (pRes);
}


template<class cObjType> inline cObjType* cInstanceList<cObjType>::GetNext(cObjType *pNextTo)
{
  cObjType *pRes;
  tInstList::iterator is;

  mListCritSec.Acquire();

  for (is = mList.begin(); is != mList.end(); ++is)
  {
    if (*is == pNextTo)
    {
      break;
    }
  }

  if (is == mList.end())
  {
    pRes = 0;
  }
  else
  {
    is++;
    if (is == mList.end())
    {
      pRes = 0;
    }
    else
    {
      pRes = *is;
    }
  }

  mListCritSec.Release();

  return (pRes);
}


template<class cObjType> inline int cInstanceList<cObjType>::GetSize()
{
  int nb;

  mListCritSec.Acquire();
  nb = mList.size();
  mListCritSec.Release();

  return (nb);
}

template<class cObjType> bool cInstanceList<cObjType>::operator==(const cInstanceList<cObjType>& rhs)
{
  bool bResult;
  mListCritSec.Acquire();
  bResult = (mList == rhs.mList);
  mListCritSec.Release();
  return bResult;
}

#endif
