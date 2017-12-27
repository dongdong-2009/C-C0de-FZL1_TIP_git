#The source code in this file is the property of
#Ripple Systems and is not for redistribution in any form.
#Source:    $DateTime: 2012/06/12 13:35:44 $
#@author:   christiandw
#@version $Revision: #1 $
#Last modification : $DateTime: 2012/06/12 13:35:44 $
#Last modified by : $Author: builder $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/system_control/system_status_agent
PROJ_NAME	= SystemStatusAgent
PROJ_SOURCE	= DataPointDriverHelper.cpp \
		  IDataPointDriver.cpp \
		  main.cpp \
		  SystemStatusAgent.cpp \
		  SystemStatusAgentConfigException.cpp \
		  VersionGen.cpp \
		  pingDriver/ConsecutiveNetworkFailureAlarmPolicy.cpp \
		  pingDriver/PercentageNetworkFailureAlarmPolicy.cpp \
		  pingDriver/PingableDataPointDriver.cpp \
		  pingDriver/PingablePoint.cpp \
		  pingDriver/SocketFailedException.cpp \
		  taComponentsDriver/ControlStationPoint.cpp \
		  taComponentsDriver/SystemControllerPoint.cpp \
		  taComponentsDriver/DpUpdateThread.cpp \
		  taComponentsDriver/DbServerPoint.cpp \
		  taComponentsDriver/TaComponentsDriver.cpp \
		  taComponentsDriver/AbstractPingDependentPoint.cpp \
		  threadedDriver/AbstractThreadedDriver.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= \
			TA_CorbaUtil \
			TA_AuthenticationCorbaDef \
			TA_DataNodeAgentCorbaDef_Impl \
			TA_DataPointAgentCorbaDef_Impl \
			TA_ScadaCommonLib \
			TA_ScadaProxyLibrary \
			TA_DataNodeLibrary \
			TA_DataPointLibrary \
			TA_ScadaProxyLibrary \
			TA_DataNodeLibrary \
			TA_DerivedDataPointDAI \
			TA_ScadaCommonLib \
			TA_ScadaCommonCorbaDef \
			TA_DataRecording \
			TA_DataRecordingDAI \
			TA_PlanDAI \
			TA_GenericAgent \
			TA_Bus_Event \
			TA_DerivedDataPointDAI \
			TA_EventTriggeredPlans \
			TA_PlanAgentLibrary \
			TA_PlanAgentCorbaDef \
			TA_PlanDAI \
			TA_PlanAgentDataCorbaDef \
			TA_PlanManagerCorbaDef \
			TA_DerivedDataPointDAI \
			TA_Alarm \
			TA_AlarmAgentCorbaDef \
			TA_EntityStatusUpdateCorbaDef \
			TA_RightsLibrary \
			TA_AuthenticationLibrary \
			TA_SecurityUtilities \
			TA_AuthenticationCorbaDef \
			TA_DutyAgentCorbaDef \
			TA_RightsAgentCorbaDef \
			TA_AuthenticationCorbaDef \
			TA_Timers \
			TA_Thread \
			TA_Naming \
			TA_DAI \
			TA_AlarmRuleDAI \
			TA_Uuid \
			TA_Message \
			TA_EventProcessing \
			TA_Event \
			TA_EventAgentCorbaDef \
			TA_EntityAccess \
			TA_DAI \
			TA_CorbaDef \
			TA_NamingCorbaDef \
			TA_MessageCorbaDef \
			TA_ManagedProcess \
			TA_ProcessManagementCorbaDef \
			TA_DataAccess \
			TA_ConfigurationUpdates \
			TA_Utilities \
			TA_Exceptions \
			TA_CommonExceptionsCorbaDef \
			TA_Version \
			TA_MathEvaluation \
			TA_Sockets \
			TA_MmsCorbaDef \
			TA_Synchronisation \
			TA_ocl \
			TA_mcl \
			clntsh \
			mysqlclient \
			ACE

DEPENDENT_LIBRARIES = \
			$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
			$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
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
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/derived_datapoints \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
			$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
			$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
			$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent \
			$(PROJ_ROOT_DIR)/code/transactive/bus/mathematical_evaluation \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/proxy_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataNodeAgentCorbaDef_Impl \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataPointAgentCorbaDef_Impl \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datapoint_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/sockets \
			$(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
			$(PROJ_ROOT_DIR)/code/transactive/bus/history/data_recording \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/data_recording_dai \
			$(PROJ_ROOT_DIR)/code/transactive/core/timers \

ADDITIONAL_OBJ_DIRECTORIES = \
	pingDriver \
        taComponentsDriver \
        threadedDriver

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
