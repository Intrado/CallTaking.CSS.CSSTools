#include "stdafx.h"
#include "ViperTools.h"
#include "Diag.h"
#include <sstream>

using namespace std;

double cViperTools::gViperVersion = 0;
int cViperTools::gViperServicePack = -1;
double cViperTools::gDefaultViperVersion = 3.0;
int cViperTools::gDefaultViperServicePack = 2;

string GetRegistryKey(string diagModuleName, string path, string value);

void cViperTools::SetDefaultViperVersion(double version, int servicePack)
{
  gDefaultViperVersion = version;
  gDefaultViperServicePack = servicePack;
}

double cViperTools::GetViperVersion(string diagModuleName)
{  
  if (diagModuleName.size() == 0)
  {
    diagModuleName = "ViperTools";
  }

  if (gViperVersion == 0)
  {
    string version = GetRegistryKey(diagModuleName, "Software\\Positron\\Pots", "Version");
    if (version.size() == 0)
    {
      version = GetRegistryKey(diagModuleName, "Software\\Positron Industries Inc.\\Viper", "Version");
    }
  
    if (version.size() > 0)
    {
      istringstream iss;
      iss.str(version.c_str());
      iss >> gViperVersion;
    }
  }
  
  if (gViperVersion == 0)
  {
    // Return default version
    return gDefaultViperVersion;
  }
  else
  {
    return gViperVersion;
  }
}

// Read the Viper service pack registry key and return the value
int cViperTools::GetViperServicePack(std::string diagModuleName)
{
  if (diagModuleName.size() == 0)
  {
    diagModuleName = "ViperTools";
  }

  if (gViperServicePack == -1)
  {
    string version = GetRegistryKey(diagModuleName, "Software\\Positron\\Pots", "SP");
    if (version.size() == 0)
    {
      version = GetRegistryKey(diagModuleName, "Software\\Positron Industries Inc.\\Viper", "SP");
    }
  
    if (version.size() > 0)
    {
      istringstream iss;
      iss.str(version.c_str());
      iss >> gViperServicePack;
    }
  }
  
  if (gViperServicePack == -1)
  {
    gViperServicePack = 0;  // prevent multiple registry check if the key not found
    // Return default SP
    return gDefaultViperServicePack;
  }
  else
  {
    return gViperServicePack;
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