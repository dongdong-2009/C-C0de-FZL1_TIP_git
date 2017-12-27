@echo off
title Copying TransActive from Build Machine to %1

:: mount mft transactive directory
  net use %2: \\%1\c$ /user:operator1 op1

  %2:
  cd temp
  del /F /S /Q transactive
  rmdir /S /Q transactive
  mkdir transactive
  xcopy z:\* %2:\temp\transactive /E /R /Z /Y

  :: Retrieve Transactive from Temp directory
  :: ASSUMPTION - BY THIS STAGE, OLD TRANSACTIVE HAS BEEN REMOVED

  :: Syntax for move command does not allow for multiple dir and files
  :: Move only allows overwrite of files, NOT directories
  cd transactive
  move /Y bin %2:\transactive
  move /Y lib %2:\transactive
  move /Y config %2:\transactive 
  move /Y help %2:\transactive
  move /Y sounds %2:\transactive
  move /Y integration_tools %2:\transactive
  move /Y * %2:\transactive
  
  c:

  :: unmount mft transactive directory
  net use %2: /delete