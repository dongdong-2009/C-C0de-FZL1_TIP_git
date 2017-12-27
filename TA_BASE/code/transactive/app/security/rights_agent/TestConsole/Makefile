# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_agent/TestConsole/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#

PROJ_ROOT_DIR = ../../../..
PROJ_NAME     = RightsAgentTestConsole
PROJ_SOURCE   = VersionGen.cpp \
				main.cpp

PROJ_TYPE     = exe
PROJ_OBJDIR   = ./obj
PROJ_INCDIR   = ./src
PROJ_LIBDIRS  =	$(PROJ_ROOT_DIR)/core/data_access_interface/obj \
				$(PROJ_ROOT_DIR)/core/database/obj \
				$(PROJ_ROOT_DIR)/core/data_access_interface/entity_access/obj \
				$(PROJ_ROOT_DIR)/core/process_management/obj \
				$(PROJ_ROOT_DIR)/core/process_management/IDL/obj \
				$(PROJ_ROOT_DIR)/core/synchronisation/obj \
				$(PROJ_ROOT_DIR)/core/threads/obj \
				$(PROJ_ROOT_DIR)/core/utilities/obj \
				$(PROJ_ROOT_DIR)/core/versioning/obj \
				$(PROJ_ROOT_DIR)/core/corba_named_obj/obj \
				$(PROJ_ROOT_DIR)/core/corba/obj \
				$(PROJ_ROOT_DIR)/core/corba/IDL/obj \
				$(PROJ_ROOT_DIR)/core/exceptions/obj \
				$(PROJ_ROOT_DIR)/core/message/obj \
				$(PROJ_ROOT_DIR)/core/message/IDL/obj \
				$(PROJ_ROOT_DIR)/core/naming/obj \
				$(PROJ_ROOT_DIR)/core/naming/IDL/obj \
				$(PROJ_ROOT_DIR)/core/uuid/obj \
				$(PROJ_ROOT_DIR)/bus/security/rights_library/obj \
				$(PROJ_ROOT_DIR)/bus/security/security_utilities/obj \
				$(PROJ_ROOT_DIR)/bus/security/authentication_agent/IDL/obj \
				$(PROJ_ROOT_DIR)/bus/security/rights_agent/IDL/obj \
				$(PROJ_ROOT_DIR)/bus/generic_agent/obj


PROJ_LIBS =	TA_RightsLibrary \
			TA_AuthenticationLibrary \
            TA_SecurityUtilities \
            TA_AuthenticationCorbaDef \
			TA_RightsAgentCorbaDef \
			TA_CorbaUtil \
			TA_Synchronisation \
			TA_Utilities \
			TA_Version \
			TA_Thread \
			NamedObjRef \
			TA_Naming \
			TA_EntityAccess \
			TA_Utilities \
			TA_Message \
			TA_DataAccessInterface \
			TA_CorbaDef \
			TA_NamingCorbaDef \
			TA_MessageCorbaDef \
			TA_GenericAgent \
			TA_EventTriggeredPlans \
			TA_PlanAgentLibrary \
			TA_PlanAgentCorbaDef \
			TA_PlanDAI \
			TA_PlanAgentDataCorbaDef \
			TA_PlanManagerCorbaDef \
			TA_ProcessManagement \
			TA_ProcessManagementCorbaDef \
			DataAccess \
			TA_Exceptions \
			TAuuid \
			clntsh

PROJ_IDL_INC  = $(PROJ_ROOT_DIR)

PROJ_INCDIRS  = $(PROJ_ROOT_DIR)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/bus/security/rights_library \
					  $(PROJ_ROOT_DIR)/bus/security/security_utilities \
					  $(PROJ_ROOT_DIR)/bus/security/authentication_agent/IDL

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak

install:
	#Do Nothing
