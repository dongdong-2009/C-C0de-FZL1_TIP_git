@ECHO OFF

  :: Open a Telnet window - it will be the window with focus
  start TELNET.EXE

  :: Create a VBS temporary file to send [Enter] key
  :: to the window with focus
  ECHO.set sh=WScript.CreateObject("WScript.Shell")>_TEMP.VBS
  
  ECHO.WScript.Sleep(200)>>_TEMP.VBS
  ECHO.sh.SendKeys("open %1{enter}")>>_TEMP.VBS

  ECHO.WScript.Sleep(2000)>>_TEMP.VBS
  ECHO.sh.SendKeys("transactive{enter}")>>_TEMP.VBS

  ECHO.WScript.Sleep(4000)>>_TEMP.VBS
  ECHO.sh.SendKeys("tra{enter}")>>_TEMP.VBS

  ECHO.WScript.Sleep(2000)>>_TEMP.VBS
  ECHO.sh.SendKeys("install_transactive2.sh %2 %3 %4 %5 %6 %7 %8 %9{Enter}")>>_TEMP.VBS

  cscript//nologo _TEMP.VBS
  DEL _TEMP.VBS
