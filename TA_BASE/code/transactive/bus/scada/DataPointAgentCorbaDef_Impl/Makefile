# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# This file builds the DataPointAgentCorbaDef_Impl library.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/DataPointAgentCorbaDef_Impl
PROJ_NAME	= TA_DataPointAgentCorbaDef_Impl
PROJ_SOURCE	= DataPointAgentCorbaDef_Impl.cpp \
			DataPointAccessCorbaDef_Impl.cpp \
		  DataPointObserver.cpp \
		  DataPointDisplayClient.cpp \
		  DataPointStateContainer.cpp \
		  DataPointAgentSynHandler.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
											$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datapoint_library

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
