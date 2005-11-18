# Microsoft Developer Studio Project File - Name="CSSTools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CSSTools - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CSSTools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CSSTools.mak" CFG="CSSTools - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CSSTools - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CSSTOOLS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /Od /I "DiagExp\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CSSTOOLS_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 sxlrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib diag.lib /nologo /dll /pdb:none /debug /machine:I386 /libpath:"DiagExp\Release"
# Begin Target

# Name "CSSTools - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cAutoLock.cpp
# End Source File
# Begin Source File

SOURCE=.\cCommandLine.cpp
# End Source File
# Begin Source File

SOURCE=.\cCondition.cpp
# End Source File
# Begin Source File

SOURCE=.\cCritSec.cpp
# End Source File
# Begin Source File

SOURCE=.\cEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\cEvtThread.cpp
# End Source File
# Begin Source File

SOURCE=.\cHandledObject.cpp
# End Source File
# Begin Source File

SOURCE=.\cHiddenNotifiableObject.cpp
# End Source File
# Begin Source File

SOURCE=.\cKEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\cLimitedTimeQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\cLockableObject.cpp
# End Source File
# Begin Source File

SOURCE=.\cLookupThreadTable.cpp
# End Source File
# Begin Source File

SOURCE=.\cMappingTable.cpp
# End Source File
# Begin Source File

SOURCE=.\cMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\cMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\cNotifiableObject.cpp
# End Source File
# Begin Source File

SOURCE=.\cNotifierObject.cpp
# End Source File
# Begin Source File

SOURCE=.\cSafeLock.cpp
# End Source File
# Begin Source File

SOURCE=.\cSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\cSharedList.cpp
# End Source File
# Begin Source File

SOURCE=.\cSharedMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSDiagEx.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSDualReportQ.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSIni.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSMultiCondition.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSReporterObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSReportQ.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSTimeout.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSTools.cpp
# End Source File
# Begin Source File

SOURCE=.\CSSTools.rc
# End Source File
# Begin Source File

SOURCE=.\cTemplateString.cpp
# End Source File
# Begin Source File

SOURCE=.\cTEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\cThread.cpp
# End Source File
# Begin Source File

SOURCE=.\cTime.cpp
# End Source File
# Begin Source File

SOURCE=.\cTimerManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CtxId.cpp
# End Source File
# Begin Source File

SOURCE=.\DLLIF.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadPool.cpp
# End Source File
# Begin Source File

SOURCE=.\TicketOutlet.cpp
# End Source File
# Begin Source File

SOURCE=.\TicketPad.cpp
# End Source File
# Begin Source File

SOURCE=.\TimedItem.cpp
# End Source File
# Begin Source File

SOURCE=.\TimedQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\TimerQueueManager.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cAutoLock.h
# End Source File
# Begin Source File

SOURCE=.\cBooleanToggler.h
# End Source File
# Begin Source File

SOURCE=.\cCommandLine.h
# End Source File
# Begin Source File

SOURCE=.\cCondition.h
# End Source File
# Begin Source File

SOURCE=.\cCritSec.h
# End Source File
# Begin Source File

SOURCE=.\cEvent.h
# End Source File
# Begin Source File

SOURCE=.\cEvtThread.h
# End Source File
# Begin Source File

SOURCE=.\cHandledObject.h
# End Source File
# Begin Source File

SOURCE=.\cHiddenNotifiableObject.h
# End Source File
# Begin Source File

SOURCE=.\cInstanceList.h
# End Source File
# Begin Source File

SOURCE=.\cInstanceMap.h
# End Source File
# Begin Source File

SOURCE=.\cKEvent.h
# End Source File
# Begin Source File

SOURCE=.\cLimitedTimeQueue.h
# End Source File
# Begin Source File

SOURCE=.\cLockableInfo.h
# End Source File
# Begin Source File

SOURCE=.\cLockableObject.h
# End Source File
# Begin Source File

SOURCE=.\cLookupThreadTable.h
# End Source File
# Begin Source File

SOURCE=.\cMappingTable.h
# End Source File
# Begin Source File

SOURCE=.\cMonitor.h
# End Source File
# Begin Source File

SOURCE=.\cMonitorAbstractBase.h
# End Source File
# Begin Source File

SOURCE=.\cMutex.h
# End Source File
# Begin Source File

SOURCE=.\cNotifiableObject.h
# End Source File
# Begin Source File

SOURCE=.\cNotifierObject.h
# End Source File
# Begin Source File

SOURCE=.\cSafeLock.h
# End Source File
# Begin Source File

SOURCE=.\cSemaphore.h
# End Source File
# Begin Source File

SOURCE=.\cSharedList.h
# End Source File
# Begin Source File

SOURCE=.\cSharedMemory.h
# End Source File
# Begin Source File

SOURCE=.\CSS.h
# End Source File
# Begin Source File

SOURCE=.\CSSAlgorithm.h
# End Source File
# Begin Source File

SOURCE=.\CSSDiag.h
# End Source File
# Begin Source File

SOURCE=.\CSSDiagEx.h
# End Source File
# Begin Source File

SOURCE=.\CSSDualReportQ.h
# End Source File
# Begin Source File

SOURCE=.\CSSEvent.h
# End Source File
# Begin Source File

SOURCE=.\CSSIni.h
# End Source File
# Begin Source File

SOURCE=.\CSSMultiCondition.h
# End Source File
# Begin Source File

SOURCE=.\CSSObject.h
# End Source File
# Begin Source File

SOURCE=.\CSSostringstream.h
# End Source File
# Begin Source File

SOURCE=.\CSSReporter.h
# End Source File
# Begin Source File

SOURCE=.\CSSReporterObject.h
# End Source File
# Begin Source File

SOURCE=.\CSSReportQ.h
# End Source File
# Begin Source File

SOURCE=.\CSSTimeout.h
# End Source File
# Begin Source File

SOURCE=.\CSSTools.h
# End Source File
# Begin Source File

SOURCE=.\CSSTrace.h
# End Source File
# Begin Source File

SOURCE=.\cSuspendReporting.h
# End Source File
# Begin Source File

SOURCE=.\cTemplateString.h
# End Source File
# Begin Source File

SOURCE=.\cTEvent.h
# End Source File
# Begin Source File

SOURCE=.\cThread.h
# End Source File
# Begin Source File

SOURCE=.\cTime.h
# End Source File
# Begin Source File

SOURCE=.\cTimerHandler.h
# End Source File
# Begin Source File

SOURCE=.\cTimerManager.h
# End Source File
# Begin Source File

SOURCE=.\CtxId.h
# End Source File
# Begin Source File

SOURCE=.\DLLIF.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\LockableStlContainers.h
# End Source File
# Begin Source File

SOURCE=.\NewAutoInitialize.h
# End Source File
# Begin Source File

SOURCE=.\RemoteEvent.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SimpleList.h
# End Source File
# Begin Source File

SOURCE=.\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\sTEventRec.h
# End Source File
# Begin Source File

SOURCE=.\ThreadPool.h
# End Source File
# Begin Source File

SOURCE=.\TicketOutlet.h
# End Source File
# Begin Source File

SOURCE=.\TicketPad.h
# End Source File
# Begin Source File

SOURCE=.\TimedItem.h
# End Source File
# Begin Source File

SOURCE=.\TimedQueue.h
# End Source File
# Begin Source File

SOURCE=.\TimerQueueManager.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
