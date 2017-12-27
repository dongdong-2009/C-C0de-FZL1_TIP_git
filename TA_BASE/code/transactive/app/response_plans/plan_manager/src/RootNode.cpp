//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/RootNode.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/08/03 17:02:56 $
// Last modified by:  $Author: qi.huang $
//
// This is a representation of the root node within the plan tree.

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "Resource.h"
#include "RootNode.h"
#include "TreeNodeFactory.h"
#include "OperatorRights.h"
#include "LabelMaker.h"
#include "PlanAgentAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// RootNode

RootNode::RootNode(const TA_Base_Core::NodeSummary &nodeSummary) : CategoryNode(nodeSummary)
{
    FUNCTION_ENTRY( "RootNode" );
    FUNCTION_EXIT;
}


RootNode::~RootNode()
{
    FUNCTION_ENTRY( "~RootNode" );
    FUNCTION_EXIT;
}


const std::string &RootNode::getNodeType()
{
    FUNCTION_ENTRY( "getNodeType" );

    static const std::string ROOT_NODE_TYPE = (LPCTSTR) LabelMaker::getRootNodeTypeLabel();

    FUNCTION_EXIT;
    return ROOT_NODE_TYPE;
}


void RootNode::updateCmdUI(CCmdUI &cmdUI)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    switch (cmdUI.m_nID)
    {
        case ID_CATEGORY_NEW:
            cmdUI.Enable(OperatorRights::getInstance().canEditCategories());
            break;

        case ID_PLAN_NEW:
        case ID_PLAN_PASTE:
            cmdUI.Enable(OperatorRights::getInstance().canEditPlans());
            break;

        default:
            cmdUI.Enable(FALSE);
    }

    FUNCTION_EXIT;
}


ETreeNodeImageType RootNode::getNodeImageType(bool isExpanded)
{
    FUNCTION_ENTRY( "getNodeImageType" );
    FUNCTION_EXIT;
    return TNIT_ROOT;
}


void RootNode::invalidateNodeName()
{
    FUNCTION_ENTRY( "invalidateNodeName" );

    // Not applicable to root node.

    FUNCTION_EXIT;
}


void RootNode::invalidateNodeSummary()
{
    FUNCTION_ENTRY( "invalidateNodeSummary" );

    // Not applicable to root node.

    FUNCTION_EXIT;
}


void RootNode::invalidateNodeDetail()
{
    FUNCTION_ENTRY( "invalidateNodeDetail" );

    // Not applicable to root node.

    FUNCTION_EXIT;
}


void RootNode::invalidateDescendantInfo()
{
    FUNCTION_ENTRY( "invalidateDescendantInfo" );

    // Not applicable to root node.

    FUNCTION_EXIT;
}


void RootNode::createChildren(TreeNodeList &childNodes)
{
    FUNCTION_ENTRY( "createChildren" );

    TreeNodeFactory::getInstance().createRootChildren(childNodes);

    FUNCTION_EXIT;
}


TA_Base_Core::NodeDetail &RootNode::getNodeDetail()
{
    FUNCTION_ENTRY( "getNodeDetail" );

    static TA_Base_Core::NodeDetail* s_pDetail = new TA_Base_Core::NodeDetail();

    return *s_pDetail;
}


void RootNode::newPlan()
{
    FUNCTION_ENTRY( "newPlan" );

    // Create new plan associated with this node
    PlanAgentAccess::getInstance().addPlanToRoot();

    FUNCTION_EXIT;
}


void RootNode::copyPlan(PlanNode &planNode, bool overwrite)
{
    FUNCTION_ENTRY( "copyPlan" );

    PlanAgentAccess::getInstance().copyPlanToRoot(planNode.getNodeId(), overwrite);

    FUNCTION_EXIT;
}


void RootNode::movePlan(PlanNode &planNode, bool overwrite)
{
    FUNCTION_ENTRY( "movePlan" );

    if (planNode.getNodeSummary().parent != getNodeId())
    {
        PlanAgentAccess::getInstance().movePlanToRoot(planNode.getNodeId(), overwrite);
    }

    FUNCTION_EXIT;
}


void RootNode::newCategory()
{
    FUNCTION_ENTRY( "newCategory" );

    // Create new category
    PlanAgentAccess::getInstance().addCategoryToRoot();

    FUNCTION_EXIT;
}


void RootNode::renameNode(std::string nodeName)
{
    FUNCTION_ENTRY( "renameNode" );

    // Root node cannot be renamed

    FUNCTION_EXIT;
}


void RootNode::updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateChildNodeList" );

    // Add to or remove from the child node list as appropriate
    switch (planConfigData.configUpdate._d())
    {
        case TA_Base_Core::PCUT_INSERT_PLAN_INTO_ROOT:
        {
            addToChildNodeList(planConfigData.configUpdate.insertPlanIntoRoot());
            break;
        }

        case TA_Base_Core::PCUT_COPY_PLAN_TO_ROOT:
        {
            addToChildNodeList(planConfigData.configUpdate.copyPlanToRoot());
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_TO_ROOT:
        {
            addToChildNodeList(planConfigData.configUpdate.movePlanToRoot().summary);
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_FROM_ROOT:
        {
            removeFromChildNodeList(planConfigData.configUpdate.movePlanFromRoot().summary.node);
            break;
        }

        case TA_Base_Core::PCUT_DELETE_PLANS_FROM_ROOT:
        {
            for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deletePlansFromRoot().length(); i++)
            {
                removeFromChildNodeList(planConfigData.configUpdate.deletePlansFromRoot()[i]);
            }
            break;
        }

        case TA_Base_Core::PCUT_INSERT_CATEGORY_INTO_ROOT:
        {
            addToChildNodeList(planConfigData.configUpdate.insertCategoryIntoRoot());
            break;
        }

        case TA_Base_Core::PCUT_DELETE_CATEGORY_FROM_ROOT:
        {
            removeFromChildNodeList(planConfigData.configUpdate.deleteCategoryFromRoot());
            break;
        }
    }

    FUNCTION_EXIT;
}


