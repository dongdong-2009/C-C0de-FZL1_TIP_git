//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditBackgroundPromptPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_BACKGROUND_PROMPT)

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "BackgroundPromptStep.h"
#include "PlanStepEditBackgroundPromptPage.h"
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

IMPLEMENT_DYNAMIC(CPlanStepEditBackgroundPromptPage, CPlanStepEditParameterPage)

CPlanStepEditBackgroundPromptPage::CPlanStepEditBackgroundPromptPage(BackgroundPromptStep * step)
: CPlanStepEditParameterPage(CPlanStepEditBackgroundPromptPage::IDD,step),
m_operatorProfileParameter(*step->getOperatorProfile())
{
    FUNCTION_ENTRY( "CPlanStepEditBackgroundPromptPage" );

    // {{AFX_DATA_INIT(CPlanStepEditBackgroundPromptPage)
    m_promptMessage = _T("");
    m_responseTimeout = MIN_INTERACTIVE_STEP_TIMEOUT;
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditBackgroundPromptPage::~CPlanStepEditBackgroundPromptPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditBackgroundPromptPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundPromptPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditBackgroundPromptPage)
    DDX_Text(pDX, IDC_PROMPT_MESSAGE, m_promptMessage);
    DDX_Control (pDX, IDC_PROMPT_MESSAGE, m_promptMsg);
    DDX_Control(pDX, IDC_RESPONSE_TIMEOUT_SPIN, m_responseTimeoutSpin);
    DDX_Text(pDX, IDC_RESPONSE_TIMEOUT, m_responseTimeout);
    DDV_MinMaxUInt(pDX, m_responseTimeout, MIN_INTERACTIVE_STEP_TIMEOUT, MAX_INTERACTIVE_STEP_TIMEOUT);
    DDX_Control(pDX, IDC_OPERATOR_PROFILE_COMBO, m_operatorProfileCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditBackgroundPromptPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditBackgroundPromptPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLogPage message handlers

BOOL CPlanStepEditBackgroundPromptPage::OnInitDialog()
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

    try
    {
        typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
        ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
        for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
        {
            int appTypeIdx = m_operatorProfileCombo.AddString((*iter)->getName().c_str());
            m_operatorProfileCombo.SetItemData(appTypeIdx, (*iter)->getKey());
            delete *iter;
        }
        allProfile.clear();
    }
    catch (...)
    {
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditBackgroundPromptPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
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

    CWnd* operatorProfileCombo = GetDlgItem(IDC_OPERATOR_PROFILE_COMBO);
    if (operatorProfileCombo)
    {
        operatorProfileCombo->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundPromptPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    UpdateData();

    BackgroundPromptStep* step = dynamic_cast<BackgroundPromptStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the prompt step page");

    m_promptMessage    = step->getMessageParameter()->getMessageText().c_str();
    m_responseTimeout = step->getTimeoutParameter()->getNumericValue();

    try
    {
        std::auto_ptr<TA_Base_Core::IProfile> iProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(m_operatorProfileParameter.getNumericValue(), false));
        int appTypeIdx = m_operatorProfileCombo.FindStringExact(-1, iProfile->getName().c_str());
        if (appTypeIdx != CB_ERR)
        {
            m_operatorProfileCombo.SetCurSel(appTypeIdx);
        }
    }
    catch(...)
    {

    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditBackgroundPromptPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    BackgroundPromptStep* step = dynamic_cast<BackgroundPromptStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the prompt step page");

    step->getMessageParameter()->setText((LPCSTR) m_promptMessage);
    step->getTimeoutParameter()->setValue(m_responseTimeout);

    int profileIdx = m_operatorProfileCombo.GetCurSel();
    if (profileIdx != CB_ERR)
    {
        CString currentProfileName;
        m_operatorProfileCombo.GetLBText(profileIdx, currentProfileName);
        step->getOperatorProfile()->setValue(m_operatorProfileCombo.GetItemData(profileIdx));
        step->getOperatorProfileName()->setText((LPCTSTR)currentProfileName);
    }

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_BACKGROUND_PROMPT)
