#include "TicketPad.h"
#include <string>
#include "ToStr.h"
#include "IniFile.h"
#include "TicketOutlet.h"
#include "cAutoLock.h"
#include "NewAutoinitialize.h"
#ifdef _LEAK_DETECT
#undef new
#endif
#include <fstream>
#ifdef _LEAK_DETECT
#define new new(detectLeak, __FILE__, __LINE__)
#endif
#include "CSS.h"
#include <shlobj.h> // Needed for SHGetFolderPath

namespace CSS
{
  const int N_SIZE_KEY = 512;
  const int N_SIZE_BUFFER = 512;
  const char *cTicketPad::moduleName = "TicketPad";
  const char *cTicketPad::COUNTER = "Counter";
  const char *cTicketPad::KEY = "Value";
  const char *cTicketPad::SECTION_COUNTER = "[Counter]\r\n";
  const char *cTicketPad::KEY_VALUE = "Value=1\r\n";
  cTicketOutlet *ourTicketOutlet;

  // Utility functions
  string GetPower911CommonDataDir()
  {
    string result = "";

// Compiler higher than VC 6.0
#if _MSC_VER > 1200

    if (getenv("PositronDevelopmentEnvironment") != NULL)
    {
      // Developper workstation: use the current working directory as the base directory
      result = "."; // base path relative to the current directory
    }
    else
    {
      // The base directory is the system's common application data directory
      // For example, C:\Documents and Settings\All Users\Application Data on a US English version of Windows XP
      // See the documentation of the SHGetFolderPath function for the exact location on different OS versions
      char dataDir[MAX_PATH];
      ZeroMemory(dataDir, MAX_PATH);
      HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, dataDir);
      if (hr != S_OK)
      {
        DiagDllError("TicketPad", "GetCommonDataDir", "Could not retrieve the common application data folder. Using current directory instead.");
        ZeroMemory(dataDir, MAX_PATH);
        GetCurrentDirectory(MAX_PATH, dataDir);
      }
      string psapName = "Default";
      char *psapNameValue = getenv("PositronPsapName");
      if (psapNameValue != NULL)
      {
        psapName = psapNameValue;
      }
      // Get data folder from registry if present. 
      // For A9C, under 
      // path = HKLM\Software\Positron Industries Inc.\A9CcallControl
      // key = ApplicationData
      // value = \Positron\A9CcallControl
      {
        string appDir = "\\Positron\\Power911\\" + psapName;
        string path = "SOFTWARE\\Positron Industries Inc.\\A9CcallControl";
        string key = "ApplicationData";
        HKEY hKey;
        LONG rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey);
        if ( rc != ERROR_SUCCESS )
        {
          DiagTrace("TicketPad", "GetCommonDataDir", "Can't open " + path + " registry key."); 
        }
        else
        {
          unsigned char value[1024];
          DWORD size = 1024;
          rc = RegQueryValueEx(hKey, key.c_str(), 0, 0, value, &size);
          if ( rc != ERROR_SUCCESS )
          {
            DiagTrace("TicketPad", "GetCommonDataDir", "Can't find " + path + "\\" + key + " registry key.");
          }
          else
          {
            // convert to "\\Positron\\A9CcallControl"
            appDir = ( value[0] != '\\' ? "\\\\" : "");
            for (unsigned i = 0; i < size && value[i] != 0; i++ )
            {
              appDir += value[i];
              if ( value[i] == '\\' ) appDir += '\\';
            }
            if (*appDir.rbegin() != '\\')
            {
              appDir += "\\";   // add '\' at the end if needed
            }
          }
        }
        //result = string(dataDir) + "\\Positron\\Power911\\" + psapName;
        result = string(dataDir) + appDir;
      }
    }

// VC 6.0
#else if

    result = "."; // base path relative to the current directory

#endif

    return result;
  }


 
  cTicketPad::cTicketPad()
  {
    bFirstReading = true;
    backupFileNumber = 0;
  };
  
  cTicketPad * cTicketPad::init(string aTicketPadName)
  {
    int WinResult;
    char dir[N_SIZE_BUFFER + 1];
    DiagTrace(moduleName, "Init", "Starting");

    {
      cAutoLock _lock(&lockTicketPad);
      if (!aTicketPadName.empty()) 
      {
        TicketFileName = "";
        WinResult = IniFile::GetSysDirectory(dir, N_SIZE_BUFFER);
        if (WinResult > 0)
        {
          dir[WinResult] = '\0';
          string sysDir = dir;
          if (*sysDir.rbegin() != '\\')
          {
            sysDir += "\\";
          }

          // Power911 uses common application data dir
          string workDir = GetPower911CommonDataDir();
          if (*workDir.rbegin() != '\\')
          {
            workDir += "\\";
          }

// Compiler higher than VC 6.0
#if _MSC_VER > 1200

          // Ensure folder does exist
          int res = SHCreateDirectoryEx(NULL, workDir.c_str(), NULL);

#endif
          TicketPath = workDir; 
          if (workDir != sysDir)
          {
            /* workDir is the temp directory.  Passing TRUE to CopyFile will not overwrite the file
               in workDir if it is already there. */
            workDir += aTicketPadName + ".sav";
            sysDir += aTicketPadName + ".sav";
            CopyFile(sysDir.c_str(), workDir.c_str(), TRUE);
          }
          DiagTrace(moduleName, "Init", "Ticket Path : ", TicketPath);;
        }

        TicketPadName = TicketPath + aTicketPadName;
        TicketFileName = TicketPadName + ".sav";

        if (VerifyAndCreate(TicketFileName) == false)
        {
          /*  Le fichier est -il present pour pouvoir faire une lecture par la suite */
          return 0;
        }
      }
      TicketNumber = 0;
    }
    return this;
  }
  
  int cTicketPad::getTicket()
  {
    int Value;
    /*  Methodes publiques */
    string section;
    string key;
    int KeyValue = 0;
    bool bRead;
    DiagTrace(moduleName, "GetTicket", "Starting");
    {
      cAutoLock _lock(&lockTicketPad);
      section = COUNTER;
      /*  Chargement de la valeur du TicketPad */
      key = KEY;
      if (bFirstReading == true)
      {
        bRead = getConfigKey(section, key, KeyValue);
        /*  Obtient la nouvelle valeur Unique */
        DiagTrace(moduleName, "GetTicket ", " Read First time : " + TicketFileName + " " + section + " " + key + " " , KeyValue);;
        if (bRead == false)
        {
          DiagError(moduleName, "GetTicket", "Error on Read File : " + TicketFileName + " " + section + " " + key + " " ,KeyValue);
          return 0;
        };
        TicketNumber = KeyValue;
        bFirstReading = false;
      };
      KeyValue = TicketNumber;
      DiagTrace(moduleName, "GetTicket", " Read Value ", KeyValue);;
      if (KeyValue >= 2147483647)
      {
        /*  Mise a jour de la Prochaine valeur */
        /*  Verifie si valeur maximal atteinte pour etre positif sur un 32bits signee  (2 Puissance 31)- 1 =2147483647 */
        DiagTrace(moduleName, "GetTicket", " Reset Value to 1 was: ", KeyValue);;
        KeyValue = 1;
      }
      else
      {
        KeyValue++;
      };
      TicketNumber = KeyValue;
      if (setConfigKey(section, key, KeyValue) == false)
      {
        DiagError(moduleName, "GetConfigKey", " Error on SetConfig " + TicketFileName + " " + section + " " + key + " ", KeyValue);
      };
      DiagTrace(moduleName, "GetTicket", " Write File : " + TicketFileName + " " + section + " " + key + " " + ToStr(KeyValue, 10));;
      Value = TicketNumber;
      return Value;
    };
  }
  
  bool cTicketPad::VerifyAndCreate(string FileName)
  {
    ofstream TicketPadFileWr;
    /* ---------------------------------METHODES PRIVEES--------------------------------------- */
    ifstream TicketPadFileRd;
    DiagTrace(moduleName, "VerifyAndCreate", " We open The file for verify if it exist : ", FileName);;
    TicketPadFileRd.open(FileName.c_str(), ios::in);
    if (TicketPadFileRd.bad())
    {
      if (CSSToolsTraceOn == true)
      {
        DiagWarning(moduleName, "VerifyAndCreate", " We will create the file " ,FileName);
      };
      TicketPadFileWr.open(FileName.c_str(), ios::out);
      if (TicketPadFileWr.bad())
      {
        DiagError(moduleName, "VerifyAndCreate", " Error on create on TicketPadFileWr file " , FileName);
        return false;
      };
      TicketPadFileWr << SECTION_COUNTER << flush;
      TicketPadFileWr << KEY_VALUE << flush;
      try
      {
        TicketPadFileWr.close();
      }
      catch(...)
      {
        DiagError(moduleName, "VerifyAndCreate", " Unknown Exception on TicketPadFileWr.Close" , FileName);
        return false;
      }
    }
    else
    {
      DiagTrace(moduleName, "VerifyAndCreate", " The file exist and we close it ", FileName);;
      try
      {
        TicketPadFileRd.close();
      }
      catch(...)
      {
        DiagError(moduleName, "VerifyAndCreate", " Unknown Exception on TicketPadFileRd.close " , FileName);
        return false;
      }
      ;
    };
    DiagTrace(moduleName, "VerifyAndCreate", " Exit");;
    return true;
  }
  
  bool cTicketPad::getConfigKey(string section, string key, int &cValue)
  {
    int WinSize;
    int WinResult;
    string backupFileName;
    /*  Obtient l'information desiree de la Section et de la Cle */
    WinSize = IniFile::GetPrivateProfileI(section, key, 0, TicketFileName);
    if (WinSize <= 0)
    {
      DiagError(moduleName, "GetConfigKey", " Error on reading value : " , ToStr(WinSize, 10) , " will be reset to 1");
      cValue = 1;
      backupFileName = TicketPadName;
      /*  Copy du fichier pour consultation ulterieure */
      backupFileName = (backupFileName + ".cop.");
      backupFileName = (backupFileName + ToStr(backupFileNumber));
      backupFileNumber++;
      DiagError(moduleName, "GetConfigKey ", " See Copy File " , backupFileName);
      WinResult = IniFile::CopyFile(TicketFileName, backupFileName, false);
      return true;
    }
    else
    {
      cValue = WinSize;
      return true;
    };
  }
  
  bool cTicketPad::setConfigKey(string section, string key, int Value)
  {
    int WinResult;
    if (TicketFileName.empty())
    {
      return false;
    };
    WinResult = IniFile::WritePrivateProfileStr(section, key, ToStr(Value, 10), TicketFileName);
    /* IF Diag.TraceOn THEN Diag.Trace(moduleName,"SetConfigKey ",section & " " & key );END; */
    /*  Mise a jour de l'information desiree de la Section et de la Cle */
    if (WinResult >= 0)
    {
      return true;
    }
    else
    {
      DiagError(moduleName, "SetConfigKey ", " Error  on IniFile.WritePrivateProfile");
      return false;
    };
  }
  
  cTicketPad* GetTicketPad(string TicketPadName)
  {
    if (ourTicketOutlet == NULL)
    {
      ourTicketOutlet = (NEW0 cTicketOutlet)->init();
      /* Creation de la billeterie , une seule instance et pour toujours */
      return ourTicketOutlet->getTicketPad(TicketPadName);
    }
    else
    {
      return ourTicketOutlet->getTicketPad(TicketPadName);
    };
  }
  class TicketPad_static
  {
  public:
    TicketPad_static()
    {
      /*  Initialisation */
      ourTicketOutlet = NULL;
    };
    ~TicketPad_static()
    {
      /*  Initialisation */
      delete ourTicketOutlet;
    }
    
  };
  static TicketPad_static TicketPad_static_instance;
}  