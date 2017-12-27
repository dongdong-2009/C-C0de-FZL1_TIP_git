//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Andy Parker
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "app/response_plans/plan_manager/src/StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/PlanStepEditParameterPage.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditParameterPage, CPropertyPage)

CPlanStepEditParameterPage::CPlanStepEditParameterPage(UINT dialogIDD, PlanStep* step)
   : CPropertyPage(dialogIDD), m_step(step), m_canEdit(false)
{
    FUNCTION_ENTRY( "CPlanStepEditParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditParameterPage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage::CPlanStepEditParameterPage(PlanStep* step)
    : CPropertyPage(CPlanStepEditParameterPage::IDD), m_step(step), m_canEdit(false)
{
    FUNCTION_ENTRY( "CPlanStepEditParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditParameterPage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage::~CPlanStepEditParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPropertyPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditParameterPage)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditParameterPage, CPropertyPage)
    // {{AFX_MSG_MAP(CPlanStepEditParameterPage)
    ON_MESSAGE_VOID(WM_DISPLAY_STEP_PARAMETERS, OnDisplayParameters)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDecisionParameterPage message handlers

BOOL CPlanStepEditParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPropertyPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    PostMessage(WM_DISPLAY_STEP_PARAMETERS, 0, 0); // Call DisplayParameters() after OnInitDialog() has returned

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditParameterPage::OnDisplayParameters()
{
    FUNCTION_ENTRY( "OnDisplayParameters" );

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    EnableParameterControls(m_canEdit);  // TD#614/1170/1409
    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditParameterPage::SaveParameters()
{
    FUNCTION_ENTRY( "SaveParameters" );

    // Check if the dialog is displayed as this is a property page
    if (GetSafeHwnd()!=NULL)
    {
        UpdateData();
        UpdateParameters();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditParameterPage::EnableParameterControls(bool /* enable */)
{
    FUNCTION_ENTRY( "EnableParameterControls" );
    FUNCTION_EXIT;
}


void CPlanStepEditParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );
    FUNCTION_EXIT;
}


void CPlanStepEditParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );
    FUNCTION_EXIT;
}


