/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// FailureSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "planmanager.h"
#include "FailureSummaryDlg.h"
 #include "LabelMaker.h"
#include "MessageBox.h"
#include "PlanAgentAccess.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AppLauncher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFailureSummaryDlg dialog

using namespace TA_Base_App;

CFailureSummaryDlg::CFailureSummaryDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail)
    : CInteractiveStepDlg(stepDetail, 0)
{
    FUNCTION_ENTRY( "CFailureSummaryDlg" );

    // {{AFX_DATA_INIT(CFailureSummaryDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CFailureSummaryDlg::~CFailureSummaryDlg()
{
    FUNCTION_ENTRY( "~CFailureSummaryDlg" );
    FUNCTION_EXIT;
}


void CFailureSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CFailureSummaryDlg)
    DDX_Control(pDX, IDC_FAILED_STEPS_LIST, m_failedStepListCtrl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CFailureSummaryDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CFailureSummaryDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFailureSummaryDlg message handlers

BOOL CFailureSummaryDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // TODO: Add extra initialization here
    m_failedStepListCtrl.InsertColumn(0, _T("Step"), LVCFMT_LEFT, 45);
    m_failedStepListCtrl.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 150);
    m_failedStepListCtrl.InsertColumn(2, _T("Error"), LVCFMT_LEFT, 200);

    TA_Base_Core::FailedStepDetails failedSteps = getStepDetail().data.failedSteps();

    if (failedSteps.length() == 0)
    {
        int item = m_failedStepListCtrl.InsertItem(0, _T(""));
        m_failedStepListCtrl.SetItemText(item, 1, _T("No Step failures."));
        m_failedStepListCtrl.EnableWindow(FALSE);
    }
    else
    {
        for (unsigned long i = 0; i < failedSteps.length(); ++i)
        {
            int item = m_failedStepListCtrl.InsertItem(m_failedStepListCtrl.GetItemCount(), _T(""));
            m_failedStepListCtrl.SetItemText(item, 0,
                _T(TA_Base_Bus::StringUtilities::convertToString(failedSteps[i].position)));
            m_failedStepListCtrl.SetItemText(item, 1, _T(failedSteps[i].name.in()));
            m_failedStepListCtrl.SetItemText(item, 2, _T(failedSteps[i].error.in()));
        }
    }

//  move the sending of the step result to reportCompletionStatus(),
//    even if design states otherwise... because the step dialog would be
//  overlapped by the next step dialog
/*    try
    {
        PlanAgentAccess::getInstance().reportFailureSummaryStepResult(getStepDetail().activePlan,
            TA_Base_Core::PSCS_SUCCESS, "");
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }
*/

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CFailureSummaryDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // TODO: Add extra validation here
    setCompletionStatus(IDOK);
    DestroyWindow();

    FUNCTION_EXIT;
}


CString CFailureSummaryDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return LabelMaker::getFailureSummaryStepDialogTitlePrefix();
}


void CFailureSummaryDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    // do nothing
    // the step is demed to have completed when the dialog is
    // displayed and NOT when the operator has closed it.
    // the completion status is reported from OnInitDialog()
    try
    {
        PlanAgentAccess::getInstance().reportFailureSummaryStepResult(getStepDetail().activePlan,
            TA_Base_Core::PSCS_SUCCESS, "");
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


