// cSharedList.h: interface for the cSharedList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSHAREDLIST_H__A243F640_C6B6_11D2_8039_00805F7DCE90__INCLUDED_)
#define AFX_CSHAREDLIST_H__A243F640_C6B6_11D2_8039_00805F7DCE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CSSTools.h"

#include "cSharedMemory.h"
#include "cMutex.h"
#include "cSemaphore.h"
#include <windows.h>
//
// Attention , interet particulierement pour interfacer avec TShareList de Delphi
// Voir Projet Cad.dpr (de Arie Amsaleme)

// Cette classe implemente directement l'implementation de la TShareList (en Delphi)
// Voir projet Cad.dpr
// Cette liste proteger permet de partage des messages qui sont emis d'une application
// (producteur) et qui sont lu par une autre application (consommateur).
// La liste est implementer dans un tampon circulaire, et les messages de la liste sont
// de taille fixe.


class CSSTOOLS_API cSharedList : public cSharedMemory  
{
public:
 	cSharedList(const char *pname,WORD NbRec, WORD recSize);
	virtual ~cSharedList();
	  
	// Enregistrement et des-enregistrement des clients
	bool RegisterClient (const char *pclient);
	bool UnRegisterClient (const char *pclient);

    bool put( void * evPtr);
    void* get(void);
	void flush (void);
	WORD getNbClient(void);

protected:

	WORD getIndexPtrWr(void);
	SetIndexPtrWr (WORD value);
	WORD getIndexPtrRd(void);
	SetIndexPtrRd (WORD value);
 	WORD getRecSize(void);
	WORD getNbRec(void);
 
private:
	
	cMutex *plock;
    cSemaphore *psema;

private:

    InitSecurity(void);


};

#endif // !defined(AFX_CSHAREDLIST_H__A243F640_C6B6_11D2_8039_00805F7DCE90__INCLUDED_)
