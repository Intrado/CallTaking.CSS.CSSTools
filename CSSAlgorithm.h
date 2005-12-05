/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2001.
 * ALL RIGHTS RESERVED.
 *
 *  created by:    fslight       date created:    2001/02/23
 * %derived_by:   fslight %       %date_modified:  2002/01/16 09:46:05 %
 *
 * %name:         CSSAlgorithm.h %
 * %version:      9 %
 *
 * Description:
 *
 *    STL-like algorithms
 *
 ***************************************************************************/

#ifndef __CSSAlgorithm_h_INCLUDED__
#define __CSSAlgorithm_h_INCLUDED__


template<class _II, class _OI, class _Pr> _OI copy_if(_II first, _II last, _OI out, _Pr _P)
{
  while (first != last)
  {
    if (_P(*first)) *out++ = *first;
    ++first;
  }
  return out;
}


/* Inspired from remove_copy_if (see <algorithm>) but modified to work with maps and to move instead of copy.
   Elements for which pred() is true are removed from src, and added to pOut if it is provided.
   _I is the container type, _II is _I's iterator type, _Pr is the predicate type. */
template<class _I, class _II, class _Pr> void map_move_if(_I &src, _II first, _II last, _I *pOut, _Pr pred)
{
  for (; first != last;)
		if (pred(*first))
    {
      if (pOut) (*pOut)[first->first] = first->second;
      first = src.erase(first);
    }
    else
      ++first;
}


/* Entries in the map for which the value is equal to _V are removed from src and added to pOut if
   it is provided. */
template<class _I, class _II, class _Ty> void map_move(_I &src, _II first, _II last, _I *pOut, const _Ty& _V)
{
  for (; first != last;)
		if (first->second == _V)
    {
      if (pOut) (*pOut)[first->first] = first->second;
      first = src.erase(first);
    }
    else
      ++first;
}


// Copy one map into another (key and value types of both maps must be compatible).
template<class _I, class _II> void map_copy(_II first, _II last, _I &out)
{
  while (first != last)
  {
    out[first->first] = first->second;
    ++first;
  }
}


// Copy a map's values into another container type (value type of both containers must be compatible)
template<class _II, class _OI> inline _OI map_copy_value(_II _F, _II _L, _OI _X)
{
  for (; _F != _L; ++_X, ++_F)
		*_X = _F->second;
	return (_X);
}


// Like for_each, but works with maps (for a map<A, B>, uses the B member).
template<class _II, class _Fn> _Fn map_for_each(_II _F, _II _L, _Fn _Op)
{
  for (; _F != _L; ++_F)
    _Op(_F->second);
  return (_Op);
}


// Like find, but works with maps (for a map<A, B>, uses the B member).
template<class _II, class _Ty> inline _II map_find(_II _F, _II _L, const _Ty& _V)
{
  for (; _F != _L; ++_F)
    if (_F->second == _V)
      break;
  return (_F);
}


// Like find_if, but works with maps (for a map<A, B>, uses the B member).
template<class _II, class _Pr> inline _II map_find_if(_II _F, _II _L, _Pr _P)
{
  for (; _F != _L; ++_F)
    if (_P(_F->second))
      break;
  return (_F);
}


// Like transform, but works with a map as input (_II can be a map<A, B>::iterator, _Uop will be called on iterator->second).
template<class _II, class _OI, class _Uop> inline _OI map_transform(_II _F, _II _L, _OI _X, _Uop _U)
{
  for (; _F != _L; ++_F, ++_X)
    *_X = _U(_F->second);
  return (_X);
}

#endif
