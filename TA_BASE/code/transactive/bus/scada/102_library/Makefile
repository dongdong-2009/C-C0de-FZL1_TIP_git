# Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/102_library/Makefile $
# @author:  haijun.li
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/21 14:56:06 $
# Last modified by:  $Author: haijun.li $
#
# This file builds the 102_library.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/102_library
PROJ_NAME	= TA_IEC102Library
PROJ_SOURCE	=	IEC102DataStruct.cpp \
				IEC102ControllerFactory.cpp \
				IEC102Library.cpp \
				ChannelManager.cpp \
				Channel.cpp \
				IEC102Protocol172.cpp \
				IEC102Protocol171.cpp \
				IEC102Protocol120.cpp \
				IEC102ProtocolInit.cpp \
				IEC102Protocol.cpp \
				IEC102FlowController.cpp \
				IEC102Controller120.cpp \
				IEC102Controller171.cpp \
				IEC102Controller172.cpp \
				IEC102ControllerInit.cpp \
				HexHelper.cpp \
				VersionGen.cpp 
		  	  

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
PROJ_LIBS     = \
				TA_CorbaUtil \
				TA_DataPointAgentCorbaDef_Impl \
				TA_DataNodeAgentCorbaDef_Impl \
				TA_AlarmRuleDAI \
				TA_PlanDAI \
				TA_GenericAgent \
				TA_EventTriggeredPlans \
				TA_PlanAgentLibrary \
				TA_PlanAgentCorbaDef \
				TA_PlanDAI \
				TA_PlanAgentDataCorbaDef \
				TA_PlanManagerCorbaDef \
				TA_ScadaProxyLibrary \
				TA_DataNodeLibrary \
				TA_DataPointLibrary \
				TA_DerivedDataPointDAI \
				TA_ScadaCommonLib \
				TA_ScadaCommonCorbaDef \
				TA_DataRecordingDAI \
				TA_MmsCorbaDef \
				TA_Alarm \
				TA_RightsLibrary \
				TA_AuthenticationLibrary \
				TA_SecurityUtilities \
				TA_AuthenticationCorbaDef \
				TA_RightsAgentCorbaDef \
				TA_DutyAgentCorbaDef \
				TA_Timers \
				TA_Thread \
				TA_Naming \
				TA_AlarmAgentCorbaDef \
				TA_EntityStatusUpdateCorbaDef \
				TA_Uuid \
				TA_Message \
				TA_EventProcessing \
				TA_EntityAccess \
				TA_DAI \
				TA_AlarmRuleDAI \
				TA_CorbaDef \
				TA_NamingCorbaDef \
				TA_MessageCorbaDef \
				TA_ManagedProcess \
				TA_ProcessManagementCorbaDef \
				TA_DataAccess \
				TA_ConfigurationUpdates \
				TA_Utilities \
				TA_Version \
				TA_ModbusComms \
				TA_Sockets \
				TA_Exceptions \
				TA_CommonExceptionsCorbaDef \
				TA_Synchronisation \
				TA_ocl \
				ACE \
				clntsh

		
 DEPENDENT_LIBRARIES =	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
			 $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
			 $(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
			 $(PROJ_ROOT_DIR)/code/transactive/core/threads \
			 $(PROJ_ROOT_DIR)/code/transactive/core/utilities \
			 $(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
			 $(PROJ_ROOT_DIR)/code/transactive/core/corba \
			 $(PROJ_ROOT_DIR)/code/transactive/core/uuid \
			 $(PROJ_ROOT_DIR)/code/transactive/core/alarm \
			 $(PROJ_ROOT_DIR)/code/transactive/core/naming \
			 $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
			 $(PROJ_ROOT_DIR)/code/transactive/core/message \
			 $(PROJ_ROOT_DIR)/code/transactive/core/database \
			 $(PROJ_ROOT_DIR)/code/transactive/core/ocl \
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
			 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
			 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
			 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
			 $(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
			 $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
			 $(PROJ_ROOT_DIR)/code/transactive/bus/modbus_comms \
			 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/data_recording_dai \
			 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
       $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
       $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
       $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
       $(PROJ_ROOT_DIR)/code/transactive/core/timers \
	   $(PROJ_ROOT_DIR)/code/transactive/core/sockets

PROJ_CFLAGS = -DTA_ASSERT_LOG=1 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
