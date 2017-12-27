# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# This file builds the SCADA Proxies library.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/proxy_library
PROJ_NAME	= TA_ScadaProxyLibrary
PROJ_SOURCE	= \
			AgentSynchronisationInterface.cpp \
			DataNodeBulkPollingSynchronizer.cpp \
			DataNodeLocalSynchronizer.cpp \
			DataNodeProxy.cpp \
			DataNodeProxy_Read.cpp \
			DataNodeProxy_Write.cpp \
			DataNodeProxyManager.cpp \
			DataNodeProxySmartPtr.cpp \
			DataNodeSyncManager.cpp \
			DataPointBulkPollingSynchronizer.cpp \
			DataPointLocalSynchronizer.cpp \
			DataPointProxy.cpp \
			DataPointProxy_Read.cpp \
			DataPointProxy_Write.cpp \
			DataPointProxyManager.cpp \
			DataPointProxySmartPtr.cpp \
			DataPointSyncManager.cpp \
			DataNodeLocalAccess.cpp \
			DataNodeRemoteAccess.cpp \
			DataPointLocalAccess.cpp \
			DataPointRemoteAccess.cpp \
			ProxyObserverNotifier.cpp \
			ObserverUpdateQueueProcessor.cpp \
			ScadaProxyFactory.cpp \
			ScadaProxyManager.cpp \
			SynchronisationManager.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive  $(PROJ_ROOT_DIR)/cots/boost/boost_1_31_0
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES =	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL

	
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
