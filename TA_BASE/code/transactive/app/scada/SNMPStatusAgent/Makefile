#The source code in this file is the property of
#Ripple Systems and is not for redistribution in any form.
#Source:    $Source: /cvs/1TS-9999-T/code/transactive/app/scada/SNMPStatusAgent/Attic/Makefile,v $
#@author:   HoaVu
#@version:  $Revision: #1 $
#Last modification: $Date: 2014/08/12 $
#Last modified by:  $Author: haijun.li $

PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/app/scada/SNMPStatusAgent
PROJ_NAME       = SNMPStatusAgent

PROJ_SOURCE     = SNMPStatusAgentMain.cpp \
				SNMPStatusAgent.cpp \
				SNMPIOManager.cpp \
				VersionGen.cpp

PROJ_TYPE       = exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive \
	$(PROJ_ROOT_DIR)/code/cots/SNMP_PP/snmp_pp_v3.2.16/include
	
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
				TA_CorbaUtil \
		TA_SNMPLib \
		TA_IOManagerLib \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_DataPointAgentCorbaDef_Impl \
		TA_ScadaProxyLibrary \
		TA_DataNodeLibrary \
		TA_DataPointLibrary \
		TA_DerivedDataPointDAI \
		TA_ScadaCommonLib \
		TA_ScadaCommonCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_RightsLibrary \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_AuthenticationLibrary \
		TA_SecurityUtilities \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_AuthenticationCorbaDef \
		TA_Timers \
		TA_Thread \
		TA_Naming \
		TA_Message \
		TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_Uuid \
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
		TA_EntityAccess \
		TA_DerivedDataPointDAI \
		TA_DataAccess \
		TA_CorbaDef \
		TA_NamingCorbaDef \
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_DutyAgentCorbaDef \
		TA_DAI \
		TA_AlarmRuleDAI \
		TA_DataAccess \
		TA_ConfigurationUpdates \
		TA_MathEvaluation \
		TA_Utilities \
		TA_Exceptions \
		TA_CommonExceptionsCorbaDef \
		TA_Version \
		TA_Synchronisation \
		TA_ocl \
		TA_mcl \
		clntsh \
		mysqlclient \
		ACE \
		snmp++

PROJ_LIB_DIRS = $(PROJ_ROOT_DIR)/code/cots/SNMP_PP/snmp_pp_v3.2.16/lib 

DEPENDENT_LIBRARIES =	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
		$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
		$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
		$(PROJ_ROOT_DIR)/code/transactive/core/threads \
		$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/corba \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming \
		$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/message \
		$(PROJ_ROOT_DIR)/code/transactive/core/database \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/derived_datapoints \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
		$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
		$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataNodeAgentCorbaDef_Impl \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataPointAgentCorbaDef_Impl \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/proxy_library \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
		$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/history/data_recording \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/data_recording_dai \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
		$(PROJ_ROOT_DIR)/code/transactive/core/timers \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/io_manager \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/SNMP_library
      
PROJ_CFLAGS = -DTA_ASSERT_LOG=1 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

LFLAGS += -L$(PROJ_ROOT_DIR)/code/cots/SNMP_PP/snmp_pp_v3.2.16/lib

install:
        #Do Nothing
