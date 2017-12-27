# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# This makefile builds the Duty Monitor Framework library
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/security/duty_monitor_framework
PROJ_NAME	= TA_DutyMonitorFramework
PROJ_SOURCE	= DataCache.cpp \
		  DefaultRegionAssignmentMatrix.cpp \
                  DutyAgentAccess.cpp \
                  DutyAgentAccessSingleton.cpp \
                  DutyDataStore.cpp \
                  DutyDataStoreSingleton.cpp \
                  DutyMonitorFrameworkException.cpp \
                  DutyRights.cpp \
                  DutyRightsSingleton.cpp \
                  IConfigDataObserver.cpp \
                  IRegionDutyObserver.cpp \
                  ISessionObserver.cpp \
                  ISubsystemDutyObserver.cpp \
                  RegionDutyMatrix.cpp \
                  RegionDutyMatrixElement.cpp \
                  SessionCache.cpp \
                  SubsystemDutyMatrix.cpp \
                  SubsystemDutyMatrixElement.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
