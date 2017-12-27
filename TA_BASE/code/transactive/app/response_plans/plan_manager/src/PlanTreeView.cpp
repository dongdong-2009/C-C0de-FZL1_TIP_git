//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanTreeView.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/08/03 17:02:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanTreeView.h"
#include "TreeNode.h"
#include "RootNode.h"
#include "PlanNode.h"
#include "TreeNodeFactory.h"
#include "ViewUpdate.h"
#include "PlanAgentAccess.h"
#include "MessageBox.h"
#include "Utilities.h"
#include "OperatorRights.h"
#include "FloatingWindowThreadRegister.h"
#include "PlanAgentAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeView

IMPLEMENT_DYNCREATE(CPlanTreeView, CTreeView)

BEGIN_MESSAGE_MAP(CPlanTreeView, CTreeView)
    // {{AFX_MSG_MAP(CPlanTreeView)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDoubleClicked)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
    ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnItemSelectionChanging)
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnItemSelected)
    ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginItemLabelEdit)
    ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndItemLabelEdit)
    ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDispInfo)
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_DESTROY()
    ON_COMMAND(ID_CATEGORY_RENAME, OnRenameCategory)
    ON_COMMAND_EX(ID_PLAN_EDIT, OnEditPlan)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_WINDOW_HAS_BECOME_VISIBLE, OnViewShown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanTreeView construction/destruction

CPlanTreeView::CPlanTreeView() : m_guardTreeAccess(false) // full tree access
{
    FUNCTION_ENTRY( "CPlanTreeView" );

    // TODO: add construction code here

    FUNCTION_EXIT;
}


CPlanTreeView::~CPlanTreeView()
{
    FUNCTION_ENTRY( "~CPlanTreeView" );
    FUNCTION_EXIT;
}


BOOL CPlanTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    // TODO: Modify the Window class or styles here by modifying
    // the CREATESTRUCT cs
    cs.style |= WS_TABSTOP | TVS_INFOTIP | TVS_HASLINES |
                TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS;

    FUNCTION_EXIT;
    return CTreeView::PreCreateWindow(cs);
}


BOOL CPlanTreeView::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY( "PreTranslateMessage" );

    // If a label edit control is active, enable it to process Del, Ctrl+X, Ctrl+C and Ctrl+V keys.
    // Normally these get routed to PlanTreeDoc where they are handled as plan or category delete,
    // and plan cut/copy/paste commands.
    if (pMsg->message == WM_KEYDOWN && GetTreeCtrl().GetEditControl() != NULL)
    {
        WPARAM key = pMsg->wParam;
        bool isCtrlPressed = (GetAsyncKeyState(VK_CONTROL) < 0);

        if ((key == VK_DELETE) || (isCtrlPressed && (key == 'C' || key == 'X' || key == 'V')))
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);

            FUNCTION_EXIT;
            return TRUE;
        }
    }

    FUNCTION_EXIT;
    return CTreeView::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeView drawing

void CPlanTreeView::OnDraw(CDC* pDC)
{
    FUNCTION_ENTRY( "OnDraw" );

    // TODO: add draw code for native data here

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeView diagnostics

#ifdef _DEBUG
void CPlanTreeView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CTreeView::AssertValid();

    FUNCTION_EXIT;
}


void CPlanTreeView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CTreeView::Dump(dc);

    FUNCTION_EXIT;
}


CPlanTreeDoc* CPlanTreeView::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlanTreeDoc)));

    FUNCTION_EXIT;
    return (CPlanTreeDoc*)m_pDocument;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanTreeView message handlers

void CPlanTreeView::OnInitialUpdate()
{
    FUNCTION_ENTRY( "OnInitialUpdate" );

    CTreeView::OnInitialUpdate();

    // TODO: You may populate your TreeView with items by directly accessing
    // its tree control through a call to GetTreeCtrl().

    // Initialise the images used for tree items.
    CImageList *nodeImages = new CImageList();
    nodeImages->Create(IDB_TREE_NODE_IMAGE_LIST_BMP, 16, 1, RGB(0, 0, 0xFF));

    GetTreeCtrl().SetImageList(nodeImages, TVSIL_NORMAL);

    FUNCTION_EXIT;
}


void CPlanTreeView::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    CImageList *nodeImages = GetTreeCtrl().GetImageList(TVSIL_NORMAL);
    GetTreeCtrl().SetImageList(NULL, TVSIL_NORMAL);

    delete nodeImages;

    CTreeView::OnDestroy();

    FUNCTION_EXIT;
}


void CPlanTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    FUNCTION_ENTRY( "OnUpdate" );

    if (lHint == ID_RPARAM_SESSIONID_UPDATE)
    {
        // Not interested
        FUNCTION_EXIT;
        return;
    }

    if (lHint == ID_RPARAM_EXECPLANID_UPDATE)
    {
        ProcessRparamExecPlanIdUpdate();

        FUNCTION_EXIT;
        return;
    }

    if (lHint == ID_RPARAM_VIEWPLANID_UPDATE)
    {
        ProcessRparamViewPlanIdUpdate();

        FUNCTION_EXIT;
        return;
    }


    if (lHint == ID_RPARAM_LOCKCATEGORY_UPDATE)
    {
        ProcessRparamLockCategoryUpdate();

        FUNCTION_EXIT;
        return;
    }


    if (lHint == ID_RPARAM_ASSOC_ALARM_UPDATE)
    {
        ProcessRparamAssocAlarmIdUpdate();

        FUNCTION_EXIT;
        return;
    }


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


LRESULT CPlanTreeView::OnViewShown(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnViewShown" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();

    if (treeCtrl.GetCount() == 0) // Tree is empty - must get root node data to display
    {
        try
        {
            // Pretend we have got an update of the lock category parameter.
            // This will have the effect of populating the root category and, if necessary,
            // locking the tree at a specific sub-category.
            ProcessRparamLockCategoryUpdate();
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_DISPLAY_TREE_CONTENTS, ex);
        }
    }
    else // User switched from another view - update toolbar and menu according to the type of selected node
    {
        AfxGetMainWnd()->SendMessage(WM_TREE_NODE_SELECTION_CHANGED);
    }

    FUNCTION_EXIT;
    return 0;
}


void CPlanTreeView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnGetDispInfo" );

    TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
    // TODO: Add your control notification handler code here
    if ((pTVDispInfo->item.mask & TVIF_TEXT) == TVIF_TEXT)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pTVDispInfo->item.lParam);
        TA_ASSERT(treeNode, "Tree node pointer is null");

        RootNode *rootNode = dynamic_cast<RootNode *>(treeNode);
        const char *nodeLabel = (rootNode? GetDocument()->GetRootNodeLabel().c_str() : treeNode->getNodeName().c_str());

        pTVDispInfo->item.pszText = const_cast<char *>(nodeLabel);
    }

    if ((pTVDispInfo->item.mask & TVIF_IMAGE) == TVIF_IMAGE ||
        (pTVDispInfo->item.mask & TVIF_SELECTEDIMAGE) == TVIF_SELECTEDIMAGE)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pTVDispInfo->item.lParam);
        TA_ASSERT(treeNode, "Tree node pointer is null");

        bool isExpanded = (pTVDispInfo->item.state & TVIS_EXPANDED) == TVIS_EXPANDED;
        pTVDispInfo->item.iImage = pTVDispInfo->item.iSelectedImage = treeNode->getNodeImageType(isExpanded);
    }

    if ((pTVDispInfo->item.mask & TVIF_CHILDREN) == TVIF_CHILDREN)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pTVDispInfo->item.lParam);
        TA_ASSERT(treeNode, "Tree node pointer is null");

        pTVDispInfo->item.cChildren = treeNode->getNodeSummary().hasChildren? 1 : 0;
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanTreeView::OnLButtonDown(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY( "OnLButtonDown" );

    // We handle left mouse click to avoid the target item being highlighted in
    // situations where it is not accessible as determined by OnItemSelectionChanging().
    CTreeCtrl &treeCtrl = GetTreeCtrl();

    if (treeCtrl.GetEditControl() == NULL) // don't allow other items to be selected when editing
    {
        UINT hitTestFlags;
        HTREEITEM item = treeCtrl.HitTest(point, &hitTestFlags);

        if (item && (hitTestFlags & TVHT_ONITEM) && item != treeCtrl.GetSelectedItem())
        {
            treeCtrl.SelectItem(item);

            FUNCTION_EXIT;
            return;
        }
    }

    CTreeView::OnLButtonDown(nFlags, point);

    FUNCTION_EXIT;
}


void CPlanTreeView::OnRButtonDown(UINT nFlags, CPoint point)
{
    FUNCTION_ENTRY( "OnRButtonDown" );

    // We handle right mouse click to avoid the target item being highlighted in
    // situations where it is not accessible as determined by OnItemSelectionChanging().
    CTreeCtrl &treeCtrl = GetTreeCtrl();

    if (treeCtrl.GetEditControl() == NULL) // don't allow other items to be selected when editing
    {
        UINT hitTestFlags;
        HTREEITEM item = treeCtrl.HitTest(point, &hitTestFlags);

        if (item && (hitTestFlags & TVHT_ONITEM) && item != treeCtrl.GetSelectedItem())
        {
            treeCtrl.SelectItem(item);

            FUNCTION_EXIT;
            return;
        }
    }

    CTreeView::OnRButtonDown(nFlags, point);

    FUNCTION_EXIT;
}


void CPlanTreeView::OnItemDoubleClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemDoubleClicked" );

    // Get the current position of the cursor.
    CPoint point;
    ::GetCursorPos(&point);
    ScreenToClient(&point);

    // Get the item that was double-clicked.
    UINT hitTestFlags;
    HTREEITEM item = GetTreeCtrl().HitTest(point, &hitTestFlags);

    // Make sure the double-clicked item is the currently selected item.
    // If category lock is enabled the user will be prevented from selecting certain nodes but
    // these nodes can still be double-clicked.
    if (!(item && (hitTestFlags & TVHT_ONITEM) && item == GetTreeCtrl().GetSelectedItem()))
    {
        *pResult = 1;

        FUNCTION_EXIT;
        return;
    }

    PlanNode* planNode = dynamic_cast<PlanNode *>(GetDocument()->GetCurrentNode());
    if (planNode)
    {
        if (planNode->isApproved())
        {
            // For approved plans launch the Plan Controller
            GetParentFrame()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_PLAN_CONTROL, 0), (LPARAM) NULL);
        }
        else
        {
            // For unapproved plans launch the Plan Editor
            GetParentFrame()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_PLAN_EDIT, 0), (LPARAM) NULL);
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanTreeView::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemExpanding" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    TreeNode *treeNode = reinterpret_cast<TreeNode *>(pNMTreeView->itemNew.lParam);
    TA_ASSERT(treeNode, "Tree node pointer is null");

    // Ensure that the node is expandable. Note that if the item has been selected then it's also expandable.
    if (m_guardTreeAccess &&
        pNMTreeView->itemNew.hItem != GetTreeCtrl().GetSelectedItem() &&
        !GetDocument()->IsNodeAccessible(treeNode->getNodeId()))
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY);
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_210018);

        *pResult = 1;

        FUNCTION_EXIT;
        return;
    }

    // Prevent the root item being collapsed
    if (pNMTreeView->action == TVE_COLLAPSE && pNMTreeView->itemNew.hItem == GetTreeCtrl().GetRootItem())
    {
        *pResult = 1;

        FUNCTION_EXIT;
        return;
    }

    // If item is being expanded for the first time
    if (pNMTreeView->action == TVE_EXPAND && (pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE) != TVIS_EXPANDEDONCE)
    {
        try
        {
            CWaitCursor waitCursor;

            // Insert the node's children into the tree under the expanding item
            treeNode->addChildrenToTree(GetTreeCtrl(), pNMTreeView->itemNew.hItem, GetDocument());
        }
        catch (const TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_EXPAND_CATEGORY, ex);

            *pResult = 1;

            FUNCTION_EXIT;
            return;
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanTreeView::OnItemSelectionChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemSelectionChanging" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Allow item deselection
    if (!pNMTreeView->itemNew.hItem)
    {
        *pResult = 0;

        FUNCTION_EXIT;
        return;
    }

    TreeNode *treeNode = reinterpret_cast<TreeNode *>(pNMTreeView->itemNew.lParam);
    TA_ASSERT(treeNode, "Tree node pointer is null");

    // Ensure that the node is selectable.
    if (m_guardTreeAccess && !GetDocument()->IsNodeAccessible(treeNode->getNodeId()))
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY);
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_210018);

        *pResult = 1;
    }
    else
    {
        *pResult = 0;
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::OnItemSelected(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemSelected" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    TreeNode *treeNode = reinterpret_cast<TreeNode *>(pNMTreeView->itemNew.lParam);

    // Store away the selected node
    GetDocument()->SetCurrentNode(treeNode);

    // Notify the main frame so that the toolbar can be updated for the selected node
    AfxGetMainWnd()->SendMessage(WM_TREE_NODE_SELECTION_CHANGED);

    // Notify the parent tree frame so that the right hand pane can be updated for the selected node
    GetParentFrame()->SendMessage(WM_TREE_NODE_SELECTION_CHANGED);

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanTreeView::OnBeginItemLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnBeginItemLabelEdit" );

    TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

    // TODO: Add your control notification handler code here
    TreeNode *treeNode = reinterpret_cast<TreeNode *>(pTVDispInfo->item.lParam);
    TA_ASSERT(treeNode, "Tree node pointer is null");

    // Root node cannot be renamed
    RootNode *rootNode = dynamic_cast<RootNode *>(treeNode);
    if (rootNode)
    {
        *pResult = 1;

        FUNCTION_EXIT;
        return;
    }

    CategoryNode *categoryNode = dynamic_cast<CategoryNode *>(treeNode);
    PlanNode *planNode = dynamic_cast<PlanNode *>(treeNode);

    // Categories can only be renamed given appropriate rights.
    // Plans can be renamed if not approved and given appropriate rights.
    if ((categoryNode && OperatorRights::getInstance().canEditCategories()) ||
        (planNode && !planNode->isApproved() && OperatorRights::getInstance().canEditPlans()))
    {
        try
        {
            // If attempting to rename a plan that is being edited, tell the operator to use the Plan Editor.
            if (planNode && FloatingWindowThreadRegister::getInstance().getPlanEditorThread(planNode->getNodeId()) != NULL)
            {
                TA_Base_Bus::TransActiveMessage().showMsgBox(IDS_UW_210022);
            }
            else
            {
                PlanAgentAccess::getInstance().getPlanEditingLock(treeNode->getNodeId());

                *pResult = 0;

                FUNCTION_EXIT;
                return;
            }
        }
        catch (TA_Base_Core::TransactiveException &ex)
        {
            MessageBox::error(IDS_EDIT, ex);
        }
    }

    *pResult = 1;

    FUNCTION_EXIT;
}


void CPlanTreeView::OnEndItemLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnEndItemLabelEdit" );

    TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
    // TODO: Add your control notification handler code here

    // pResult is 1, update to the new text, if error occur then set to 0 to revert it
    *pResult = 1;

    TreeNode *treeNode = reinterpret_cast<TreeNode *>(pTVDispInfo->item.lParam);
    TA_ASSERT(treeNode, "Tree node pointer is null");

    if (pTVDispInfo->item.mask == TVIF_TEXT)
    {
        try
        {
            treeNode->renameNode(pTVDispInfo->item.pszText);
        }
        catch (TA_Base_Core::TransactiveException &ex)
        {
            *pResult = 0;
            MessageBox::error(IDS_RENAME, ex);
        }
    }

    try
    {
        // Should release edit lock whenever finish editing
        PlanAgentAccess::getInstance().releasePlanEditingLock(treeNode->getNodeId());
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unkonwn exception when releasePlanEditingLock" );
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::OnRenameCategory()
{
    FUNCTION_ENTRY( "OnRenameCategory" );

    // TODO: Add your command handler code here
    GetTreeCtrl().SetFocus();

    HTREEITEM item = GetTreeCtrl().GetSelectedItem();
    TA_ASSERT(item, "Tree item is null");

    GetTreeCtrl().EditLabel(item);

    FUNCTION_EXIT;
}


BOOL CPlanTreeView::OnEditPlan(UINT cmdId)
{
    FUNCTION_ENTRY( "OnEditPlan" );

    // It could happen that the Edit Plan command is invoked while the plan's
    // label is being edited inside the plan tree. Before the editor is invoked,
    // the plan name change should be completed and the existing edit lock released.
    // To ensure that this is done, we set the focus back to the tree view, which
    // results in a TVN_ENDLABELEDIT notification.
    if (cmdId == ID_PLAN_EDIT && GetTreeCtrl().GetEditControl() != NULL)
    {
        SetFocus();
    }

    // It's now safe to let the document handle the command.
    FUNCTION_EXIT;
    return FALSE;
}


HTREEITEM CPlanTreeView::FindNode(const std::string& nodePath, bool expandTree)
{
    FUNCTION_ENTRY( "FindNode" );

    // The specified path must be fully qualified. It should follow the format:
    // "[/<category1>/<categoryN>]/<plan_name>".

    try
    {
        // Walk down the plan path starting from the root.
        HTREEITEM currentItem = GetTreeCtrl().GetRootItem();
        if (!currentItem)
        {
            FUNCTION_EXIT;
            return NULL;
        }

        // Remove the leading slash from the node path.
        const std::string PATH_SEPARATOR("/");
        std::string remainingNodePath(nodePath.substr(1));

        // Iterate over the nodes on the path.
        while (!remainingNodePath.empty())
        {
            std::string nextNodeName(remainingNodePath.substr(0, remainingNodePath.find(PATH_SEPARATOR)));
            remainingNodePath.erase(0, nextNodeName.length() + 1);

            // Iterate over the child items. Need to find one that matches the name of the next node
            // in the path.
            HTREEITEM childItem = GetTreeCtrl().GetChildItem(currentItem);
            while (childItem)
            {
                // See if we have a node ID match.
                TreeNode *childNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(childItem));
                TA_ASSERT(childNode, "Child node pointer is null");

                if (childNode->getNodeName() == nextNodeName)
                {
                    // See if we are at the end of the path.
                    if (remainingNodePath.empty())
                    {
                        FUNCTION_EXIT;
                        return childItem;
                    }
                    else
                    {
                        // Must be an intermediate category. We'll continue down this branch.
                        currentItem = childItem;

                        // Expand the category if requested. This will cause the category to be populated
                        // with its child nodes if necessary.
                        if (expandTree)
                        {
                            GetTreeCtrl().Expand(childItem, TVE_EXPAND);
                        }

                        break;
                    }
                }

                // Let's look at the next child item
                childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
            }

            // If we have run out of options...
            if (!childItem)
            {
                break;
            }
        }
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_FIND_TREE_NODE, nodePath.c_str(), ex);
    }

    FUNCTION_EXIT;
    return NULL;
}


HTREEITEM CPlanTreeView::FindNode(TreeNodeId nodeId, bool expandTree)
{
    FUNCTION_ENTRY( "FindNode" );

    try
    {
        // Get a list of node IDs in the path leading to the specified node.
        TreeNodeIdList nodeIds;
        TreeNodeFactory::getInstance().getNodeIdsInNodePath(nodeId, nodeIds);

        // Walk down the plan path starting from the root.
        HTREEITEM currentItem = GetTreeCtrl().GetRootItem();
        if (!currentItem)
        {
            FUNCTION_EXIT;
            return NULL;
        }

        // Iterate over the nodes on the path, skipping the root node.
        nodeIds.erase(nodeIds.begin());
        for (TreeNodeIdList::iterator iter = nodeIds.begin(); iter != nodeIds.end(); iter++)
        {
            TreeNodeId nextNodeId = *iter;

            // Iterate over the child items. Need to find one that matches the ID of the next node
            // in the path.
            HTREEITEM childItem = GetTreeCtrl().GetChildItem(currentItem);
            while (childItem)
            {
                // See if we have a node ID match.
                TreeNode *childNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(childItem));
                TA_ASSERT(childNode, "Child node pointer is null");

                if (childNode->getNodeId() == nextNodeId)
                {
                    // See if we are at the end of the path.
                    if (childNode->getNodeId() == nodeId)
                    {
                        FUNCTION_EXIT;
                        return childItem;
                    }
                    else
                    {
                        // Must be an intermediate category. We'll continue down this branch.
                        currentItem = childItem;

                        // Expand the category if requested. This will cause the category to be populated
                        // with its child nodes if necessary.
                        if (expandTree)
                        {
                            GetTreeCtrl().Expand(childItem, TVE_EXPAND);
                        }

                        break;
                    }
                }

                // Let's look at the next child item
                childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
            }

            // If we have run out of options...
            if (!childItem)
            {
                break;
            }
        }
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_FIND_TREE_NODE, TA_Base_Bus::StringUtilities::convertToString(nodeId), ex);
    }

    FUNCTION_EXIT;
    return NULL;
}


HTREEITEM CPlanTreeView::FindClosestAncestor(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "FindClosestAncestor" );

    try
    {
        // Get a list of node IDs in the path leading to the specified node.
        TreeNodeIdList nodeIds;
        TreeNodeFactory::getInstance().getNodeIdsInNodePath(nodeId, nodeIds);

        // Walk down the plan path starting from the root.
        HTREEITEM currentItem = GetTreeCtrl().GetRootItem();
        if (!currentItem)
        {
            FUNCTION_EXIT;
            return NULL;
        }

        // Iterate over the nodes on the path, skipping the root node and the last node
        nodeIds.erase(nodeIds.begin());
        for (TreeNodeIdList::iterator iter = nodeIds.begin(); iter != nodeIds.end(); iter++)
        {
            TreeNodeId nextNodeId = *iter;

            // Iterate over the child items. Need to find one that matches the ID of the next node
            // in the path.
            HTREEITEM childItem = GetTreeCtrl().GetChildItem(currentItem);
            while (childItem)
            {
                // See if we have a node ID match.
                TreeNode *childNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(childItem));
                TA_ASSERT(childNode, "Child node pointer is null");

                if (childNode->getNodeId() == nextNodeId)
                {
                    // Got a match. We'll try to descend down this path.
                    currentItem = childItem;
                    break;
                }

                // Let's look at the next child item
                childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
            }

            // If we have run out of options...
            if (!childItem)
            {
                break;
            }
        }

        FUNCTION_EXIT;
        return currentItem;
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_FIND_TREE_NODE, TA_Base_Bus::StringUtilities::convertToString(nodeId), ex);
    }

    FUNCTION_EXIT;
    return NULL;
}


HTREEITEM CPlanTreeView::FindNodeOrClosestAncestor(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "FindNodeOrClosestAncestor" );

    // Get the closest ancestor
    HTREEITEM ancestorItem = FindClosestAncestor(nodeId);
    if (ancestorItem)
    {
        // Search the ancestor's children for the specified node
        HTREEITEM childItem = GetTreeCtrl().GetChildItem(ancestorItem);
        while (childItem)
        {
            // See if we have a node ID match.
            TreeNode *childNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(childItem));
            TA_ASSERT(childNode, "Child node pointer is null");

            if (childNode->getNodeId() == nodeId)
            {
                // Got him.
                FUNCTION_EXIT;
                return childItem;
            }

            // Let's look at the next child item
            childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
        }

        // Managed to only find an ancestor.
        FUNCTION_EXIT;
        return ancestorItem;
    }

    FUNCTION_EXIT;
    return NULL;
}


void CPlanTreeView::DeleteNode(TreeNodeId nodeId, bool purgeEmptyAncestors)
{
    FUNCTION_ENTRY( "DeleteNode" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    HTREEITEM nodeItem = FindNode(nodeId);

    if (nodeItem)
    {
        bool isSelected = (treeCtrl.GetSelectedItem() == nodeItem);

        HTREEITEM parentItem = treeCtrl.GetParentItem(nodeItem);
        treeCtrl.DeleteItem(nodeItem);

        if (purgeEmptyAncestors)
        {
            while (parentItem && parentItem != treeCtrl.GetRootItem() && !treeCtrl.ItemHasChildren(parentItem))
            {
                nodeItem = parentItem;
                parentItem = treeCtrl.GetParentItem(nodeItem);
                treeCtrl.DeleteItem(nodeItem);
            }
        }

        if (isSelected)
        {
            treeCtrl.SelectItem(parentItem? parentItem : treeCtrl.GetRootItem());
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::AddNode(TreeNodeId nodeId, HTREEITEM parentItem)
{
    FUNCTION_ENTRY( "AddNode" );

    TA_ASSERT(parentItem, "Parent item is null");

    TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeId);

    if (treeNode)
    {
        treeNode->addToTree(GetTreeCtrl(), parentItem, GetDocument());
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::SortChildren(HTREEITEM parentItem)
{
    FUNCTION_ENTRY( "SortChildren" );

    TA_ASSERT(parentItem, "Parent item is null");

    TVSORTCB tvSort;
    tvSort.hParent = parentItem;
    tvSort.lParam = 0;
    tvSort.lpfnCompare = TreeNode::compareSiblingNodes;

    GetTreeCtrl().SortChildrenCB(&tvSort);

    FUNCTION_EXIT;
}


void CPlanTreeView::CollapseBranch(HTREEITEM item)
{
    FUNCTION_ENTRY( "CollapseBranch" );

    if (item)
    {
        // Recursively collapse the item's children
        HTREEITEM childItem = GetTreeCtrl().GetChildItem(item);
        while (childItem)
        {
            CollapseBranch(childItem);

            childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
        }

        // Collapse the item itself
        GetTreeCtrl().Expand(item, TVE_COLLAPSE);
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessRparamExecPlanIdUpdate()
{
    FUNCTION_ENTRY( "ProcessRparamExecPlanIdUpdate" );

    // Handled by derived classes that care

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessRparamViewPlanIdUpdate()
{
    FUNCTION_ENTRY( "ProcessRparamViewPlanIdUpdate" );

    // Handled by derived classes that care

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessRparamAssocAlarmIdUpdate()
{
    FUNCTION_ENTRY( "ProcessRparamAssocAlarmIdUpdate" );

    // Handled by derived classes that care

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessRparamLockCategoryUpdate()
{
    FUNCTION_ENTRY( "ProcessRparamLockCategoryUpdate" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    CWaitCursor waitCursor;

    // Disable the tree access guard while populating and searching the tree for the locked category.
    // The search may involve expanding the tree, which is normally protected by the guard.
    m_guardTreeAccess = false;

    // Create the root of the tree.
    RootNode *rootNode = TreeNodeFactory::getInstance().createRootNode();
    treeCtrl.DeleteAllItems();
    rootNode->addToTree(treeCtrl);
    treeCtrl.SelectItem(treeCtrl.GetRootItem());
    treeCtrl.Expand(treeCtrl.GetRootItem(), TVE_EXPAND);

    // Check if plan tree needs to be locked
    if (GetDocument()->IsPlanTreeLocked())
    {
        // Fing the locked category, expanding the tree as necessary.
        TreeNodeId lockCategoryId = GetDocument()->GetLockedCategoryId();
        HTREEITEM item = FindNode(lockCategoryId, true);

        // If the locked category is not found, then that means the tree has not been populated yet.
        if (item)
        {
            treeCtrl.SelectItem(item);
            treeCtrl.Expand(item, TVE_EXPAND);
            treeCtrl.EnsureVisible(item);

            // Now the tree access guard can be activated.
            m_guardTreeAccess = true;
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate & /*approvalStateUpdate*/)
{
    FUNCTION_ENTRY( "ProcessPlanApprovalStateUpdate" );

    // Handled by derived classes that care

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessModifyNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessModifyNodeUpdate" );

    HTREEITEM item = FindNode(nodeSummary.node);

    if (item)
    {
         HTREEITEM parentItem = GetTreeCtrl().GetParentItem(item);
         TA_ASSERT(parentItem, "Parent item is null");

         // The only change to node configuration that's visible in the plan tree is a name change.
         // This could potentially affect the order in which categories/plans are arranged, so items
         // must be re-sorted.
         GetTreeCtrl().SetItemText(item, nodeSummary.name.in());
         SortChildren(parentItem);

         // If the node lies on the path to the locked category, it must be a category itself. The only
         // change that could have occurred is a change of name. If that is the case inform the user that
         // they are now going to be locked in a different category.
         if (GetDocument()->IsPlanTreeLocked() && GetDocument()->IsNodeOnLockedCategoryPath(nodeSummary.node))
         {
             try
             {
                 std::string lockCategoryPath = PlanAgentAccess::getInstance().getNodePath(GetDocument()->GetLockedCategoryId());

                 if (lockCategoryPath != TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY))
                 {
                     TA_Base_Bus::TransActiveMessage userMsg;
                     userMsg << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY) << lockCategoryPath;
                     UINT selectedButton = userMsg.showMsgBox(IDS_UI_210058);

                     TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCKCATEGORY, lockCategoryPath.c_str());
                 }
             }
             catch (TA_Base_Core::TransactiveException&)
             {
                 TA_Base_Bus::TransActiveMessage userMsg;
                 UINT selectedButton = userMsg.showMsgBox(IDS_UE_210078);
                 TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCKCATEGORY, "");  // reset lock category
             }
         }

         // as per TD 14880
         // only after renaming is done should we ask the user if they want to generate the report
//         if (TA_Base_Core::PLAN_NODE == nodeSummary.type ||
//             (TA_Base_Core::CATEGORY_NODE == nodeSummary.type && nodeSummary.hasChildren))
//         {
//             std::string newName = nodeSummary.name;
//             TA_Base_Core::NodeId planId = nodeSummary.node;
//             TA_Base_Bus::TransActiveMessage userMsg;
//             std::string description = "being renamed";
//             userMsg << newName;
//             userMsg << description;
//             if (userMsg.showMsgBox(IDS_UW_210031) == IDYES)
//             {
//                //TD15753++
//                std::string strPlanPath ="";
//                try
//                {
//                    strPlanPath = PlanAgentAccess::getInstance().getNodePath (planId);
//                    PlanAgentAccess::getInstance().generateReport (strPlanPath);
//                }
//                catch (...)
//                {
//
//                }
//                //TD15753++
//
//             }
//         }
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessInsertNodeUpdate" );

    HTREEITEM parentItem = FindNode(nodeSummary.parent);

    if (parentItem)
    {
        AddNode(nodeSummary.node, parentItem);

        if (sessionId == Utilities::getCurrentSessionId())
        {
            GetTreeCtrl().EnsureVisible(FindNode(nodeSummary.node));
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessInsertNodeIntoRootUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessInsertNodeIntoRootUpdate" );

    HTREEITEM parentItem = GetTreeCtrl().GetRootItem();

    if (parentItem)
    {
        AddNode(nodeSummary.node, parentItem);

        if (sessionId == Utilities::getCurrentSessionId())
        {
            GetTreeCtrl().EnsureVisible(FindNode(nodeSummary.node));
        }
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessMoveNodeUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId)
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


void CPlanTreeView::ProcessMoveNodeToRootUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId)
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


void CPlanTreeView::ProcessMoveNodeFromRootUpdate(const TA_Base_Core::TransferNodeUpdate &transferNodeUpdate, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessMoveNodeFromRootUpdate" );

    // A node move is essentially a delete followed by an insert.
    ProcessDeleteNodeFromRootUpdate(transferNodeUpdate.summary.node);
    ProcessInsertNodeUpdate(transferNodeUpdate.summary, sessionId);

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeUpdate" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    HTREEITEM parentItem = FindNode(deleteNodeUpdate.parent);

    if (parentItem)
    {
         for (CORBA::ULong i = 0; i < deleteNodeUpdate.nodes.length(); i++)
         {
             HTREEITEM childItem = treeCtrl.GetChildItem(parentItem);
             while (childItem)
             {
                 // See if we have a node ID match.
                 TreeNode *childNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(childItem));
                 TA_ASSERT(childNode, "Child node pointer is null");

                 if (childNode->getNodeId() == deleteNodeUpdate.nodes[i])
                 {
                     // If the node being removed is the locked category, then the lock can no longer apply.
                     if (GetDocument()->IsPlanTreeLocked() && GetDocument()->GetLockedCategoryId() == childNode->getNodeId())
                     {
                         TA_Base_Bus::TransActiveMessage userMsg;
                         userMsg << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY);
                         UINT selectedButton = userMsg.showMsgBox(IDS_UW_210019);

                         TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCKCATEGORY, "");

                         FUNCTION_EXIT;
                         return;
                     }

                     treeCtrl.DeleteItem(childItem);

                     // as per TD 14947
                     // only after deleting is done should we ask the user if they want to generate the report
//                     std::string newName = childNode->getNodeName ();
//                     TA_Base_Core::NodeId planId = childNode->getNodeId ();
//                     TA_Base_Bus::TransActiveMessage userMsg;
//                     std::string description = "being deleted";
//                     userMsg << newName;
//                     userMsg << description;
//                     if (userMsg.showMsgBox(IDS_UW_210031) == IDYES)
//                     {
//                        //TD15753++
//                        std::string strPlanPath ="";
//                        try
//                        {
//                            strPlanPath = PlanAgentAccess::getInstance().getNodePath (planId);
//                        }
//                        catch (...)
//                        {
//                            strPlanPath = childNode->getNodeName();
//                        }
//                        //++TD15753
//
//                        PlanAgentAccess::getInstance().generateReport (strPlanPath);
//                     }

                     break;
                 }

                 // Let's look at the next child item
                 childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
             }
         }

    }

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessDeleteNodeFromRootUpdate(const TA_Base_Core::NodeIds &nodeIds)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeFromRootUpdate" );

    for (CORBA::ULong i = 0; i < nodeIds.length(); i++)
    {
        ProcessDeleteNodeFromRootUpdate(nodeIds[i]);
    }

    FUNCTION_EXIT;
}


void CPlanTreeView::ProcessDeleteNodeFromRootUpdate(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeFromRootUpdate" );

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    HTREEITEM parentItem = treeCtrl.GetRootItem();

    if (parentItem)
    {
        HTREEITEM childItem = treeCtrl.GetChildItem(parentItem);
        while (childItem)
        {
            // See if we have a node ID match.
            TreeNode *childNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(childItem));
            TA_ASSERT(childNode, "Child node pointer is null");

            if (childNode->getNodeId() == nodeId)
            {
                    // If the node being removed is the locked category, then the lock can no longer apply.
                    if (GetDocument()->IsPlanTreeLocked() && GetDocument()->GetLockedCategoryId() == childNode->getNodeId())
                    {
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY);
                        UINT selectedButton = userMsg.showMsgBox(IDS_UW_210019);

                        TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCKCATEGORY, "");

                        FUNCTION_EXIT;
                        return;
                    }

                    treeCtrl.DeleteItem(childItem);

                    // as per TD 14947
                    // only after deleting is done should we ask the user if they want to generate the report
//                    std::string newName = childNode->getNodeName ();
//                    TA_Base_Core::NodeId planId = childNode->getNodeId ();
//                    TA_Base_Bus::TransActiveMessage userMsg;
//                    std::string description = "being deleted";
//                    userMsg << newName;
//                    userMsg << description;
//                    if (userMsg.showMsgBox(IDS_UW_210031) == IDYES)
//                    {
//                        //TD15753++
//                        std::string strPlanPath ="";
//                        try
//                        {
//                            strPlanPath = PlanAgentAccess::getInstance().getNodePath (planId);
//                        }
//                        catch (...)
//                        {
//                            strPlanPath = childNode->getNodeName();
//                        }
//                        //TD15753++
//
//                        PlanAgentAccess::getInstance().generateReport (strPlanPath);
//                    }

                    break;
            }

            // Let's look at the next child item
            childItem = GetTreeCtrl().GetNextSiblingItem(childItem);
        }
    }

    FUNCTION_EXIT;
}


