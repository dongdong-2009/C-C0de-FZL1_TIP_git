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

#include "stdafx.h"
#include "PlanManager.h"
#include "LogEntryDlg.h"
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
// CLogEntryDlg dialog

CLogEntryDlg::CLogEntryDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    CInteractiveStepDlg(stepDetail, stepDetail.data.log().responseTimeout),
    m_query(_T(stepDetail.data.log().message.in())),
    m_isLogEntryMandatory(stepDetail.data.log().logRequired)
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
    ON_NOTIFY(EN_MSGFILTER, IDC_LOG_MESSAGE_EDIT, OnLogMsgFilterNotify)
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
    m_logMessageEdit.SetEventMask(ENM_CHANGE | ENM_KEYEVENTS | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS);
    m_logMessageEdit.SetWindowText(_T(""));

    // If log entry is mandatory, then disable the OK button to begin with
    m_okBtn.EnableWindow(!m_isLogEntryMandatory);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CLogEntryDlg::OnLogMsgFilterNotify(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnLogMsgFilterNotify" );

    MSGFILTER *msgFilter = (MSGFILTER *) pNMHDR;
    *pResult = 0;

#ifdef TIMEOUT_COUNTER_RESET
    // Any activity within the log message edit other than mouse movement
    // or Esc key presses will reset the response timer.
    if (msgFilter->msg == WM_MOUSEMOVE ||
        (msgFilter->msg == WM_KEYDOWN || msgFilter->msg == WM_KEYUP || msgFilter->msg == WM_CHAR) &&
         msgFilter->wParam == VK_ESCAPE)
    {
        FUNCTION_EXIT;
        return;
    }

    StartTimeoutCountdown();
#endif

    FUNCTION_EXIT;
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
    return LabelMaker::getLogEntryStepDialogTitlePrefix();
}


void CLogEntryDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
        CString logMessage;
        m_logMessageEdit.GetWindowText(logMessage);

        PlanAgentAccess::getInstance().reportLogStepResult(getStepDetail().activePlan, getCompletionStatus(),
                                                           static_cast<LPCTSTR>(logMessage));
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


