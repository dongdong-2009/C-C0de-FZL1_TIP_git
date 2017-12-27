//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/UnapprovedPlanTreeView.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "PlanManager.h"
#include "UnapprovedPlanTreeView.h"
#include "UnapprovedPlanDoc.h"
#include "TreeNode.h"
#include "TreeNodeFactory.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlanTreeView

IMPLEMENT_DYNCREATE(CUnapprovedPlanTreeView, CPlanTreeView)

CUnapprovedPlanTreeView::CUnapprovedPlanTreeView()
{
    FUNCTION_ENTRY( "CUnapprovedPlanTreeView" );
    FUNCTION_EXIT;
}


CUnapprovedPlanTreeView::~CUnapprovedPlanTreeView()
{
    FUNCTION_ENTRY( "~CUnapprovedPlanTreeView" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CUnapprovedPlanTreeView, CPlanTreeView)
    // {{AFX_MSG_MAP(CUnapprovedPlanTreeView)
    ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDispInfo)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlanTreeView diagnostics

#ifdef _DEBUG
void CUnapprovedPlanTreeView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CTreeView::AssertValid();

    FUNCTION_EXIT;
}


void CUnapprovedPlanTreeView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CTreeView::Dump(dc);

    FUNCTION_EXIT;
}


CUnapprovedPlanDoc* CUnapprovedPlanTreeView::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    CPlanTreeDoc *document = CPlanTreeView::GetDocument();
    ASSERT(document->IsKindOf(RUNTIME_CLASS(CUnapprovedPlanDoc)));

    FUNCTION_EXIT;
    return (CUnapprovedPlanDoc*)document;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlanTreeView message handlers

void CUnapprovedPlanTreeView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnGetDispInfo" );

    CPlanTreeView::OnGetDispInfo(pNMHDR, pResult);

    TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

    if ((pTVDispInfo->item.mask & TVIF_CHILDREN) == TVIF_CHILDREN)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pTVDispInfo->item.lParam);
        TA_ASSERT(treeNode, "Tree node pointer is null");

        pTVDispInfo->item.cChildren = treeNode->getNodeSummary().hasUnapprovedDescendant? 1 : 0;

        // If the node does not have any unapproved descendants, it may still need to be shown
        // as having children (i.e. be expandable). This applies when a category lock is
        // active and the node lies on the path to the locked category. The locked category
        // itself should not be flagged as having children if it does not have unapproved
        // descendants. What this achieves is it enables the locked category to be added to
        // the unapproved plan tree so that the user can be tied to it.
        if (!pTVDispInfo->item.cChildren)
        {
            CUnapprovedPlanDoc *planDoc = GetDocument();

            if (planDoc->IsPlanTreeLocked() &&
                planDoc->GetLockedCategoryId() != treeNode->getNodeId() &&
                planDoc->IsNodeOnLockedCategoryPath(treeNode->getNodeId()))
            {
                pTVDispInfo->item.cChildren = 1;
            }
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CUnapprovedPlanTreeView::ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate &approvalStateUpdate)
{
    FUNCTION_ENTRY( "ProcessPlanApprovalStateUpdate" );

    try
    {
        CTreeCtrl &treeCtrl = GetTreeCtrl();
        HTREEITEM ancestorItem = NULL;

        if (approvalStateUpdate.approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE) // plan has been unapproved
        {
            // Add the plan to the tree. We need to find the nearest ancestor and update its child items
            // in order to open up a route to the newly unapproved plan. Note that this ancestor may not
            // necessarily be the parent, but a category further up in the tree.
            ancestorItem = FindClosestAncestor(approvalStateUpdate.plan);

            if (ancestorItem)
            {
                TreeNode *ancestorNode = reinterpret_cast<TreeNode *>(treeCtrl.GetItemData(ancestorItem));
                TA_ASSERT(ancestorNode, "Nearest ancestor node pointer is null");

                ancestorNode->addChildrenToTree(treeCtrl, ancestorItem, GetDocument());
            }
        }
        else // plan has been approved
        {
            // Remove the plan from the tree. First get rid of the corresponding tree item, if found.
            // Then walk back up the tree purging all childless ancestors.
            TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(approvalStateUpdate.plan);
            if (treeNode)
            {
                HTREEITEM treeNodeItem = FindNode(treeNode->getNodeId());
                if (treeNodeItem)
                {
                    ancestorItem = treeCtrl.GetParentItem(treeNodeItem);
                    treeCtrl.DeleteItem(treeNodeItem);
                }
            }

            if (!ancestorItem)
            {
                ancestorItem = FindClosestAncestor(approvalStateUpdate.plan);
            }

            while (ancestorItem && !treeCtrl.ItemHasChildren(ancestorItem))
            {
                // If a category lock is active, then make sure that the locked category does not get
                // removed from the tree (even though it may not have any unapproved descendants) so that
                // the lock can continue to be enforced.
                if (GetDocument()->IsPlanTreeLocked())
                {
                    TreeNode *ancestorNode = reinterpret_cast<TreeNode *>(treeCtrl.GetItemData(ancestorItem));
                    TA_ASSERT(ancestorNode, "Ancestor node pointer is null");

                    if (ancestorNode->getNodeId() == GetDocument()->GetLockedCategoryId())
                    {
                        break;
                    }
                }

                HTREEITEM ancestorParent = treeCtrl.GetParentItem(ancestorItem);
                treeCtrl.DeleteItem(ancestorItem);

                ancestorItem = ancestorParent;
            }
        }
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_PROCESS_APPROVAL_STATE_UPDATE, ex);
    }
    catch (...)
    {
        MessageBox::error(IDS_PROCESS_APPROVAL_STATE_UPDATE);
    }

    FUNCTION_EXIT;
}


void CUnapprovedPlanTreeView::ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessInsertNodeUpdate" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    HTREEITEM ancestorItem = FindClosestAncestor(nodeSummary.node);

    if (ancestorItem)
    {
        TreeNode *ancestorNode = reinterpret_cast<TreeNode *>(treeCtrl.GetItemData(ancestorItem));
        TA_ASSERT(ancestorNode, "Nearest ancestor node pointer is null");

        if (ancestorNode->getNodeId() == nodeSummary.parent)
        {
            CPlanTreeView::ProcessInsertNodeUpdate(nodeSummary, sessionId);
        }
        else
        {
            ancestorNode->addChildrenToTree(treeCtrl, ancestorItem, GetDocument());
        }
    }

    FUNCTION_EXIT;
}


void CUnapprovedPlanTreeView::ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeUpdate" );

    // Get rid of the deleted node
    CPlanTreeView::ProcessDeleteNodeUpdate(deleteNodeUpdate);

    // Get rid of all ancestors of the deleted node that do not have unapproved descendants.
    CTreeCtrl &treeCtrl = GetTreeCtrl();
    HTREEITEM ancestorItem = FindNodeOrClosestAncestor(deleteNodeUpdate.parent);

    while (ancestorItem && !treeCtrl.ItemHasChildren(ancestorItem))
    {
        // If a category lock is active, then make sure that the locked category does not get
        // removed from the tree (even though it may not have any unapproved descendants) so that
        // the lock can continue to be enforced.
        if (GetDocument()->IsPlanTreeLocked())
        {
            TreeNode *ancestorNode = reinterpret_cast<TreeNode *>(treeCtrl.GetItemData(ancestorItem));
            TA_ASSERT(ancestorNode, "Ancestor node pointer is null");

            if (ancestorNode->getNodeId() == GetDocument()->GetLockedCategoryId())
            {
                break;
            }
        }

        HTREEITEM ancestorParent = treeCtrl.GetParentItem(ancestorItem);
        treeCtrl.DeleteItem(ancestorItem);

        ancestorItem = ancestorParent;
    }

    FUNCTION_EXIT;
}


