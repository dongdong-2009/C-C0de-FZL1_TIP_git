# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/datalog/datalog_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2010/12/17 15:51:31 $
# Last modified by:  $Author: huirong.luo $


PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/datalog/datalog_agent
PROJ_NAME	= DataLogAgent
PROJ_SOURCE	= DataLogAgent.cpp main.cpp \
		  VersionGen.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = TA_CommonExceptionsCorbaDef \
		TA_DataLogLibrary \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_DataPointAgentCorbaDef_Impl \
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
		TA_EventTriggeredPlans \
		TA_ConfigurationUpdates \
		TA_Naming \
		TA_Message \
		TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_CorbaDef \
		TA_Alarm \
                            TA_PlanAgentLibrary \
		TA_ManagedProcess \
                            TA_CorbaUtil \
		TA_AlarmAgentCorbaDef \
		TA_DataLogAgentCorbaDef \
		TA_EntityStatusUpdateCorbaDef \
		TA_MmsCorbaDef \
		TA_NamingCorbaDef \
		TA_PlanAgentCorbaDef \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef\
		TA_MessageCorbaDef \
		TA_ProcessManagementCorbaDef \
		TA_PlanDAI \
		TA_EntityAccess \
		TA_DAI \
		TA_AlarmRuleDAI \
		TA_DataLogDAI \
		TA_DerivedDataPointDAI \
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
		ACE
             
DEPENDENT_LIBRARIES = \
 $(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
 $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
 $(PROJ_ROOT_DIR)/code/transactive/core/corba \
 $(PROJ_ROOT_DIR)/code/transactive/core/threads \
 $(PROJ_ROOT_DIR)/code/transactive/core/utilities \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/datalog \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/database \
 $(PROJ_ROOT_DIR)/code/transactive/core/message \
 $(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
 $(PROJ_ROOT_DIR)/code/transactive/core/uuid \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
 $(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/process_management \
 $(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent \
 $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/naming \
 $(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/alarm \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_manager/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
 $(PROJ_ROOT_DIR)/code/transactive/core/datalog/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
 $(PROJ_ROOT_DIR)/code/transactive/bus//history/data_recording \
 $(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/data_recording_dai \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/derived_datapoints \
 $(PROJ_ROOT_DIR)/code/transactive/core/timers \
 $(PROJ_ROOT_DIR)/code/transactive/core/ocl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/proxy_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataNodeAgentCorbaDef_Impl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataPointAgentCorbaDef_Impl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/datalog/datalog_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/mathematical_evaluation

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
