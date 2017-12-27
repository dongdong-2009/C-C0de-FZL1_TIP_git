@ECHO OFF

for /F "eol=# tokens=1 delims=, " %%i in (servers.txt) do @(

  RemoteInstallSolaris.bat %%i 00.17.01.00 Debug CODE_4669_00170100 1.0 y y TRANSACT IT171
)
