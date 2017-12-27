//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CategoryNodeView.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include <sstream>
#include "core/utilities/src/RunParams.h"
#include "PlanManager.h"
#include "CategoryNodeView.h"
#include "PlanTreeDoc.h"
#include "ViewUpdate.h"
#include "PlanAgentAccess.h"
#include "TreeNodeFactory.h"
#include "TreeNode.h"
#include "CategoryNode.h"
#include "RootNode.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CCategoryNodeView

IMPLEMENT_DYNCREATE(CCategoryNodeView, CView)

CCategoryNodeView::CCategoryNodeView()
{
    FUNCTION_ENTRY( "CCategoryNodeView" );
    FUNCTION_EXIT;
}


CCategoryNodeView::~CCategoryNodeView()
{
    FUNCTION_ENTRY( "~CCategoryNodeView" );
    FUNCTION_EXIT;
}


CCategoryListCtrl& CCategoryNodeView::GetListCtrl()
{
    FUNCTION_ENTRY( "GetListCtrl" );
    FUNCTION_EXIT;
    return m_categoryListCtrl;
}


CPlanTreeDoc *CCategoryNodeView::GetDocument()
{
    FUNCTION_ENTRY( "GetDocument" );

    CPlanTreeDoc *planTreeDoc = dynamic_cast<CPlanTreeDoc *>(CView::GetDocument());
    TA_ASSERT(planTreeDoc != NULL, "Associated document is not a CPlanTreeDoc");

    FUNCTION_EXIT;
    return planTreeDoc;
}


CategoryNode *CCategoryNodeView::GetCategoryNode()
{
    FUNCTION_ENTRY( "GetCategoryNode" );

    CategoryNode *categoryNode = dynamic_cast<CategoryNode *>(GetDocument()->GetCurrentNode());
    TA_ASSERT(categoryNode != NULL, "Category node pointer is null");

    FUNCTION_EXIT;
    return categoryNode;
}


void CCategoryNodeView::PostNcDestroy()
{
    FUNCTION_ENTRY( "PostNcDestroy" );

    // TODO: Add your specialized code here and/or call the base class
    GetDocument()->PurgeCategoryViewNodes();

    CView::PostNcDestroy();

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CCategoryNodeView, CView)
    // {{AFX_MSG_MAP(CCategoryNodeView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_CATEGORY_CONTENT_LIST, OnCategoryListItemChanged)
    ON_NOTIFY(NM_DBLCLK, IDC_CATEGORY_CONTENT_LIST, OnCategoryListItemDblclk)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_WINDOW_HAS_BECOME_VISIBLE, OnViewShown)
    ON_MESSAGE(WM_RESET_TREE_NODE_VIEW, OnResetTreeNodeView)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCategoryNodeView drawing

void CCategoryNodeView::OnDraw(CDC* pDC)
{
    FUNCTION_ENTRY( "OnDraw" );

    CDocument* pDoc = GetDocument();
    // TODO: add draw code here

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CCategoryNodeView diagnostics

#ifdef _DEBUG
void CCategoryNodeView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CView::AssertValid();

    FUNCTION_EXIT;
}


void CCategoryNodeView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCategoryNodeView message handlers

int CCategoryNodeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CView::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }


    // TODO: Add your specialized creation code here

    // Create the active instances list control.
    if (!m_categoryListCtrl.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0), this, IDC_CATEGORY_CONTENT_LIST))
    {
        FUNCTION_EXIT;
        return -1;
    }

    FUNCTION_EXIT;
    return 0;
}


void CCategoryNodeView::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (::IsWindow(GetListCtrl().GetSafeHwnd()))
    {
        GetListCtrl().MoveWindow(0, 0, cx, cy, FALSE);
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::OnCategoryListItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnCategoryListItemChanged" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED &&
        (pNMListView->uOldState & LVIS_SELECTED) != LVIS_SELECTED)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pNMListView->lParam);
        TA_ASSERT(treeNode, "Tree node is null");

        GetDocument()->AddToCategoryViewNodes(treeNode);
        AfxGetMainWnd()->SendMessage(WM_CATEGORY_VIEW_SELECTION_CHANGED); // update main toolbar
    }
    else if ((pNMListView->uNewState & LVIS_SELECTED) != LVIS_SELECTED &&
             (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pNMListView->lParam);
        TA_ASSERT(treeNode, "Tree node is null");

        GetDocument()->RemoveFromCategoryViewNodes(treeNode);
        AfxGetMainWnd()->SendMessage(WM_CATEGORY_VIEW_SELECTION_CHANGED); // update main toolbar
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CCategoryNodeView::OnCategoryListItemDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnCategoryListItemDblclk" );

    // If the selected nodes are not all plans, the double click has no effect.
    if (!GetDocument()->ArePlanNodesSelected())
    {
        *pResult = 0;

        FUNCTION_EXIT;
        return;
    }

    TreeNodeList selectedNodes = GetDocument()->GetSelectedNodes();

    bool selectedNodesAllApproved = !selectedNodes.empty();
    bool selectedNodesAllUnapproved = !selectedNodes.empty();

    // Iterate over selected nodes and determine whether they are all approved or unapproved plans.
    // Where a mixture of approved and unapproved plans is selected, the double click has no effect.
    for (TreeNodeList::iterator iter = selectedNodes.begin(); iter != selectedNodes.end(); iter++)
    {
        TreeNode *treeNode = *iter;

        selectedNodesAllApproved = selectedNodesAllApproved && treeNode->isApproved();
        selectedNodesAllUnapproved = selectedNodesAllUnapproved && !treeNode->isApproved();
    }

    if (selectedNodesAllApproved)
    {
        // For approved plans launch the Plan Controller
        GetParentFrame()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_PLAN_CONTROL, 0), (LPARAM) NULL);
    }
    else if (selectedNodesAllUnapproved)
    {
        // For unapproved plans launch the Plan Editor
        GetParentFrame()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_PLAN_EDIT, 0), (LPARAM) NULL);
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


LRESULT CCategoryNodeView::OnViewShown(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnViewShown" );

    // User switched from another plan tree view - update main toolbar according to current selection
    AfxGetMainWnd()->SendMessage(WM_CATEGORY_VIEW_SELECTION_CHANGED);

    FUNCTION_EXIT;
    return 0;
}


LRESULT CCategoryNodeView::OnResetTreeNodeView(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnResetTreeNodeView" );

    GetDocument()->PurgeCategoryViewNodes();
    CategoryNode *categoryNode = GetCategoryNode();

    try
    {
        CWaitCursor waitCursor;

        GetListCtrl().DeleteAllItems();
        categoryNode->addChildrenToList(GetListCtrl(), GetDocument());
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_DISPLAY_CATEGORY, ex);
    }

    FUNCTION_EXIT;
    return 0;
}


void CCategoryNodeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    FUNCTION_ENTRY( "OnUpdate" );

    // TODO: Add your specialized code here and/or call the base class
    CViewUpdate<TA_Base_Core::PlanConfigUpdateData> *pcdUpdate = dynamic_cast<CViewUpdate<TA_Base_Core::PlanConfigUpdateData> *>(pHint);
    if (pcdUpdate)
    {
        TA_Base_Core::PlanConfigUpdateData planConfigData = pcdUpdate->GetUpdateData();

        switch (planConfigData.configUpdate._d())
        {
            case TA_Base_Core::PCUT_APPROVAL_STATE:
                ProcessPlanApprovalStateUpdate(planConfigData.configUpdate.approvalState());
                break;

            case TA_Base_Core::PCUT_UPDATE_PLAN:
                ProcessModifyNodeUpdate(planConfigData.configUpdate.updatePlan(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_INSERT_PLAN:
                ProcessInsertNodeUpdate(planConfigData.configUpdate.insertPlan(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_INSERT_PLAN_INTO_ROOT:
                ProcessInsertNodeIntoRootUpdate(planConfigData.configUpdate.insertPlanIntoRoot(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_COPY_PLAN: // a plan copy is essentially an insert
                ProcessInsertNodeUpdate(planConfigData.configUpdate.copyPlan(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_COPY_PLAN_TO_ROOT:
                ProcessInsertNodeIntoRootUpdate(planConfigData.configUpdate.copyPlanToRoot(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_MOVE_PLAN:
                ProcessMoveNodeUpdate(planConfigData.configUpdate.movePlan(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_MOVE_PLAN_TO_ROOT:
                ProcessMoveNodeToRootUpdate(planConfigData.configUpdate.movePlanToRoot(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_MOVE_PLAN_FROM_ROOT:
                ProcessMoveNodeFromRootUpdate(planConfigData.configUpdate.movePlanFromRoot(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_DELETE_PLANS:
                ProcessDeleteNodeUpdate(planConfigData.configUpdate.deletePlans());
                break;

            case TA_Base_Core::PCUT_DELETE_PLANS_FROM_ROOT:
                ProcessDeleteNodeFromRootUpdate(planConfigData.configUpdate.deletePlansFromRoot());
                break;

            case TA_Base_Core::PCUT_UPDATE_CATEGORY:
                ProcessModifyNodeUpdate(planConfigData.configUpdate.updateCategory(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_INSERT_CATEGORY:
                ProcessInsertNodeUpdate(planConfigData.configUpdate.insertCategory(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_INSERT_CATEGORY_INTO_ROOT:
                ProcessInsertNodeIntoRootUpdate(planConfigData.configUpdate.insertCategoryIntoRoot(), planConfigData.session.in());
                break;

            case TA_Base_Core::PCUT_DELETE_CATEGORY:
                ProcessDeleteNodeUpdate(planConfigData.configUpdate.deleteCategory());
                break;

            case TA_Base_Core::PCUT_DELETE_CATEGORY_FROM_ROOT:
                ProcessDeleteNodeFromRootUpdate(planConfigData.configUpdate.deleteCategoryFromRoot());
                break;

            default:
                FUNCTION_EXIT;
                return;
        }

        // Force a refresh of the plan tree
        Invalidate();
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate &approvalStateUpdate)
{
    FUNCTION_ENTRY( "ProcessPlanApprovalStateUpdate" );

    TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(approvalStateUpdate.plan);
    if (treeNode)
    {
        // If the node does not belong to the current category we'll need to have a look further down
        if (treeNode->getNodeSummary().parent != GetCategoryNode()->getNodeId())
        {
            treeNode = NULL;
        }
    }

    if (!treeNode)
    {
        try
        {
            // The node could belong to a descendant of the current category.
            TreeNodeIdList nodeIds;
            TreeNodeFactory::getInstance().getNodeIdsInNodePath(approvalStateUpdate.plan, nodeIds);

            // skip the plan node ID
            for (unsigned int i = 0; i < (nodeIds.size() - 1); i++)
            {
                if (nodeIds[i] == GetCategoryNode()->getNodeId())
                {
                    // Get the next sub-category in the path
                    treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeIds[i+1]);

                    break;
                }
            }
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            std::ostringstream errorMessage;
            MessageBox::error(IDS_SEARCH_CATEGORY_PATH, ex);
        }
    }

    if (treeNode)
    {
        // Add or remove the node from the view as per the filter. If the node is already part of the view,
        // it remains untouched.
        if (GetDocument()->isRelevant(*treeNode))
        {
            treeNode->addToList(GetListCtrl(), GetDocument());
        }
        else
        {
            GetListCtrl().RemoveNode(treeNode);
        }
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessModifyNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessModifyNodeUpdate" );

    // If the node belongs to the current category
    if (nodeSummary.parent == GetCategoryNode()->getNodeId())
    {
        // Sort the contents of the list in case the node has been renamed
        GetListCtrl().SortItems(TreeNode::compareSiblingNodes, 0);
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessInsertNodeUpdate" );

    // If the node belongs to the current category
    if (nodeSummary.parent == GetCategoryNode()->getNodeId())
    {
        TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeSummary.node);
        TA_ASSERT(treeNode, "Tree node does not exist");

        treeNode->addToList(GetListCtrl(), GetDocument());

        FUNCTION_EXIT;
        return;
    }

    try
    {
        // The node could belong to a descendant of the current category.
        TreeNodeIdList nodeIds;
        TreeNodeFactory::getInstance().getNodeIdsInNodePath(nodeSummary.parent, nodeIds);

        for (unsigned int i = 0; i < (nodeIds.size() - 1); i++)
        {
            if (nodeIds[i] == GetCategoryNode()->getNodeId())
            {
                // Get the next sub-category in the path
                TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeIds[i+1]);
                TA_ASSERT(treeNode, "Tree node does not exist");

                treeNode->addToList(GetListCtrl(), GetDocument());

                break;
            }
        }
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_SEARCH_CATEGORY_PATH, ex);
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessInsertNodeIntoRootUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessInsertNodeIntoRootUpdate" );

    // If current category is a root one
    if (dynamic_cast<RootNode *>(GetCategoryNode()) != NULL)
    {
        TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeSummary.node);
        TA_ASSERT(treeNode, "Tree node does not exist");

        treeNode->addToList(GetListCtrl(), GetDocument());
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessMoveNodeUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessMoveNodeUpdate" );

    // A node move is essentially a delete followed by an insert.
    TA_Base_Core::DeleteNodeUpdate deleteNodeUpdate;
    deleteNodeUpdate.parent = transferNodeUpdate.oldParent;
    deleteNodeUpdate.nodes.length(1);
    deleteNodeUpdate.nodes[0] = transferNodeUpdate.summary.node;

    ProcessDeleteNodeUpdate(deleteNodeUpdate);
    ProcessInsertNodeUpdate(transferNodeUpdate.summary, sessionId);

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessMoveNodeToRootUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessMoveNodeToRootUpdate" );

    // A node move is essentially a delete followed by an insert.
    TA_Base_Core::DeleteNodeUpdate deleteNodeUpdate;
    deleteNodeUpdate.parent = transferNodeUpdate.oldParent;
    deleteNodeUpdate.nodes.length(1);
    deleteNodeUpdate.nodes[0] = transferNodeUpdate.summary.node;

    ProcessDeleteNodeUpdate(deleteNodeUpdate);
    ProcessInsertNodeIntoRootUpdate(transferNodeUpdate.summary, sessionId);

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessMoveNodeFromRootUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessMoveNodeFromRootUpdate" );

    // A node move is essentially a delete followed by an insert.
    ProcessDeleteNodeFromRootUpdate(transferNodeUpdate.summary.node);
    ProcessInsertNodeUpdate(transferNodeUpdate.summary, sessionId);

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeUpdate" );

    // If the node(s) were deleted from the current category
    if (deleteNodeUpdate.parent == GetCategoryNode()->getNodeId())
    {
        for (CORBA::ULong i = 0; i < deleteNodeUpdate.nodes.length(); i++)
        {
            TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(deleteNodeUpdate.nodes[i]);
            TA_ASSERT(treeNode, "Tree node does not exist");

            GetListCtrl().RemoveNode(treeNode);
        }

        FUNCTION_EXIT;
        return;
    }

    try
    {
        // The node could belong to a descendant of the current category.
        TreeNodeIdList nodeIds;
        TreeNodeFactory::getInstance().getNodeIdsInNodePath(deleteNodeUpdate.parent, nodeIds);

        for (unsigned int i = 0; i < (nodeIds.size() - 1); i++)
        {
            if (nodeIds[i] == GetCategoryNode()->getNodeId())
            {
                // Get the next sub-category in the path
                TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeIds[i+1]);
                TA_ASSERT(treeNode, "Tree node does not exist");

                if (!GetDocument()->isRelevant(*treeNode))
                {
                    GetListCtrl().RemoveNode(treeNode);
                }

                break;
            }
        }
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_SEARCH_CATEGORY_PATH, ex);
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessDeleteNodeFromRootUpdate(const TA_Base_Core::NodeIds &nodeIds)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeFromRootUpdate" );

    for (CORBA::ULong i = 0; i < nodeIds.length(); i++)
    {
        ProcessDeleteNodeFromRootUpdate(nodeIds[i]);
    }

    FUNCTION_EXIT;
}


void CCategoryNodeView::ProcessDeleteNodeFromRootUpdate(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeFromRootUpdate" );

    // If current category is a root one
    if (dynamic_cast<RootNode *>(GetCategoryNode()) != NULL)
    {
        TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeId);
        TA_ASSERT(treeNode, "Tree node does not exist");

        GetListCtrl().RemoveNode(treeNode);
    }

    FUNCTION_EXIT;
}


