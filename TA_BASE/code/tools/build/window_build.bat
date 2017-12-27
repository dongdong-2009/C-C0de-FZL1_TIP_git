@echo off

call "C:\Program Files\Microsoft Visual Studio 9.0\VC\bin\VCVARS32.bat"

set ANT_ARGS="-logger" "buildsystem.log.ParallelLogRedirector"

set p4USER=builder
set p4CLIENT=Builder_Window_3001_Win7_TIPNEW-Windows
set p4PASSWD=builderbuilder

REM Make sure we're on the right drive.
C:

REM The directories
set ROOT_BASE="C:\955_TIPNEW\Base_3001\Base"

REM Build Base
cd %ROOT_BASE%

REM Checkout the code here.
call ant clean_all p4checkoutCode version build_Net2008 stage

