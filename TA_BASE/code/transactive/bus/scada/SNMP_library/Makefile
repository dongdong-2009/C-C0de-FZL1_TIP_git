# The source code in this file is the property of
# Ripple Systems and is not for redistribution in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/21 16:51:05 $
# Last modified by:  $Author: haijun.li $
#
# This file builds the SNMP_library lib.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/SNMP_library
PROJ_NAME	= TA_SNMPLib
PROJ_SOURCE	= DataTranslater.cpp \
				SNMPDataUpdater.cpp \
				SNMPDevice.cpp \
				SNMPOutputSender.cpp \
				SNMPProtocolManager.cpp \
				ThreadWorker.cpp \
				TrapReceiver.cpp \
				WorkerPool.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive \
	$(PROJ_ROOT_DIR)/code/cots/SNMP_PP/snmp_pp_v3.2.16/include 
	
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
