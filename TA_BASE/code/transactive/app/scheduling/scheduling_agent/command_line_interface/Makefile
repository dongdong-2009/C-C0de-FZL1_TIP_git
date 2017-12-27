# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/command_line_interface/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $

PROJ_ROOT_DIR	= ../../../..
PROJ_NAME	= SchedulingAgentCLI


PROJ_SOURCE	= main.cpp \
		  CommandLine.cpp \
		  SchedulingAgentCLI.cpp \
		  VersionGen.cpp \
		  ../../src/SchedulingTimer.cpp
		  
		  
PROJ_TYPE	= exe

PROJ_OBJDIR	= ./obj
PROJ_INCDIR	= ./src

PROJ_LIBDIRS  = $(PROJ_ROOT_DIR)/core/exceptions/obj \
	        $(PROJ_ROOT_DIR)/core/alarm/IDL/obj \
	        $(PROJ_ROOT_DIR)/core/alarm/obj \
	        $(PROJ_ROOT_DIR)/core/versioning/obj \
   	        $(PROJ_ROOT_DIR)/core/synchronisation/obj \
   	        $(PROJ_ROOT_DIR)/core/utilities/obj \
	        $(PROJ_ROOT_DIR)/core/corba/obj \
	        $(PROJ_ROOT_DIR)/core/corba/IDL/obj \
	        $(PROJ_ROOT_DIR)/core/data_access_interface/obj \
	        $(PROJ_ROOT_DIR)/core/data_access_interface/entity_access/obj \
	        $(PROJ_ROOT_DIR)/core/data_access_interface/plans/obj \
	        $(PROJ_ROOT_DIR)/core/data_access_interface/alarm_rule/obj \
	        $(PROJ_ROOT_DIR)/core/configuration_updates/obj \
	        $(PROJ_ROOT_DIR)/core/threads/obj \
	        $(PROJ_ROOT_DIR)/core/naming/obj \
	        $(PROJ_ROOT_DIR)/core/naming/IDL/obj \
	        $(PROJ_ROOT_DIR)/core/utilities/obj \
	        $(PROJ_ROOT_DIR)/core/message/obj \
	        $(PROJ_ROOT_DIR)/core/message/IDL/obj \
	        $(PROJ_ROOT_DIR)/core/database/obj \
	        $(PROJ_ROOT_DIR)/core/process_management/obj \
	        $(PROJ_ROOT_DIR)/core/process_management/IDL/obj \
	        $(PROJ_ROOT_DIR)/bus/security/authentication_agent/IDL/obj \
	        $(PROJ_ROOT_DIR)/bus/security/authentication_library/obj \
	        $(PROJ_ROOT_DIR)/bus/security/rights_agent/IDL/obj \
	        $(PROJ_ROOT_DIR)/bus/security/rights_library/obj \
	        $(PROJ_ROOT_DIR)/bus/security/security_utilities/obj \
	        $(PROJ_ROOT_DIR)/bus/generic_agent/obj \
	        $(PROJ_ROOT_DIR)/core/alarm/obj \
	        $(PROJ_ROOT_DIR)/core/alarm/IDL/obj \
	        $(PROJ_ROOT_DIR)/bus/response_plans/plan_agent/obj \
	        $(PROJ_ROOT_DIR)/bus/response_plans/plan_agent/IDL/obj \
	        $(PROJ_ROOT_DIR)/bus/response_plans/plan_manager/IDL/obj \
	        $(PROJ_ROOT_DIR)/core/logging/obj \
	        $(PROJ_ROOT_DIR)/core/uuid/obj \
	        $(PROJ_ROOT_DIR)/app/scheduling/scheduling_agent/IDL/obj

PROJ_LIBS     = TA_CorbaUtil \
		TA_RightsLibrary \
		TA_SecurityUtilities \
		TA_RightsAgentCorbaDef \
		TA_GenericAgent \
		TA_EventTriggeredPlans \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanDAI \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef \
		TA_Alarm \
		TA_AlarmAgentCorbaDef \
		TA_AuthenticationLibrary \
		TA_AuthenticationCorbaDef \
		TA_ConfigurationUpdates \
		TA_Naming \
		TA_Message \
		TA_LoggingLibrary \
		TA_CorbaDef \
		TA_NamingCorbaDef \
		TA_MessageCorbaDef \
		TA_ProcessManagement \
		TA_ProcessManagementCorbaDef \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanDAI \
		TA_EntityAccess \
		TA_PlanManagerCorbaDef \
		TA_DataAccessInterface \
		DataAccess \
		TA_Synchronisation \
		TA_Thread \
		TA_Utilities \
		TA_Version \
		TAuuid \
		TA_Exceptions \
		TA_SchedulingAgentCorbaDef \
		clntsh \
                ACE
             
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)

PROJ_INCDIRS  = $(PROJ_ROOT_DIR)
		

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/core/exceptions \
                      $(PROJ_ROOT_DIR)/core/versioning \
	              $(PROJ_ROOT_DIR)/bus/generic_agent/ \
	              $(PROJ_ROOT_DIR)/core/naming/ \
	              $(PROJ_ROOT_DIR)/core/naming/IDL \
	              $(PROJ_ROOT_DIR)/core/alarm/IDL \
	              $(PROJ_ROOT_DIR)/core/alarm/ \
	              $(PROJ_ROOT_DIR)/bus/response_plans/plan_agent/IDL \
	              $(PROJ_ROOT_DIR)/bus/response_plans/plan_agent \
	              $(PROJ_ROOT_DIR)/bus/response_plans/plan_manager/IDL \
	              $(PROJ_ROOT_DIR)/bus/security/authentication_agent/IDL \
	              $(PROJ_ROOT_DIR)/core/data_access_interface/plans \
	              $(PROJ_ROOT_DIR)/core/data_access_interface \
	              $(PROJ_ROOT_DIR)/core/database \
	              $(PROJ_ROOT_DIR)/bus/security/authentication_library \
	              $(PROJ_ROOT_DIR)/bus/security/rights_agent/IDL \
	              $(PROJ_ROOT_DIR)/bus/security/rights_library \
	              $(PROJ_ROOT_DIR)/bus/security/security_utilities \
	              $(PROJ_ROOT_DIR)/core/configuration_updates \
	              $(PROJ_ROOT_DIR)/app/scheduling/scheduling_agent/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak


install:
        #Do Nothing
