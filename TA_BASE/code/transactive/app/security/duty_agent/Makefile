# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#
# Make file for the DutyAgent
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/security/duty_agent
PROJ_NAME	= DutyAgent
PROJ_SOURCE	= AllocateDAC.cpp \
		  ControlExecutive.cpp \
		  CorbaDutyStateConverter.cpp \
		  DegradeDAC.cpp \
		  DegradedPopupConverter.cpp \
		  DutyAgentImpl.cpp \
		  DutyAgentUser.cpp \
		  DutyAlarmSender.cpp \
		  DutyAuditMessageConverter.cpp \
		  DutyAuditMessageSender.cpp \
		  DutyManagerSnapshotConverter.cpp \
		  DutyManagerUpdateConverter.cpp \
		  DutyManagerUpdateSender.cpp \
		  FreeDAC.cpp \
		  GetDutyConverter.cpp \
		  GetExclusiveDutyConverter.cpp \
		  GetStateDAC.cpp \
		  GetSessionStateDAC.cpp \
		  GetSubsystemStateDAC.cpp \
		  GetTransferableRegionsDAC.cpp \
		  GetTransferableSubsystemsDAC.cpp \
		  HandoverAlarmConverter.cpp \
		  HandoverPopupConverter.cpp \
		  HasDutyDAC.cpp \
		  HasExclusiveDutyDAC.cpp \
		  IDutyAgentCommand.cpp \
		  MonitorExecutive.cpp \
		  OverrideBeginDAC.cpp \
		  OverrideEndDAC.cpp \
		  PopupMessageSender.cpp \
		  RightsLibraryUpdateConverter.cpp \
		  RightsLibraryUpdateSender.cpp \
		  TransferRegionDAC.cpp \
		  TransferSubsystemDAC.cpp \
		  TransferableRegionStateConverter.cpp \
		  TransferableSubsystemStateConverter.cpp \
		  StateUpdateSender.cpp \
		  SubsystemStateSnapshotConverter.cpp \
		  SubsystemStateUpdateConverter.cpp \
		  SubsystemStateUpdateSender.cpp \
		  DutyMonitor.cpp \
		  DutyMonitorAlarmSender.cpp \
		  DutyMonitorManager.cpp \
		  DutyMonitorStateConverter.cpp \
		  DutyMonitorTask.cpp \
		  VersionGen.cpp \
    		  main.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS =	\
		TA_CorbaUtil \
		TA_Utilities \
		TA_Version \
		TA_RightsLibrary \
		TA_SecurityUtilities \
		TA_Naming \
		TA_Thread \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_EventTriggeredPlans \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef \
		TA_Alarm \
		TA_Utilities \
		TA_Message \
		TA_Timers \
                TA_EventProcessing \
				TA_Event \
				TA_EventAgentCorbaDef \
		TA_CorbaDef \
		TA_NamingCorbaDef \
		TA_AlarmAgentCorbaDef \
                TA_EntityStatusUpdateCorbaDef \
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_Synchronisation \
		TA_ConfigurationUpdates \
		TA_DutyAgentState \
		TA_DutyAgentCorbaDef \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_AuthenticationLibrary \
		TA_DAI \
                TA_AlarmRuleDAI \
		TA_DerivedDataPointDAI \
                TA_PlanDAI \
		TA_EntityAccess \
		TA_DataAccess \
		TA_Uuid \
		TA_Utilities \
		TA_Exceptions \
		TA_CommonExceptionsCorbaDef \
		TA_ocl \
		TA_mcl \
		ACE \
		mysqlclient \
		clntsh
             
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
		$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
		$(PROJ_ROOT_DIR)/code/transactive/core/threads \
		$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/message \
		$(PROJ_ROOT_DIR)/code/transactive/core/database \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/duty_agent_state \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
		$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/event_processing \
		$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
