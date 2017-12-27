# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $


PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/security/authentication_agent
PROJ_NAME	= AuthenticationAgent
PROJ_SOURCE	= main.cpp  \
              	  Authentication.cpp \
              	  AuthenticationAgent.cpp \
              	  AuthenticationAgentImpl.cpp \
		  SessionQueue.cpp \
		  SessionQueueProcessor.cpp \
		  QueueStateUpdater.cpp \
              	  SessionRegister.cpp \
              	  UserCache.cpp \
              	  VersionGen.cpp \
	      	  DeadSessionCheckerThread.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
                TA_AuthenticationLibrary \
                TA_GenericAgent \
				TA_Bus_Event \
				TA_AuthenticationLibrary \
		TA_EventTriggeredPlans \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanDAI \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef \
                TA_CorbaDef \
                TA_Alarm \
                TA_AlarmAgentCorbaDef \
                TA_EntityStatusUpdateCorbaDef \
                TA_ConfigurationUpdates \
                TA_CorbaUtil \
                TA_ManagedProcess \
                TA_ProcessManagementCorbaDef \
                TA_DAI \
                TA_AlarmRuleDAI \
		TA_DerivedDataPointDAI \
                TA_EntityAccess \
                TA_Naming \
                TA_Thread \
                TA_Message \
                TA_EventProcessing \
				TA_Event \
				TA_EventAgentCorbaDef \
                TA_CorbaDef \
                TA_Uuid \
                TA_DAI \
                TA_DataAccess \
                TA_MessageCorbaDef \
                TA_NamingCorbaDef \
                TA_Utilities \
                TA_Exceptions \
                TA_DutyAgentCorbaDef \
                TA_RightsAgentCorbaDef \
		TA_mcl \
		TA_ocl \
                TA_Version \
                TA_DutyAgentCorbaDef \
                TA_AuthenticationCorbaDef \
		TA_CommonExceptionsCorbaDef \
                TA_Synchronisation \
		mysqlclient \
		clntsh \
		ACE
             
DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
	        $(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent/ \
                $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
	        $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
	        $(PROJ_ROOT_DIR)/code/transactive/core/message \
   	        $(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
                $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
                $(PROJ_ROOT_DIR)/code/transactive/core/alarm

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
