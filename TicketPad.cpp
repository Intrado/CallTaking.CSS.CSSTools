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

    // Determine whether we are in a Power911 telephony or not
    bool isPower911 = true;
    char telephonyAppName[256];
    DWORD result = GetEnvironmentVariable("TelephonyAppName", telephonyAppName, 256);
    if (result != 0 && strcmp(telephonyAppName, "Power911") != 0)
    {
      isPower911 = false;
    }

    
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

          /* We want the file to be in the temp directory.  If there is already a file in the temp directory,
             use it.  If not, copy the one from the system directory.  If the temp directory doesn't exist,
             work in the system directory. */
          string workDir;

          if (!isPower911)
          {
            // Simon uses TEMP dir
            char *envTemp = getenv("TEMP");
            if (envTemp == 0)
            {
              workDir = sysDir;
            }
            else
            {
              workDir = envTemp;
            }
          }
          else
          {
            // Power911 uses current dir
            char currDir[1024];
            DWORD result = GetCurrentDirectory(1024, currDir);
            if (result > 0)
            {
              workDir = currDir;
            }
            else
            {
              workDir = sysDir;
            }              
          }
          if (*workDir.rbegin() != '\\')
          {
            workDir += "\\";
          }

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