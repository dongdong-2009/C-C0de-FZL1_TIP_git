/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This is a representation of a plan node within the plan tree.
  *
  **/

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/applauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "Resource.h"
#include "PlanNode.h"
#include "RootNode.h"
#include "TreeNodeFactory.h"
#include "PlanInstance.h"
#include "PlanInstanceMap.h"
#include "PlanStep.h"
#include "PlanStepMap.h"
#include "PlanAgentAccess.h"
#include "PlanNodeView.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "MessageBox.h"
#include "OperatorRights.h"
#include "PlanManagerCommonDefs.h"
#include "PlanInstanceFilter.h"
#include "PlanGeneralDetailPage.h"
#include "PlanStepDetailPage.h"
#include "PlanEditorThread.h"
#include "PlanControllerThread.h"
#include "FloatingWindowThreadRegister.h"
#include "InteractiveStep.h"
#include "InteractiveStepException.h"

#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"
#include "core/utilities/src/RunParams.h"

#include "PlanAgentAccessException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanNode

PlanNode::PlanNode(const TA_Base_Core::NodeSummary &nodeSummary) :
    TreeNode(nodeSummary),
    m_isCopiedPlan(false),
    m_localPlanInstanceMap(NULL),
    m_remotePlanInstanceMap(NULL),
    m_planStepMap(NULL),
    m_areStepsUpdated(false)
{
    FUNCTION_ENTRY( "PlanNode" );

    TA_ASSERT(nodeSummary.type == TA_Base_Core::PLAN_NODE, "Not a plan node summary");

    FUNCTION_EXIT;
}


PlanNode::PlanNode(const PlanNode& planNode) :
    TreeNode(const_cast<PlanNode &>(planNode).getNodeSummary()),
    m_isCopiedPlan(true),
    m_localPlanInstanceMap(NULL),
    m_remotePlanInstanceMap(NULL),
    m_planStepMap(NULL),
    m_areStepsUpdated(false)
{
    FUNCTION_ENTRY( "PlanNode" );
    FUNCTION_EXIT;
}


PlanNode::~PlanNode()
{
    FUNCTION_ENTRY( "~PlanNode" );

    delete m_localPlanInstanceMap;
    delete m_remotePlanInstanceMap;
    delete m_planStepMap;

    FUNCTION_EXIT;
}


const std::string &PlanNode::getNodeType()
{
    FUNCTION_ENTRY( "getNodeType" );

    static const std::string PLAN_NODE_TYPE = (LPCTSTR) LabelMaker::getPlanNodeTypeLabel();

    FUNCTION_EXIT;
    return PLAN_NODE_TYPE;
}


const std::string &PlanNode::getNodeStatus()
{
    FUNCTION_ENTRY( "getNodeStatus" );

    static const std::string APPROVED_PLAN_STATUS = Utilities::convertToString(TA_Base_Core::APPROVED_APPROVAL_STATE);
    static const std::string UNAPPROVED_PLAN_STATUS = Utilities::convertToString(TA_Base_Core::UNAPPROVED_APPROVAL_STATE);

    FUNCTION_EXIT;
    return (isApproved()? APPROVED_PLAN_STATUS : UNAPPROVED_PLAN_STATUS);
}


PlanInstanceSharedPtr PlanNode::getPlanInstance(const PlanInstanceId &instanceId)
{
    FUNCTION_ENTRY( "getPlanInstance" );

    // This method may be called in the course of processing a Plan Controller launch request from
    // the Plan Agent. The agent identifies the instance of interest, however, it may be one that
    // it created moments earlier and it's possible that the Plan Manager has not yet received an
    // active plan update for it.
    // 
    // The procedure therefore is to first look up the local plan instance store. If the instance
    // cannot be found, we refresh the store contents by retrieving all the active instances for
    // the plan from the agent.

    PlanInstanceSharedPtr instance = getPlanInstanceMap(instanceId.m_location).getPlanInstance(instanceId);

    if (instance == NULL)
    {
        try
        {
            TA_Base_Core::ActivePlanDetails_var activePlanDetails =
                PlanAgentAccess::getInstance().getActivePlanDetailsAtLocation(getNodeId(), instanceId.m_location);

            getPlanInstanceMap(instanceId.m_location).addPlanInstances(activePlanDetails);
        }
        catch (TA_Base_Core::TransactiveException &ex)
        {
            MessageBox::error(IDS_RETRIEVE_PLAN, ex);
        }

        instance = getPlanInstanceMap(instanceId.m_location).getPlanInstance(instanceId);
    }

    FUNCTION_EXIT;
    return instance;
}


CRuntimeClass *PlanNode::getNodeViewClass()
{
    FUNCTION_ENTRY( "getNodeViewClass" );
    FUNCTION_EXIT;
    return RUNTIME_CLASS(CPlanNodeView);
}


void PlanNode::updateCmdUI(CCmdUI &cmdUI)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    switch (cmdUI.m_nID)
    {
        case ID_PLAN_SCHEDULE:
            cmdUI.Enable(isApproved() && OperatorRights::getInstance().canSchedulePlans());
            break;

        case ID_PLAN_EXECUTE:
        case ID_PLAN_CONTROL:
            cmdUI.Enable(isApproved() && OperatorRights::getInstance().canControlPlans());
            break;

        case ID_PLAN_CUSTOMISE:
            cmdUI.Enable(isApproved() && OperatorRights::getInstance().canCustomisePlans());
            break;

        case ID_PLAN_APPROVE:
            cmdUI.Enable(!isApproved() && OperatorRights::getInstance().canApprovePlans());
            break;

        case ID_PLAN_UNAPPROVE:
            cmdUI.Enable(isApproved() && OperatorRights::getInstance().canApprovePlans());
            break;

        case ID_PLAN_EDIT:
        case ID_PLAN_CUT:
        case ID_PLAN_DELETE:
        case ID_CATEGORY_OR_PLAN_DELETE:
            cmdUI.Enable(!isApproved() && OperatorRights::getInstance().canEditPlans());
            break;

        case ID_PLAN_NEW:
        case ID_PLAN_COPY:
        case ID_PLAN_PASTE:
            cmdUI.Enable(OperatorRights::getInstance().canEditPlans());
            break;

        case ID_CATEGORY_NEW:
          case ID_EXPORT_PLAN:
            cmdUI.Enable(OperatorRights::getInstance().canEditCategories());
            break;
        case ID_PLAN_VISIBLE:
            cmdUI.Enable(!isVisible());
            break;
        case ID_PLAN_INVISIBLE:
            cmdUI.Enable(isVisible());
            break;
        default:
            cmdUI.Enable(FALSE);
    }

    FUNCTION_EXIT;
}


ETreeNodeImageType PlanNode::getNodeImageType(bool isExpanded)
{
    FUNCTION_ENTRY( "getNodeImageType" );

    TA_ASSERT(!isExpanded, "Plan nodes cannot be expanded");

    FUNCTION_EXIT;
    return (isApproved()? TNIT_APPROVED_PLAN : TNIT_UNAPPROVED_PLAN);
}


void PlanNode::addChildrenToTree(CTreeCtrl &treeCtrl, HTREEITEM parent, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addChildrenToTree" );

    TA_ASSERT(false, "Plan nodes do not have children");

    FUNCTION_EXIT;
}


void PlanNode::addChildrenToList(CListCtrl &listCtrl, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addChildrenToList" );

    TA_ASSERT(false, "Plan nodes do not have children");

    FUNCTION_EXIT;
}


void PlanNode::renameNode(std::string nodeName)
{
    FUNCTION_ENTRY( "renameNode" );

    TA_Base_Core::Plan plan;

    plan.detail = getPlanDetail();
    std::string oldName = plan.detail.name;
    std::string newName = nodeName;

    // as per TD 14880
    if (oldName.compare (newName) == 0)
    {
        FUNCTION_EXIT;
        return;
    }


//        TA_Base_Core::NodeId planId = plan.detail.plan;
//        TA_Base_Bus::TransActiveMessage userMsg;
//        std::string description = "being renamed";
//        userMsg << nodeName;
//        userMsg << description;
//        if (userMsg.showMsgBox(IDS_UW_210031) == IDYES)
//        {
//            //TD17493++
//            //std::string planPath = PlanAgentAccess::getInstance().getNodePath (planId);
//            std::string planPath ="";
//            try
//            {
//                planPath = PlanAgentAccess::getInstance().getNodePath (planId);
//            }
//            catch (...)
//            {
//                planPath = nodeName;
//            }
//            //++TD17493
//            PlanAgentAccess::getInstance().generateReport (planPath);
//        }
//

         plan.detail.name = CORBA::string_dup(nodeName.c_str());
         plan.stepseq.length(0);

         PlanAgentAccess::getInstance().updatePlan(plan);

         // Reset node information used in updating plan tree items. The agent will be requeried and should respond
         // with the new name. This means that after the name of a plan has been edited in the tree it will not reset
         // to its old value before a plan change notification comes back from the agent.
         invalidateNodeName();
         invalidateNodeSummary();
         invalidateNodeDetail();

    FUNCTION_EXIT;
}


void PlanNode::updatePlanView(CPlanGeneralDetailPage &generalDetailPage, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanView" );

    generalDetailPage.m_nameEdit.SetWindowText(_T(getPlanDetail().name));
    generalDetailPage.m_descriptionEdit.SetWindowText(_T(getPlanDetail().description));
    generalDetailPage.m_approvalStatusEdit.SetWindowText(_T(Utilities::convertToString(getPlanDetail().approvalState).c_str()));
    generalDetailPage.m_planModByEdit.SetWindowText(_T(getPlanDetail().modifiedBy));
    generalDetailPage.m_planModTimeEdit.SetWindowText(_T(getPlanDetail().modifiedTime));
    generalDetailPage.m_approvalModByEdit.SetWindowText(_T(getPlanDetail().approvalChangedBy));
    generalDetailPage.m_approvalModTimeEdit.SetWindowText(_T(getPlanDetail().approvalChangedTime));

    if (isApproved())
    {
        generalDetailPage.m_allOwnersCheck.SetCheck(filter.allOwnersNeeded()? BST_CHECKED : BST_UNCHECKED);
        generalDetailPage.m_allLocationsCheck.SetCheck(filter.allLocationsNeeded()? BST_CHECKED : BST_UNCHECKED);

        generalDetailPage.m_planInstanceListCtrl.DeleteAllItems();
        updatePlanInstanceList(generalDetailPage.m_planInstanceListCtrl, filter);
    }

    FUNCTION_EXIT;
}


void PlanNode::updatePlanView(CPlanStepDetailPage &stepDetailPage, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanView" );

    stepDetailPage.m_stepParametersListCtrl.DeleteAllItems();

    addStepsToStepList(stepDetailPage.m_planStepsListCtrl);

    if (isApproved())
    {
        stepDetailPage.m_allOwnersCheck.SetCheck(filter.allOwnersNeeded()? BST_CHECKED : BST_UNCHECKED);
        stepDetailPage.m_allLocationsCheck.SetCheck(filter.allLocationsNeeded()? BST_CHECKED : BST_UNCHECKED);

        // Instance "None" is always added to the plan instance combo, regardless of any active instances
        stepDetailPage.m_planInstanceCombo.ResetContent();
        m_planInstanceNone.addToPlanInstanceCombo(stepDetailPage.m_planInstanceCombo);

        updatePlanInstanceCombo(stepDetailPage.m_planInstanceCombo, filter);
    }

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstanceList(CPlanInstanceListCtrl &instanceListCtrl, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanInstanceList" );

    if (isApproved())
    {
        PlanInstanceList planInstances;
        getLocalPlanInstanceMap().getPlanInstances(planInstances);

        // If plans at all locations are required
        if (filter.allLocationsNeeded())
        {
            PlanInstanceList remotePlanInstances;
            getRemotePlanInstanceMap().getPlanInstances(remotePlanInstances);

            planInstances.insert(planInstances.end(), remotePlanInstances.begin(), remotePlanInstances.end());
        }

        instanceListCtrl.SetItemCount(planInstances.size());

        for (PlanInstanceList::const_iterator iiter = planInstances.begin(); iiter != planInstances.end(); iiter++)
        {
            PlanInstanceSharedPtr planInstance = *iiter;

            if ( !TreeNodeFactory::getInstance().canTreeNodeBeShowed(planInstance->getActivePlanId().plan) )
            {
                continue;
            }
            planInstance->addToPlanInstanceList(instanceListCtrl);
        }
    }

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstanceCombo(CPlanInstanceComboBox &instanceComboBox, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanInstanceCombo" );

    if (isApproved())
    {
        PlanInstanceList planInstances;
        getLocalPlanInstanceMap().getPlanInstances(planInstances);

        // If plans at all locations are required
        if (filter.allLocationsNeeded())
        {
            PlanInstanceList remotePlanInstances;
            getRemotePlanInstanceMap().getPlanInstances(remotePlanInstances);

            planInstances.insert(planInstances.end(), remotePlanInstances.begin(), remotePlanInstances.end());
        }

        for (PlanInstanceList::const_iterator iiter = planInstances.begin(); iiter != planInstances.end(); iiter++)
        {
            PlanInstanceSharedPtr planInstance = *iiter;
            planInstance->addToPlanInstanceCombo(instanceComboBox);
        }
    }

    FUNCTION_EXIT;
}


void PlanNode::addStepsToStepList(CPlanStepListCtrl& planStepsListCtrl)
{
    FUNCTION_ENTRY( "addStepsToStepList" );

    planStepsListCtrl.DeleteAllItems();

    PlanStepList planSteps;
    getPlanStepMap().getPlanSteps(planSteps);

    planStepsListCtrl.SetItemCount(planSteps.size());

    for (PlanStepList::const_iterator siter = planSteps.begin(); siter != planSteps.end(); siter++)
    {
        PlanStep *planStep = *siter;
        planStep->addToStepList(planStepsListCtrl);
    }

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    TA_ASSERT(activePlanDetail.plan.plan == getNodeId(), "Plan node ID mismatch");

    getPlanInstanceMap(activePlanDetail.plan.location).updatePlanInstance(activePlanDetail);

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstance(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    TA_ASSERT(planCustomDetail.plan.plan == getNodeId(), "Plan node ID mismatch");

    getPlanInstanceMap(planCustomDetail.plan.location).updatePlanInstance(planCustomDetail);

    FUNCTION_EXIT;
}


void PlanNode::updateNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateNode" );

    TreeNode::updateNode(planConfigData);

    switch (planConfigData.configUpdate._d())
    {
        case TA_Base_Core::PCUT_APPROVAL_STATE:
        {
            if (!isApproved())
            {
                // If a Plan Controller is running for this plan, then tell it the plan's been unapproved.
                CPlanControllerThread *planControllerThread = FloatingWindowThreadRegister::getInstance().getPlanControllerThread(getNodeId());
                if (planControllerThread)
                {
                    planControllerThread->PostThreadMessage(WM_CONTROLLED_PLAN_APPROVAL_STATE_CHANGED, getNodeId(), 0);
                }
            }
            break;
        }

        case TA_Base_Core::PCUT_UPDATE_PLAN:
        {
            invalidatePlanStepMap();
            break;
        }

        default:
            ;
    }

    FUNCTION_EXIT;
}


void PlanNode::updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateChildNodeList" );

    TA_ASSERT(false, "Plan nodes do not have children");

    FUNCTION_EXIT;
}


void PlanNode::setPlanName(std::string  name)
{
    FUNCTION_ENTRY( "setPlanName" );

    TA_ASSERT(m_isCopiedPlan,"Attempting to update an original plan.");
    getPlanDetail().name = CORBA::string_dup(name.c_str());

    FUNCTION_EXIT;
}


void PlanNode::setPlanDescription(std::string description)
{
    FUNCTION_ENTRY( "setPlanDescription" );

    TA_ASSERT(m_isCopiedPlan,"Attempting to update an original plan.");
    getPlanDetail().description = CORBA::string_dup(description.c_str());

    FUNCTION_EXIT;
}


void PlanNode::schedulePlan()
{
    FUNCTION_ENTRY( "schedulePlan" );

    if (OperatorRights::getInstance().canSchedulePlans())
    {
        // Get the full path of this plan.
        std::string planPath("");

        try
        {
            planPath = PlanAgentAccess::getInstance().getNodePath(getNodeId());
        }
        catch (...)
        {

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Failed to determine the path of node ID %lu", getNodeId());

            CString actionName;
            actionName.LoadString(IDS_LAUNCH_SCHEDULING_MANAGER);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            userMsg.showMsgBox(IDS_UE_010028);

            FUNCTION_EXIT;
            return;
        }

        std::ostringstream commandLine;
        commandLine << "--job-info=Plan:PlanPath/" << planPath;

        CPoint pt = GetMessagePos();
        RECT newRect = TA_Base_Bus::AppLauncher::getInstance().getRect(
            TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, pt.x);
        // Run the Plan Scheduler
        TA_Base_Bus::AppLauncher::getInstance().launchApplication(SCHEDULING_MANAGER, commandLine.str(),
            TA_Base_Bus::TA_GenericGui::POS_BOUNDED, TA_Base_Bus::TA_GenericGui::ALIGN_FIT, NULL, &newRect);
    }
    else
    {
        CString actionName;
        actionName.LoadString(IDS_LAUNCH_SCHEDULING_MANAGER);
        TA_Base_Bus::TransActiveMessage userMsg;
          std::string userName;
          std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
          TA_Base_Bus::AuthenticationLibrary authLibrary(true);
          TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
          std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
          userName = iOperator->getName();
          userMsg << userName;
        userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_010021);
    }

    FUNCTION_EXIT;
}


void PlanNode::approvePlan(bool isApproved)
{
    FUNCTION_ENTRY( "approvePlan" );

    PlanAgentAccess::getInstance().setPlanApprovalState(
        getNodeId(), (isApproved? TA_Base_Core::APPROVED_APPROVAL_STATE : TA_Base_Core::UNAPPROVED_APPROVAL_STATE));

    FUNCTION_EXIT;
}


void PlanNode::savePlan()
{
    FUNCTION_ENTRY( "savePlan" );

    TA_ASSERT(m_isCopiedPlan,"Attempting to save a non-editted file");

    TA_Base_Core::Plan   plan;
    int                stepNumber=0;

    // Create plan structure
    plan.detail = getPlanDetail();
    plan.detail.plan = getNodeId();  // Fix up as getNodeDetail is not populating this field yet!! AJP

    if (m_areStepsUpdated)
    {
        // Add all the plan steps
        PlanStepList          planSteps;

        getPlanStepMap().getPlanSteps(planSteps);

        plan.stepseq.length(planSteps.size());
        for (PlanStepList::const_iterator siter = planSteps.begin(); siter != planSteps.end(); siter++)
        {
            plan.stepseq[stepNumber] = (*siter)->getUpdatedStepAndParameters();
            stepNumber++;
        }  // For each plan step
    }

    // Save this plan
    PlanAgentAccess::getInstance().updatePlan(plan);

    // Plan modification author and time must be refreshed.
    invalidateNodeDetail();
    notifyPlanGeneralDetailUpdate();

    m_areStepsUpdated = false;

    FUNCTION_EXIT;
}


TA_Base_Core::AuxPlanDetail* PlanNode::exportPlan ()
{
    FUNCTION_ENTRY( "exportPlan" );

    TA_Base_Core::AuxPlanDetail_var auxPlanVar;
    auxPlanVar = PlanAgentAccess::getInstance().exportPlan(getNodeId ());

    FUNCTION_EXIT;
    return auxPlanVar._retn();
}


void PlanNode::deletePlan()
{
    FUNCTION_ENTRY( "deletePlan" );

    PlanAgentAccess::getInstance().deletePlan(getNodeId());

    FUNCTION_EXIT;
}


void PlanNode::startNewInstance()
{
    FUNCTION_ENTRY( "startNewInstance" );

    PlanAgentAccess::getInstance().startPlan(getNodeId());

    FUNCTION_EXIT;
}


void PlanNode::customiseNewInstance()
{
    FUNCTION_ENTRY( "customiseNewInstance" );

    PlanAgentAccess::getInstance().customisePlan(getNodeId());

    FUNCTION_EXIT;
}


void PlanNode::executeInteractiveStep(InteractiveStep& interactiveStep)
{
    FUNCTION_ENTRY( "executeInteractiveStep" );

    // Open a Plan Controller for the interactive step to execute within. Ignore any limits that may be
    // placed on the number of simultaneously open Plan Controllers. They should not prevent the step's
    // successful execution.
    controlPlan(true);

    // Ask the Plan Controller to execute the interactive step.
    CPlanControllerThread *planControllerThread = FloatingWindowThreadRegister::getInstance().getPlanControllerThread(getNodeId());
    if (planControllerThread)
    {
        if (planControllerThread->PostThreadMessage(WM_INTERACTIVE_STEP_EXEC_REQUEST, reinterpret_cast<WPARAM>(&interactiveStep), 0))
        {
            // Wait for the interactive step window to come up. Watch out for errors.
            std::string launchError = interactiveStep.waitUntilLaunched();
            if (!launchError.empty())
            {
                TA_THROW(InteractiveStepException(launchError));
            }
        }
        else
        {
            TA_THROW(InteractiveStepException("Failed to post interactive step execution request to Plan Controller."));
        }
    }
    else
    {
        TA_THROW(InteractiveStepException("Plan Controller thread is not running."));
    }

    FUNCTION_EXIT;
}


void PlanNode::controlPlan(bool ignoreActivePlanControllerLimit)
{
    FUNCTION_ENTRY( "controlPlan" );

    if (OperatorRights::getInstance().canControlPlans())
    {
        // Create a new Plan Controller or activate the existing one if already open
        FloatingWindowThreadRegister::getInstance().createPlanControllerThread(getNodeId(), ignoreActivePlanControllerLimit);
    }
    else
    {
        CString actionName;
        actionName.LoadString(IDS_LAUNCH_PLAN_CONTROLLER);
        TA_Base_Bus::TransActiveMessage userMsg;
          std::string userName;
          std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
          TA_Base_Bus::AuthenticationLibrary authLibrary(true);
          TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
          std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
          userName = iOperator->getName();
          userMsg << userName;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
    }

    FUNCTION_EXIT;
}


void PlanNode::controlPlan(const PlanInstanceId &planInstanceId)
{
    FUNCTION_ENTRY( "controlPlan" );

    // Open a Plan Controller.
    controlPlan();

    // Ask the Plan Controller to select the specified plan instance.
    CPlanControllerThread *planControllerThread = FloatingWindowThreadRegister::getInstance().getPlanControllerThread(getNodeId());
    if (planControllerThread)
    {
        // lizettejl++
        PlanInstanceId targetInstanceId(planInstanceId);
        // If no plan instance specified, try to nominate one.
        if (targetInstanceId.m_instance == static_cast<PlanInstanceNumber>(-1))
        {            // Consolidate all active instances, local as well as remote.
            PlanInstanceList planInstances, tmpPlanInstances;
            getRemotePlanInstanceMap().getPlanInstances(planInstances);
            getLocalPlanInstanceMap().getPlanInstances(tmpPlanInstances);
            // planInstances.insert(tmpPlanInstances.begin(), tmpPlanInstances.end());

            // If there are two or more active instance, then iterate over them
            // and discard any that have been completed, stopped, aborted or expired.

            if (planInstances.size() > 1)
            {
                for (PlanInstanceList::iterator instIt = planInstances.begin(); instIt != planInstances.end(); )
                {
                    const int instanceState  = (*instIt)->getActivePlanState();
                    if (instanceState == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE ||
                        instanceState == TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE ||
                        instanceState == TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE ||
                        instanceState == TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE ||
                        instanceState == TA_Base_Core::PENDING_ACTIVE_PLAN_STATE)
                    {
                        // Keep this instance.
                        instIt++;
                        continue;
                    }
                    // This instance is disqualified as it fails above criteria.
                    instIt = planInstances.erase(instIt);
                }
            }
            // If we are left with exactly one active instance, then this is what
            // Plan Controller should preselect.
            if ( planInstances.size() == 1)
            {
                targetInstanceId = (*planInstances.begin())->getInstanceId();
            }
        }
        // ++lizettejl
        planControllerThread->PostThreadMessage(WM_SELECT_ACTIVE_PLAN_INSTANCE, getNodeId(),

        MAKELPARAM(planInstanceId.m_instance, planInstanceId.m_location));
    }

    FUNCTION_EXIT;
}


void PlanNode::editPlan()
{
    FUNCTION_ENTRY( "editPlan" );

    if (OperatorRights::getInstance().canEditPlans())
    {
        // Create a new Plan Editor or activate the existing one if already open.
        // Note that createPlanEditorThread() returns NULL if plan edit lock cannot be obtained.
        FloatingWindowThreadRegister::getInstance().createPlanEditorThread(getNodeId());
    }
    else
    {
        CString actionName;
        actionName.LoadString(IDS_LAUNCH_PLAN_EDITOR);
        TA_Base_Bus::TransActiveMessage userMsg;
          std::string userName;
          std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
          TA_Base_Bus::AuthenticationLibrary authLibrary(true);
          TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
          std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
          userName = iOperator->getName();
          userMsg << userName;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
    }

    FUNCTION_EXIT;
}


// Returns true if this plan node precedes the specified sibling node on a plan tree branch.
bool PlanNode::hasPrecedence(TreeNode &siblingNode)
{
    FUNCTION_ENTRY( "hasPrecedence" );

    PlanNode *planNode = dynamic_cast<PlanNode *>(&siblingNode);

    // Plan nodes trail category nodes. If the other node is not a plan (i.e. must be a category),
    // then it should actually precede this node.
    if (!planNode)
    {
        FUNCTION_EXIT;
        return false;
    }

    // If both this and the sibling are plan nodes, then compare them on name basis.
    std::string thisNodeName = getNodeName();
    std::string otherNodeName = planNode->getNodeName();

    FUNCTION_EXIT;
    return std::lexicographical_compare(thisNodeName.begin(), thisNodeName.end(),
                                        otherNodeName.begin(), otherNodeName.end(),
                                        CaseInsensitiveCompare());
}


TA_Base_Core::PlanDetail &PlanNode::getPlanDetail()
{
    FUNCTION_ENTRY( "getPlanDetail" );
    FUNCTION_EXIT;
    return getNodeDetail().plan();
}


PlanInstanceMap &PlanNode::getPlanInstanceMap(LocationKey location)
{
    FUNCTION_ENTRY( "getPlanInstanceMap" );

    if (location == TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation())
    {
        FUNCTION_EXIT;
        return getLocalPlanInstanceMap();
    }

    FUNCTION_EXIT;
    return getRemotePlanInstanceMap();
}


PlanInstanceMap &PlanNode::getLocalPlanInstanceMap()
{
    FUNCTION_ENTRY( "getLocalPlanInstanceMap" );

    if (!m_localPlanInstanceMap)
    {
        // Contact the plan agent at the profile location and get its active plan details
        TA_Base_Core::ActivePlanDetails_var activePlanDetails = PlanAgentAccess::getInstance().getActivePlanDetails(getNodeId());

        m_localPlanInstanceMap = new PlanInstanceMap(activePlanDetails);
    }

    FUNCTION_EXIT;
    return *m_localPlanInstanceMap;
}


PlanInstanceMap &PlanNode::getRemotePlanInstanceMap()
{
    FUNCTION_ENTRY( "getRemotePlanInstanceMap" );

    if (!m_remotePlanInstanceMap)
    {
        m_remotePlanInstanceMap = new PlanInstanceMap();

        // Get all the remote locations
        typedef std::vector<unsigned long> LocationList;
        LocationList locations = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getNonDefaultPlanAgentLocations();

        // Contact the plan agent at each of the remote locations and get its active plan details.
        // getActivePlanDetailsAtLocation() will skip a location if no Plan Agent entity is configured for it.
        for (LocationList::iterator iter = locations.begin(); iter != locations.end(); iter++)
        {
            LocationKey location = *iter;

            try
            {
                TA_Base_Core::ActivePlanDetails_var activePlanDetails =
                    PlanAgentAccess::getInstance().getActivePlanDetailsAtLocation(getNodeId(), location);

                m_remotePlanInstanceMap->addPlanInstances(activePlanDetails);
            }
            catch (PlanAgentAccessException& paex)
            {
                CString actionName;
                std::string locationName;
                TA_Base_Bus::TransActiveMessage userMsg;
                unsigned long location = atol (paex.getDetail ().c_str ());
                locationName = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName (location).c_str();
                userMsg << locationName;
                actionName.LoadString(IDS_RETRIEVE_PLAN);
                userMsg << actionName;
                // userMsg.showMsgBox(IDS_UE_210011);
            }
            catch (TA_Base_Core::TransactiveException &ex)
            {
                MessageBox::error(IDS_RETRIEVE_PLAN, ex);
            }
        }
    }

    FUNCTION_EXIT;
    return *m_remotePlanInstanceMap;
}


PlanStepMap &PlanNode::getPlanStepMap()
{
    FUNCTION_ENTRY( "getPlanStepMap" );

    if (!m_planStepMap)
    {
        TA_Base_Core::StepDetails_var stepDetails = PlanAgentAccess::getInstance().getStepDetails(getNodeId());

        m_planStepMap = new PlanStepMap(this,stepDetails);
    }

    FUNCTION_EXIT;
    return *m_planStepMap;
}


void PlanNode::invalidatePlanStepMap()
{
    FUNCTION_ENTRY( "invalidatePlanStepMap" );

    delete m_planStepMap;
    m_planStepMap = NULL;

    FUNCTION_EXIT;
}


PlanStep* PlanNode::getPlanStep(PlanStepId stepId)
{
    FUNCTION_ENTRY( "getPlanStep" );

    PlanStepList planSteps;
    getPlanStepMap().getPlanSteps(planSteps);

    PlanStep *planStep;

    for (PlanStepList::const_iterator siter = planSteps.begin(); siter != planSteps.end(); siter++)
    {
        planStep = *siter;
        if (planStep->getStepId() == stepId)
        {
            FUNCTION_EXIT;
            return planStep;
        }
    }

    FUNCTION_EXIT;
    return NULL;
}


void PlanNode::notifyPlanGeneralDetailUpdate()
{
    FUNCTION_ENTRY( "notifyPlanGeneralDetailUpdate" );

    CPlanEditorThread *planEditorThread = FloatingWindowThreadRegister::getInstance().getPlanEditorThread(getNodeId());
    if (planEditorThread)
    {
        planEditorThread->PostThreadMessage(WM_REFRESH_PLAN_GENERAL_DETAIL, 0, 0);
    }

    FUNCTION_EXIT;
}


void PlanNode::notifyPlanStepUpdate(PlanStepNumber stepNumber, unsigned long stepCount)
{
    FUNCTION_ENTRY( "notifyPlanStepUpdate" );

    CPlanEditorThread *planEditorThread = FloatingWindowThreadRegister::getInstance().getPlanEditorThread(getNodeId());
    if (planEditorThread)
    {
        planEditorThread->PostThreadMessage(WM_REFRESH_PLAN_STEP_LIST, (WPARAM) stepNumber, (LPARAM) stepCount);
    }

    FUNCTION_EXIT;
}


bool PlanNode::editPlanStep(PlanStep* planStep)
{
    FUNCTION_ENTRY( "editPlanStep" );

    TA_ASSERT(planStep!=NULL, "No plan step specified.");

    if (planStep->editStep())
    {
        m_areStepsUpdated = true;

        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


void PlanNode::insertPlanStep(TA_Base_Core::EStep stepType, PlanStepNumber position)
{
    FUNCTION_ENTRY( "insertPlanStep" );

    TA_ASSERT(m_planStepMap!=NULL, "Plan steps not loaded to insert.");

    m_planStepMap->insertPlanStep(stepType, position);

    // Refresh the step list in the editor. Request selection of the new step added at the specified position.
    notifyPlanStepUpdate(position, 1);

    m_areStepsUpdated = true;

    FUNCTION_EXIT;
}


void PlanNode::deletePlanSteps(PlanStepList &deleteSteps)
{
    FUNCTION_ENTRY( "deletePlanSteps" );

    TA_ASSERT(m_planStepMap != NULL, "Plan steps not loaded to delete.");
    TA_ASSERT(deleteSteps.size() > 0, "No plan step specified.");

    PlanStepNumber firstRemovedPosition = m_planStepMap->deletePlanSteps(deleteSteps);
    TA_ASSERT(deleteSteps.empty(), "Deleted step list is not empty");

    // Refresh the step list in the editor. Request selection of the step that now has the position
    // of the first of the deleted steps.
    notifyPlanStepUpdate(firstRemovedPosition, 1);

    m_areStepsUpdated = true;

    FUNCTION_EXIT;
}


void PlanNode::copyPlanSteps(const PlanStepList &copySteps, PlanStepNumber position)
{
    FUNCTION_ENTRY( "copyPlanSteps" );

    TA_ASSERT(m_planStepMap != NULL, "Plan steps not loaded to insert.");
    TA_ASSERT(copySteps.size() > 0, "No plan step specified.");

    m_planStepMap->copyPlanSteps(copySteps, position);

    // Refresh the step list in the editor. Request selection of the copied steps added at the specified position.
    notifyPlanStepUpdate(position, copySteps.size());

    m_areStepsUpdated = true;

    FUNCTION_EXIT;
}


void PlanNode::movePlanSteps(const PlanStepList &moveSteps, PlanStepNumber position)
{
    FUNCTION_ENTRY( "movePlanSteps" );

    TA_ASSERT(m_planStepMap != NULL,"Plan steps not loaded to insert.");
    TA_ASSERT(moveSteps.size() > 0, "No plan step specified.");

    PlanStepNumber adjustedPosition = m_planStepMap->movePlanSteps(moveSteps, position);

    // Refresh the step list in the editor. Request selection of the moved steps at their final position.
    // This position will not be the same as the specified target position if steps were moved from before
    // the target position.
    notifyPlanStepUpdate(adjustedPosition, moveSteps.size());

    m_areStepsUpdated = true;

    FUNCTION_EXIT;
}


PlanStepNumber PlanNode::getLastStepPosition()
{
    FUNCTION_ENTRY( "getLastStepPosition" );
    FUNCTION_EXIT;
    return getPlanStepMap().getLastStepPosition();
}


