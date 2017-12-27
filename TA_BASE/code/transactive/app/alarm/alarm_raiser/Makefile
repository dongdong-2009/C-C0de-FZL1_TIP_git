# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_raiser/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/alarm/alarm_raiser
PROJ_NAME	= AlarmRaiser
PROJ_SOURCE	= main.cpp VersionGen.cpp
PROJ_TYPE	= exe

PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


PROJ_LIBS     = TA_CorbaUtil TA_CorbaDef \
		TA_AuthenticationLibrary \
                TA_AuthenticationCorbaDef \
		TA_Naming \
                TA_NamingCorbaDef \
		TA_Message \
		TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_ConfigurationUpdates \
		TA_Alarm \
                TA_AlarmAgentCorbaDef \
                TA_EntityStatusUpdateCorbaDef \
                TA_MessageCorbaDef \
                TA_EntityAccess \
                TA_DAI \
		TA_DerivedDataPointDAI \
                TA_AlarmRuleDAI \
                TA_PlanDAI \
                TA_Utilities \
		TA_Synchronisation \
                TA_Thread \
                TA_DataAccess \
		TA_ocl \
		TA_mcl \
                TA_Uuid \
		TA_Exceptions \
                TA_CommonExceptionsCorbaDef \
		TA_ProcessManagementCorbaDef \
                TA_CorbaDef \
                TA_Version \
		ACE \
		clntsh  \
		mysqlclient 

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
			$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
			$(PROJ_ROOT_DIR)/code/transactive/core/threads \
			$(PROJ_ROOT_DIR)/code/transactive/core/message \
			$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/corba \
			$(PROJ_ROOT_DIR)/code/transactive/core/database \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
			$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
			$(PROJ_ROOT_DIR)/code/transactive/core/event_processing \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
			$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
			$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
			$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation/

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
