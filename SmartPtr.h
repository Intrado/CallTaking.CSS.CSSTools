/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 1999.
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    username     date created:    yyyy/mm/dd
 * %derived_by:    sroy %       %date_modified:   2001/09/24 14:01:48 %
 *
 * %name:          SmartPtr.h %
 * %version:       2 %
 *
 * References:
 *
 *     list of relevant design documents
 *
 * Description:
 *
 *     description
 *
 ***************************************************************************/

//*********************************************************************************
//																																								
//	SmartPtr																																			
//																																								
//	Author: Dany Papillon, ext. 2569          21/02/99														
//																																								
//	USAGE:	derive your base class from RefCount and															
//					declare your pointers like this:																			
//																																								
//							SmartPtr<TYPE> ptr;																								
//							SmartPtr<TYPE> ptr = new TYPE; 
//							SmartPtr<TYPE> ptr = TYPE_POINTER;
//							SmartPtr<TYPE> ptr = SMARTPTR_OBJECT;
//
//					From that point on, you use this object the same 
//					way you would use any pointer, and it will adopt the
//					behavior of the contained pointer type.
//
//	NOTE:	This object is capable of doing any legal cast by itself
//				(inculding what would normally require an explicit cast)
//
//				POINTER CASTING... Smart pointers have the ability to cast
//				themselfves in the right type. So, if you do something 
//				like:
//							SmartPtr<base_class> p1 = ...;
//							SmartPtr<class>			 p2;
//							p2 = p1;
//				you won't get any compiler error. The SmartPtr will 
//				interpret this as "p2 = (SmartPtr<class>) p1;" 
//				presenting the same dangers as explicit casts with
//				normal pointers. The next point can help you with this.
//
//				"UNSAFE_CASTING"... This is meant to help you 
//				find unsafe casting. If an assignment operation causes
//				a situation where the object pointer may be unsafe.
//				Doing an explicit cast on an object disables this 
//				feature for the mentionned object and your on your own.
//
//				DON'T DELETE POINTERS MANUALLY, let them do their thing.
//				They know best when to delete themselves through
//				reference counting.
//
//				"SMARTPTR_OBJECT = NULL" will make the internal ptr
//				equal to NULL and lower the refcount by one. This usage
//				is perfectly fine.
//
//				USAGE OVERHEAD... Be aware that this is still an object
//				and passing it as a function parameter, or making 
//				assignment operations on it will call the constructor and 
//				destructor of this object, thereby causing a small overhead.
//
//
//				CAREFULL... 
//
//					if you happen to pass "this" to a function call
//					and the parametter is a SmartPtr, "this" could be deleted if 
//					it's not stored in a smartPtr.
//
//					SmartPtr<T1> p1 = OBJECT;
//					SmartPtr<T1> p2 = p1;      -> OK.
//					T1 *p3 = p1;               -> RefCount not incremented. 
//																				If p1 & p2 go out of scope
//																				before we are done with p3,
//																				p3 becomes invalid.
//
//					Example of an explicit cast to make a function call. The casting 
//					operation should be done on the internal pointer like in the 
//					following example.
//
//					class CObject : public CBaseObject
//					{
//						...
//						void Function();
//					};
//					void fnc()
//					{
//						SmartPtr<CBaseObject> PBaseObject = ...;
//						...
//						((Object*)PBbaseObject))->Function();									-> NOT OK
//						((Object*)PBbaseObject.GetIntenalPtr()))->Function();  -> OK
//					}
//						
//*********************************************************************************

#ifndef SmartPtr_h
#define SmartPtr_h

// unsafe assignment detection
//define UNSAFE_CASTING

#include "assert.h"
#define IsType(ref,type) (dynamic_cast<type>(ref.GetInternalPtr()) != NULL)
#define IsTypeOrNull(ref,type) ( (ref.GetInternalPtr()) == NULL) || (dynamic_cast<type>(ref.GetInternalPtr()) != NULL) )

//*************************************************************
// Refcount
//*************************************************************
class RefCount 
{
	LONG crefs;

public:

	RefCount(void) 
  { 
    crefs = 0; 
  }

  virtual ~RefCount(){}

	virtual void upcount(void *source = NULL)
  { 
    InterlockedIncrement(&crefs); 
  }

  virtual void downcount(void *source = NULL)
  {
	  if(InterlockedDecrement(&crefs) == 0)
	  {
      selfDestroy();
	  }
  }

  LONG getCount() 
  {
    return crefs;
  }
  
private:

  // Can be overloaded for destruction when used with a dll.
  virtual void selfDestroy()
  {  
    delete this ; 
  }

};

//*************************************************************
// BaseSmartPtr
//*************************************************************
template <class T> class BaseSmartPtr 
{
protected:

	T*p;
	
public:

	BaseSmartPtr() { p = NULL; }
	
	BaseSmartPtr(T*p_) : p(p_) 
	{
	  if(p) 
	    p->upcount(this);
	}
	
	BaseSmartPtr(const BaseSmartPtr<T> &p_) 
	{
		p = p_.p; 
		if(p) 
			p->upcount(this);
	}
	
	virtual ~BaseSmartPtr() 
	{
		if(p) 
			p->downcount(this);
	}
	
	operator T*() { return p; }
	
	// different types of smartPtr may contain the same object.
	template <class NewType>
	bool operator==(const BaseSmartPtr<NewType> &p_) const 
	{ 
		return p == dynamic_cast<T*>(((volatile BaseSmartPtr<NewType>) p_).GetInternalPtr()); 
	}

	// different types of smartPtr may contain the same object.
	template <class NewType>
	bool operator!=(const BaseSmartPtr<NewType> &p_) const 
	{ 
		return p != dynamic_cast<T*>(((volatile BaseSmartPtr<NewType>) p_).GetInternalPtr()); 
	}

	// auto-cast pointer assignment.
	template <class NewType>
	BaseSmartPtr& operator=(const BaseSmartPtr<NewType> &p_)
  {
		return operator=(p_.GetInternalPtr());
	}

	// Retrieves the pointer within the SmartPtr.
	T*GetInternalPtr() { return p; }
};

//*************************************************************
// SmartPtr
//*************************************************************
template <class T> class SmartPtr : public BaseSmartPtr<T> 
{
public:
	
	SmartPtr(){}
	SmartPtr(T*p_) : BaseSmartPtr<T>(p_) {}
	SmartPtr(const SmartPtr<T> &p_) : BaseSmartPtr<T>(p_){}
	
	// casting copy constructor.
	template<class NewType>
	SmartPtr(BaseSmartPtr<NewType> &p_) : BaseSmartPtr<T>(dynamic_cast<T*>(p_.GetInternalPtr()))
	{
		#ifdef UNSAFE_CASTING
			assert(IsTypeOrNull(((volatile BaseSmartPtr<NewType>) p_),T*));
		#endif
	}

	virtual ~SmartPtr(void) {}

  operator T*()  { return p; }
  T& operator*() { return *p; }
	T*operator->() const { return p; }

	bool operator==(const SmartPtr<T> &p_) const { return p == p_.p; }

	// different types of smartPtr may contain the same object.
	template <class NewType>
	bool operator==(const BaseSmartPtr<NewType> &p_) const { return(BaseSmartPtr<T>)p == p_; }

	// if compared with NULL or other pointer
	bool operator==(const T*t) const { return p == t; }
	
	bool operator!=(const SmartPtr<T> &p_) const { return p != p_.p; }

	// different types of smartPtr may contain the same object.
	template <class NewType>
	bool operator!=(const BaseSmartPtr<NewType> &p_) const { return(BaseSmartPtr<T>)p != p_; }

	// if compared with NULL or other pointer.
	bool operator!=(const T*t) const { return p != t; }
	
	bool operator<(const SmartPtr<T> &p_) const { return p < p_.p; }

	// assignment that do not require to be casted.
  SmartPtr& operator=(const SmartPtr<T> &p_) { return operator=(((volatile SmartPtr<T>) p_).GetInternalPtr()); }

	// casting pointer assignment.
	template <class NewType>
	SmartPtr& operator=(const BaseSmartPtr<NewType> &p_)
  {
		#ifdef UNSAFE_CASTING
			assert(IsTypeOrNull(((volatile BaseSmartPtr<NewType>) p_),T*));
		#endif
		return operator=(((volatile BaseSmartPtr<NewType>) p_).GetInternalPtr());
	}

	// copy the actual object pointer.
	template <class NewType>
	SmartPtr& operator=(NewType*p_) 
	{
		if(dynamic_cast<T*>(p) != dynamic_cast<T*>(p_))
		{
			if(p)
				p->downcount(this); 
			p = dynamic_cast<T*>(p_); 
			if(p)
				p->upcount(this); 
		}
		return *this;
  }
	
	template<class NewType>
	operator SmartPtr<NewType>()
	{
		return SmartPtr<NewType> (p);
	}
	
	// Special case... NULL assignment.
  int operator=(int p_)
	{
		assert(!p_);
		if(p)
			p->downcount(this);
		p = NULL;
		return NULL;
	}
};

#endif // SmartPtr_h
