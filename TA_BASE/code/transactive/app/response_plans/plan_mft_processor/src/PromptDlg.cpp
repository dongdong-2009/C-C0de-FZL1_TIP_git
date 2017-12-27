//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PromptDlg.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#include <afxwin.h>
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/PromptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPromptDlg dialog
IMPLEMENT_DYNAMIC(CPromptDlg, CInteractiveStepDlg)

CPromptDlg::CPromptDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
CInteractiveStepDlg(CPromptDlg::IDD, stepDetail, stepDetail.data.bgPrompt().dismissTimeout),
    m_prompt(_T(stepDetail.data.bgPrompt().message.in()))
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
    return "ÌבÊ¾";
    //return CInteractiveStepDlg::getLabel(IDS_PROMPT_STEP_DIALOG_TITLE_PREFIX, "Prompt");
}


void CPromptDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
        ActivePlanAgency::instance()->getPlanAgent()->processBackgroundPromptStepResponse(getStepDetail(), getCompletionStatus());
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", ex.what() );
        //MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


