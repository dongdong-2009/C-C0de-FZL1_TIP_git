:: Author:
::   S.Liew  &
::   P.Greenwood
:: -----------------------------
::
:: Usage: 
::
::   creatprof <Username>
::
:: Examples:
::
::   creatprof newmacro
::   creatprof "Tom Jones"
::
:: Description:
::
::   Batch file to tighten down a user's deskop by configuring
::   their profile.
::
:: ------------------------------------------------------------------
:: Event sequence:
::
::   - Remove user shortcuts
::   - Load user's binary hash file - NTUser.dat into local registry
::   - Edit the registry keys & values 
::   - Unload the hash (saves file)
::
:: Parameters:
:: 
::   Username - login name of the target user
::
:: Notes:
::
::   Assumes all user profiles are located in C:\Documents and Settings
::   and that the profile dir is the same as the user name
::
::   If the username contains spaces, the entire parameter should 
::   be quoted
::
::   This script requires the following files - the first should be
::   in the same dir as this batch file: 
::
::      registry file  -  addsecurity.reg
::      binary file    -  reg.exe  (to be in the path)
::
:: ------------------------------------------------------------------
@echo off
:: Sanity checks - 


:: 1. Three parameters required

if not " %1" == " " goto ok1
echo Incorrect usage: addSecurity ^<Username^>
echo. 
echo e.g.             addSecurity operator2
goto end
:ok1


:: 2. Directory\file exists

if exist "C:\Documents and Settings\%~1\ntuser.dat" goto ok2
echo File C:\Documents and Settings\%~1\ntuser.dat does not exist
goto end
:ok2

:: 3. Files exist?

if not exist addSecurity.reg (
  echo Associated file addSecurity.reg not found
  goto end
)


:: Remove user short-cuts:

del /f /q "C:\Documents and Settings\%~1\Application Data\Microsoft\Internet Explorer\Quick Launch\*"
del /f /q "C:\Documents and Settings\%~1\Desktop\*"
del /f /q /s "C:\Documents and Settings\%~1\Start Menu\Programs\*"

:: Load user hive:

reg load "C:\Documents and Settings\%~1\ntuser.dat" HKLM\TempHive

if errorlevel 0 goto ok4
echo Failed to load HKLM\TempHive
goto end
:ok4


:: Make the changes:
echo off
if exist tmpaddsecurity.reg del tmpaddsecurity.reg

copy addsecurity.reg tmpaddsecurity.reg

echo [HKEY_LOCAL_MACHINE\TempHive\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon] >> tmpaddsecurity.reg

echo "Shell"="C:\\transActive\\bin\\ControlStation.exe --entity-name=ControlStation --run-param-file=c:\\transactive\\start.ini"  >> tmpaddsecurity.reg
echo loading registry tmpaddSecurity.reg
regedit /s tmpaddSecurity.reg

echo finished loading registry addSecurity.reg
del tmpaddsecurity.reg

::Unload user hive:

reg unload HKLM\TempHive

if errorlevel 0 goto ok5
echo Failed to unload HKLM\TempHive
goto end
:ok5

echo Process completed
:end
