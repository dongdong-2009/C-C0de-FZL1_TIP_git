//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanControllerThread.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanControllerThread.h"
#include "PlanControllerDoc.h"
#include "PlanControllerFrame.h"
#include "PlanNodeView.h"
#include "TreeNodeFactory.h"
#include "PlanNode.h"
#include "FloatingDocTemplate.h"
#include "MessageBox.h"
#include "InteractiveStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerThread

IMPLEMENT_DYNAMIC(CPlanControllerThread, CFloatingWindowThread)

CPlanControllerThread::CPlanControllerThread(TreeNodeId nodeId) : CFloatingWindowThread(nodeId)
{
    FUNCTION_ENTRY( "CPlanControllerThread" );
    FUNCTION_EXIT;
}


CPlanControllerThread::~CPlanControllerThread()
{
    FUNCTION_ENTRY( "~CPlanControllerThread" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanControllerThread, CFloatingWindowThread)
    // {{AFX_MSG_MAP(CPlanControllerThread)
    ON_THREAD_MESSAGE(WM_INTERACTIVE_STEP_EXEC_REQUEST, OnInteractiveStepExecRequest)
    ON_THREAD_MESSAGE(WM_PLAN_STEP_FAILED, OnPlanStepFailed)
    ON_THREAD_MESSAGE(WM_SELECT_ACTIVE_PLAN_INSTANCE, OnSelectActivePlanInstance)
    ON_THREAD_MESSAGE(WM_CONTROLLED_PLAN_APPROVAL_STATE_CHANGED, OnControlledPlanApprovalStateChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanControllerThread message handlers

// LRESULT CPlanControllerThread::OnInteractiveStepExecRequest(WPARAM wParam, LPARAM /*lParam*/)
void CPlanControllerThread::OnInteractiveStepExecRequest(WPARAM wParam, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnInteractiveStepExecRequest" );

    InteractiveStep *interactiveStep = reinterpret_cast<InteractiveStep *>(wParam);
    TA_ASSERT(interactiveStep, "No interactive step to be executed");

    TA_Base_Core::ActivePlanId activePlanDetail = interactiveStep->getStepDetail().activePlan;

    OnSelectActivePlanInstance(activePlanDetail.plan, MAKELPARAM(activePlanDetail.instance, activePlanDetail.location));

    interactiveStep->launchAndSignal();

    FUNCTION_EXIT;
    // return 0;
}


// LRESULT CPlanControllerThread::OnPlanStepFailed(WPARAM wParam, LPARAM /*lParam*/)
void CPlanControllerThread::OnPlanStepFailed(WPARAM wParam, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnPlanStepFailed" );

    TA_Base_Bus::TransActiveMessage userMsg;
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_210127);

    FUNCTION_EXIT;
    // return 0;
}


// LRESULT CPlanControllerThread::OnSelectActivePlanInstance(WPARAM wParam, LPARAM lParam)
void CPlanControllerThread::OnSelectActivePlanInstance(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnSelectActivePlanInstance" );

    TreeNodeId planId = (TreeNodeId) wParam;

    if (planId == GetNodeId())
    {
        if (m_pMainWnd)
        {
            m_pMainWnd->SendMessageToDescendants(WM_SELECT_ACTIVE_PLAN_INSTANCE, wParam, lParam);
        }
    }

    FUNCTION_EXIT;
    // return 0;
}


// LRESULT CPlanControllerThread::OnControlledPlanApprovalStateChanged(WPARAM wParam, LPARAM lParam)
void CPlanControllerThread::OnControlledPlanApprovalStateChanged(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnControlledPlanApprovalStateChanged" );

    TreeNodeId planId = (TreeNodeId) wParam;

    if (planId == GetNodeId())
    {
        PlanNode *planNode = dynamic_cast<PlanNode *>(TreeNodeFactory::getInstance().getTreeNode(GetNodeId()));
        TA_ASSERT(planNode != NULL, "Plan node is null");

        if (!planNode->isApproved())
        {
            if (m_pMainWnd)
            {
                // Ensure that plan control functions are disabled
                m_pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI, 0, 0);
            }

            // Display a warning and self-destruct once the warning is acknowledged by the user.
            // The warning message is visible only when the Plan Controller is active, so if the user
            // is not interacting with the Plan Controller they will not see it until they bring
            // the Plan Controller to the foreground.
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << planNode->getNodeName();
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_210015);

            Terminate();
        }
    }

    FUNCTION_EXIT;
    // return 0;
}


CDocTemplate *CPlanControllerThread::GetDocumentTemplate() const
{
    FUNCTION_ENTRY( "GetDocumentTemplate" );

    PlanNode *planNode = dynamic_cast<PlanNode *>(TreeNodeFactory::getInstance().getTreeNode(GetNodeId()));
    TA_ASSERT(planNode != NULL, "Plan node is null");

    FUNCTION_EXIT;
    return new CFloatingDocTemplate(IDR_PLAN_CONTROLLER,
                                    RUNTIME_CLASS(CPlanControllerDoc),
                                    RUNTIME_CLASS(CPlanControllerFrame),
                                    RUNTIME_CLASS(CPlanNodeView),
                                    planNode);
}


