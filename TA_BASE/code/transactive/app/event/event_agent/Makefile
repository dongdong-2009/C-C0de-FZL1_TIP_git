# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #4 $
#
# Last modification: $DateTime: 2012/07/24 11:03:41 $
# Last modified by:  $Author: Shiva $


PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/event/event_agent
PROJ_NAME	= EventAgent
PROJ_SOURCE	= EventAgent.cpp \
			main.cpp \
			EventCache.cpp \
			EventAgentImpl.cpp \
			DatabaseLoader.cpp \
			VersionGen.cpp \
			AgentStateController.cpp \
			DbEventRetriever.cpp \
			PropagateActorCentral.cpp \
			PropagateActorLocal.cpp \
			EventDataProcessor.cpp \
			ReceiveEventCache.cpp
						
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS   = TA_VideoSwitchAgentAccess \
		TA_VideoSwitchAgentCorbaDef \
		TA_CommonExceptionsCorbaDef \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_DataPointAgentCorbaDef_Impl \
		TA_MMSHelp \
		TA_CorbaUtil \
		TA_ScadaCommonLib \
		TA_ScadaProxyLibrary \
		TA_DataNodeLibrary \
		TA_DataPointLibrary \
		TA_ScadaCommonLib \
		TA_ScadaCommonCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_RightsLibrary \
		TA_SecurityUtilities \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_RightsAgentCorbaDef \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_AuthenticationLibrary \
		TA_AuthenticationCorbaDef \
		TA_DerivedDataPointDAI \
		TA_EventTriggeredPlans \
		TA_ConfigurationUpdates \
		TA_Naming \
		TA_NamingCorbaDef \
		TA_Message \
		TA_EventProcessing \
		TA_CorbaDef \
		TA_Alarm \
		TA_AlarmAgentCorbaDef \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_EntityStatusUpdateCorbaDef \
		TA_MmsCorbaDef \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef\
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_PlanDAI \
		TA_EntityAccess \
		TA_DAI \
		TA_AlarmRuleDAI \
		TA_Timers \
		TA_Thread \
		TA_MathEvaluation \
		TA_Utilities \
		TA_Version \
		TA_DataAccess \
		TA_Uuid \
		TA_Exceptions \
		TA_Synchronisation \
		TA_ocl \
		TA_mcl \
		clntsh \
		mysqlclient \
		zlib \
		ACE
        
DEPENDENT_LIBRARIES = \
 $(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
 $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
 $(PROJ_ROOT_DIR)/code/transactive/core/corba \
 $(PROJ_ROOT_DIR)/code/transactive/core/threads \
 $(PROJ_ROOT_DIR)/code/transactive/core/utilities \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/database \
 $(PROJ_ROOT_DIR)/code/transactive/core/message \
 $(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
 $(PROJ_ROOT_DIR)/code/transactive/core/uuid \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
 $(PROJ_ROOT_DIR)/code/transactive/core/process_management \
 $(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent/ \
 $(PROJ_ROOT_DIR)/code/transactive/core/naming/ \
 $(PROJ_ROOT_DIR)/code/transactive/core/event/ \
 $(PROJ_ROOT_DIR)/code/transactive/core/event/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/alarm/ \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_manager/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
 $(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsHelper \
 $(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/agent_access \
 $(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/agent_access \
 $(PROJ_ROOT_DIR)/code/transactive/bus//history/data_recording \
 $(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/data_recording_dai \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/derived_datapoints \
 $(PROJ_ROOT_DIR)/code/transactive/core/timers \
 $(PROJ_ROOT_DIR)/code/transactive/core/ocl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/proxy_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataNodeAgentCorbaDef_Impl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataPointAgentCorbaDef_Impl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/mathematical_evaluation
 

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak
STD_LIB_DIRS += $(PROJ_ROOT_DIR)/code/cots/ZipArchive/zlib

install:
        #Do Nothing
