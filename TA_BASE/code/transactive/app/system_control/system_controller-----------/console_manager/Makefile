#
# $DateTime: 2012/06/12 13:35:44 $
# $Revision: #1 $
# $DateTime: 2012/06/12 13:35:44 $ 
# $Author: builder $
#
# Make file for the ConsoleManager.
#
# History
#
#
PROJ_ROOT_DIR   = ../../../../../..
PROJ_CODE_DIR	= transactive/app/system_control/system_controller/console_manager
PROJ_NAME		= ConsoleManager 
PROJ_SOURCE		= ProcessManager.cpp \
                          ConsoleManager.cpp \
                          VersionGen.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR    = ./src \
                $(PROJ_ROOT_DIR)/code/transactive \
                $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/transactive \
                $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/transactive/core/process_management/IDL/src
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)
PROJ_LIBS	= \
	TA_AuthenticationLibrary \
        TA_AuthenticationCorbaDef \
	TA_ProcessManagementCorbaDef \
	TA_Alarm \
	TA_AlarmAgentCorbaDef \
        TA_EntityStatusUpdateCorbaDef \
        TA_DAI \
        TA_AlarmRuleDAI \
	TA_Naming \
	TA_Message \
	TA_Uuid \
        TA_EventProcessing \
	TA_ConfigurationUpdates \
	TA_MessageCorbaDef \
	TA_NamingCorbaDef \
	TA_CorbaUtil \
        TA_Version \
        TA_ProcessManagementCorbaDef \
	TA_CorbaDef \
	TA_EntityAccess \
	TA_DAI \
	TA_DataAccess \
	TA_Utilities \
	TA_Synchronisation \
	TA_Thread \
	TA_Version \
	TA_Exceptions \
	TA_CommonExceptionsCorbaDef \
	TA_DsmCorbaDef \
	TA_Dsm \
	clntsh \
        ACE

#
# Libraries that this library is dependent upon.
#
# DEPENDENT_LIBRARIES
DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
            	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
		$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming \
		$(PROJ_ROOT_DIR)/code/transactive/core/message \
		$(PROJ_ROOT_DIR)/code/transactive/core/database \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
		$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
		$(PROJ_ROOT_DIR)/code/transactive/core/threads \
		$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
		$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
#The source code in this file is the property of
#Ripple Systems and is not for redistribution in any form.
