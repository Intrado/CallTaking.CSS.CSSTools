echo off
setlocal

if .%1==.setup goto buildsetup

IF EXIST prepare.bat call prepare.bat

if .%1==.release goto release

goto make
:release

echo Modifying versions
call _import\BuildScripts\setversion.bat Version.rc %2

:make
echo Building CDRManager.exe
call msdev CDRManager.dsp /MAKE "CDRManager - Win32 Release" /REBUILD
echo %ERRORLEVEL%
if not %errorlevel% EQU 0 echo MAKE FAILED. Build stopped.
if not %errorlevel% EQU 0 goto end
if NOT .%1==.release goto end

echo Copy files from working folders to _bin
IF EXIST _bin goto copybin
mkdir _bin
:copybin
copy /Y Release\CDRManager.exe _bin\

:checkin
echo Commiting changes and tagging version %2
svn copy . svn://vdfile/svn1/_tags/ServerApps/CDRManager/V%2 -m "Tag V%2 created by build script"
svn commit -m "Tag V%2 created by build script"
if not %errorlevel% EQU 0 goto end

goto end

REM Setup build 
:buildsetup
goto end

:end
echo ********** DONE ********** 
echo.