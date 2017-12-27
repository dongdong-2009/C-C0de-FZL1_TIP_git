# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/data_recording_dai
PROJ_NAME	= TA_DataRecordingDAI
PROJ_SOURCE	= ConfigLivePropertyData.cpp \
		  LivePropertyAccessFactory.cpp \
		  LivePropertyData.cpp \
		  LivePropertyDataHelper.cpp \
		  RecordableItem.cpp \
		  RecordableItemAccessFactory.cpp \
		  PersistedState.cpp \
	          PersistedStateAccessFactory.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
