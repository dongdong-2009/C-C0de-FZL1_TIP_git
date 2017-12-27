# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  Grace Koh
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
#!/bin/ksh
##
## Run perl build.
## 06/05/03
##
##
ksh
export PATH=.:/usr/bin:/usr/local/bin
export CVSROOT=:pserver:itsbuild@cvs:/cvs/1TS-9999-T
export CODE_ROOT=~/Autobuild_4669
export ACE_ROOT=$CODE_ROOT/code/cots/ACE/5_3/ACE_wrappers
export ORACLE_HOME=/u01/oracle/app/OraHome920
export OMNIORB_ROOT=$CODE_ROOT/code/cots/omniORB/omniORB_4.0.1/Solaris8/omni
export LD_LIBRARY_PATH=/usr/local/lib:$ACE_ROOT/ace:$OMNIORB_ROOT/lib/sun4_sosV_5.8:$CODE_ROOT/code/cots/oracle/920/solaris/lib:$CODE_ROOT/code/cots/TAO/TAO_1.3.1/Solaris8/lib/sun4_sosV_5.8
#
cd /home/itsbuild/Autobuild_4669
perl autobuild.pl -r -c -b CODE_4669_030
#perl nightlyBuild.pl
#env
