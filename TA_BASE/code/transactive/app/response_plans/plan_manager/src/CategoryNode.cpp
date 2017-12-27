//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CategoryNode.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/08/03 17:02:56 $
// Last modified by:  $Author: qi.huang $
//
// This is a representation of a category node within the plan tree.

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/response_plans/plan_manager/src/Resource.h"
#include "app/response_plans/plan_manager/src/CategoryNode.h"
#include "app/response_plans/plan_manager/src/TreeNodeFactory.h"
#include "app/response_plans/plan_manager/src/ITreeNodeFilter.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/OperatorRights.h"
#include "app/response_plans/plan_manager/src/CategoryNodeView.h"
#include "app/response_plans/plan_manager/src/LabelMaker.h"
#include "app/response_plans/plan_manager/src/PlanAgentAccess.h"
#include "app/response_plans/plan_manager/src/Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CategoryNode

CategoryNode::CategoryNode(const TA_Base_Core::NodeSummary &nodeSummary) :
    TreeNode(nodeSummary),
    m_childNodeMap(NULL)
{
    FUNCTION_ENTRY( "CategoryNode" );

    TA_ASSERT(nodeSummary.type == TA_Base_Core::CATEGORY_NODE, "Not a category node summary");

    FUNCTION_EXIT;
}


CategoryNode::~CategoryNode()
{
    FUNCTION_ENTRY( "~CategoryNode" );

    if (m_childNodeMap)
    {
        m_childNodeMap->clear();
    }

    delete m_childNodeMap;

    FUNCTION_EXIT;
}


const std::string &CategoryNode::getNodeType()
{
    FUNCTION_ENTRY( "getNodeType" );

    static const std::string CATEGORY_NODE_TYPE = (LPCTSTR) LabelMaker::getCategoryNodeTypeLabel();

    FUNCTION_EXIT;
    return CATEGORY_NODE_TYPE;
}


const std::string &CategoryNode::getNodeStatus()
{
    FUNCTION_ENTRY( "getNodeStatus" );

    static const std::string EMPTY_CATEGORY_STATUS = (LPCTSTR) LabelMaker::getEmptyCategoryLabel();
    static const std::string NONEMPTY_CATEGORY_STATUS = (LPCTSTR) LabelMaker::getNonEmptyCategoryLabel();

    FUNCTION_EXIT;
    return (getNodeSummary().hasChildren? NONEMPTY_CATEGORY_STATUS : EMPTY_CATEGORY_STATUS);
}


CRuntimeClass *CategoryNode::getNodeViewClass()
{
    FUNCTION_ENTRY( "getNodeViewClass" );
    FUNCTION_EXIT;
    return RUNTIME_CLASS(CCategoryNodeView);
}


void CategoryNode::updateCmdUI(CCmdUI &cmdUI)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    switch (cmdUI.m_nID)
    {
        case ID_CATEGORY_NEW:
        case ID_CATEGORY_RENAME:
          case ID_IMPORT_PLAN:
            cmdUI.Enable(OperatorRights::getInstance().canEditCategories());
            break;

        case ID_CATEGORY_DELETE:            // only empty categories can be deleted
        case ID_CATEGORY_OR_PLAN_DELETE:
            cmdUI.Enable(!getNodeSummary().hasChildren && OperatorRights::getInstance().canEditCategories());
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


ETreeNodeImageType CategoryNode::getNodeImageType(bool isExpanded)
{
    FUNCTION_ENTRY( "getNodeImageType" );
    FUNCTION_EXIT;
    return (isExpanded? TNIT_OPEN_FOLDER : TNIT_CLOSED_FOLDER);
}


void CategoryNode::addChildrenToTree(CTreeCtrl &treeCtrl, HTREEITEM parent, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addChildrenToTree" );

    TreeNodeList childNodes;
    getChildren(childNodes);

    for (TreeNodeList::iterator iter = childNodes.begin(); iter != childNodes.end(); iter++)
    {
        TreeNode *childNode = (TreeNode *) *iter;
        childNode->addToTree(treeCtrl, parent, filter);
    }

    FUNCTION_EXIT;
}


void CategoryNode::addChildrenToList(CListCtrl &listCtrl, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addChildrenToList" );

    TreeNodeList childNodes;
    getChildren(childNodes);

    listCtrl.SetItemCount(childNodes.size());

    for (TreeNodeList::iterator iter = childNodes.begin(); iter != childNodes.end(); iter++)
    {
        TreeNode *childNode = (TreeNode *) *iter;
        childNode->addToList(listCtrl, filter);
    }

    FUNCTION_EXIT;
}


void CategoryNode::renameNode(std::string nodeName)
{
    FUNCTION_ENTRY( "renameNode" );

    TA_Base_Core::CategoryDetail newCategoryDetail = getCategoryDetail();
     std::string oldName = newCategoryDetail.name;
     std::string newName = nodeName;

     // as per TD 14880
    if (oldName.compare (newName) == 0)
    {
        FUNCTION_EXIT;
        return;
    }

    newCategoryDetail.name = CORBA::string_dup(nodeName.c_str());

    PlanAgentAccess::getInstance().updateCategory(newCategoryDetail);

    // Reset node information used in updating plan tree items. The agent will be requeried and should respond
    // with the new name. This means that after the name of a plan has been edited in the tree it will not reset
    // to its old value before a plan change notification comes back from the agent.
    invalidateNodeName();
    invalidateNodeSummary();
    invalidateNodeDetail();

    FUNCTION_EXIT;
}


void CategoryNode::updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateChildNodeList" );

    // Reset approved/unapproved descendant flags
    invalidateDescendantInfo();

    // Add to or remove from the child node list as appropriate
    switch (planConfigData.configUpdate._d())
    {
        case TA_Base_Core::PCUT_INSERT_PLAN:
        {
            if (planConfigData.configUpdate.insertPlan().parent == getNodeId())
            {
                addToChildNodeList(planConfigData.configUpdate.insertPlan());
            }
            break;
        }

        case TA_Base_Core::PCUT_COPY_PLAN:
        {
            if (planConfigData.configUpdate.copyPlan().parent == getNodeId())
            {
                addToChildNodeList(planConfigData.configUpdate.copyPlan());
            }
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN:
        {
            if (planConfigData.configUpdate.movePlan().summary.parent == getNodeId())
            {
                addToChildNodeList(planConfigData.configUpdate.movePlan().summary);
            }
            if (planConfigData.configUpdate.movePlan().oldParent == getNodeId())
            {
                removeFromChildNodeList(planConfigData.configUpdate.movePlan().summary.node);
            }
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_TO_ROOT:
        {
            if (planConfigData.configUpdate.movePlanToRoot().oldParent == getNodeId())
            {
                removeFromChildNodeList(planConfigData.configUpdate.movePlanToRoot().summary.node);
            }
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_FROM_ROOT:
        {
            if (planConfigData.configUpdate.movePlanFromRoot().summary.parent == getNodeId())
            {
                addToChildNodeList(planConfigData.configUpdate.movePlanFromRoot().summary);
            }
            break;
        }

        case TA_Base_Core::PCUT_DELETE_PLANS:
        {
            if (planConfigData.configUpdate.deletePlans().parent == getNodeId())
            {
                for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deletePlans().nodes.length(); i++)
                {
                    removeFromChildNodeList(planConfigData.configUpdate.deletePlans().nodes[i]);
                }
            }
            break;
        }

        case TA_Base_Core::PCUT_INSERT_CATEGORY:
        {
            if (planConfigData.configUpdate.insertCategory().parent == getNodeId())
            {
                addToChildNodeList(planConfigData.configUpdate.insertCategory());
            }
            break;
        }

        case TA_Base_Core::PCUT_DELETE_CATEGORY:
        {
            if (planConfigData.configUpdate.deleteCategory().parent == getNodeId())
            {
                for (CORBA::ULong i = 0; i < planConfigData.configUpdate.deleteCategory().nodes.length(); i++)
                {
                    removeFromChildNodeList(planConfigData.configUpdate.deleteCategory().nodes[i]);
                }
            }
            break;
        }
    }

    FUNCTION_EXIT;
}


void CategoryNode::invalidateDescendantInfo()
{
    FUNCTION_ENTRY( "invalidateDescendantInfo" );

    // Information about descendants is contained within the node summary.
    // To reset this information we invalidate the entire summary. A subsequent
    // request for summary will refresh its contents.
    // All ancestor nodes, except the root, need to be invalidated as well.

    invalidateNodeSummary();

    CategoryNode *parentNode = dynamic_cast<CategoryNode *>(getParentNode());
    if (parentNode)
    {
        parentNode->invalidateDescendantInfo();
    }

    FUNCTION_EXIT;
}


// Returns true if this category node precedes the specified sibling node on a plan tree branch.
bool CategoryNode::hasPrecedence(TreeNode &siblingNode)
{
    FUNCTION_ENTRY( "hasPrecedence" );

    CategoryNode *categoryNode = dynamic_cast<CategoryNode *>(&siblingNode);

    // Category nodes precede plan nodes. If the sibling node is not a category (i.e. must be a plan),
    // then this category is given precedence.
    if (!categoryNode)
    {
        FUNCTION_EXIT;
        return true;
    }

    // If both this and the sibling are category nodes, then compare them on name basis.
    std::string thisNodeName = getNodeName();
    std::string otherNodeName = categoryNode->getNodeName();

    FUNCTION_EXIT;
    return std::lexicographical_compare(thisNodeName.begin(), thisNodeName.end(),
                                        otherNodeName.begin(), otherNodeName.end(),
                                        CaseInsensitiveCompare());
}


TA_Base_Core::CategoryDetail &CategoryNode::getCategoryDetail()
{
    FUNCTION_ENTRY( "getCategoryDetail" );
    FUNCTION_EXIT;
    return getNodeDetail().category();
}


void CategoryNode::getChildren(TreeNodeList &childNodes)
{
    FUNCTION_ENTRY( "getChildren" );

    if (!m_childNodeMap)
    {
        createChildren(childNodes);

        m_childNodeMap = new SynchronisedMap<TreeNodeId, TreeNode *>;

        for (TreeNodeList::iterator iter = childNodes.begin(); iter != childNodes.end(); iter++)
        {
            TreeNode *treeNode = (TreeNode *) *iter;

            m_childNodeMap->insert(treeNode->getNodeId(), treeNode);
        }
    }

    m_childNodeMap->getItems(childNodes);

    FUNCTION_EXIT;
}


void CategoryNode::createChildren(TreeNodeList &childNodes)
{
    FUNCTION_ENTRY( "createChildren" );

    TreeNodeFactory::getInstance().createChildren(getNodeId(), childNodes);

    FUNCTION_EXIT;
}


void CategoryNode::addToChildNodeList(const TA_Base_Core::NodeSummary &nodeSummary)
{
    FUNCTION_ENTRY( "addToChildNodeList" );

    // If this category's children have not been created yet, then there's nothing to update.
    if (m_childNodeMap && !m_childNodeMap->find(nodeSummary.node))
    {
        TreeNode *treeNode = TreeNodeFactory::getInstance().createTreeNode(nodeSummary);
        m_childNodeMap->insert(treeNode->getNodeId(), treeNode);
    }

    FUNCTION_EXIT;
}


void CategoryNode::removeFromChildNodeList(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "removeFromChildNodeList" );

    // If this category's children have not been created yet, then there's nothing to update.
    if (m_childNodeMap && m_childNodeMap->find(nodeId))
    {
        // Remove the node from the map but don't delete it. The update processor will do that once all update subscribers
        // have been notified.
        m_childNodeMap->erase(nodeId);
    }

    FUNCTION_EXIT;
}


void CategoryNode::newPlan()
{
    FUNCTION_ENTRY( "newPlan" );

    // Create new plan associated with this node
    PlanAgentAccess::getInstance().addPlan(getNodeId());

    FUNCTION_EXIT;
}


void CategoryNode::importPlan (TA_Base_Core::AuxPlanDetail& plan)
{
    FUNCTION_ENTRY( "importPlan" );

    PlanAgentAccess::getInstance().importPlan(plan, getNodeId ());

    FUNCTION_EXIT;
}


void CategoryNode::copyPlan(PlanNode &planNode, bool overwrite)
{
    FUNCTION_ENTRY( "copyPlan" );

    PlanAgentAccess::getInstance().copyPlan(planNode.getNodeId(), getNodeId(), overwrite);

    FUNCTION_EXIT;
}


void CategoryNode::movePlan(PlanNode &planNode, bool overwrite)
{
    FUNCTION_ENTRY( "movePlan" );

    if (planNode.getNodeSummary().parent != getNodeId())
    {
        PlanAgentAccess::getInstance().movePlan(planNode.getNodeId(), getNodeId(), overwrite);
    }

    FUNCTION_EXIT;
}


void CategoryNode::newCategory()
{
    FUNCTION_ENTRY( "newCategory" );

    static int nMaxCategoryDepth = -1;
    if ( -1 == nMaxCategoryDepth )
    {
        Utilities::convertFromString( TA_Base_Core::RunParams::getInstance().get( RPARAM_MAXCATEGORY ), nMaxCategoryDepth );
    }

    if ( nMaxCategoryDepth <= TreeNodeFactory::getInstance().getTreeNodeLevelInTree( getNodeId() ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Max depth [%d] of category reached", nMaxCategoryDepth );
        AfxMessageBox( "Could not create category since maximize depth of category reached" );
        return;
    }

    // Create new sub-category
    PlanAgentAccess::getInstance().addCategory(getNodeId());

    FUNCTION_EXIT;
}


void CategoryNode::deleteCategory()
{
    FUNCTION_ENTRY( "deleteCategory" );

    // Remove this category
    PlanAgentAccess::getInstance().deleteCategory(getNodeId());

    FUNCTION_EXIT;
}

}


