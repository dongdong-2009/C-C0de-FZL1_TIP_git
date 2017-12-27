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

#include "app/response_plans/plan_manager/src/stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/InteractiveStepDlg.h"
#include "app/response_plans/plan_manager/src/PlanAgentAccess.h"
#include "app/response_plans/plan_manager/src/MessageBox.h"
#include "app/response_plans/plan_manager/src/PlanAgentUpdateProcessor.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CALLBACK RepositionCallback(HWND hWnd, LPARAM lParam)
{
    CWnd *enumeratedWnd = CWnd::FromHandlePermanent(hWnd);
    CWnd *wndToPosition = (CWnd *) lParam;

    // Skip all temporary windows
    if (enumeratedWnd == NULL)
    {
        return TRUE;
    }

    // Skip the enumerated window if it is the one being positioned.
    if (wndToPosition == enumeratedWnd)
    {
        return TRUE;
    }

    // Skip the enumerated window if it is not topmost
    if ((::GetWindowLong(enumeratedWnd->m_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != WS_EX_TOPMOST)
    {
        return TRUE;
    }

    // Determine the respective locations of the enumerated window and the one being positioned.
    CRect enumeratedWndRect, wndToPositionRect;
    enumeratedWnd->GetWindowRect(enumeratedWndRect);
    wndToPosition->GetWindowRect(wndToPositionRect);

    // Skip the enumerated window if it does not share its top left corner with the window being
    // positioned.
    if (wndToPositionRect.TopLeft() != enumeratedWndRect.TopLeft())
    {
        return TRUE;
    }

    // There is a window origin overlap. Reposition the window slightly to make sure that it does
    // not completely obscure the other one. If the new position takes the window out of the
    // bounds of the desktop, reposition the window again, this time anchoring it in desktop's
    // top-left corner.
    wndToPositionRect.OffsetRect(20, 20);

    CRect desktopWndRect;
    CWnd::GetDesktopWindow()->GetWindowRect(desktopWndRect);

    if (!desktopWndRect.PtInRect(wndToPositionRect.BottomRight()))
    {
        wndToPositionRect.OffsetRect(-wndToPositionRect.left, -wndToPositionRect.top);
    }

    wndToPosition->MoveWindow(wndToPositionRect);

    // Moving the window may have resulted in another window being covered. Indicate the move
    // by returning false so that the windows are re-enumerated and, if needed, moved again.
    return FALSE;
}

static const UINT INTERACTIVE_STEP_TIMEOUT         = 100;
static const UINT INTERACTIVE_STEP_TIMER_COUNTDOWN = 101;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CInteractiveStepDlg dialog


CInteractiveStepDlg::CInteractiveStepDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail, TA_Base_Core::Seconds timeout) :
    CDialog((UINT)0, NULL),
    m_stepDetail(stepDetail),
    m_completionStatus(TA_Base_Core::PSCS_UNDEFINED),
    m_timeout(timeout),
    m_secondsToTimeout(0),
    m_stepExecutionInterrupted(false)
{
    FUNCTION_ENTRY( "CInteractiveStepDlg" );

    PlanAgentUpdateProcessor::getInstance().subscribeForActivePlanUpdates(this);

    FUNCTION_EXIT;
}


CInteractiveStepDlg::~CInteractiveStepDlg()
{
    FUNCTION_ENTRY( "~CInteractiveStepDlg" );

    PlanAgentUpdateProcessor::getInstance().unsubscribeFromActivePlanUpdates(this);

    FUNCTION_EXIT;
}


// IActivePlanObserver interface
void CInteractiveStepDlg::activeStateChanged(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "activeStateChanged" );

    // If the plan instance associated with this interactive step has been stopped we must close the
    // dialog immediately.
    if (activePlanDetail.activePlanState == TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE &&
        activePlanDetail.plan.plan == m_stepDetail.activePlan.plan &&
        activePlanDetail.plan.instance == m_stepDetail.activePlan.instance)
    {
        SendMessage(WM_INTERACTIVE_STEP_EXECUTION_INTERRUPTED, 0, 0);
    }

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CInteractiveStepDlg)
    DDX_Text(pDX, IDC_ACTIVE_INSTANCE_EDIT, m_instance);
    DDX_Text(pDX, IDC_PLAN_PATH_EDIT, m_plan);
    DDX_Text(pDX, IDC_LOCATION_NAME_EDIT, m_location);
    DDX_Text(pDX, IDC_STEP_NUMBER_EDIT, m_step);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CInteractiveStepDlg, CDialog)
    // {{AFX_MSG_MAP(CInteractiveStepDlg)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_INTERACTIVE_STEP_EXECUTION_INTERRUPTED, OnStepExecutionInterrupted)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInteractiveStepDlg message handlers

BOOL CInteractiveStepDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // TODO: Add extra initialization here

    try
    {
        m_plan = PlanAgentAccess::getInstance().getNodePath(m_stepDetail.activePlan.plan).c_str();
        m_location = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName(m_stepDetail.activePlan.location).c_str();
    }
    catch (...)
    {
        m_plan = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.activePlan.plan);
        m_location = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.activePlan.location);
    }

    m_step = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.step);
    m_instance = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.activePlan.instance);

    // Position the dialog relative to the parent window and other interactive
    // step dialogs that the parent window may have active. Start by placing the
    // dialog in the centre of the parent window and then continue moving it
    // as necessary until an unoccupied position is found.
    CenterWindow();
    while (!EnumWindows((WNDENUMPROC) RepositionCallback, (LPARAM) this));

    // Ensure the window stays on top of other windows and is not obscured so that the
    // operator can respond to the step asap.
    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    UpdateData(FALSE);

    // Start user response timer
    if (m_stepDetail.data._d() == TA_Base_Core::FAILURE_SUMMARY_STEP)
    {
        // time out does not apply for the failure summary step
        // therefore the INTERACTIVE_STEP_TIMEOUT timer should not be
        // started
        SetWindowText(_T(getTitlePrefix()));
    }
    else
    {
        startTimeoutCountdown();
    }

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CInteractiveStepDlg::OnTimer(UINT nIDEvent)
{
    FUNCTION_ENTRY( "OnTimer" );

    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == INTERACTIVE_STEP_TIMER_COUNTDOWN)
    {
        updateTimeoutCountdownDisplay();
    }
    else if (nIDEvent == INTERACTIVE_STEP_TIMEOUT)
    {
        setCompletionStatus(IDABORT);
        DestroyWindow();
    }
    else
    {
        CDialog::OnTimer(nIDEvent);
    }

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    KillTimer(INTERACTIVE_STEP_TIMEOUT);
    KillTimer(INTERACTIVE_STEP_TIMER_COUNTDOWN);

    // If the window is being destroyed due to the plan instance having been stopped,
    // then we should not attempt to report the step's completion status.
    if (!m_stepExecutionInterrupted)
    {
        reportCompletionStatus();
    }

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::PostNcDestroy()
{
    FUNCTION_ENTRY( "PostNcDestroy" );

    CDialog::PostNcDestroy();

    // Don't need this window anymore
    delete this;

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    setCompletionStatus(IDOK);
    DestroyWindow();

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );

    // Trap cancellations such as those resulting from the Esc key being pressed

    FUNCTION_EXIT;
}


LRESULT CInteractiveStepDlg::OnStepExecutionInterrupted(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnStepExecutionInterrupted" );

    m_stepExecutionInterrupted = true;
    DestroyWindow();

    FUNCTION_EXIT;
    return 0;
}


void CInteractiveStepDlg::startTimeoutCountdown()
{
    FUNCTION_ENTRY( "startTimeoutCountdown" );

    SetTimer(INTERACTIVE_STEP_TIMEOUT, m_timeout * 1000, NULL);
    SetTimer(INTERACTIVE_STEP_TIMER_COUNTDOWN, 1000, NULL);

    m_secondsToTimeout = m_timeout;

    updateTimeoutCountdownDisplay();

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::updateTimeoutCountdownDisplay()
{
    FUNCTION_ENTRY( "updateTimeoutCountdownDisplay" );

    CString title;
	title.Format("%s [%lu√Î∫Û≥¨ ±]", getTitlePrefix(), m_secondsToTimeout);

    SetWindowText(_T(title));

    m_secondsToTimeout--;

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::setCompletionStatus(int exitCode)
{
    FUNCTION_ENTRY( "setCompletionStatus" );

    switch (exitCode)
    {
        case IDOK:
            m_completionStatus = TA_Base_Core::PSCS_ACKNOWLEDGEMENT;
            break;

        case IDYES:
            m_completionStatus = TA_Base_Core::PSCS_YES_RESPONSE;
            break;

        case IDNO:
            m_completionStatus = TA_Base_Core::PSCS_NO_RESPONSE;
            break;

        case IDABORT:
            m_completionStatus = TA_Base_Core::PSCS_TIMEOUT;
            break;
        // lizettejl++ (DP-changes)
        case IDRETRY:
            m_completionStatus = TA_Base_Core::PSCS_PAUSE_RESPONSE;
            break;
        // ++lizettejl
        default:
            m_completionStatus = TA_Base_Core::PSCS_UNDEFINED;
    }

    FUNCTION_EXIT;
}


TA_Base_Core::EPlanStepCompletionStatus CInteractiveStepDlg::getCompletionStatus() const
{
    FUNCTION_ENTRY( "getCompletionStatus" );
    FUNCTION_EXIT;
    return m_completionStatus;
}


TA_Base_Core::CurrentPlanStepDetail CInteractiveStepDlg::getStepDetail() const
{
    FUNCTION_ENTRY( "getStepDetail" );
    FUNCTION_EXIT;
    return m_stepDetail;
}


