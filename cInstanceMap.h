/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 1999.
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    olarouche    date created:    1999/03/30
 * %derived_by:   sroy %       %date_modified:  1999/06/09 17:15:46 %
 *
 * %name:         cInstanceMap.h %
 * %version:      2 %
 *
 * References:
 *
 *     documents
 *
 * Description:
 *
 *     Encapsulates STL MAP.
 *
 ***************************************************************************/

#ifndef _CINSTANCEMAP_H
#define _CINSTANCEMAP_H

#pragma warning(disable:4786 4503)

#include <map>
#include "cCritSec.h"
#include "cAutoLock.h"

template<class cKey, class cElement>
class cInstanceMap
{
public:

  cInstanceMap();
  cInstanceMap(cInstanceMap &MapToCopy);
  bool Insert(cKey key, cElement element);
  bool Replace(cKey key, cElement element, bool bAlwaysInsert = false, bool* pbRequestRemoved = NULL);
  bool Remove(cKey key, cElement *element = NULL);
  bool Find(cKey key, cElement *element);
  bool FindElement(cElement element, cKey &key);
  bool GetFirst(cKey &key, cElement *element);
  bool GetNext(cKey Key, cKey &nextKey, cElement *nextElement);
  bool IsEmpty();
  void Clear();

protected:

  typedef std::map<cKey, cElement> tInstMap;
  tInstMap mMap;

  // Critical section used to control simultaneous access to the map.
  cCritSec mMapCritSec;

};

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	cInstanceMap
 *
 * Arguments: 
 *
 * Returns:	
 *
 * Exceptions:
 *
 * Description:	Constructor.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
cInstanceMap<cKey, cElement>::cInstanceMap()
{
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	cInstanceMap
 *
 * Arguments: cInstanceMap &MapToCopy [in],
 *
 * Returns:	
 *
 * Exceptions:
 *
 * Description:	Copy Constructor.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
cInstanceMap<cKey, cElement>::cInstanceMap(cInstanceMap &MapToCopy)
{
  cAutoLock CopyLock(MapToCopy.mMapCritSec);
  cAutoLock Lock(mMapCritSec);

  mMap = MapToCopy.mMap;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	Insert
 *
 * Arguments: cKey key [in],
 *            cElement element [in],
 *
 * Returns:	true:   Success
 *          false:  An element with the same key is already in map.
 *
 * Exceptions:
 *
 * Description:	Inserts an element associated with key.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::Insert(cKey key, cElement element)
{
  bool success = false;
  tInstMap::iterator theIterator;
  pair<tInstMap::iterator, bool> retVal;

  mMapCritSec.Acquire();

  theIterator = mMap.find(key);
  if(theIterator != mMap.end())
  {
    // Already in map.
  }
  else
  {
    retVal = mMap.insert(tInstMap::value_type(key, element));
    success = retVal.second;
  }

  mMapCritSec.Release();

  return success;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	Replace
 *
 * Arguments: cKey key [in],
 *            cElement element [in],
 *            bool bAlwaysInsert [in],  true: remove and always insert the element.
 *                                      false: insert only if already in map.
 *            bool* pbRequestRemoved [out], true:   the element was removed.
 *                                          false:  the request was not
 *                                                  previously in the map.
 *
 * Returns:	true:   The element was inserted.
 *          false:  The element could not be inserted.
 *
 * Exceptions:
 *
 * Description:	If the key is in map, replaces the element value.
 *    If it is not in map, inserts the element associated with key only if
 *    bAlwaysInsert is true.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::Replace(cKey key, cElement element, bool bAlwaysInsert, bool* pbRequestRemoved)
{
  tInstMap::iterator theIterator;
  pair<tInstMap::iterator, bool> retVal;
  bool bSuccess = false;

  mMapCritSec.Acquire();

  if(pbRequestRemoved != NULL)
  {
    *pbRequestRemoved = false;
  }

  theIterator = mMap.find(key);

  if(theIterator != mMap.end())
  {
    // The key is already in map.
	int numberOfElementRemove = mMap.erase(key);

    if(numberOfElementRemove > 0)
    {
      bSuccess = true;
      if(pbRequestRemoved != NULL)
      {
        *pbRequestRemoved = true;
      }
    }
  }

  if(bSuccess || bAlwaysInsert)
  {
    retVal = mMap.insert(tInstMap::value_type(key, element));
    bSuccess = retVal.second;
  }

  mMapCritSec.Release();

  return bSuccess;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	Remove
 *
 * Arguments: cKey key [in],
 *
 * Returns:	true:   Success
 *          false:  Element could not be removed.
 *
 * Exceptions:
 *
 * Description:	Remove the element associated with key.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::Remove(cKey key, cElement *element)
{
  bool success = false;
  tInstMap::iterator theIterator;

  mMapCritSec.Acquire();

  theIterator = mMap.find(key);
  if(theIterator != mMap.end())
  {
    if(element != NULL)
	{
		*element = (*theIterator).second;
	}
	mMap.erase(key);
    success = true;
  }

  mMapCritSec.Release();

  return success;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	Find
 *
 * Arguments: cKey key [in],
 *            cElement *element [out],
 *
 * Returns:	true:   Element found.
 *          false:  Element NOT found.
 *
 * Exceptions:
 *
 * Description:	Find the element associated with key.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::Find(cKey key, cElement *element)
{
  bool success = false;

  tInstMap::iterator theIterator;

  mMapCritSec.Acquire();

  theIterator = mMap.find(key);
  if(theIterator != mMap.end())
  {
    if(element != NULL)
	{
		*element = (*theIterator).second;
	}
    success = true;
  }
  mMapCritSec.Release();
  return success;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	FindElement
 *
 * Arguments: cElement element [in],
 *            cKey &key [out],
 *
 * Returns:	true:   Found.
 *          false:  NOT found.
 *
 * Exceptions:
 *
 * Description:	If found, returns the key associated with the first matching
 *    element found.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::FindElement(cElement element, cKey &key)
{
  bool found = false;

  tInstMap::iterator theIterator;

  mMapCritSec.Acquire();

  theIterator = mMap.begin();
  while(theIterator != mMap.end())
  {
    if(theIterator->second == element)
    {
      key = theIterator->first;
      found = true;
    }
    theIterator++;
  }
  mMapCritSec.Release();

  return found;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	GetFirst
 *
 * Arguments: cKey &key [out],
 *            cElement *element [out],
 *
 * Returns:	true:   Element found.
 *          false:  No Element found.
 *
 * Exceptions:
 *
 * Description:	Returns the first element in map.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::GetFirst(cKey &key, cElement *element)
{
  bool success = false;
  mMapCritSec.Acquire();

  tInstMap::iterator theIterator = mMap.begin();
  if(theIterator != mMap.end() )
  {
    key = (*theIterator).first;
    if(element != NULL)
    {
      *element = (*theIterator).second;
    }
    success = true;
  }
  mMapCritSec.Release();
  return success;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	GetNext
 *
 * Arguments: cKey key [in], 
 *            cKey &nextKey [out], 
 *            cElement *element [out], 
 *
 * Returns:	true:   Element found.
 *          false:  No Element found.
 *
 * Exceptions:
 *
 * Description:	Returns the next element in map.
 *
 * Notes:
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::GetNext(cKey Key, cKey &nextKey, cElement *nextElement)
{
  bool success = false;
  mMapCritSec.Acquire();

  tInstMap::iterator theIterator = mMap.find(Key);

  if(theIterator != mMap.end())
  {
    // Element is in map.
    theIterator++;
    if(theIterator != mMap.end())
    {
      nextKey = (*theIterator).first;
      if(nextElement != NULL)
      {
        *nextElement = (*theIterator).second;
      }
      success = true;
    }
  }
  mMapCritSec.Release();
  return success;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	IsEmpty
 *
 * Arguments:	
 *
 * Returns:	true:   Map is empty.
 *          false:  Map is NOT empty.
 *
 * Exceptions:
 *
 * Description:	Check if the map is empty.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
bool cInstanceMap<cKey, cElement>::IsEmpty()
{
  bool empty;

  mMapCritSec.Acquire();

  empty = mMap.empty();

  mMapCritSec.Release();

  return empty;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Name:	Clear
 *
 * Arguments:	
 *
 * Returns:	
 *
 * Exceptions:
 *
 * Description:	Empty the map.
 *
 * Notes:	
 *
 *-------------------------------------------------------------------------*/

template<class cKey, class cElement>
void cInstanceMap<cKey, cElement>::Clear()
{
  mMapCritSec.Acquire();

  mMap.clear();

  mMapCritSec.Release();
}

#endif
