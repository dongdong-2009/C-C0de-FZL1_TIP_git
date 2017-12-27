@ECHO OFF

:: mount staging directory of Windows build machine
net use z: \\192.168.70.108\Autobuild_4669\staging\CODE_4669_00170000\mft /user:southperth\tabuild tabuild 

for /F "eol=# tokens=1,2,3 delims=, " %%i in (mfts.txt) do @(
    
  :: %%i is ip address, %%j is location name, %%k is mapping drive letter
  start CopyTransactive.bat %%i %%k

)

for /F "eol=# tokens=1,2,3 delims=, " %%i in (mfts.txt) do @(
    
  :: %%i is ip address, %%j is location name
  call RemoteInstallWindows.bat %%i %%j

)

echo When all MFTs have been deployed, press any key to continue...

pause

:: unmount 
net use z: /delete


echo Finished...

pause