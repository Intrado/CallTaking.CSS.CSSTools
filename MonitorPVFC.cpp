#include "MonitorPVFC.h"
#include "cThread.h"
#include "cEvent.h"
#include "Diag.h"
#include "windows.h"

#define kModuleName "MonitorPVFC"

class cMonitorPVFCThread : public cThread
{
public:
  cMonitorPVFCThread() : cThread("cMonitorPVFCThread"), mTerminate(false), mFrequencyInSeconds(0) {};   // Coverity #15194: initialize scalar field 
  virtual long ThreadProc();
  void Signal(){mWaitEvent.Signal();}
  bool CheckForPVFC();

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
      DiagError(kModuleName, "cMonitorPVFCThread", "Pure Virtual Function Call detected. Exiting process right away");
      ExitProcess(0);
    }

    mWaitEvent.Wait(mFrequencyInSeconds * 1000);
  }
  DiagTrace(kModuleName, "cMonitorPVFCThread", "Exiting");
  return 0;
}

bool cMonitorPVFCThread::CheckForPVFC()
{
  HWND hWnd = FindWindow(NULL, "Microsoft Visual C++ Runtime Library");
  if (hWnd != NULL)
  {
    return true;
  }
  return false;
}