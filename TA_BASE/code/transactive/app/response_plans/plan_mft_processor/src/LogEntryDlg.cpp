//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/LogEntryDlg.cpp $
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
#include "app/response_plans/plan_mft_processor/src/LogEntryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CLogEntryDlg dialog
IMPLEMENT_DYNAMIC(CLogEntryDlg, CInteractiveStepDlg)

CLogEntryDlg::CLogEntryDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
CInteractiveStepDlg(CLogEntryDlg::IDD, stepDetail, stepDetail.data.bgLog().responseTimeout),
    m_query(_T(stepDetail.data.bgLog().message.in())),
    m_isLogEntryMandatory(stepDetail.data.bgLog().logRequired)
{
    FUNCTION_ENTRY( "CLogEntryDlg" );

    // {{AFX_DATA_INIT(CLogEntryDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CLogEntryDlg::~CLogEntryDlg()
{
    FUNCTION_ENTRY( "~CLogEntryDlg" );
    FUNCTION_EXIT;
}


void CLogEntryDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CLogEntryDlg)
    DDX_Text(pDX, IDC_QUERY_EDIT, m_query);
    DDX_Control(pDX, IDC_LOG_MESSAGE_EDIT, m_logMessageEdit);
    DDX_Control(pDX, IDOK, m_okBtn);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CLogEntryDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CLogEntryDlg)
    ON_EN_CHANGE(IDC_LOG_MESSAGE_EDIT, OnLogMsgChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogEntryDlg message handlers

BOOL CLogEntryDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // TODO: Add extra initialization here
    //m_logMessageEdit.SetEventMask(ENM_CHANGE | ENM_KEYEVENTS | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS);
    m_logMessageEdit.SetWindowText(_T(""));

    // If log entry is mandatory, then disable the OK button to begin with
    m_okBtn.EnableWindow(!m_isLogEntryMandatory);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogEntryDlg::OnLogMsgChanged()
{
    FUNCTION_ENTRY( "OnLogMsgChanged" );

    // Where log entry is mandatory ensure that the message is non-empty before enabling the OK button.
    // Blank spaces and tabs do not constitute a valid entry.
    if (m_isLogEntryMandatory)
    {
        CString logMessage;
        m_logMessageEdit.GetWindowText(logMessage);
        logMessage.TrimLeft();

        m_okBtn.EnableWindow(!logMessage.IsEmpty());
    }

    FUNCTION_EXIT;
}


CString CLogEntryDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return "ÈÕÖ¾";
    //return CInteractiveStepDlg::getLabel(IDS_LOGENTRY_STEP_DIALOG_TITLE_PREFIX, "Log");
}


void CLogEntryDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
        CString logMessage;
        m_logMessageEdit.GetWindowText(logMessage);

        ActivePlanAgency::instance()->getPlanAgent()->processBackgroundLogStepResponse(getStepDetail(), getCompletionStatus(), static_cast<LPCTSTR>(logMessage));
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", ex.what() );
        //MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


