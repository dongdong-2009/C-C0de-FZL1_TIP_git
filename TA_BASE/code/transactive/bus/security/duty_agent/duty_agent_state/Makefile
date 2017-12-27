# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:43:23 $
# Last modified by:  $Author: CM $
#
# This file builds the TA_DutyAgentState library
#
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/security/duty_agent/duty_agent_state
PROJ_NAME	= TA_DutyAgentState
PROJ_SOURCE	= AgentModeSubject.cpp \
		  AgentTerminateSubject.cpp \
		  DefaultApplyConverter.cpp \
		  DegradeMO.cpp \
		  DutyContext.cpp \
		  DutyContextBuilderFactory.cpp \
		  DutyDbCache.cpp \
		  DutyDbCacheUpdater.cpp \
		  DutyPeerTask.cpp \
		  DutyPeerTaskMO.cpp \
		  DutyPeerTaskManager.cpp \
		  DutyPeerTaskResultHandler.cpp \
		  DutyPolicyCombAlg.cpp \
		  DutyPolicyFactory.cpp \
		  DutyPrimitive.cpp \
		  DutyRequestConverter.cpp \
		  DutyRequestSender.cpp \
		  DutyResponseSubject.cpp \
		  DutySessionCache.cpp \
		  DutyState.cpp \
		  EvaluateDutyPolicy.cpp \
		  ExecuteQueryMO.cpp \
		  FrontEndStateChangeSubject.cpp \
		  GetDutyQuery.cpp \
		  GetExclusiveDutyQuery.cpp \
		  IDutyContextBuilder.cpp \
		  LocalDutyPeer.cpp \
		  PeerStateChangeSubject.cpp \
		  PendingDutyHelper.cpp \
		  Region.cpp \
		  RemoteDutyPeer.cpp \
		  SessionBeginMO.cpp \
		  SessionEndMO.cpp \
		  SessionOvrBeginMO.cpp \
		  SessionOvrEndMO.cpp \
		  SnapshotQuery.cpp \
		  SesSnapshotQuery.cpp \
		  SubSnapshotQuery.cpp \
		  StateUpdateConverter.cpp \
		  StateUpdateSubject.cpp \
		  Subsystem.cpp \
		  SubsystemState.cpp \
		  TransferRegionsMO.cpp \
		  TransferSubsystemsMO.cpp \
		  TransferableRegionQuery.cpp \
		  TransferableSubsystemQuery.cpp \
		  PeerStateChangeTask.cpp \
		  PeerStateChangeMO.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/naming \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL 
				
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
