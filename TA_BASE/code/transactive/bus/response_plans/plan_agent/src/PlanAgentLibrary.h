/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_agent/src/PlanAgentLibrary.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2015/04/15 19:15:46 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
#ifndef PLAN_AGENT_LIBRARY_HEADER_INCLUDED
#define PLAN_AGENT_LIBRARY_HEADER_INCLUDED

#include "core/utilities/src/DebugUtil.h"
#include "core/naming/src/NamedObject.h"
#pragma warning (disable : 4250)
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#pragma warning (default : 4250)

//Exceptions


namespace TA_Base_Bus
{
    class PlanAgentLibrary
    {
    public:
        PlanAgentLibrary();
        PlanAgentLibrary(unsigned long locationKey);
        ~PlanAgentLibrary();

        void setPlanAgentTimeout(unsigned short timeout);
        unsigned long getPlanAgentLocationKey() const;

        //Methods
		CORBA::Boolean isRegisteredManager(const char* session, const char* pmId);
		void registerPlanManager(const char* session, const char* pmId, TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerRef);
		void deregisterPlanManager(const char* session, const char* pmId);
		CORBA::Boolean isOwner(const char* session, const TA_Base_Core::ActivePlanId& plan);
        void processDecisionStepResponse(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status);
        void processPromptStepResponse(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status);
        void processLogStepResponse(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* logMessage);
        void processRunPlanStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error);
        void processLaunchGuiAppStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error);
        void processLaunchSchematicStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error);
        void processBackgroundDecisionStepResponse(const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status);
        void processBackgroundLogStepResponse(const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status, const char* logMessage);
        void processBackgroundPromptStepResponse(const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status);
        void processBackgroundLaunchGuiAppStepResponse(const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status);
        void processBackgroundLaunchSchematicStepResponse(const TA_Base_Core::CurrentPlanStepDetail& currentStepDetail, TA_Base_Core::EPlanStepCompletionStatus status);
		void processFailureSummaryStepResult(const char* session, const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const char* error);
		void runPlan(const char* session, TA_Base_Core::ActivePlanId& plan);
		unsigned long runPlanByName(const char* session, const char* path, const std::string param = "");		
		// TD20368 Yanrong++
		unsigned long runPlanByNameWithoutActionPermittedCheck(const char* session, const char* path, const std::string param = "");
		// ++Yanrong
		//marvin++
		//TD13527
		unsigned long runPlanByNameWithEntityRestriction( const char* session, const char* path, unsigned long entitykey);
		//marvin++
		//TD13527
        void runOperatorTriggeredPlan(const char* session, const char* pmId, TA_Base_Core::ActivePlanId& plan);
		void customisePlan(const char* session, const char* pmId, TA_Base_Core::ActivePlanId& plan);
		void pausePlan(const char* session, const TA_Base_Core::ActivePlanId& plan);
		void resumePlan(const char* session, const TA_Base_Core::ActivePlanId& plan);
		void stopPlan(const char* session, const TA_Base_Core::ActivePlanId& plan);
		void takeControl(const char* session, const char* pmId, const TA_Base_Core::ActivePlanId& plan);
		void customiseStep(const char* session, const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail);
		void getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::ExecutionHistory_out history);
		void getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::CustomisationHistory_out history);
		void setPlanApprovalState(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EApprovalState state);
        void setPlanType(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EPlanType eType);
		void getRootNodeChildren(TA_Base_Core::SiblingNodes_out children);
		void getChildren(TA_Base_Core::NodeId parent, TA_Base_Core::SiblingNodes_out children);
		void getNodeDetail(TA_Base_Core::NodeId node, TA_Base_Core::NodeDetail_out detail);
		void getNodeSummary(TA_Base_Core::NodeId node, TA_Base_Core::NodeSummary_out summary);
		void getNodePath(TA_Base_Core::NodeId node, TA_Base_Core::NodePath_out path);
		void getNodeIdsInNodePath(TA_Base_Core::NodeId plan, TA_Base_Core::NodeIds_out plans);
        TA_Base_Core::NodeId getNodeId(const char* path);
		void getActivePlanDetail(TA_Base_Core::NodeId plan, TA_Base_Core::ActivePlanDetails_out details);
		void getAssocAlarmPlanDetail(const char* alarmId, TA_Base_Core::ActivePlanDetail_out detail);
		void getActivePlanDetailByName(const char* path, TA_Base_Core::ActivePlanDetails_out details);		
		void getAllActivePlanDetails(TA_Base_Core::ActivePlanDetails_out details);
		void getEditPlanLock(const char* session, const char* pmId, TA_Base_Core::NodeId plan);
		void releaseEditPlanLock(const char* session, TA_Base_Core::NodeId plan);
		void getStepNames(TA_Base_Core::StepNames_out names);
		void getAllStepDetails(TA_Base_Core::NodeId plan, TA_Base_Core::StepDetails_out details);
		void getSpecifiedStepDetails(TA_Base_Core::NodeId plan, const TA_Base_Core::StepIds& steps, TA_Base_Core::StepDetails_out details);
		void getStepNumber(TA_Base_Core::StepId step, TA_Base_Core::StepNumber& position);
		void getParameterNames(TA_Base_Core::EStep stepType, TA_Base_Core::ParameterNames_out names);
		void getStepNumberParameters(TA_Base_Core::NodeId plan, TA_Base_Core::StepNumber position, TA_Base_Core::StepParameters_out parameters);
		void getStepIdParameters(TA_Base_Core::StepId step, TA_Base_Core::StepParameters_out parameters);
		void updateCategoryDetail(const char* session, const TA_Base_Core::CategoryDetail& detail);
		void insertCategory(const char* session, TA_Base_Core::NodeId parent);
		void insertCategoryIntoRoot(const char* session);
		void deleteCategory(const char* session, TA_Base_Core::NodeId category);
		void updatePlanDetail(const char* session, const TA_Base_Core::PlanDetail& detail);
		void updatePlan(const char* session, const TA_Base_Core::Plan& plan);
		void insertPlan(const char* session, TA_Base_Core::NodeId parent);
		void insertPlanIntoRoot(const char* session);
		void deletePlans(const char* session, const TA_Base_Core::NodeIds& plans);
		void movePlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce);
		void movePlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce);
		void copyPlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce);
		void copyPlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce);
		void getNextStepId(const char* session, TA_Base_Core::StepId& step);
		void exportPlan (const char* session, TA_Base_Core::AuxPlanDetail_out plan, TA_Base_Core::NodeId planId);
		void importPlan (const char* session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId);

    private:

        PlanAgentLibrary( const PlanAgentLibrary& thePlanAgentLibrary);
        PlanAgentLibrary& operator=(const PlanAgentLibrary& rhs);

	    TA_Base_Core::NamedObject<TA_Base_Bus::IPlanAgentCorbaDef,
			                 TA_Base_Bus::IPlanAgentCorbaDef_ptr,
			                 TA_Base_Bus::IPlanAgentCorbaDef_var> m_planAgent;

        unsigned long m_planAgentLocationKey;
    };

}
#endif //PLAN_AGENT_LIBRARY_HEADER_INCLUDED
