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

#include "StdAfx.h"
#include "PlanManager.h"
#include "DecisionDlg.h"
#include "LabelMaker.h"
#include "MessageBox.h"
#include "PlanAgentAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CDecisionDlg dialog

CDecisionDlg::CDecisionDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    CInteractiveStepDlg(stepDetail, stepDetail.data.decision().responseTimeout),
    m_query(_T(stepDetail.data.decision().decisionMessage.in()))
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
    TA_Base_Core::DecisionParameters parameters = getStepDetail().data.decision();
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
    DestroyWindow();

    FUNCTION_EXIT;
}


//++lizettejl
void CDecisionDlg::OnYes()
{
    FUNCTION_ENTRY( "OnYes" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDYES);
    DestroyWindow();

    FUNCTION_EXIT;
}


void CDecisionDlg::OnNo()
{
    FUNCTION_ENTRY( "OnNo" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDNO);
    DestroyWindow();

    FUNCTION_EXIT;
}


CString CDecisionDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return LabelMaker::getDecisionStepDialogTitlePrefix();
}


void CDecisionDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
        PlanAgentAccess::getInstance().reportDecisionStepResult(getStepDetail().activePlan, getCompletionStatus());
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


