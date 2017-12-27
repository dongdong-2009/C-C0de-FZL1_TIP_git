# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/Makefile $ 
# Author:  Ripple
# Version: $Revision: #3 $
#
# Last modification: $DateTime: 2015/12/09 17:40:41 $
# Last modified by:  $Author: hongzhi.zhang $
#
# Make file for the TA_TISAgentDAI classes.
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/tis_agent
PROJ_NAME	= TA_TISAgentDAI

PROJ_SOURCE	= PredefinedMessageLibraryHelper.cpp \
		PredefinedMessageLibraryAccessFactory.cpp \
                PredefinedMessageLibraryData.cpp \
                ConfigPredefinedMessageLibraryData.cpp \
		PidStationIdAccessFactory.cpp \
                

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent/IDL

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
