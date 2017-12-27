//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanEditorThread.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "PlanEditorThread.h"
#include "PlanEditorDoc.h"
#include "PlanEditorFrame.h"
#include "PlanNodeView.h"
#include "TreeNodeFactory.h"
#include "FloatingDocTemplate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanEditorThread

IMPLEMENT_DYNAMIC(CPlanEditorThread, CFloatingWindowThread)

CPlanEditorThread::CPlanEditorThread(TreeNodeId  nodeId) : CFloatingWindowThread(nodeId)
{
    FUNCTION_ENTRY( "CPlanEditorThread" );
    FUNCTION_EXIT;
}


CPlanEditorThread::~CPlanEditorThread()
{
    FUNCTION_ENTRY( "~CPlanEditorThread" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanEditorThread, CFloatingWindowThread)
    // {{AFX_MSG_MAP(CPlanEditorThread)
    ON_THREAD_MESSAGE(WM_REFRESH_PLAN_GENERAL_DETAIL, OnRefreshPlanGeneralDetail)
    ON_THREAD_MESSAGE(WM_REFRESH_PLAN_STEP_LIST, OnRefreshPlanStepDetail)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanEditorThread message handlers

// LRESULT CPlanEditorThread::OnRefreshPlanGeneralDetail(WPARAM wParam, LPARAM lParam)
void CPlanEditorThread::OnRefreshPlanGeneralDetail(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnRefreshPlanGeneralDetail" );

    if (m_pMainWnd)
    {
        m_pMainWnd->SendMessageToDescendants(WM_REFRESH_PLAN_GENERAL_DETAIL, wParam, lParam);
    }

    FUNCTION_EXIT;
	// return 0;
}


// LRESULT CPlanEditorThread::OnRefreshPlanStepDetail(WPARAM wParam, LPARAM lParam)
void CPlanEditorThread::OnRefreshPlanStepDetail(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnRefreshPlanStepDetail" );

    if (m_pMainWnd)
    {
        m_pMainWnd->SendMessageToDescendants(WM_REFRESH_PLAN_STEP_LIST, wParam, lParam);
    }

    FUNCTION_EXIT;
	// return 0;
}


CDocTemplate *CPlanEditorThread::GetDocumentTemplate() const
{
    FUNCTION_ENTRY( "GetDocumentTemplate" );

    TreeNode* treeNode = TreeNodeFactory::getInstance().getTreeNode(GetNodeId());
    PlanNode* planNode = dynamic_cast<PlanNode*>(treeNode);
    TA_ASSERT(planNode != NULL, "Not a plan node");

    FUNCTION_EXIT;
    return new CFloatingDocTemplate(IDR_PLAN_EDITOR,
                                    RUNTIME_CLASS(CPlanEditorDoc),
                                    RUNTIME_CLASS(CPlanEditorFrame),
                                    RUNTIME_CLASS(CPlanNodeView),
                                    planNode);
}


