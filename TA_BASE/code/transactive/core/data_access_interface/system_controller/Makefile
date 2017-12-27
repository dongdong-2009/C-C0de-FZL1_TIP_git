# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/system_controller
PROJ_NAME	= TA_SystemControllerDAI
PROJ_SOURCE	= SystemControllerAccessFactory.cpp SystemControllerData.cpp \
                  ConfigSystemController.cpp SystemControllerHelper.cpp \
		  ConfigGroupMonitor.cpp GroupMonitorAccessFactory.cpp \
		  ManagedProcessAccessFactory.cpp ManagedProcessData.cpp ManagedProcessHelper.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
