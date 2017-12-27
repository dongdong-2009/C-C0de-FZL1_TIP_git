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
#include "PromptDlg.h"
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
// CPromptDlg dialog

CPromptDlg::CPromptDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    CInteractiveStepDlg(stepDetail, stepDetail.data.prompt().dismissTimeout),
    m_prompt(_T(stepDetail.data.prompt().message.in()))
{
    FUNCTION_ENTRY( "CPromptDlg" );

    // {{AFX_DATA_INIT(CPromptDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPromptDlg::~CPromptDlg()
{
    FUNCTION_ENTRY( "~CPromptDlg" );
    FUNCTION_EXIT;
}


void CPromptDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPromptDlg)
    DDX_Text(pDX, IDC_PROMPT_EDIT, m_prompt);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPromptDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CPromptDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPromptDlg message handlers

CString CPromptDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return LabelMaker::getPromptStepDialogTitlePrefix();
}


void CPromptDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
        PlanAgentAccess::getInstance().reportPromptStepResult(getStepDetail().activePlan, getCompletionStatus());
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


