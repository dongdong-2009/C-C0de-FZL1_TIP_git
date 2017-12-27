# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/datalog/Makefile $ 
# Author:  Ripple
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/datalog
PROJ_NAME	= TA_DataLogDAI
PROJ_SOURCE	= \
		  DatalogSettingsAccessFactory.cpp \
		  DatalogRecordingData.cpp \
		  DatalogRecordingAccessFactory.cpp \
		  DatalogDPData.cpp \
		  DatalogConfigSettingsData.cpp 
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
