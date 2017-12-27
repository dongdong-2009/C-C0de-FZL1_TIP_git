//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/DecisionDlg.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#include <afxwin.h>
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/DecisionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CDecisionDlg dialog

IMPLEMENT_DYNAMIC(CDecisionDlg, CInteractiveStepDlg)

CDecisionDlg::CDecisionDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
CInteractiveStepDlg(CDecisionDlg::IDD, stepDetail, stepDetail.data.bgDecision().responseTimeout),
    m_query(_T(stepDetail.data.bgDecision().decisionMessage.in()))
{
    FUNCTION_ENTRY( "CDecisionDlg" );

    // {{AFX_DATA_INIT(CDecisionDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CDecisionDlg::~CDecisionDlg()
{
    FUNCTION_ENTRY( "~CDecisionDlg" );
    FUNCTION_EXIT;
}


void CDecisionDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CDecisionDlg)
    DDX_Text(pDX, IDC_QUERY_EDIT, m_query);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CDecisionDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CDecisionDlg)
    ON_BN_CLICKED(IDYES, OnYes)
    ON_BN_CLICKED(IDNO, OnNo)
    ON_BN_CLICKED(IDPAUSE, OnPause)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDecisionDlg message handlers

//lizettejl++ (DP-changes)
BOOL CDecisionDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // TODO: Add extra initialization here
    TA_Base_Core::BackgroundDecisionParameters parameters = getStepDetail().data.bgDecision();
    GetDlgItem(IDYES)->SetWindowText(parameters.yesButtonLabel.in());
    GetDlgItem(IDNO)->SetWindowText(parameters.noButtonLabel.in());
    GetDlgItem(IDPAUSE)->SetWindowText(parameters.pauseButtonLabel.in());
    GetDlgItem(IDNO)->EnableWindow(parameters.noButtonEnabled);
    GetDlgItem(IDPAUSE)->EnableWindow(parameters.pauseButtonEnabled);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDecisionDlg::OnPause()
{
    FUNCTION_ENTRY( "OnPause" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDRETRY);
    EndDialog(IDRETRY);
    //DestroyWindow();

    FUNCTION_EXIT;
}


//++lizettejl
void CDecisionDlg::OnYes()
{
    FUNCTION_ENTRY( "OnYes" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDYES);
    EndDialog(IDYES);
    //DestroyWindow();

    FUNCTION_EXIT;
}


void CDecisionDlg::OnNo()
{
    FUNCTION_ENTRY( "OnNo" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDNO);
    EndDialog(IDNO);
    //DestroyWindow();

    FUNCTION_EXIT;
}


CString CDecisionDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return "¾ö²ß";
    //return CInteractiveStepDlg::getLabel(IDS_DECISION_STEP_DIALOG_TITLE_PREFIX, "Decision");
}


void CDecisionDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
        ActivePlanAgency::instance()->getPlanAgent()->processBackgroundDecisionStepResponse(getStepDetail(), getCompletionStatus());
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", ex.what() );
        //MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


