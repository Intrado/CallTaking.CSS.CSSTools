#include "stdafx.h"
#include "ViperTools.h"
#include "Diag.h"
#include <sstream>

using namespace std;

double cViperTools::mViperVersion = 0;

string GetRegistryKey(string diagModuleName, string path, string value);

double cViperTools::GetViperVersion(string diagModuleName)
{  
  if (diagModuleName.size() == 0)
  {
    diagModuleName = "ViperTools";
  }

  if (mViperVersion == 0)
  {
    string version = GetRegistryKey(diagModuleName, "Software\\Positron\\Pots", "Version");
    if (version.size() == 0)
    {
      string version = GetRegistryKey(diagModuleName, "Software\\Positron\\Positron Industries Inc.\\Viper", "Version");
    }
  
    if (version.size() > 0)
    {
      istringstream iss;
      iss.str(version.c_str());
      iss >> mViperVersion;
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


string GetRegistryKey(string diagModuleName, string path, string value)
{
  HKEY hKey;
  string returnValue;
  LONG r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey);
  if (r != ERROR_SUCCESS)
  {        
    DiagTrace(diagModuleName, "GetRegistryKey", "Can't open " + path + " registry key.");
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
        DiagTrace(diagModuleName, "GetRegistryKey", "Value [" + value + "] in Viper key not found");
      }
      else
      {
        DiagTrace(diagModuleName, "GetRegistryKey", "Can't open " + path + "\\" + value + " Version registry key.");
      }
    }
    else
    {
      returnValue = val;
    }
   
    RegCloseKey(hKey);
  }
  return returnValue;
}