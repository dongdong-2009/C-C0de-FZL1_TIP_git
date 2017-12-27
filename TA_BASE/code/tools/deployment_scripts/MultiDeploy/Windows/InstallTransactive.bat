@echo off
title MFT: %1

:: Stop all Transactive apps; stop alarm store and OPC bridge service;
:: delete existing Transactive

call c:\transactive\superKillTransActive.bat

echo This MFT is %1: When deployment completes, press any key
echo to register app and start services...
pause

:: Register Application

call c:\transactive\registerApplications%1.bat

net start alarmstore
net start "OPC Bridge"

:: stop Telnet server

net stop tlntsvr 