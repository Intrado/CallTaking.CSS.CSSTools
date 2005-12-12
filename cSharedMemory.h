// cSharedMemory.h: interface for the cSharedMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSHAREDMEMORY_H__54DD0D50_C69C_11D2_8039_00805F7DCE90__INCLUDED_)
#define AFX_CSHAREDMEMORY_H__54DD0D50_C69C_11D2_8039_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once

#endif // _MSC_VER > 1000

#include "CSSTools.h" 


#include "cLockableObject.h"
#include "cMutex.h"
#include <windows.h>

class CSSTOOLS_API cSharedMemory : public cLockableObject 
{
public:			  
      cSharedMemory( const char *object_name, DWORD size_in_bytes,LPSECURITY_ATTRIBUTES sa=NULL );
 	  virtual ~cSharedMemory();

	  // Propriete de l'objet
   
      void * Buffer( void );
	  bool IsCreated(void);
	  virtual bool Acquire(long TimeOut=INFINITE);
      virtual void Release();
	  virtual operator HANDLE(){return handle;};

 

private:

      // Don't allow canonical behavior (i.e. don't allow this class
      // to be passed by value)

//       cSharedMemory( const CSharedMemoryObject& ) {};
//       cSharedMemory& operator=( const CSharedMemoryObject& ) { return( *this ); };

protected:
	   
  cMutex Lock;

  const char *mp_SharedMemName;
  DWORD m_Size;

  void * m_Buffer;
  bool bCreated;
  LPSECURITY_ATTRIBUTES sa;
  PSECURITY_DESCRIPTOR sd;

  void InitSecurity();
  int Size();


  void   Close( void );



};

#endif // !defined(AFX_CSHAREDMEMORY_H__54DD0D50_C69C_11D2_8039_00805F7DCE90__INCLUDED_)
