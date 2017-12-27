:: Ye zhaohua
:: -----------------------------
::
:: Usage: 
::
::   removestartup <Username>
::
:: Examples:
::
::   removestartup newmacro
::   removestartup "Tom Jones"
::
:: Description:
::
::   NT batch file to remove tight startup settings from a user profile
::
:: Event sequence:
::
::   - Load user's binary hash file - ntuser.dat into local registry
::   - Edit the registry 
::   - removes the outlook express shortcut
::   - Unload the hash (saves file)
::
:: Parameters:
:: 
::   Username - login name of the target user
::
:: Notes:
::
::   Assumes all user profiles are located in 
::   C:\Documents and Settings
::
::   If the username contains spaces, the entire parameter should 
::   be quoted
::
::   This script should be run from its own directory as it 
::   requires removestartup.reg 
::
:: ------------------------------------------------------------------
@echo off
:: Sanity checks - 

:: 1. Single parameter

if not " %~1" == " " goto ok1
echo Incorrect usage: removestartup ^<Username^>
echo. 
echo e.g.             removestartup newmacro
echo e.g.             removestartup "Tom Jones"
goto end
:ok1

:: 2. Directory\file exists

if exist "C:\Documents and Settings\%~1\ntuser.dat" goto ok2
echo File C:\Documents and Settings\%~1\ntuser.dat does not exist
goto end
:ok2

:: 3. removestartup.reg exists

if exist removestartup.reg goto ok3
echo Associated file removestartup.reg not found
goto end
:ok3


:: Load user hive:

reg load "C:\Documents and Settings\%~1\ntuser.dat" HKLM\TempHive

if errorlevel 0 goto ok4
echo Failed to load HKLM\TempHive
goto end
:ok4

:: Make the changes:

regedit /s removestartup.reg

:: Remove stupid outlook express short cut if it exists:

if exist "C:\Documents and Settings\%~1\Start Menu\Programs\Outlook Express.lnk" del "C:\Documents and Settings\%~1\Start Menu\Programs\Outlook Express.lnk"    

::Unload user hive:

reg unload HKLM\TempHive

if errorlevel 0 goto ok5
echo Failed to unload HKLM\TempHive
goto end
:ok5

echo Process completed
:end
