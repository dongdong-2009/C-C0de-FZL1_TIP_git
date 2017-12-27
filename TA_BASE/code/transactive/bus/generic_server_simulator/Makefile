# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# This file builds the TA_GenericServerSimulator library
#
PROJ_ROOT_DIR 	= ../../../..
PROJ_CODE_DIR	= transactive/bus/generic_server_simulator
PROJ_NAME		= TA_GenericServerSimulator

PROJ_SOURCE		= BackgroundCommsObserver.cpp \
			BackgroundModuleCreator.cpp \
			BackgroundSimulationModule.cpp \
			CommandException.cpp \
			CommandMonitor.cpp \
			CommandOwner.cpp \
			ConsoleDetachUtility.cpp \
			DetachCommand.cpp \
			ExecCommand.cpp \
			GenericCommandProcessor.cpp \
			GenericSimulator.cpp \
			GssAbstractModuleCreator.cpp \
			GssCommandMonitor.cpp \
			GssCommandProcessor.cpp \
			GssCommon.cpp \
			GssCommsObserver.cpp \
			GssSimulationModule.cpp \
			GssSimulationModuleFactory.cpp \
			GssUtility.cpp \
			IntermediateCommand.cpp \
			LeafCommand.cpp \
			StatusCommand.cpp \
			TelnetCommon.cpp \
			TelnetCommsObserver.cpp \
			VersionCommand.cpp

PROJ_TYPE		= lib

PROJ_INC_DIR 	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive

PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBDIRS	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
			$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

