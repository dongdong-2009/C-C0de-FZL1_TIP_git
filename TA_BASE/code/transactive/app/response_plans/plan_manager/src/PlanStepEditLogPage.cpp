//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLogPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_LOG)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepEditLogPage.h"
#include "LogStep.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "YesNoStepParameter.h"
#include "PlanNode.h"
#include "PlanManagerCommonDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const unsigned int LOG_EDIT_TXT = 950;//DB limit is 1000 but this should be more then enough

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLogPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditLogPage, CPlanStepEditParameterPage)

CPlanStepEditLogPage::CPlanStepEditLogPage(LogStep* step)
  : CPlanStepEditParameterPage(CPlanStepEditLogPage::IDD,step)
{
    FUNCTION_ENTRY( "CPlanStepEditLogPage" );

    // {{AFX_DATA_INIT(CPlanStepEditLogPage)
    m_promptMessage = _T("");
    m_responseTimeout = MIN_INTERACTIVE_STEP_TIMEOUT;
    m_mandatoryLogEntry = TRUE;
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditLogPage::~CPlanStepEditLogPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditLogPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditLogPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditLogPage)
    DDX_Text(pDX, IDC_PROMPT_MESSAGE, m_promptMessage);
    DDX_Control(pDX, IDC_PROMPT_MESSAGE, m_promptMsgTxt);
    DDX_Control(pDX, IDC_RESPONSE_TIMEOUT_SPIN, m_responseTimeoutSpin);
    DDX_Text(pDX, IDC_RESPONSE_TIMEOUT, m_responseTimeout);
    DDV_MinMaxUInt(pDX, m_responseTimeout, MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT);
    DDX_Check(pDX, IDC_MANDATORY_LOG_ENTRY_CHECK, m_mandatoryLogEntry);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditLogPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditLogPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLogPage message handlers

BOOL CPlanStepEditLogPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    m_responseTimeoutSpin.SetBuddy(GetDlgItem(IDC_RESPONSE_TIMEOUT));
    m_responseTimeoutSpin.SetRange(MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT);

    // as per TD 12385
    m_promptMsgTxt.SetLimitText (LOG_EDIT_TXT);

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditLogPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
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

    CWnd* mandatoryLogEntry = GetDlgItem(IDC_MANDATORY_LOG_ENTRY_CHECK);
    if (mandatoryLogEntry)
    {
        mandatoryLogEntry->EnableWindow(enable);
    }

    m_responseTimeoutSpin.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditLogPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    UpdateData();

    LogStep* step = dynamic_cast<LogStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the log step page");

    m_promptMessage    = step->getMessageParameter()->getMessageText().c_str();
    m_responseTimeout = step->getTimeoutParameter()->getNumericValue();
    m_mandatoryLogEntry = step->getMandatoryLogParameter()->getAffirmative();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditLogPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    LogStep* step = dynamic_cast<LogStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the log step page");

    UpdateData();

    step->getMessageParameter()->setText((LPCSTR) m_promptMessage);
    step->getTimeoutParameter()->setValue(m_responseTimeout);
    step->getMandatoryLogParameter()->setAffirmative(m_mandatoryLogEntry != 0);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_LOG)
