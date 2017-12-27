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
  * Plan Manager's one and only subscriber of Plan Agent updates for plan execution, customisation and configuration.
  * Update consumers should register with PlanAgentUpdateProcessor to be notified of updates as they are received.
  *
  **/

#include "StdAfx.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "Resource.h"
#include "PlanAgentUpdateProcessor.h"
#include "FloatingWindowThreadRegister.h"
#include "PlanControllerThread.h"
#include "IActivePlanObserver.h"
#include "IPlanConfigObserver.h"
#include "IPlanCustomisationObserver.h"
#include "TreeNodeFactory.h"
#include "TreeNode.h"
#include "PlanNode.h"
#include "RootNode.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TA_Base_App::PlanAgentUpdateProcessor *TA_Base_App::PlanAgentUpdateProcessor::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::PlanAgentUpdateProcessor::s_singletonLock;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanAgentUpdateProcessor

PlanAgentUpdateProcessor::PlanAgentUpdateProcessor()
{
    FUNCTION_ENTRY( "PlanAgentUpdateProcessor" );

    // Subscribe for active plan updates
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate, this, NULL);

    // Subscribe for plan config updates
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::PlanAgentBroadcastComms::PlanConfigUpdate, this, NULL);

    // Subscribe for plan customisation updates
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::PlanAgentBroadcastComms::PlanCustomisationUpdate, this, NULL);

    FUNCTION_EXIT;
}


PlanAgentUpdateProcessor::~PlanAgentUpdateProcessor()
{
    FUNCTION_ENTRY( "~PlanAgentUpdateProcessor" );

    // Unsubscribe from all plan updates
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    // Clear update observer lists
    m_activePlanObservers.clear();
    m_planConfigObservers.clear();
    m_planCustomisationObservers.clear();

    FUNCTION_EXIT;
}


PlanAgentUpdateProcessor& PlanAgentUpdateProcessor::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    if ( NULL == s_instance )
    {
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if ( NULL == s_instance)
        {
            s_instance = new PlanAgentUpdateProcessor();
        }
    }

    FUNCTION_EXIT;
    return *s_instance;
}


void PlanAgentUpdateProcessor::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_Base_Core::ThreadGuard guard(s_singletonLock);

    if ( NULL != s_instance )
    {
        delete s_instance;

        s_instance = NULL;
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::subscribeForActivePlanUpdates(IActivePlanObserver *observer)
{
    FUNCTION_ENTRY( "subscribeForActivePlanUpdates" );

    for (int i = 0; i < m_activePlanObservers.size(); i++)
    {
        if (m_activePlanObservers[i] == observer)
        {
            FUNCTION_EXIT;
            return;
        }
    }

    m_activePlanObservers.push_back(observer);

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::subscribeForPlanConfigUpdates(IPlanConfigObserver *observer)
{
    FUNCTION_ENTRY( "subscribeForPlanConfigUpdates" );

    for (int i = 0; i < m_planConfigObservers.size(); i++)
    {
        if (m_planConfigObservers[i] == observer)
        {
            FUNCTION_EXIT;
            return;
        }
    }

    m_planConfigObservers.push_back(observer);

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::subscribeForPlanCustomisationUpdates(IPlanCustomisationObserver *observer)
{
    FUNCTION_ENTRY( "subscribeForPlanCustomisationUpdates" );

    for (int i = 0; i < m_planCustomisationObservers.size(); i++)
    {
        if (m_planCustomisationObservers[i] == observer)
        {
            FUNCTION_EXIT;
            return;
        }
    }

    m_planCustomisationObservers.push_back(observer);

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::unsubscribeFromActivePlanUpdates(IActivePlanObserver *observer)
{
    FUNCTION_ENTRY( "unsubscribeFromActivePlanUpdates" );

    for (int i = 0; i < m_activePlanObservers.size(); i++)
    {
        if (m_activePlanObservers[i] == observer)
        {
            m_activePlanObservers.erase(i);

            FUNCTION_EXIT;
            return;
        }
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::unsubscribeFromPlanConfigUpdates(IPlanConfigObserver *observer)
{
    FUNCTION_ENTRY( "unsubscribeFromPlanConfigUpdates" );

    for (int i = 0; i < m_planConfigObservers.size(); i++)
    {
        if (m_planConfigObservers[i] == observer)
        {
            m_planConfigObservers.erase(i);

            FUNCTION_EXIT;
            return;
        }
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::unsubscribeFromPlanCustomisationUpdates(IPlanCustomisationObserver *observer)
{
    FUNCTION_ENTRY( "unsubscribeFromPlanCustomisationUpdates" );

    for (int i = 0; i < m_planCustomisationObservers.size(); i++)
    {
        if (m_planCustomisationObservers[i] == observer)
        {
            m_planCustomisationObservers.erase(i);

            FUNCTION_EXIT;
            return;
        }
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY( "receiveSpecialisedMessage" );

    // Try extracting to ActivePlanDetail.
    TA_Base_Core::ActivePlanDetail *activePlanDetail = NULL;
    if ((message.messageState >>= activePlanDetail))
    {
        TA_Base_Core::ActivePlanDetail activePlanDetailtmp = *activePlanDetail;
        processActivePlanDetailUpdate(activePlanDetailtmp);

        FUNCTION_EXIT;
        return;
    }

    // Not ActivePlanDetail. Try extracting to PlanConfigUpdateData.
    TA_Base_Core::PlanConfigUpdateData *planConfigData = NULL;
    if ((message.messageState >>= planConfigData))
    {
        TA_Base_Core::PlanConfigUpdateData planConfigDatatmp =  *planConfigData;
        processPlanConfigUpdate(planConfigDatatmp);

        FUNCTION_EXIT;
        return;
    }

    // Not PlanConfigUpdateData either. Must be PlanCustomisationUpdate.
    TA_Base_Core::PlanCustomisationUpdate *planCustomDetail = NULL;
    if ((message.messageState >>= planCustomDetail))
    {
        TA_Base_Core::PlanCustomisationUpdate planCustomDetailtmp = *planCustomDetail;
        processPlanCustomisationUpdate(planCustomDetailtmp);
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::processActivePlanDetailUpdate(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "processActivePlanDetailUpdate" );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "Update [%lu] - Plan [%lu:%s] - Location [%lu] - Instance [%lu] - Owner [%s] - Step [%lu:%s] - Plan State [%d] - Step State [%d]",
        activePlanDetail.activeUpdateId, activePlanDetail.plan.plan, activePlanDetail.path.in(), activePlanDetail.plan.location,
        activePlanDetail.plan.instance, activePlanDetail.owner.in(), activePlanDetail.currentStepDetail.position,
        activePlanDetail.currentStepDetail.name.in(), activePlanDetail.activePlanState, activePlanDetail.currentStepDetail.state);

    PlanNode *planNode = static_cast<PlanNode*> (TreeNodeFactory::getInstance().getTreeNode(activePlanDetail.plan.plan));

    if (planNode)
    {
        planNode->updatePlanInstance(activePlanDetail);
    }

    for (int i = 0; i < m_activePlanObservers.size(); i++)
    {
        m_activePlanObservers[i]->activeStateChanged(activePlanDetail);
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::processPlanConfigUpdate(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "processPlanConfigUpdate" );

    // Update tree node(s) affected by the update. Where appropriate, inform the ancestor node that its internal descendant
    // flags (i.e. has children, has approved/unapproved descendants) may no longer be valid and will need to be refreshed
    // by contacting the Plan Agent. There's no need to do this for the root node - the update will contain enough information
    // to be able to maintain the state of the corresponding item in the plan tree.

    bool retiringTreeNode = false;

    switch (planConfigData.configUpdate._d())
    {
        // Plan approval state change
        case TA_Base_Core::PCUT_APPROVAL_STATE:
        {
            updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.approvalState().plan));
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getClosestAncestor(planConfigData.configUpdate.approvalState().plan));
            break;
        }

        // Plan node configuration changes
        case TA_Base_Core::PCUT_UPDATE_PLAN:
        {
            updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.updatePlan().node));
            break;
        }

        case TA_Base_Core::PCUT_INSERT_PLAN:
        {
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.insertPlan().parent));
            break;
        }

        case TA_Base_Core::PCUT_INSERT_PLAN_INTO_ROOT:
        {
            updateRootNode(planConfigData);
            break;
        }

        case TA_Base_Core::PCUT_COPY_PLAN:
        {
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.copyPlan().parent));
            break;
        }

        case TA_Base_Core::PCUT_COPY_PLAN_TO_ROOT:
        {
            updateRootNode(planConfigData);
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN:
        {
            updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.movePlan().summary.node));
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.movePlan().summary.parent));
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.movePlan().oldParent));
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_TO_ROOT:
        {
            updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.movePlanToRoot().summary.node));
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.movePlanToRoot().oldParent));
            updateRootNode(planConfigData);
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_FROM_ROOT:
        {
            updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.movePlanFromRoot().summary.node));
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.movePlanFromRoot().summary.parent));
            updateRootNode(planConfigData);
            break;
        }

        case TA_Base_Core::PCUT_DELETE_PLANS:
        {
            retiringTreeNode = true;

            for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deletePlans().nodes.length(); i++)
            {
                updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.deletePlans().nodes[i]));
            }
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.deletePlans().parent));
            break;
        }

        case TA_Base_Core::PCUT_DELETE_PLANS_FROM_ROOT:
        {
            retiringTreeNode = true;

            for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deletePlansFromRoot().length(); i++)
            {
                updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.deletePlansFromRoot()[i]));
            }
            updateRootNode(planConfigData);
            break;
        }

        // Category node configuration changes
        case TA_Base_Core::PCUT_UPDATE_CATEGORY:
        {
            updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.updateCategory().node));
            break;
        }

        case TA_Base_Core::PCUT_INSERT_CATEGORY:
        {
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.insertCategory().parent));
            break;
        }

        case TA_Base_Core::PCUT_INSERT_CATEGORY_INTO_ROOT:
        {
            updateRootNode(planConfigData);
            break;
        }

        case TA_Base_Core::PCUT_DELETE_CATEGORY:
        {
            retiringTreeNode = true;

            for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deleteCategory().nodes.length(); i++)
            {
                updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.deleteCategory().nodes[i]));
            }
            updateAncestorNode(planConfigData, TreeNodeFactory::getInstance().getTreeNodeOrClosestAncestor(planConfigData.configUpdate.deleteCategory().parent));
            break;
        }

        case TA_Base_Core::PCUT_DELETE_CATEGORY_FROM_ROOT:
        {
            retiringTreeNode = true;

            updateTreeNode(planConfigData, TreeNodeFactory::getInstance().getTreeNode(planConfigData.configUpdate.deleteCategoryFromRoot()));
            updateRootNode(planConfigData);
            break;
        }
    }

    // Notify subscribers so that they can process the update
    for (int i = 0; i < m_planConfigObservers.size(); i++)
    {
        if (retiringTreeNode)
        {
            m_planConfigObservers[i]->planTreeNodeDeleted(planConfigData);
        }
        else
        {
            m_planConfigObservers[i]->planConfigChanged(planConfigData);
        }
    }

    // The factory must be told when node(s) have been deleted. Subscribers should by now have removed all
    // references to these nodes.
    switch (planConfigData.configUpdate._d())
    {
        case TA_Base_Core::PCUT_DELETE_PLANS:
        {
            for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deletePlans().nodes.length(); i++)
            {
                TreeNodeFactory::getInstance().retireTreeNode(planConfigData.configUpdate.deletePlans().nodes[i]);
            }
            break;
        }

        case TA_Base_Core::PCUT_DELETE_PLANS_FROM_ROOT:
        {
            for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deletePlansFromRoot().length(); i++)
            {
                TreeNodeFactory::getInstance().retireTreeNode(planConfigData.configUpdate.deletePlansFromRoot()[i]);
            }
            break;
        }

        case TA_Base_Core::PCUT_DELETE_CATEGORY:
        {
            for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deleteCategory().nodes.length(); i++)
            {
                TreeNodeFactory::getInstance().retireTreeNode(planConfigData.configUpdate.deleteCategory().nodes[i]);
            }
            break;
        }

        case TA_Base_Core::PCUT_DELETE_CATEGORY_FROM_ROOT:
        {
            TreeNodeFactory::getInstance().retireTreeNode(planConfigData.configUpdate.deleteCategoryFromRoot());
            break;
        }
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::processPlanCustomisationUpdate(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail)
{
    FUNCTION_ENTRY( "processPlanCustomisationUpdate" );

    PlanNode *planNode = static_cast<PlanNode *> (TreeNodeFactory::getInstance().getTreeNode(planCustomDetail.plan.plan));

    if (planNode)
    {
        planNode->updatePlanInstance(planCustomDetail);
    }

    for (int i = 0; i < m_planCustomisationObservers.size(); i++)
    {
        m_planCustomisationObservers[i]->planCustomised(planCustomDetail);
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::updateTreeNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData, TreeNode *treeNode)
{
    FUNCTION_ENTRY( "updateTreeNode" );

    if (treeNode)
    {
        treeNode->updateNode(planConfigData);
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::updateAncestorNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData, TreeNode *ancestorNode)
{
    FUNCTION_ENTRY( "updateAncestorNode" );

    if (ancestorNode)
    {
        ancestorNode->updateChildNodeList(planConfigData);
    }

    FUNCTION_EXIT;
}


void PlanAgentUpdateProcessor::updateRootNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateRootNode" );

    RootNode *rootNode = TreeNodeFactory::getInstance().getRootNode();

    if (rootNode)
    {
        rootNode->updateChildNodeList(planConfigData);
    }

    FUNCTION_EXIT;
}


