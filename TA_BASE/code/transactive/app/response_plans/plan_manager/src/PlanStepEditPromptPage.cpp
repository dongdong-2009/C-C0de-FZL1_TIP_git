//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditPromptPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_PROMPT)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepEditPromptPage.h"
#include "PromptStep.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "PlanNode.h"
#include "PlanManagerCommonDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

static const unsigned int PROMPT_EDIT_TXT = 950;//DB limit is 1000 but this should be more then enoug

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLogPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditPromptPage, CPlanStepEditParameterPage)

CPlanStepEditPromptPage::CPlanStepEditPromptPage(PromptStep* step)
  : CPlanStepEditParameterPage(CPlanStepEditPromptPage::IDD,step)
{
    FUNCTION_ENTRY( "CPlanStepEditPromptPage" );

    // {{AFX_DATA_INIT(CPlanStepEditPromptPage)
    m_promptMessage = _T("");
    m_responseTimeout = MIN_INTERACTIVE_STEP_TIMEOUT;
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditPromptPage::~CPlanStepEditPromptPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditPromptPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditPromptPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditPromptPage)
    DDX_Text(pDX, IDC_PROMPT_MESSAGE, m_promptMessage);
    DDX_Control (pDX, IDC_PROMPT_MESSAGE, m_promptMsg);
    DDX_Control(pDX, IDC_RESPONSE_TIMEOUT_SPIN, m_responseTimeoutSpin);
    DDX_Text(pDX, IDC_RESPONSE_TIMEOUT, m_responseTimeout);
    DDV_MinMaxUInt(pDX, m_responseTimeout, MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditPromptPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditPromptPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLogPage message handlers

BOOL CPlanStepEditPromptPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    m_responseTimeoutSpin.SetBuddy(GetDlgItem(IDC_RESPONSE_TIMEOUT));
    m_responseTimeoutSpin.SetRange(MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT);
    m_promptMsg.SetLimitText (PROMPT_EDIT_TXT);//as per TD 14921
    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditPromptPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CEdit* promptMessage = static_cast<CEdit*>(GetDlgItem(IDC_PROMPT_MESSAGE));
    if (promptMessage)
    {
        promptMessage->SetReadOnly(!enable);
    }

    CEdit* responseTimeout = static_cast<CEdit*>(GetDlgItem(IDC_RESPONSE_TIMEOUT));
    if (responseTimeout)
    {
        responseTimeout->SetReadOnly(!enable);
    }

    m_responseTimeoutSpin.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditPromptPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    UpdateData();

    PromptStep* step = dynamic_cast<PromptStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the prompt step page");

    m_promptMessage    = step->getMessageParameter()->getMessageText().c_str();
    m_responseTimeout = step->getTimeoutParameter()->getNumericValue();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditPromptPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    PromptStep* step = dynamic_cast<PromptStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the prompt step page");

    step->getMessageParameter()->setText((LPCSTR) m_promptMessage);
    step->getTimeoutParameter()->setValue(m_responseTimeout);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_PROMPT)
