/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andy Parker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Plan step editor/customiser/viewer dialog
  *
  **/

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanStepEditorDlg.h"
#include "app/response_plans/plan_manager/src/PlanStep.h"
#include "app/response_plans/plan_manager/src/PlanStepParameter.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorDlg dialog

CPlanStepEditorDlg::CPlanStepEditorDlg(PlanStep* step,bool canEdit)
    : m_planStep(step),
      m_customStepDetail(NULL),
      m_canEdit(canEdit),
      m_canCustomise(false),
      m_stepDetailsPage(step,canEdit),
      m_parametersPage(NULL),
      CDialog(CPlanStepEditorDlg::IDD, NULL)
{
    FUNCTION_ENTRY( "CPlanStepEditorDlg" );

    m_parametersPage = m_planStep->getParameterEditPage();
    if (m_parametersPage)
    {
        m_parametersPage->EnableEditing(canEdit);  // TD#614/1170/1409
    }

    FUNCTION_EXIT;
}


CPlanStepEditorDlg::CPlanStepEditorDlg(PlanStep* step, TA_Base_Core::StepCustomisableDetail *customStepDetail) :
    m_planStep(step),
    m_customStepDetail(customStepDetail),
    m_canEdit(false),
    m_canCustomise(true),
    m_stepDetailsPage(step, customStepDetail),
    m_parametersPage(NULL),
    CDialog(CPlanStepEditorDlg::IDD, NULL)
{
    FUNCTION_ENTRY( "CPlanStepEditorDlg" );

    m_parametersPage = m_planStep->getParameterEditPage();
    if (m_parametersPage)
    {
        m_parametersPage->EnableEditing(false);  // TD#614/1170/1409
    }

    FUNCTION_EXIT;
}


CPlanStepEditorDlg::~CPlanStepEditorDlg()
{
    FUNCTION_ENTRY( "~CPlanStepEditorDlg" );

    delete m_parametersPage;

    FUNCTION_EXIT;
}


void CPlanStepEditorDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditorDlg)
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditorDlg, CDialog)
    // {{AFX_MSG_MAP(CPlanStepEditorDlg)
    ON_BN_CLICKED(IDOK, OnOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorDlg message handlers

BOOL CPlanStepEditorDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    m_stepPropertySheet.Add(&m_stepDetailsPage, true);
    if (m_parametersPage != NULL)
    {
        m_stepPropertySheet.Add(m_parametersPage, true);
    }
    else
    {
        m_stepPropertySheet.Add(m_parametersPage = new CPlanStepEditParameterPage(NULL), false);
    }

    m_stepPropertySheet.Activate(this);

    // Set the title
    std::string title = m_planStep->getStepName();
    if (m_canEdit)
    {
		title += " - 步骤编辑器";
    }
    else if (m_canCustomise)
    {
		title += " - 步骤定制器";
    }
    else
    {
		title += " - 步骤查看器";
    }
    SetWindowText(title.c_str());

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditorDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    if (m_canEdit)
    {
        // Give the active page a chance to perform any last-minute data validation
        if (!m_stepPropertySheet.GetActivePage()->OnKillActive())
        {
            FUNCTION_EXIT;
            return;
        }

        // Save any detail changes
        m_stepDetailsPage.UpdateStepDetails();

        // Save any paramater changes
        m_parametersPage->SaveParameters();
    }
    else if (m_canCustomise)
    {
        // Give the active page a chance to perform any last-minute data validation
        if (!m_stepPropertySheet.GetActivePage()->OnKillActive()) // TD#1486 - suppress OK when OnKillActive() returns false, not true.
        {
            FUNCTION_EXIT;
            return;
        }

        // Save temporary delay/skip changes
        m_stepDetailsPage.UpdateCustomDetails();
    }

    // Update the step in the plan
    CDialog::OnOK();

    FUNCTION_EXIT;
}


