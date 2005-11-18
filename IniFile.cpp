#include <windows.h>
#include <WinNT.h>
#include "WinBase.h"
#include "IniFile.h"
#include "ToStr.h"
#include "CSS.h"

/*  All rights reserved. */
/*  See the file COPYRIGHT for a full description. */
/*  Author: Rejean Belanger, 6 Juin 1997 */
/* --------------------------------------------------------------------------------------------
   Fonction Win32 GetProfileString() 

   DWORD GetProfileString (
   LPCTSTR lpAppName,   // address of section name 
   LPCTSTR lpKeyName,   // address of key name 
   LPCTSTR lpDefault,   // address of default string 
   LPTSTR lpReturnedString, // address of destination buffer 
   DWORD nSize  // size of destination buffer 
   );

   ------------------------------------------------------------------------------------------- */
CSSTOOLS_API int IniFile::
GetPrivateProfileStr(string lpAppName, string lpKeyName, string lpDefault, void *lpReturnString, int nSize, string lpFileName)
{
  int WinResult;
/*  LPCSTR tlpAppName;
  LPCSTR tlpKeyName;
  LPCSTR tlpDefault;
  LPCSTR tlpFileName;
  tlpAppName = lpAppName.c_str(); //T2S
  tlpKeyName = lpKeyName.c_str(); //T2S
  tlpDefault = lpDefault.c_str(); //T2S
  tlpFileName = lpFileName.c_str(); //T2S
*/
//  WinResult = GetPrivateProfileString(tlpAppName, tlpKeyName, tlpDefault, (char *)lpReturnString, nSize, tlpFileName);
  WinResult = GetPrivateProfileString(lpAppName.c_str(), lpKeyName.c_str(), lpDefault.c_str(), (char *)lpReturnString, nSize, lpFileName.c_str());
  return WinResult;
}

CSSTOOLS_API int IniFile::
PathSearch(string lpPath, string lpFileName, string lpExtension, int nBufferLength, void *lpBuffer, void *lpFilePart)
{
  int WinResult;
/* --------------------------------------------------------------------------------------------
   Fonction Win32 SearchPath() 
   DWORD SearchPath(
   LPCTSTR lpPath,  // address of search path 
   LPCTSTR lpFileName,  // address of filename 
   LPCTSTR lpExtension, // address of extension 
   DWORD nBufferLength, // size, in characters, of buffer 
   LPTSTR lpBuffer, // address of buffer for found filename 
   LPTSTR *lpFilePart   // address of pointer to file component 
   );   
   ---------------------------------------------------------------------------------------------- */
  LPCSTR tlpPath;
  LPCSTR tlpFileName;
  LPCSTR tlpExtension;

  if (lpPath.size() > 0)
    tlpPath=lpPath.c_str();
  else
    tlpPath = NULL;

  tlpFileName=lpFileName.c_str();

  if (lpExtension.size() > 0)
    tlpExtension=lpExtension.c_str();
  else
    tlpExtension = NULL;

  WinResult = SearchPath(tlpPath, tlpFileName, tlpExtension, nBufferLength, (char *)lpBuffer, (char **)&lpFilePart);

  return WinResult;
}

CSSTOOLS_API int IniFile::
GetPrivateProfileI(string lpAppName, string lpKeyName, int nDefault, string lpFileName)
{
  int WinResult;
/* --------------------------------------------------------------------------------------------
   Fonction Win32 GetPrivateProfileInt() 
   UINT GetPrivateProfileInt(
   LPCTSTR lpAppName,   // address of section name
   LPCTSTR lpKeyName,   // address of key name
   INT nDefault,    // return value if key name is not found
   LPCTSTR lpFileName   // address of initialization filename
   );   
   -------------------------------------------------------------------------------------------- */
/*  LPCSTR tlpAppName;
  LPCSTR tlpKeyName;
  LPCSTR tlpFileName;
  tlpAppName = lpAppName.c_str(); //T2S
  tlpKeyName = lpKeyName.c_str(); //T2S
  tlpFileName = lpFileName.c_str(); //T2S
//  WinResult = GetPrivateProfileInt(tlpAppName, tlpKeyName, nDefault, tlpFileName);
*/
  WinResult = GetPrivateProfileInt(lpAppName.c_str(), lpKeyName.c_str(), nDefault, lpFileName.c_str());
  if (CSSToolsTraceOn)
  {
	  DiagTrace(moduleName, "GetPrivateProfileInt", "Value: ",ToStr(WinResult));
  };
  return WinResult;
}

CSSTOOLS_API int IniFile::
GetPrivateProfileSect(string lpAppName, void *lpReturnedString, int nSize, string lpFileName)
{
  int WinResult;
/* --------------------------------------------------------------------------------------------
   Fonction Win32 GetPrivateProfileSection()
   DWORD GetPrivateProfileSection(
   LPCTSTR lpAppName,   // address of section name 
   LPTSTR lpReturnedString, // address of return buffer 
   DWORD nSize, // size of return buffer 
   LPCTSTR lpFileName   // address of initialization filename  
   );   
   -------------------------------------------------------------------------------------------- */
/*  LPCSTR tlpAppName;
  LPCSTR tlpFileName;
  tlpAppName = lpAppName.c_str(); //T2S
  tlpFileName = lpFileName.c_str(); //T2S
*/
//  WinResult = GetPrivateProfileSection(tlpAppName, (char *)lpReturnedString, nSize, tlpFileName);
  WinResult = GetPrivateProfileSection(lpAppName.c_str(), (char *)lpReturnedString, nSize, lpFileName.c_str());
  return WinResult;
}

CSSTOOLS_API int IniFile::
GetSysDirectory(void *lpBuffer, int nSize)
{
  int WinResult;
  WinResult = GetSystemDirectory((char *)lpBuffer, nSize);
  return WinResult;
}

CSSTOOLS_API int IniFile::
WritePrivateProfileStr(string lpAppName, string lpKeyName, string lpString, string lpFileName)
{
  int WinResult;
/*  pointer to section name */
/*  pointer to key name */
/*  pointer to string to add */
/*  pointer to initialization filename */
/*  LPCSTR tlpAppName;
  LPCSTR tlpKeyName;
  LPCSTR tlpString;
  LPCSTR tlpFileName;
  tlpAppName = lpAppName.c_str(); //T2S
  tlpKeyName = lpKeyName.c_str(); //T2S
  tlpString = lpString.c_str(); //T2S
  tlpFileName = lpFileName.c_str(); //T2S
*/
//  WinResult = WritePrivateProfileString(tlpAppName, tlpKeyName, tlpString, tlpFileName);
  WinResult = WritePrivateProfileString(lpAppName.c_str(), lpKeyName.c_str(), lpString.c_str(), lpFileName.c_str());
  return WinResult;
}

CSSTOOLS_API int IniFile::
CopyFile(string FromFile, string ToFile, bool bFailIfExists)
{
  LPCSTR tFromFile;
  LPCSTR tToFile;
  int WinResult;
  tFromFile = FromFile.c_str(); //T2S
  tToFile = ToFile.c_str(); //T2S
  WinResult = ::CopyFile(tFromFile, tToFile, bFailIfExists);
  return WinResult;
}

namespace IniFile
{
  class IniFile_static
  {
	public:
	  IniFile_static()
	{
	};
  };
  static IniFile_static IniFile_static_instance;
}
