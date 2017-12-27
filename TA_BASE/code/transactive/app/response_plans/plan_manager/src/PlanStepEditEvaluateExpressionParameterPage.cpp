//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditEvaluateExpressionParameterPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates editing of parameters associated with a Evaluate Expression step.

#include "StdAfx.h"

#if defined (STEPTYPE_DATA_POINT_CHECK)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "PlanManager.h"
#include "PlanStepEditEvaluateExpressionParameterPage.h"
#include "EvaluateExpressionStep.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditEvaluateExpressionParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditEvaluateExpressionParameterPage, CPlanStepEditParameterPage)

CPlanStepEditEvaluateExpressionParameterPage::CPlanStepEditEvaluateExpressionParameterPage(EvaluateExpressionStep* step) :
   CPlanStepEditParameterPage(CPlanStepEditEvaluateExpressionParameterPage::IDD, step),
    m_expressionValue (*step->getEvaluateExpressionValueParameter ()),
   m_successParameter(*step->getSuccessParameter()),
   m_failureParameter(*step->getFailParameter())
   {
    FUNCTION_ENTRY( "CPlanStepEditEvaluateExpressionParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditEvaluateExpressionParameterPage)
    m_failStepLabel = _T("");
    m_successStepLabel = _T("");
    m_dataPointLabel = _T("");
    m_dataPointTypeLabel = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditEvaluateExpressionParameterPage::~CPlanStepEditEvaluateExpressionParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditEvaluateExpressionParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditEvaluateExpressionParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditEvaluateExpressionParameterPage)
    DDX_Control(pDX, IDC_EXPRESSION_TEXT, m_expression);
    DDX_Text(pDX, IDC_FAIL_STEP, m_failStepLabel);
    DDX_Text(pDX, IDC_SUCCESS_STEP, m_successStepLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditEvaluateExpressionParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditEvaluateExpressionParameterPage)
    ON_BN_CLICKED(IDC_PICK_FAIL_STEP_BUTTON, OnPickFailStepButton)
    ON_BN_CLICKED(IDC_PICK_SUCCESS_STEP_BUTTON, OnPickSuccessStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPlanStepEditEvaluateExpressionParameterPage::OnInitDialog ()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    CWnd* instMsgText = GetDlgItem (IDC_INSTRUCTIONS_TXT);
    if (instMsgText)
    {
        CString instructionMsg;
        instructionMsg.LoadString (IDS_STEP_PARAMETER_EVAL_INSTRUCTIONS);
        instMsgText->SetWindowText (instructionMsg);
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditEvaluateExpressionParameterPage::OnPickFailStepButton()
{
    FUNCTION_ENTRY( "OnPickFailStepButton" );

    UpdateData(); // TD#1537

    m_failureParameter.editParameter();

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditEvaluateExpressionParameterPage::OnPickSuccessStepButton()
{
    FUNCTION_ENTRY( "OnPickSuccessStepButton" );

    UpdateData(); // TD#1537

    m_successParameter.editParameter();

    ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditEvaluateExpressionParameterPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* expressionText =  GetDlgItem (IDC_EXPRESSION_TEXT);
    if (expressionText)
    {
        expressionText->EnableWindow(enable);
    }

    CWnd* successStepButton = GetDlgItem(IDC_PICK_SUCCESS_STEP_BUTTON);
    if (successStepButton)
    {
        successStepButton->EnableWindow(enable);
    }

    CWnd* failStepButton = GetDlgItem(IDC_PICK_FAIL_STEP_BUTTON);
    if (failStepButton)
    {
        failStepButton->EnableWindow(enable);
    }

     CWnd* instMsgText = GetDlgItem (IDC_INSTRUCTIONS_TXT);
     if (instMsgText)
     {
         CString instructionMsg;
         instructionMsg.LoadString (IDS_STEP_PARAMETER_EVAL_INSTRUCTIONS);
         instMsgText->SetWindowText (instructionMsg);
     }

    FUNCTION_EXIT;
}


void CPlanStepEditEvaluateExpressionParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    CWaitCursor waitCursor;
    UpdateData();

    m_expression.SetReadOnly (FALSE);
    m_expression.SetWindowText (m_expressionValue.getMessageText().c_str());
    m_expression.ShowWindow (SW_SHOW);

    // Set the success and failure step display to show the step number and description.
   ResetSuccessAndFailureStepDisplay();

    FUNCTION_EXIT;
}


void CPlanStepEditEvaluateExpressionParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    EvaluateExpressionStep* step = dynamic_cast<EvaluateExpressionStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the evaluate expression step page");

    CString expressionValueStr;
    m_expression.GetWindowText(expressionValueStr);
   m_expressionValue.setText (CORBA::string_dup((LPCTSTR) expressionValueStr));

    step->getEvaluateExpressionValueParameter()->setText (m_expressionValue.getMessageText().c_str());
    step->getSuccessParameter()->setStepId(m_successParameter.getStepId());
    step->getFailParameter()->setStepId(m_failureParameter.getStepId());

    FUNCTION_EXIT;
}


void CPlanStepEditEvaluateExpressionParameterPage::ResetSuccessAndFailureStepDisplay()
{
    FUNCTION_ENTRY( "ResetSuccessAndFailureStepDisplay" );

    // Set the success and failure step display with step number and description
    m_successStepLabel = m_successParameter.getDestinationStepDescription().c_str();
    m_failStepLabel = m_failureParameter.getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_DATA_POINT_CHECK)
