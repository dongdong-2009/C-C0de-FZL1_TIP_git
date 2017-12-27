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
// <description>

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/PlanTreeFrame.h"
#include "app/response_plans/plan_manager/src/PlanTreeView.h"
#include "app/response_plans/plan_manager/src/BlankView.h"
#include "app/response_plans/plan_manager/src/CategoryNodeView.h"
#include "app/response_plans/plan_manager/src/PlanNodeView.h"
#include "app/response_plans/plan_manager/src/TreeNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeFrame

IMPLEMENT_DYNCREATE(CPlanTreeFrame, CPlanBrowserFrame)

BEGIN_MESSAGE_MAP(CPlanTreeFrame, CPlanBrowserFrame)
    // {{AFX_MSG_MAP(CPlanTreeFrame)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_TREE_NODE_SELECTION_CHANGED, OnTreeNodeSelectionChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanTreeFrame construction/destruction
//        enum {COPY_PLAN,COPY_CATEGORY,CUT_PLAN,CUT_CATEGORY,NO_ACTION} ECopyNodeAction;

CPlanTreeFrame::CPlanTreeFrame()
{
    FUNCTION_ENTRY( "CPlanTreeFrame" );

    // TODO: add member initialization code here

    FUNCTION_EXIT;
}


CPlanTreeFrame::~CPlanTreeFrame()
{
    FUNCTION_ENTRY( "~CPlanTreeFrame" );
    FUNCTION_EXIT;
}


BOOL CPlanTreeFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    FUNCTION_ENTRY( "OnCreateClient" );

    // create splitter window
    if (m_wndSplitter.CreateStatic(this, 1, 2))
    {
        if (m_wndSplitter.CreateView(0, 0, GetLeftPaneRuntimeClass(), CSize(0, 0), pContext) &&
            m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CBlankView), CSize(0, 0), pContext))
        {
            const int minimumWidth = 0;
            const int defaultWidth = 300;

            m_wndSplitter.SetColumnInfo(0, defaultWidth, minimumWidth);
            m_wndSplitter.RecalcLayout();

            FUNCTION_EXIT;
            return TRUE;
        }

        m_wndSplitter.DestroyWindow();
    }

    FUNCTION_EXIT;
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeFrame diagnostics

#ifdef _DEBUG
void CPlanTreeFrame::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CMDIChildWnd::AssertValid();

    FUNCTION_EXIT;
}


void CPlanTreeFrame::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CMDIChildWnd::Dump(dc);

    FUNCTION_EXIT;
}


CPlanTreeDoc* CPlanTreeFrame::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    ASSERT(GetActiveDocument()->IsKindOf(RUNTIME_CLASS(CPlanTreeDoc)));

    FUNCTION_EXIT;
    return (CPlanTreeDoc*)GetActiveDocument();
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanTreeFrame message handlers

CRuntimeClass *CPlanTreeFrame::GetLeftPaneRuntimeClass()
{
    FUNCTION_ENTRY( "GetLeftPaneRuntimeClass" );
    FUNCTION_EXIT;
    return RUNTIME_CLASS(CPlanTreeView);
}


CRuntimeClass *CPlanTreeFrame::GetRightPaneRuntimeClass()
{
    FUNCTION_ENTRY( "GetRightPaneRuntimeClass" );

    TreeNode *treeNode = GetDocument()->GetCurrentNode();
    if (treeNode)
    {
        FUNCTION_EXIT;
        return treeNode->getNodeViewClass();
    }

    FUNCTION_EXIT;
    return RUNTIME_CLASS(CBlankView);
}


CPlanTreeView *CPlanTreeFrame::GetLeftPane()
{
    FUNCTION_ENTRY( "GetLeftPane" );
    FUNCTION_EXIT;
    return dynamic_cast<CPlanTreeView *>(m_wndSplitter.GetPane(0, 0));
}


CView *CPlanTreeFrame::GetRightPane()
{
    FUNCTION_ENTRY( "GetRightPane" );
    FUNCTION_EXIT;
    return dynamic_cast<CView *>(m_wndSplitter.GetPane(0, 1));
}


LRESULT CPlanTreeFrame::OnTreeNodeSelectionChanged(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnTreeNodeSelectionChanged" );

    // Select the class of node detail view corresponding to the node type.
    CRuntimeClass *rightPaneRuntimeClass = GetRightPaneRuntimeClass();

    // If necessary, update the right hand pane with the new view.
    if (!GetRightPane()->IsKindOf(rightPaneRuntimeClass))
    {
        m_wndSplitter.DeleteView(0, 1);
        m_wndSplitter.CreateView(0, 1, rightPaneRuntimeClass, CSize(0, 0), NULL);
        m_wndSplitter.RecalcLayout();

        CTreeView *treeView = GetLeftPane();
        if (CWnd::GetFocus() != (CWnd *)treeView)
        {
            treeView->SetFocus();
        }
    }

    // Tell the right hand pane view to update its contents.
    GetRightPane()->PostMessage(WM_RESET_TREE_NODE_VIEW);

    FUNCTION_EXIT;
    return 0;
}


