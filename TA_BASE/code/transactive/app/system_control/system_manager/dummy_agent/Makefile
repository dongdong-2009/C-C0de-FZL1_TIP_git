#The source code in this file is the property of
#Ripple Systems and is not for redistribution in any form.
 
#Source:    $DateTime: 2012/06/12 13:35:44 $
#@author:   Justin Ebedes
#@version:  $Revision: #1 $

#Last modification: $DateTime: 2012/06/12 13:35:44 $
#Last modified by:  $Author: builder $


PROJ_ROOT_DIR	= ../../../..
PROJ_NAME	= DummyAgent


PROJ_SOURCE	= TestAgent.cpp ProcessIdResolver.cpp ManagedAgentCorbaDef.cpp VersionGen.cpp
		  
PROJ_TYPE	= exe

PROJ_OBJDIR	= ./obj
PROJ_INCDIR	= ./src

PROJ_LIBDIRS  = $(PROJ_ROOT_DIR)/core/exceptions/obj \
	        $(PROJ_ROOT_DIR)/core/versioning/obj \
   	        $(PROJ_ROOT_DIR)/core/synchronisation/obj \
   	        $(PROJ_ROOT_DIR)/core/utilities/obj \
		  $(PROJ_ROOT_DIR)/core/uuid/obj \
	        $(PROJ_ROOT_DIR)/core/corba/obj \
	        $(PROJ_ROOT_DIR)/core/corba/IDL/obj \
		  $(PROJ_ROOT_DIR)/core/logging/obj \
	        $(PROJ_ROOT_DIR)/core/data_access_interface/obj \
	        $(PROJ_ROOT_DIR)/core/data_access_interface/system_controller/obj \
	        $(PROJ_ROOT_DIR)/core/data_access_interface/entity_access/obj \
	        $(PROJ_ROOT_DIR)/core/threads/obj \
	        $(PROJ_ROOT_DIR)/core/naming/obj \
	        $(PROJ_ROOT_DIR)/core/naming/IDL/obj \
	        $(PROJ_ROOT_DIR)/core/message/obj \
	        $(PROJ_ROOT_DIR)/core/message/IDL/obj \
		  $(PROJ_ROOT_DIR)/core/alarm/obj \
		  $(PROJ_ROOT_DIR)/core/alarm/IDL/obj \
	        $(PROJ_ROOT_DIR)/core/database/obj \
	        $(PROJ_ROOT_DIR)/core/process_management/obj \
	        $(PROJ_ROOT_DIR)/core/process_management/IDL/obj \

PROJ_LIBS     = TA_ProcessManagementCorbaDef \
		TA_CorbaUtil \
		TA_Naming \
		TA_NamingCorbaDef \
		TA_Message \
		TA_MessageCorbaDef \
		TA_LoggingLibrary \
		TAuuid \
		TA_CorbaDef \
		TA_EntityAccess \
		TA_DataAccessInterface \
		DataAccess \
		TA_Utilities \
		TA_Synchronisation \
		TA_Thread \
		TA_Version \
		TA_Exceptions \
		clntsh \
                ACE
		             
PROJ_INCDIRS  = $(PROJ_ROOT_DIR) \
		    $(PROJ_ROOT_DIR)/core/process_management/IDL/src

DEPENDENT_LIBRARIES =	$(PROJ_ROOT_DIR)/core/exceptions \
				$(PROJ_ROOT_DIR)/core/process_management/IDL \
				$(PROJ_ROOT_DIR)/core/threads \
				$(PROJ_ROOT_DIR)/core/synchronisation \
				$(PROJ_ROOT_DIR)/core/utilities \
				$(PROJ_ROOT_DIR)/core/corba \
				$(PROJ_ROOT_DIR)/core/message \
				$(PROJ_ROOT_DIR)/core/alarm \
				$(PROJ_ROOT_DIR)/core/versioning \
				$(PROJ_ROOT_DIR)/core/naming \

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak


install:
        #Do Nothing
