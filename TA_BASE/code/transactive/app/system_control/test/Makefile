#The source code in this file is the property of
#Ripple Systems and is not for redistribution in any form.
 
#Source:    $DateTime: 2012/06/12 13:35:44 $
#@author:   Justin Ebedes
#@version:  $Revision: #1 $

#Last modification: $DateTime: 2012/06/12 13:35:44 $
#Last modified by:  $Author: builder $


PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/system_control/system_controller
PROJ_NAME	= SystemController
PROJ_SOURCE	= main.cpp SystemControllerMonitor.cpp PeersMonitor.cpp GroupMonitor.cpp \
		  PeerSCInfo.cpp ProcessManager.cpp PeerProcessManager.cpp ProcessInfo.cpp \
		  ManagedProcessInfo.cpp ProcessUtilities.cpp SignalEventHandler.cpp \
		  SystemControllerAdmin.cpp SystemController.cpp ShutdownExecutor.cpp VersionGen.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= \
	TA_Bus_Event \
	TA_AuthenticationLibrary \
	TA_AuthenticationCorbaDef \
	TA_ProcessManagementCorbaDef \
        TA_PlanAgentLibrary \
        TA_PlanAgentCorbaDef \
        TA_PlanAgentDataCorbaDef \
        TA_PlanManagerCorbaDef \
        TA_Alarm \
	TA_AlarmAgentCorbaDef \
        TA_EntityStatusUpdateCorbaDef \
        TA_DAI \
        TA_AlarmRuleDAI \
        TA_PlanDAI \
	TA_Uuid \
	TA_Naming \
	TA_Message \
        TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
	TA_ConfigurationUpdates \
	TA_MessageCorbaDef \
	TA_NamingCorbaDef \
	TA_CorbaUtil \
	TA_CorbaDef \
	TA_EntityAccess \
	TA_SystemControllerDAI \
	TA_DAI \
	TA_DsmCorbaDef \
	TA_Dsm \
	TA_DataAccess \
	TA_Timers \
	TA_Utilities \
	TA_Synchronisation \
	TA_Thread \
	TA_Version \
	TA_Exceptions \
	TA_CommonExceptionsCorbaDef \
	TA_mcl \
	TA_ocl \
	mysqlclient \
	clntsh \
        ACE
		             
DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/system_controller \
	$(PROJ_ROOT_DIR)/code/transactive/core/threads \
	$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
	$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
	$(PROJ_ROOT_DIR)/code/transactive/core/corba \
	$(PROJ_ROOT_DIR)/code/transactive/core/message \
	$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
	$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
	$(PROJ_ROOT_DIR)/code/transactive/bus/database_status_monitor \

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
