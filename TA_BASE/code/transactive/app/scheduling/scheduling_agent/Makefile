# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/scheduling/scheduling_agent
PROJ_NAME	= SchedulingAgent
PROJ_SOURCE	= AbstractJob.cpp \
			AbstractJobInstance.cpp \
			JobFactory.cpp \
			JobInstanceManager.cpp \
			JobQueue.cpp \
			JobRunner.cpp \
			JobThread.cpp \
			main.cpp \
			PlanAgentMap.cpp \
			PlanAgentLocationAccess.cpp \
			PlanRunnerJob.cpp \
			PlanRunnerJobFactory.cpp \
			PlanRunnerJobInstance.cpp \
			AlarmGeneratorJob.cpp \
			AlarmGeneratorJobInstance.cpp \
			AlarmGeneratorJobFactory.cpp \
			Schedule.cpp \
			SchedulingAccessManager.cpp \
			SchedulingAgent.cpp \
			SchedulingAlarmSender.cpp \
			SchedulingController.cpp \
			SchedulingMessageSender.cpp \
			SchedulingMonitor.cpp \
			SchedulingSessionManager.cpp \
			SchedulingTimer.cpp \
			VersionGen.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = TA_CorbaUtil \
		TA_AuthenticationCorbaDef \
		TA_RightsLibrary \
                TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_RightsAgentCorbaDef \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_EventTriggeredPlans \
                TA_CorbaDef \
		TA_Alarm \
		TA_AlarmAgentCorbaDef \
                TA_EntityStatusUpdateCorbaDef \
		TA_AuthenticationLibrary \
		TA_AuthenticationCorbaDef \
		TA_ConfigurationUpdates \
		TA_Naming \
		TA_Message \
                TA_EventProcessing \
				TA_Event \
				TA_EventAgentCorbaDef \
		TA_CorbaDef \
		TA_NamingCorbaDef \
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanAgentDataCorbaDef \
                TA_DAI \
                TA_AlarmRuleDAI \
		TA_DerivedDataPointDAI \
		TA_PlanDAI \
		TA_EntityAccess \
		TA_PlanManagerCorbaDef \
		TA_DAI \
		TA_DataAccess \
		TA_Thread \
		TA_Utilities \
		TA_Version \
		TA_Uuid \
		TA_Exceptions \
		TA_SchedulingAgentDAI \
		TA_SchedulingAgentCorbaDef \
		TA_CommonExceptionsCorbaDef \
		TA_Synchronisation \
		clntsh \
		mysqlclient \
		TA_ocl \
		TA_mcl \
                ACE
             
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
        $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
        $(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent/ \
        $(PROJ_ROOT_DIR)/code/transactive/core/naming/ \
        $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/core/alarm/ \
        $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
        $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_manager/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/core/database \
        $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
        $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
        $(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
        $(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
        $(PROJ_ROOT_DIR)/code/transactive/bus/scheduling/SchedulingAgentCorbaDef/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/scheduling_agent

# Use this line to disable access control. ie when a request is made to
# change the state of the agent through CORBA, security will not be checked
#
#PROJ_CFLAGS = -DNO_ACCESS_CONTROL

# If this is uncommented, the Scheduling Agent will NOT show jobs including the
# ones for Alarm Generation.
#PROJ_CFLAGS = -DHIDE_ALARM_GENERATOR_JOB

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
