#include "stdafx.h"
#include "ViperTools.h"
#include "Diag.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

double cViperTools::gViperVersion = 0;
int cViperTools::gViperServicePack = -1;
double cViperTools::gDefaultViperVersion = 3.0;
int cViperTools::gDefaultViperServicePack = 2;
bool cViperTools::gViperKBChecked = false;
cViperTools::tStringMap cViperTools::gViperKBs;

string GetRegistryKey(string diagModuleName, string path, string value);
bool GetAllRegistryKeySZValues(string diagModuleName, string path, cViperTools::tStringMap *valueMap);

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
    string sp = GetRegistryKey(diagModuleName, "Software\\Positron\\Pots", "SP");
    if (sp.size() == 0)
    {
      sp = GetRegistryKey(diagModuleName, "Software\\Positron Industries Inc.\\Viper", "SP");
    }
    if (sp.size() == 0)
    {
      string version = GetRegistryKey(diagModuleName, "Software\\Positron\\Pots", "Version");

      // We will try to determine the most recent sp applied by checking in the registery if this sp was applied
      bool checkfornewersp = true;
      int count = 0;
      do
      {
         // Create the key to look for(for example 4.1.0.10-sp01 for service pack 1)
         ostringstream ost;
         ost << version << "-sp" << std::setw(2) << std::setfill('0') << count+1;
         string vsp = ost.str();

         // Try to get the value of this key
         string val = GetRegistryKey(diagModuleName, "Software\\Positron\\Pots\\Servicepacks", vsp);
         if (val.size() > 0 && val.find("Applied")!=string::npos)
         {
           // This key is in the registry. It means that this sp was applied, continue to check if a newer one was applied
           count++;
         }
         else
         {  
           // This key is not in the registry. It means that this sp was not applied, do not continue to check for newer sp
           // The latest sp applied is the one we are looking for 
           checkfornewersp = false;            
         }
      }
      while(checkfornewersp);

      ostringstream ost;
      ost.str("");
      ost << count;
      sp = ost.str();
    }

    if (sp.size() > 0)
    {
      istringstream iss;
      iss.str(sp.c_str());
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

// Check if the Viper KBs registry key is present
bool cViperTools::IsViperKBPresent(std::string diagModuleName, std::string kbToCheck)
{
  bool isApplied = false;
  string kbValueToCheck = kbToCheck;
  // Convert to lowercase for comparison
  std::transform(kbValueToCheck.begin(), kbValueToCheck.end(), kbValueToCheck.begin(), ::tolower);

  if (diagModuleName.size() == 0)
  {
    diagModuleName = "ViperTools";
  }

  if (!gViperKBChecked)
  {
    // KBs key has not been retrieve yet try to retrieve it then validate the KBs presence.
    bool keyExists = GetAllRegistryKeySZValues(diagModuleName, "Software\\Positron\\Pots\\KBs", &gViperKBs);
    if (!keyExists)
    {
      keyExists = GetAllRegistryKeySZValues(diagModuleName, "Software\\Positron Industries Inc.\\Viper\\KBs", &gViperKBs);
    }
  
    if (keyExists)
    {
      cViperTools::tStringMap::iterator it;
      it = gViperKBs.find(kbValueToCheck);
      if ((it != gViperKBs.end()) && (it->second == "applied"))
      {
        isApplied = true;
      }
    }

    gViperKBChecked = true;  // prevent multiple registry check if the key not found
  }
  else
  {
    // The KBs kay has already been stored in the gViperKBs map, check for KBs presence in map.
    cViperTools::tStringMap::iterator it;
    it = gViperKBs.find(kbValueToCheck);
    if ((it != gViperKBs.end()) && (it->second == "applied"))
    {
      isApplied = true;
    }
  }

  return isApplied;
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
    r = RegQueryValueEx(hKey, value.c_str(), 0, 0, (unsigned char*)val, &size);
    if (r != ERROR_SUCCESS)
    {
      if (r == ERROR_FILE_NOT_FOUND)
      {
        DiagTrace(diagModuleName, "GetRegistryKey", "Value [" + value + "] in Viper key not found");
      }
      else
      {
        DiagTrace(diagModuleName, "GetRegistryKey", "Can't open " + path + "\\" + value + " registry key.");
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

bool GetAllRegistryKeySZValues(std::string diagModuleName, std::string path, cViperTools::tStringMap *valueMap)
{
  bool returnValue = true;
  HKEY hKey;
  LONG r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey);
  if (r != ERROR_SUCCESS)
  {
    returnValue = false;
    DiagTrace(diagModuleName, "GetAllRegistryKeyValues", "Can't open " + path + " registry key.");
  }
  else
  {
    char val[1024];
    BYTE data[1024];
    DWORD val_size = 1024, data_size = 1024, type = 0;
    int index = 0;

    // Retrieve all REG_SZ value from the key and add to the map provided
    r = RegEnumValue(hKey, index, val, &val_size, 0, &type, data, &data_size);
    while( r == ERROR_SUCCESS )
    {
      if (type == REG_SZ)
      {
        string strVal = val;
        string strData = (char*)data;
        // Convert to lowercase before storing the value and data
        std::transform(strVal.begin(), strVal.end(), strVal.begin(), ::tolower);
        std::transform(strData.begin(), strData.end(), strData.begin(), ::tolower);
        valueMap->insert(cViperTools::tStringMap::value_type(strVal, strData));
        DiagTrace(diagModuleName, "GetAllRegistryKeyValues", "Found value " + strVal + " with data " + strData + "in registry key " + path + ".");
      }
      index++;
      val_size = data_size = 1024;
      r = RegEnumValue(hKey, index, val, &val_size, 0, &type, data, &data_size);
    }

    if (index == 0)
    {
      DiagTrace(diagModuleName, "GetAllRegistryKeyValues", "Can't find any value in " + path + " registry key.");
    }

    RegCloseKey(hKey);
  }

  return returnValue;
}
