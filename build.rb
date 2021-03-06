#!/usr/bin/env ruby

require 'fileutils'
require '../gitscripts/CompileEnvDef.rb'

def execCmd(cmd)
  begin
    puts ":> " + cmd
    raise if not system(cmd)  
  rescue
    puts "Command FAILED: exiting ..."
    exit 1
  end
end

# build command variable
buildCmd = [
  "#{$devenv12} CSSToolsVS12.sln /Rebuild Release",
  "#{$devenv12} CSSToolsVS12.sln /Rebuild Debug",
  "#{$devenv12} CSSToolsVS12.sln /Rebuild BackroomRelease",
]


# Files to be copied

toCopy = [
  %w(cAutoLock.h _export),
  %w(cBooleanToggler.h _export),
  %w(cCommandLine.h _export),
  %w(cCondition.h _export),
  %w(cCritSec.h _export),
  %w(cEvent.h _export),
  %w(cEvtThread.h _export),
  %w(cHandledObject.h _export),
  %w(cHiddenNotifiableObject.h _export),
  %w(cInstanceList.h _export),
  %w(cInstanceMap.h _export),
  %w(cKEvent.h _export),
  %w(cLimitedTimeQueue.h _export),
  %w(cLockableInfo.h _export),
  %w(cLockableObject.h _export),
  %w(cLookupThreadTable.h _export),
  %w(cMappingTable.h _export),
  %w(cMonitor.h _export),
  %w(cMonitorAbstractBase.h _export),
  %w(cMutex.h _export),
  %w(cNotifiableObject.h _export),
  %w(cNotifierObject.h _export),
  %w(cSafeLock.h _export),
  %w(cSemaphore.h _export),
  %w(cSharedList.h _export),
  %w(cSharedMemory.h _export),
  %w(CSS.h _export),
  %w(CSSAlgorithm.h _export),
  %w(CSSDiag.h _export),
  %w(CSSDiagEx.h _export),
  %w(CSSDualReportQ.h _export),
  %w(CSSEvent.h _export),
  %w(CSSIni.h _export),
  %w(CSSMultiCondition.h _export),
  %w(CSSObject.h _export),
  %w(CSSostringstream.h _export),
  %w(CSSReporter.h _export),
  %w(CSSReporterObject.h _export),
  %w(CSSReportQ.h _export),
  %w(CSSTimeout.h _export),
  %w(CSSTools.h _export),
  %w(CSSTrace.h _export),
  %w(cSuspendReporting.h _export),
  %w(cTcp.h _export),
  %w(cTcpClient.h _export),
  %w(cTcpMsg.h _export),
  %w(cTcpServer.h _export),
  %w(cTemplateString.h _export),
  %w(cTEvent.h _export),
  %w(cThread.h _export),
  %w(cTime.h _export),
  %w(cTimerHandler.h _export),
  %w(cTimerManager.h _export),
  %w(CtxId.h _export),
  %w(DLLIF.h _export),
  %w(IniFile.h _export),
  %w(LockableStlContainers.h _export),
  %w(MonitorPVFC.h _export),
  %w(NewAutoInitialize.h _export),
  %w(RemoteEvent.h _export),
  %w(SimpleList.h _export),
  %w(SmartPtr.h _export),
  %w(StringConversion.h _export),
  %w(sTEventRec.h _export),
  %w(ThreadPool.h _export),
  %w(TicketOutlet.h _export),
  %w(TicketPad.h _export),
  %w(TimedItem.h _export),
  %w(TimedStringMap.h _export),
  %w(TimedQueue.h _export),
  %w(TimerQueueManager.h _export),
  %w(ToStr.h _export),
  %w(util.h _export),
  %w(ViperTools.h _export),

  %w(ReleaseVS12/CSSTools.dll _export/vs12),
  %w(ReleaseVS12/CSSTools.lib _export/vs12),
  %w(ReleaseVS12/CSSTools.pdb _export/vs12),
  %w(ReleaseLibVS12/CSSToolsLib.lib _export/vs12),

  %w(DebugVS12/CSSToolsd.dll _export/vs12),
  %w(DebugVS12/CSSToolsd.lib _export/vs12),
  %w(DebugVS12/CSSToolsd.pdb _export/vs12),
  %w(DebugLibVS12/CSSToolsLibd.lib _export/vs12),

  %w(ReleaseVS12_Backroom/CSSTools.dll _export/Backroom/VS12),
  %w(ReleaseVS12_Backroom/CSSTools.lib _export/Backroom/VS12),
  %w(ReleaseVS12_Backroom/CSSTools.pdb _export/Backroom/VS12)
  ]


# execute all build command lines
buildCmd.each {|cmd| execCmd(cmd)}


# execute all copies
puts "\nCopying Files..."
toCopy.each {|file| 
  begin
    puts 'Copying ' + file[0] + ' to ' + file[1]
    FileUtils.mkdir_p(file[1])
    FileUtils.cp_r(file[0], file[1], :remove_destination => true) 
  rescue
    puts "ERROR: Failed to copy, exiting ..."
    exit 1
  end
}


