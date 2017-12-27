# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/maintenance_management/mmsHelper/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/12/29 14:17:48 $
# Last modified by:  $Author: grace.koh $
#
# This file builds the ScadaCommonLib library.
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/maintenance_management/mmsHelper
PROJ_NAME		= TA_MMSHelp
PROJ_SOURCE	= IMMSAPI.cpp \
		  MMSUtility.cpp \
		  MMSDatabaseCache.cpp \
		  MMSSubmitManager.cpp \
		  MMSSubmitterDatabase.cpp \
		  MMSGlobalInhibitionManager.cpp \
		  MMSAPIManager.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
	$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL
	
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
