/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000.
 * ALL RIGHTS RESERVED.
 *
 *  created by:    fslight       date created:    2000/03/29
 * %derived_by:   fslight %       %date_modified:  %
 *
 * %name:         LockableStlContainers.h %
 * %version:      5 %
 *
 * Description:
 *
 *    Lockable versions of popular STL containers
 *
 ***************************************************************************/

#ifndef __LockableStlContainers_h_INCLUDED__
#define __LockableStlContainers_h_INCLUDED__

#pragma warning(disable:4786 4503)
#include <map>
#include <set>
#include <list>
#include <deque>
#include "cCritSec.h"

template<class Key, class T, class Pred = std::less<Key>, class A = std::allocator<T> >
class cLockableMap: public cCritSec, public std::map<Key, T, Pred, A>
{
public:
};

template<class Key, class Pred = std::less<Key>, class A = std::allocator<Key> >
class cLockableSet: public cCritSec, public std::set<Key, Pred, A>
{
public:
};

template<class T, class A = std::allocator<T> >
class cLockableList: public cCritSec, public std::list<T, A>
{
public:
};

template<class T, class A = std::allocator<T> >
class cLockableDeque: public cCritSec, public std::deque<T, A>
{
public:
};

template<class Key, class T, class Pred = std::less<Key>, class A = std::allocator<T> >
class cLockableMultiMap: public cCritSec, public std::multimap<Key, T, Pred, A>
{
public:
};

template<class Key, class Pred = std::less<Key>, class A = std::allocator<Key> >
class cLockableMultiSet: public cCritSec, public std::multiset<Key, Pred, A>
{
public:
};

#endif
