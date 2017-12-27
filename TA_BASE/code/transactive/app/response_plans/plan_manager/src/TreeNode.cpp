//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// This is a representation of a node within the plan tree.

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "Resource.h"
#include "TreeNode.h"
#include "TreeNodeFactory.h"
#include "ITreeNodeFilter.h"
#include "BlankView.h"
#include "PlanAgentAccess.h"
#include "RootNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TreeNode

TreeNode::TreeNode(const TA_Base_Core::NodeSummary &nodeSummary) :
    m_nodeId(nodeSummary.node),
    m_parentId(nodeSummary.parent),
    m_nodeName(new std::string(nodeSummary.name.in())),
    m_nodeSummary(new TA_Base_Core::NodeSummary(nodeSummary)),
    m_nodeDetail(NULL),
    m_nodeVisible(TRUE)
{
    FUNCTION_ENTRY( "TreeNode" );
    FUNCTION_EXIT;
}


TreeNode::~TreeNode()
{
    FUNCTION_ENTRY( "~TreeNode" );

    delete m_nodeName;
    delete m_nodeSummary;
    delete m_nodeDetail;

    FUNCTION_EXIT;
}


TreeNodeId TreeNode::getNodeId() const
{
    FUNCTION_ENTRY( "getNodeId" );
    FUNCTION_EXIT;
    return m_nodeId;
}


TreeNodeId TreeNode::getParentId() const
{
    FUNCTION_ENTRY( "getParentId" );
    FUNCTION_EXIT;
    return m_parentId;
}


const std::string &TreeNode::getNodeName()
{
    FUNCTION_ENTRY( "getNodeName" );

    if (!m_nodeName)
    {
        m_nodeName = new std::string(getNodeSummary().name.in());
    }

    FUNCTION_EXIT;
    return *m_nodeName;
}


bool TreeNode::isApproved()
{
    FUNCTION_ENTRY( "isApproved" );
    FUNCTION_EXIT;
    return getNodeSummary().approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE;
}


TA_Base_Core::NodeSummary TreeNode::getNodeSummary()
{
    FUNCTION_ENTRY( "getNodeSummary" );

    if (!m_nodeSummary)
    {
        m_nodeSummary = PlanAgentAccess::getInstance().getNodeSummary(getNodeId());
    }

    FUNCTION_EXIT;
    return *m_nodeSummary;
}


TA_Base_Core::NodeDetail& TreeNode::getNodeDetail()
{
    FUNCTION_ENTRY( "getNodeDetail" );

    if (!m_nodeDetail)
    {
        m_nodeDetail = PlanAgentAccess::getInstance().getNodeDetail(getNodeId());
    }

    FUNCTION_EXIT;
    return *m_nodeDetail;
}


TreeNode *TreeNode::getParentNode()
{
    FUNCTION_ENTRY( "getParentNode" );

    TreeNodeId parentNodeId = getNodeSummary().parent;

    FUNCTION_EXIT;
    return TreeNodeFactory::getInstance().getTreeNode(parentNodeId);
}


CRuntimeClass *TreeNode::getNodeViewClass()
{
    FUNCTION_ENTRY( "getNodeViewClass" );
    FUNCTION_EXIT;
    return RUNTIME_CLASS(CBlankView);
}


void TreeNode::updateCmdUI(CCmdUI &cmdUI)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    cmdUI.Enable(FALSE);

    FUNCTION_EXIT;
}


void TreeNode::addToTree(CTreeCtrl &treeCtrl, HTREEITEM parent, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addToTree" );

    if (filter && !filter->isRelevant(*this))
    {
        FUNCTION_EXIT;
        return;
    }

    HTREEITEM addAfter = TVI_FIRST;

    if (parent && treeCtrl.ItemHasChildren(parent) && treeCtrl.GetChildItem(parent))
    {
        HTREEITEM siblingItem = treeCtrl.GetChildItem(parent);
        // TA_ASSERT(siblingItem != NULL, "Sibling tree item is null");

        while (siblingItem)
        {
            TreeNode *siblingNode = reinterpret_cast<TreeNode *>(treeCtrl.GetItemData(siblingItem));
            TA_ASSERT(siblingNode, "Sibling node pointer is null");

            // See if node is already in the tree
            if (siblingNode == this)
            {
                FUNCTION_EXIT;
                return;
            }

            // See if node should go below its sibling.
            if (siblingNode->hasPrecedence(*this))
            {
                addAfter = siblingItem;  // this node must go below its sibling
            }
            else
            {
                break;
            }

            siblingItem = treeCtrl.GetNextSiblingItem(siblingItem);
        }
    }


    TVINSERTSTRUCT tvis;

    tvis.hParent = parent;
    tvis.hInsertAfter = addAfter;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    tvis.item.pszText = LPSTR_TEXTCALLBACK;
    tvis.item.iImage = tvis.item.iSelectedImage = I_IMAGECALLBACK;
    tvis.item.cChildren = I_CHILDRENCALLBACK;
    tvis.item.lParam = (LPARAM) this;

    treeCtrl.InsertItem(&tvis);

    FUNCTION_EXIT;
}


void TreeNode::addToList(CListCtrl &listCtrl, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addToList" );

    if (filter && !filter->isRelevant(*this))
    {
        FUNCTION_EXIT;
        return;
    }

    int addPosition = 0;
    for (int i = 0; i < listCtrl.GetItemCount(); i++)
    {
        TreeNode *siblingNode = reinterpret_cast<TreeNode *>(listCtrl.GetItemData(i));
        TA_ASSERT(siblingNode, "Sibling node pointer is null");

        // See if node is already in the list
        if (siblingNode == this)
        {
            FUNCTION_EXIT;
            return;
        }

        // See if node should go below its sibling.
        if (siblingNode->hasPrecedence(*this))
        {
            addPosition++;  // this node must go below its sibling
        }
        else
        {
            break;
        }
    }


    LVITEM lvitem;

    lvitem.iItem = addPosition;
    lvitem.iSubItem = 0;
    lvitem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvitem.pszText = LPSTR_TEXTCALLBACK;
    lvitem.iImage = I_IMAGECALLBACK;
    lvitem.lParam = (LPARAM) this;

    listCtrl.InsertItem(&lvitem);

    FUNCTION_EXIT;
}


void TreeNode::updateNode(const TA_Base_Core::NodeSummary &nodeSummary)
{
    FUNCTION_ENTRY( "updateNode" );

    invalidateNodeName();
    invalidateNodeSummary();
    invalidateNodeDetail();

    m_nodeSummary = new TA_Base_Core::NodeSummary(nodeSummary);

    FUNCTION_EXIT;
}


void TreeNode::updateNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateNode" );

    switch (planConfigData.configUpdate._d())
    {
        case TA_Base_Core::PCUT_APPROVAL_STATE:
        {
            TA_ASSERT(getNodeId() == planConfigData.configUpdate.approvalState().plan, "Node ID mismatch");
            TA_Base_Core::NodeSummary nodeSummary = getNodeSummary();
            nodeSummary.approvalState = planConfigData.configUpdate.approvalState().approvalState;
            updateNode(nodeSummary);
            break;
        }

        case TA_Base_Core::PCUT_UPDATE_CATEGORY:
        {
            TA_ASSERT(getNodeId() == planConfigData.configUpdate.updateCategory().node, "Node ID mismatch");
            updateNode(planConfigData.configUpdate.updateCategory());
            break;
        }

        case TA_Base_Core::PCUT_UPDATE_PLAN:
        {
            TA_ASSERT(getNodeId() == planConfigData.configUpdate.updatePlan().node, "Node ID mismatch");
            updateNode(planConfigData.configUpdate.updatePlan());
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN:
        {
            TA_ASSERT(getNodeId() == planConfigData.configUpdate.movePlan().summary.node, "Node ID mismatch");
            updateNode(planConfigData.configUpdate.movePlan().summary);
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_TO_ROOT:
        {
            TA_ASSERT(getNodeId() == planConfigData.configUpdate.movePlanToRoot().summary.node, "Node ID mismatch");
            updateNode(planConfigData.configUpdate.movePlanToRoot().summary);
            break;
        }

        case TA_Base_Core::PCUT_MOVE_PLAN_FROM_ROOT:
        {
            TA_ASSERT(getNodeId() == planConfigData.configUpdate.movePlanFromRoot().summary.node, "Node ID mismatch");
            updateNode(planConfigData.configUpdate.movePlanFromRoot().summary);
            break;
        }

        default:
            ;
    }

    FUNCTION_EXIT;
}


void TreeNode::invalidateNodeName()
{
    FUNCTION_ENTRY( "invalidateNodeName" );

    delete m_nodeName;
    m_nodeName = NULL;

    FUNCTION_EXIT;
}


void TreeNode::invalidateNodeSummary()
{
    FUNCTION_ENTRY( "invalidateNodeSummary" );

    delete m_nodeSummary;
    m_nodeSummary = NULL;

    FUNCTION_EXIT;
}


void TreeNode::invalidateNodeDetail()
{
    FUNCTION_ENTRY( "invalidateNodeDetail" );

    delete m_nodeDetail;
    m_nodeDetail = NULL;

    FUNCTION_EXIT;
}


int CALLBACK TreeNode::compareSiblingNodes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    FUNCTION_ENTRY( "compareSiblingNodes" );

    TreeNode *treeNode1 = reinterpret_cast<TreeNode *>(lParam1);
    TA_ASSERT(treeNode1, "First tree node pointer is null");

    TreeNode *treeNode2 = reinterpret_cast<TreeNode *>(lParam2);
    TA_ASSERT(treeNode1, "Second tree node pointer is null");

    if (treeNode1 == treeNode2)
    {
        FUNCTION_EXIT;
        return 0;
    }

    FUNCTION_EXIT;
    return (treeNode1->hasPrecedence(*treeNode2)? -1 : 1);
}


bool TreeNode::isVisible()
{
    return m_nodeVisible;
}

void TreeNode::setVisible( bool bVisible, bool writeToDb )
{
    m_nodeVisible = bVisible;
    if (writeToDb)
    {
        PlanAgentAccess::getInstance().setPlanType(getNodeId(), (m_nodeVisible? TA_Base_Core::VISIBLE_TYPE : TA_Base_Core::INVISIBLE_TYPE));
    }
}
