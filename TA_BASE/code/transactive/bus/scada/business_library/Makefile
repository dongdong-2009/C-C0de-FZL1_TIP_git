# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/business_library/Makefile $
# @author:  dazhi
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/21 17:58:35 $
# Last modified by:  $Author: haijun.li $
#
# This file builds the SCADA Proxies library.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/business_library
PROJ_NAME	= TA_ScadaBusinessLibrary
PROJ_SOURCE	= \
			BusinessFactory.cpp \
			BusinessManager.cpp \
			BusinessHeartbeat.cpp \
			BusinessMonitorAndWrite.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive  $(PROJ_ROOT_DIR)/cots/boost/boost_1_39_0
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES =	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \

	
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
