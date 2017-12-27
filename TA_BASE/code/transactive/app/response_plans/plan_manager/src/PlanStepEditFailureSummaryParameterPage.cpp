
//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditFailureSummaryParameterPage.cpp $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

// PlanStepEditFailureSummaryParameterPage.cpp : implementation file
//

#include "stdafx.h"

#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)

#include "planmanager.h"
#include "PlanStepEditFailureSummaryParameterPage.h"
#include "FailureSummaryStep.h"
#include "FailureCategoryStepParameter.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditFailureSummaryParameterPage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditFailureSummaryParameterPage, CPlanStepEditParameterPage)


CPlanStepEditFailureSummaryParameterPage::CPlanStepEditFailureSummaryParameterPage(FailureSummaryStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditFailureSummaryParameterPage::IDD, step)
{
    FUNCTION_ENTRY( "CPlanStepEditFailureSummaryParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditFailureSummaryParameterPage)
    m_failureStepCategory = UNDEFINED_STEPS;
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepEditFailureSummaryParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditFailureSummaryParameterPage)
    DDX_Radio(pDX, IDC_ALL_STEPS_RADIO, m_failureStepCategory);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditFailureSummaryParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditFailureSummaryParameterPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditFailureSummaryParameterPage message handlers

BOOL CPlanStepEditFailureSummaryParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    // TODO: Add extra initialization here
    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditFailureSummaryParameterPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* allStepsRadio = GetDlgItem(IDC_ALL_STEPS_RADIO);
    if (NULL != allStepsRadio)
    {
        allStepsRadio->EnableWindow(enable);
    }
    CWnd* brcStepsRadio = GetDlgItem(IDC_SET_DATA_POINT_STEPS_RADIO);
    if (NULL != brcStepsRadio)
    {
        brcStepsRadio->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditFailureSummaryParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    FailureSummaryStep* step = dynamic_cast<FailureSummaryStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the failure summay page");

    TA_Base_Core::EStepFailureCategory category = step->getFailureCategoryParameter()->getCategory();
    switch (category)
    {
    case TA_Base_Core::SFC_ALL_STEPS:
        m_failureStepCategory = ALL_STEPS;
        break;
    case TA_Base_Core::SFC_SET_DATA_POINT_STEPS:
        m_failureStepCategory = BRC_STEPS_ONLY;
        break;
    default:
        m_failureStepCategory = UNDEFINED_STEPS;
        break;
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditFailureSummaryParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    FailureSummaryStep* step = dynamic_cast<FailureSummaryStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the failure summay page");

    UpdateData(TRUE);
    TA_Base_Core::EStepFailureCategory category = TA_Base_Core::SFC_UNDEFINED;

    switch (m_failureStepCategory)
    {
    case ALL_STEPS:
        category = TA_Base_Core::SFC_ALL_STEPS;
        break;
    case BRC_STEPS_ONLY:
        category = TA_Base_Core::SFC_SET_DATA_POINT_STEPS;
        break;
    }

    step->getFailureCategoryParameter()->setCategory(category);

    FUNCTION_EXIT;
}


#endif //STEPTYPE_FAILURE_SUMMARY_STEP
