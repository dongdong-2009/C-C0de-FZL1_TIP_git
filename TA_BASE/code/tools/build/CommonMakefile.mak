#The source code in this file is the property of
#Ripple Systems and is not for redistribution
#in any form.
#
#Source:   $File: $ 
#Author:  Grace Koh
#Version: $Revision: $
#
#Last modification: $DateTime: $
#Last modified by:  $Author: $

# BACKWARDS COMPATIBILITY
# Most Makefiles include CommonMakefile.mak, which has been split into two
# parts - definitions and rules.  This file now merely includes both of these
# makefiles.

# Custom rules that override the rules in CommonMakefileRules.mak can be 
# defined AFTER CommonMakefileDefs.mak has been included.

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefileDefs.mak
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefileRules.mak
