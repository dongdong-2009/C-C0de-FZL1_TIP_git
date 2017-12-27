:: assume running script on one of the mfts

rem @ECHO OFF

  :: start the telnet server
  sc \\%1 start tlntsvr

  :: Open a Telnet window - it will be the window with focus
  ::start TELNET.EXE
  start cmd.exe /k title MFT %2

  :: Create a VBS temporary file to send [Enter] key
  :: to the window with focus
  ECHO.set sh=WScript.CreateObject("WScript.Shell")>_TEMP.VBS
  
  ECHO.WScript.Sleep(2000)>>_TEMP.VBS
  ECHO.sh.SendKeys("telnet %1{enter}")>>_TEMP.VBS

  ECHO.WScript.Sleep(2000)>>_TEMP.VBS
  ECHO.sh.SendKeys("y{enter}")>>_TEMP.VBS

  rem ECHO.WScript.Sleep(2000)>>_TEMP.VBS
  rem ECHO.sh.SendKeys("operator1{enter}")>>_TEMP.VBS

  rem ECHO.WScript.Sleep(4000)>>_TEMP.VBS
  rem ECHO.sh.SendKeys("op1{enter}")>>_TEMP.VBS

  ECHO.WScript.Sleep(2000)>>_TEMP.VBS
  rem ECHO.sh.SendKeys("c:\transactive\superKillTransActive.bat{enter}")>>_TEMP.VBS
  ECHO.sh.SendKeys("c:\transactive\InstallTransactive.bat %2{enter}")>>_TEMP.VBS

  ECHO.WScript.Sleep(2000)>>_TEMP.VBS

  cscript//nologo _TEMP.VBS
  DEL _TEMP.VBS
