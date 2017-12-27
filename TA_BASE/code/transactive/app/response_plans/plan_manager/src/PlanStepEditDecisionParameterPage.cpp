//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDecisionParameterPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"

#if defined (STEPTYPE_DECISION)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepEditDecisionParameterPage.h"
#include "DecisionStep.h"
#include "StepNumberStepParameter.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "YesNoStepParameter.h"
#include "PlanStepParameterStepNumberEditDlg.h"
#include "PlanNode.h"
#include "PlanManagerCommonDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditDecisionParameterPage, CPlanStepEditParameterPage)

CPlanStepEditDecisionParameterPage::CPlanStepEditDecisionParameterPage(DecisionStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditDecisionParameterPage::IDD, step),
    m_noStepParameter(*step->getNoParameter()),
    m_yesStepParameter(*step->getYesParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditDecisionParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditDecisionParameterPage)
    m_decisionMessage = _T("");
    m_responseTimeout = MIN_INTERACTIVE_STEP_TIMEOUT;
    m_yesStep = _T("");
    m_noStep = _T("");
    m_yesBtnLabel = _T("");
    m_noBtnLabel = _T("");
    m_pauseBtnLabel = _T("");
    m_noBtnEnabled = true;
    m_pauseBtnEnabled = false;
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditDecisionParameterPage::~CPlanStepEditDecisionParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditDecisionParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditDecisionParameterPage)
    DDX_Text(pDX, IDC_DECISION_MESSAGE, m_decisionMessage);
    DDX_Control(pDX, IDC_RESPONSE_TIMEOUT_SPIN, m_responseTimeoutSpin);
    DDX_Text(pDX, IDC_RESPONSE_TIMEOUT, m_responseTimeout);
    DDV_MinMaxUInt(pDX, m_responseTimeout, MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT);
    DDX_Text(pDX, IDC_NO_STEP, m_noStep);
    DDX_Text(pDX, IDC_YES_STEP, m_yesStep);
    DDX_Text(pDX, IDC_YES_BUTTON_LABEL, m_yesBtnLabel);
    DDV_MaxChars(pDX, m_yesBtnLabel, 10);
    DDX_Text(pDX, IDC_NO_BUTTON_LABEL, m_noBtnLabel);
    DDV_MaxChars(pDX, m_noBtnLabel, 10);
    DDX_Text(pDX, IDC_PAUSE_BUTTON_LABEL, m_pauseBtnLabel);
    DDV_MaxChars(pDX, m_pauseBtnLabel, 10);
    DDX_Check(pDX, IDC_NO_BUTTON_ENABLED, m_noBtnEnabled);
    DDX_Check(pDX, IDC_PAUSE_BUTTON_ENABLED, m_pauseBtnEnabled);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditDecisionParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditDecisionParameterPage)
    ON_BN_CLICKED(IDC_NO_PICK_STEP_BUTTON, OnNoPickStepButton)
    ON_BN_CLICKED(IDC_YES_PICK_STEP_BUTTON, OnYesPickStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage message handlers

BOOL CPlanStepEditDecisionParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    m_responseTimeoutSpin.SetBuddy(GetDlgItem(IDC_RESPONSE_TIMEOUT));
    m_responseTimeoutSpin.SetRange(MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT);

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditDecisionParameterPage::OnNoPickStepButton()
{
    FUNCTION_ENTRY( "OnNoPickStepButton" );

    UpdateData(); // TD#1537

    // Select the required step
    m_noStepParameter.editParameter();
    m_noStep = m_noStepParameter.getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::OnYesPickStepButton()
{
    FUNCTION_ENTRY( "OnYesPickStepButton" );

    UpdateData(); // TD#1537

    // Select the required step
    m_yesStepParameter.editParameter();
    m_yesStep = m_yesStepParameter.getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CEdit* decisionMessage = static_cast<CEdit*>(GetDlgItem(IDC_DECISION_MESSAGE));
    if (decisionMessage)
    {
        decisionMessage->SetReadOnly(!enable);
    }

    CEdit* responseTimeout = static_cast<CEdit*>(GetDlgItem(IDC_RESPONSE_TIMEOUT));
    if (responseTimeout)
    {
        responseTimeout->SetReadOnly(!enable);
    }

    CWnd* yesStepButton = GetDlgItem(IDC_YES_PICK_STEP_BUTTON);
    if (yesStepButton)
    {
        yesStepButton->EnableWindow(enable);
    }

    CWnd* noStepButton = GetDlgItem(IDC_NO_PICK_STEP_BUTTON);
    if (noStepButton)
    {
        noStepButton->EnableWindow(enable);
    }

    m_responseTimeoutSpin.EnableWindow(enable);
    // lizettejl++ (DP-changes)
    CEdit *yesButtonLabelEdit = static_cast<CEdit*>(GetDlgItem(IDC_YES_BUTTON_LABEL));
    if (NULL != yesButtonLabelEdit)
    {
        yesButtonLabelEdit->SetReadOnly(!enable);
    }

    CEdit *noButtonLabelEdit = static_cast<CEdit*>(GetDlgItem(IDC_NO_BUTTON_LABEL));
    if (NULL != noButtonLabelEdit)
    {
        noButtonLabelEdit->SetReadOnly(!enable);
    }

    CEdit *pauseButtonLabelEdit = static_cast<CEdit*>(GetDlgItem(IDC_PAUSE_BUTTON_LABEL));
    if (NULL != pauseButtonLabelEdit)
    {
        pauseButtonLabelEdit->SetReadOnly(!enable);
    }

    CWnd* noButtonCheck = GetDlgItem(IDC_NO_BUTTON_ENABLED);
    if (NULL != noButtonCheck)
    {
        noButtonCheck->EnableWindow(enable);
    }

    CWnd* pauseButtonCheck = GetDlgItem(IDC_PAUSE_BUTTON_ENABLED);
    if (NULL != pauseButtonCheck)
    {
        pauseButtonCheck->EnableWindow(enable);
    }
    // ++lizettejl

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    UpdateData();

    DecisionStep* step = dynamic_cast<DecisionStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the decision step page");

    m_decisionMessage = step->getMessageParameter()->getMessageText().c_str();
    m_responseTimeout = step->getTimeoutParameter()->getNumericValue();
    m_yesStep = m_yesStepParameter.getDestinationStepDescription().c_str();
    m_noStep = m_noStepParameter.getDestinationStepDescription().c_str();
    // lizettejl++ (DP-changes)
    m_yesBtnLabel = step->getYesLabelParameter()->getMessageText().c_str();
    m_noBtnLabel = step->getNoLabelParameter()->getMessageText().c_str();
    m_pauseBtnLabel = step->getPauseLabelParameter()->getMessageText().c_str();
    m_noBtnEnabled = step->getEnableNoParameter()->getAffirmative();
    m_pauseBtnEnabled = step->getEnablePauseParameter()->getAffirmative();
    // ++lizettejl
    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditDecisionParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    DecisionStep* step = dynamic_cast<DecisionStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the decision step page");

    UpdateData(TRUE);

    step->getMessageParameter()->setText((LPCSTR) m_decisionMessage);
    step->getTimeoutParameter()->setValue(m_responseTimeout);
    step->getYesParameter()->setStepId(m_yesStepParameter.getStepId());
    step->getNoParameter()->setStepId(m_noStepParameter.getStepId());
    // lizettejl++ (DP-changes)
    step->getYesLabelParameter()->setText((LPCSTR)m_yesBtnLabel);
    step->getNoLabelParameter()->setText((LPCSTR)m_noBtnLabel);
    step->getPauseLabelParameter()->setText((LPCSTR)m_pauseBtnLabel);
    step->getEnableNoParameter()->setAffirmative(m_noBtnEnabled != 0 );
    step->getEnablePauseParameter()->setAffirmative(m_pauseBtnEnabled != 0 );
    // ++lizettejl

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_DECISION)
