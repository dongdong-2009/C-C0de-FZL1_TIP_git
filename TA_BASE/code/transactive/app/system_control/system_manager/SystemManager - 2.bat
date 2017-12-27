@ECHO OFF
DEL /Q /F SystemManager_2*.log
SET PATH=C:\LocalTest\FZL1_TIP\DLL;C:\LocalTest\DLL_VS2008;%PATH%
C:\C0de\FZL1_TIP\TA_BASE\bin\win32_nd\SystemManager.exe --run-param-file=start-2.ini
