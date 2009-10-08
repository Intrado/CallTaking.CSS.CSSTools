#include "stdafx.h"
#include "ViperTools.h"
#include "Diag.h"
#include <sstream>

using namespace std;

double cViperTools::mViperVersion = 0;

double cViperTools::GetViperVersion(string diagModuleName)
{  
  if (diagModuleName.size() == 0)
  {
    diagModuleName = "ViperTools";
  }

  if (mViperVersion == 0)
  {
    HKEY hKey;
    LONG r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Positron Industries Inc.\\Viper", 0, KEY_READ, &hKey);
    if (r != ERROR_SUCCESS)
    {        
      DiagWarning(diagModuleName, "GetViperVersion", "Can't open Viper registry key");
    }
    else
    {
      char val[1024];
      DWORD size = 1024;
      r = RegQueryValueEx(hKey, "Version", 0, 0, (unsigned char*)val, &size);
      if (r != ERROR_SUCCESS)
      {
        if (r == ERROR_FILE_NOT_FOUND)
        {
          DiagTrace(diagModuleName, "GetViperVersion", "Value Version in Viper key not found");
        }
        else
        {
          DiagWarning(diagModuleName, "GetViperVersion", "Error while trying to extract value Version in Viper key");
        }
      }
      else
      {
        istringstream iss;
        iss.str(val);
        iss >> mViperVersion;
      }
      RegCloseKey(hKey);
    }
  }

  // Return viper version, 1.2 if not found
  if (mViperVersion == 0)
  {
    return 1.2;
  }
  else
  {
    return mViperVersion;
  }
}


