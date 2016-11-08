#include "MonitorPVFC.h"
#include "cThread.h"
#include "cEvent.h"
#include "Diag.h"
#include "windows.h"
#include <Tlhelp32.h>


#define kModuleName "MonitorPVFC"

class cMonitorPVFCThread : public cThread
{
public:
  cMonitorPVFCThread() : cThread("cMonitorPVFCThread"), mTerminate(false), mFrequencyInSeconds(0) {};   // Coverity #15194: initialize scalar field 
  virtual long ThreadProc();
  void Signal(){mWaitEvent.Signal();}
  bool CheckForPVFC();
  void StopWerfaultIfRunning();

  int mFrequencyInSeconds;
  bool mTerminate;
  cEvent mWaitEvent;
 
};

cMonitorPVFCThread gPMonitorPVFCThread;

bool StartMonitorForPVFC(int frequencyInSeconds)
{  
  if (!gPMonitorPVFCThread.IsRunning())
  {
    DiagTrace(kModuleName, "StartMonitorForPVFC", "Starting PVFC monitoring");
    gPMonitorPVFCThread.mFrequencyInSeconds = frequencyInSeconds;
    gPMonitorPVFCThread.Run();
    return true;
  }
  else
  {
    DiagTrace(kModuleName, "StartMonitorForPVFC", "PVFC monitoring start requested, but monitoring already active");
  }
  return false;
}

bool StopMonitorForPVFC()
{
  if (gPMonitorPVFCThread.IsRunning())
  {
    DiagTrace(kModuleName, "StartMonitorForPVFC", "Stopping PVFC monitoring");
    gPMonitorPVFCThread.mTerminate = true;
    gPMonitorPVFCThread.Signal();
    cThread::Join(&gPMonitorPVFCThread);
    DiagTrace(kModuleName, "StartMonitorForPVFC", "PVFC monitoring stopped");
    return true;
  }
  else
  {
    DiagTrace(kModuleName, "StopMonitorForPVFC", "PVFC monitoring stop requested, but monitoring already active");
    return false;
  }  
}


long cMonitorPVFCThread::ThreadProc()
{
  DiagTrace(kModuleName, "cMonitorPVFCThread", "Entering");
  while (!mTerminate)
  {
    if (CheckForPVFC())
    {
      Sleep(1000); // give some time to WerFault to show up before checking if running to stop it
      StopWerfaultIfRunning(); 
      DiagError(kModuleName, "cMonitorPVFCThread", "Pure Virtual Function Call detected. Exiting process right away");
      ExitProcess(0);
    }

    mWaitEvent.Wait(mFrequencyInSeconds * 1000);
  }
  DiagTrace(kModuleName, "cMonitorPVFCThread", "Exiting");
  return 0;
}

struct EnumWindowsCallbackArgs 
{
  EnumWindowsCallbackArgs( DWORD p, string t ) : pid( p ), title( t), gotMatch(false) { }
  const DWORD pid;
  bool gotMatch;
  string title;
};

BOOL CALLBACK EnumWindowsProcMatch(HWND hwnd,LPARAM lParam)
{
    EnumWindowsCallbackArgs *args = (EnumWindowsCallbackArgs *)lParam;
    char wnd_title[256];
    string titleStr;
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd,&lpdwProcessId);
    if(lpdwProcessId==args->pid)
    {
      GetWindowText(hwnd,wnd_title,sizeof(wnd_title));
      titleStr = wnd_title;
      if (!titleStr.empty() && (titleStr.find(args->title) != string::npos))
      {
        args->gotMatch = true;
        return FALSE;
      }
    }
    return TRUE;
}

bool cMonitorPVFCThread::CheckForPVFC()
{
  EnumWindowsCallbackArgs args( ::GetCurrentProcessId() , "Microsoft Visual C++ Runtime Library");
  ::EnumWindows(EnumWindowsProcMatch, (LPARAM)&args);
  return args.gotMatch;
}

void cMonitorPVFCThread::StopWerfaultIfRunning()
{
  // check if a POTS - only kill WerFault.exe if a POTS
  DWORD ftyp = GetFileAttributesA("p:\\programs\\ppmsservice");
  if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY))
  {
    //PPMSDiag.SendWarning("Process::StopWerfaultIfRunning()", "Not a POTS server.");
    return;
  }

  const char* filename = "WerFault.exe";
  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
  PROCESSENTRY32 pEntry;
  pEntry.dwSize = sizeof (pEntry);
  BOOL hRes = Process32First(hSnapShot, &pEntry);
  while (hRes)
  {
    if (stricmp(pEntry.szExeFile, filename) == 0)
    {
      DiagWarning(kModuleName, "cMonitorPVFCThread::StopWerfaultIfRunning()", "Found a match for WerFault.exe process.");
      HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD) pEntry.th32ProcessID);
      if (hProcess != NULL)
      {
        DiagWarning(kModuleName, "cMonitorPVFCThread::StopWerfaultIfRunning()", "Killing WerFault.exe process.");
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
      }
    }
    hRes = Process32Next(hSnapShot, &pEntry);
  }
  CloseHandle(hSnapShot);
}
