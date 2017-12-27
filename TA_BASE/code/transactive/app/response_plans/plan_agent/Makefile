# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File$
# @author:  Ripple
# @version: $Revision$
#
# Last modification: $DateTime$
# Last modified by:  $Author$
#
# Make file for the Plan Agent
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/response_plans/plan_agent
PROJ_NAME	= PlanAgent
PROJ_SOURCE	= PlanAgentCorbaDefImpl.cpp \
			DatapointTriggerObserver.cpp \
			DssAbstractRule.cpp \
			DssDatapointRule.cpp \
			DssEngine.cpp \
			DssKnowledgeBase.cpp \
			DssRuleProcessor.cpp \
			DssManager.cpp \
			AAMessageSender.cpp \
			ActiveEndStep.cpp \
			ActiveAbortStep.cpp \
			ActiveDataPointCheckStep.cpp \
			ActiveDataPointSetStep.cpp \
			ActiveDecisionStep.cpp \
			ActiveJumpStep.cpp \
			ActiveAssignCameraToMonitorStep.cpp \
			ActiveAssignCameraToPresetStep.cpp \
			ActiveVmsBlankoutStep.cpp \
			ActiveVmsRestoreScheduleStep.cpp \
			ActiveVmsSetSceneStep.cpp \
			ActiveTrainPaStep.cpp \
			ActiveTrainTisStep.cpp \
			ActiveVideoWallStep.cpp \
			ActiveStationPaStep.cpp \
			ActiveStationTisStep.cpp \
			ActiveTerminatePlanStep.cpp \
			ActiveEvaluateExpressionStep.cpp\
			ActiveRunPlanStep.cpp \
			ActiveLaunchGuiAppStep.cpp \
			ActiveLaunchSchematicStep.cpp \
			ActiveActivePlanCheckStep.cpp \
			ActiveSendSmsMessageStep.cpp \
			ActiveFailureSummaryStep.cpp \
			PlanDetail.cpp \
			ActivePlan.cpp \
			ActivePlanBulletinBoard.cpp \
			ActivePlanCustomiseStepMessage.cpp \
			ActivePlanDecisionStepMessage.cpp \
			ActivePlanLogStepMessage.cpp \
			ActivePlanRunPlanStepMessage.cpp \
			ActivePlanLaunchGuiAppStepMessage.cpp \
			ActivePlanLaunchSchematicStepMessage.cpp \
			ActivePlanExecutingState.cpp \
			ActivePlanMessageQueue.cpp \
			ActivePlanMessageBlock.cpp \
			ActivePlanPauseMessage.cpp \
			ActivePlanPauseForOperatorMessage.cpp \
			ActivePlanDetailUpdateMessage.cpp \
			ActivePlanPromptStepMessage.cpp \
			ActivePlanFailureSummaryStepMessage.cpp \
			ActivePlanPausedState.cpp \
			ActivePlanDelayedState.cpp \
			ActivePlanOperatorPausedState.cpp \
			ActivePlanPendingState.cpp \
			ActivePlanReassignMessage.cpp \
			ActivePlanResumeMessage.cpp \
			ActivePlanState.cpp \
			ActivePlanStopMessage.cpp \
			ActivePlanCompleteMessage.cpp \
			ActivePlanUnassignMessage.cpp \
			ActivePromptStep.cpp \
			ActiveLogStep.cpp \
			ActiveStep.cpp \
			PlanAccessFactory.cpp \
			PlanAgent.cpp \
			PlanAgentUser.cpp \
			PlanExecutor.cpp \
			PlanExecutionEngine.cpp \
			RunPlanMethodObject.cpp \
			PlanHouseKeepingMethodObject.cpp \
			PlanNotFoundException.cpp \
			ActivePlanMessageException.cpp \
			ActiveStepParamException.cpp \
			ThirdPartyLibException.cpp \
			SortablePlanId.cpp \
			PlanDataWriter.cpp \
			GRCType3SubTaskMO.cpp \
			GRCType3SubTask.cpp \
			GRC3DataPointSetComplexStep.cpp \
			ActiveNullStep.cpp \
			ActiveStationPaByAlarmStep.cpp \
			ActiveBackgroundLaunchSchematicStep.cpp \
			ActiveBackgroundLaunchGuiAppStep.cpp \
			ActiveBackgroundDecisionStep.cpp \
			ActiveBackgroundLogStep.cpp \
			ActiveBackgroundPromptStep.cpp \
			CMMessageSender.cpp \
			VersionGen.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive  $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= \
			TA_ScadaProxyLibrary \
			TA_DataNodeLibrary \
			TA_DataPointLibrary \
			TA_DerivedDataPointDAI \
			TA_ScadaCommonLib \
			TA_ScadaCommonCorbaDef \
			TA_DataRecording \
			TA_DataRecordingDAI \
			TA_Timers \
			TA_CorbaUtil \
			TA_Thread \
			TA_Naming \
			TA_Message \
			TA_EventProcessing \
			TA_Event \
			TA_EventAgentCorbaDef \
			TA_Utilities \
			TA_Version \
			TA_RightsLibrary \
			TA_DutyAgentCorbaDef \
			TA_SecurityUtilities \
			TA_RightsAgentCorbaDef \
			TA_GenericAgent \
			TA_DerivedDataPointDAI \
			TA_Bus_Event \
			TA_AuthenticationLibrary \
			TA_AuthenticationCorbaDef \
			TA_CorbaDef \
			TA_NamingCorbaDef \
			TA_GenericAgent \
			TA_EventTriggeredPlans \
			TA_PlanAgentLibrary \
			TA_PlanAgentCorbaDef \
			TA_PlanDAI \
			TA_PlanAgentDataCorbaDef \
			TA_PlanManagerCorbaDef \
			TA_ManagedProcess \
			TA_ProcessManagementCorbaDef \
			TA_PlanAgentCorbaDef \
			TA_PlanAgentDataCorbaDef \
			TA_Alarm \
			TA_AlarmAgentCorbaDef \
			TA_EntityStatusUpdateCorbaDef \
			TA_DAI \
			TA_AlarmRuleDAI \
			TA_Synchronisation \
			TA_MessageCorbaDef \
			TA_PidsAgentCorbaDef \
			TA_PaAgentProxy \
			TA_PAAgentCorbaDef \
			TA_TrainAgentCorba \
			TA_TISAgentDAI \
			TA_TISAgentAccess \
			TA_TISManagerCorbaDef \
			TA_TISAgentCorbaDef \
			TA_VideoSwitchAgentAccess \
			TA_VideoSwitchAgentCorbaDef \
			TA_MathEvaluation \
			TA_EntityAccess \
			TA_DAI \
			TA_DataAccess \
			TA_ConfigurationUpdates \
			TA_Exceptions \
			TA_CommonExceptionsCorbaDef \
			TA_DataPointLibrary \
			TA_DataPointAgentCorbaDef_Impl \
			TA_ScadaCommonLib \
			TA_ScadaCommonCorbaDef \
			TA_ScadaProxyLibrary \
			TA_PlanManagerCorbaDef \
			TA_PlanDataAccess \
			TA_Utilities \
			TA_Uuid \
 			TA_ocl \
			TA_mcl \
			ACE \
			clntsh \
			mysqlclient

             
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)

DEPENDENT_LIBRARIES = \
			$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
			$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
			$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
			$(PROJ_ROOT_DIR)/code/transactive/core/threads \
			$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
			$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
			$(PROJ_ROOT_DIR)/code/transactive/core/corba \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/message \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/database \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent \
			$(PROJ_ROOT_DIR)/code/transactive/bus/pa/pa_agent_proxy \
			$(PROJ_ROOT_DIR)/code/transactive/bus/pa/pa_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/signs/tis_agent_access \
			$(PROJ_ROOT_DIR)/code/transactive/bus/signs/TisManagerCorbaDef \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent/ \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_manager/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/proxy_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataPointAgentCorbaDef_Impl \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datapoint_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
			$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/PlanDataAccess \
			$(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/agent_access \
			$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
			$(PROJ_ROOT_DIR)/code/transactive/bus/mathematical_evaluation \
			$(PROJ_ROOT_DIR)/code/transactive/bus/signs/pids_agent/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
