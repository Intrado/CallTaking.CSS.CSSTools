// cSharedMemory.cpp: implementation of the cSharedMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "cSharedMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



cSharedMemory::~cSharedMemory()
{  
   Close();

   handle  = INVALID_HANDLE_VALUE;
   m_Buffer   = NULL;
   m_Size      = 0;
   bCreated = false;

}

 void cSharedMemory::InitSecurity(void)
{
  sa = new (SECURITY_ATTRIBUTES);
  sd = new (PSECURITY_DESCRIPTOR);
  sa->nLength= sizeof(sa);
  sa->lpSecurityDescriptor= sd;

  InitializeSecurityDescriptor(sa->lpSecurityDescriptor,1);
  SetSecurityDescriptorDacl (sa->lpSecurityDescriptor,true,NULL,false);
}

cSharedMemory::cSharedMemory(const char *pobject_name, DWORD size_in_bytes,LPSECURITY_ATTRIBUTES sa)
{

   handle  = INVALID_HANDLE_VALUE;
   m_Buffer   = NULL;
   m_Size      = 0;
   bCreated = false;

   InitSecurity();

   handle = ::CreateFileMapping( reinterpret_cast< HANDLE >( 0xFFFFFFFF ),
                                      sa,
                                      PAGE_READWRITE,
                                      0,
                                      size_in_bytes,
                                      pobject_name);

   if ( handle == NULL )
   {
      handle = INVALID_HANDLE_VALUE;
      bCreated=false;
   }

   m_Size = size_in_bytes;
   mp_SharedMemName= pobject_name;

   m_Buffer = ::MapViewOfFile( handle, FILE_MAP_ALL_ACCESS, 0, 0, m_Size );

   if ( m_Buffer == NULL )
   {

      Close();
      bCreated=false;
   }

   bCreated=true;

}



bool cSharedMemory::Acquire(long TimeOut)
{
	return Lock.Acquire(TimeOut);

}

void cSharedMemory::Release()
{
	Lock.Release();
}


void cSharedMemory::Close( void )
{
   if ( handle == INVALID_HANDLE_VALUE )
   {
      return;
   }

   if ( m_Buffer != NULL )
   {
      if ( ::UnmapViewOfFile( m_Buffer ) == FALSE )
      {
      }

      m_Buffer = NULL;
   }

   if ( ::CloseHandle( handle ) == FALSE )
   {
   }

   handle = INVALID_HANDLE_VALUE;
   m_Size = 0;
   mp_SharedMemName="";
}

     


int cSharedMemory::Size()
{
	return m_Size;
}

void * cSharedMemory::Buffer()
{
	return m_Buffer;
}

bool cSharedMemory::IsCreated(void)
{
	return bCreated;
}
