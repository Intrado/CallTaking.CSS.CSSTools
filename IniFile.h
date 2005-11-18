#ifndef __IniFile_INCLUDED__
#define __IniFile_INCLUDED__
#pragma warning(disable:4786)
#include <string>
/* ************************************************************** */
/*  Copyright (C) 1996, Positron Industries, Public Safety Division. */
/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description */
/* *************************************************************************** */
/*  The Objet Config Implementation */
/*  Author: Rejean Belanger, 5 Juin 1997 */
/* *************************************************************************** */
#include "CSSTools.h"
namespace IniFile
{
  static const char *moduleName = "IniFile";
  CSSTOOLS_API int GetPrivateProfileStr (std::string lpAppName, std::string lpKeyName, std::string lpDefault, void *lpReturnString, int nSize, std::string lpFileName);
  CSSTOOLS_API int PathSearch (std::string lpPath, std::string lpFileName, std::string lpExtension, int nBufferLength, void *lpBuffer, void *lpFilePart);
  CSSTOOLS_API int GetPrivateProfileI (std::string lpAppName, std::string lpKeyName, int nDefault, std::string lpFileName);
  CSSTOOLS_API int GetPrivateProfileSect (std::string lpAppName, void *lpReturnedString, int nSize, std::string lpFileName);
  CSSTOOLS_API int GetSysDirectory (void *lpBuffer, int nSize);
  CSSTOOLS_API int WritePrivateProfileStr (std::string lpAppName, std::string lpKeyName, std::string lpString, std::string lpFileName);
  CSSTOOLS_API int CopyFile (std::string FromFile, std::string ToFile, bool bFailIfExists);
};
#endif
