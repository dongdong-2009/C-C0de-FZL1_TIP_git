# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#
# Makefile for the Online Printing Agent
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/events/online_printing_agent
PROJ_NAME	= OnlinePrintingAgent
PROJ_SOURCE	= \
                ConfigData.cpp \
                OnlinePrintingAgent.cpp \
                OnlinePrintingAgentException.cpp \
                OnlinePrintingAgentUser.cpp \
                PrintableData.cpp \
                PrintableEventGenerator.cpp \
                PrintAlarmSender.cpp \
                PrintingService.cpp \
                PrintJobSpooler.cpp \
                PrintMO.cpp \
                PrintOutputFormatter.cpp \
                StateUpdateSender.cpp \
                VersionGen.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
PROJ_LIBS       = \
		TA_CorbaUtil \
		TA_Version \
		TA_Utilities \
		TA_Message \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_AuthenticationLibrary \
		TA_EventTriggeredPlans \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanDAI \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef \
		TA_Message \
		TA_PrintDai \
		TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_EntityAccess \
		TA_Alarm \
		TA_Naming \
		TA_Thread \
		TA_DAI \
		TA_Version \
		TA_Utilities \
		TA_CorbaDef \
		TA_AlarmAgentCorbaDef \
                TA_EntityStatusUpdateCorbaDef \
                TA_AlarmRuleDAI \
		TA_DerivedDataPointDAI \
		TA_NamingCorbaDef \
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_DataAccess \
		TA_Synchronisation \
		TA_ConfigurationUpdates \
		TA_SecurityUtilities \
		TA_Version \
		TA_Utilities \
		TA_Exceptions \
		TA_RightsAgentCorbaDef \
		TA_AuthenticationCorbaDef \
		TA_DutyAgentCorbaDef \
		TA_CommonExceptionsCorbaDef \
		TA_Uuid \
		TA_ocl \
		TA_mcl \
		ACE \
		clntsh \
		mysqlclient
             
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
            	$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
            	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
                $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
		$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
		$(PROJ_ROOT_DIR)/code/transactive/core/threads \
		$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming \
		$(PROJ_ROOT_DIR)/code/transactive/core/message \
		$(PROJ_ROOT_DIR)/code/transactive/core/database \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
		$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
		$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
		$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent/ \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/online_printing_agent \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
