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
// This class represents a simple dialog displayed to the operator when pasting steps within a plan.
// It enables the operator to pick the paste position wrt currently selected step(s), i.e. before or after.
// If the currently selected step is the End Step, the paste after option is disabled.

#include "app/response_plans/plan_manager/src/StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/PlanStepPastePositionDlg.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepPastePositionDlg dialog

CPlanStepPastePositionDlg::CPlanStepPastePositionDlg(bool endStepSelected, CWnd* pParent /*=NULL*/) :
    CDialog(CPlanStepPastePositionDlg::IDD, pParent),
    m_endStepSelected(endStepSelected),
    m_pastePosition(PPT_PASTE_AFTER)
{
    FUNCTION_ENTRY( "CPlanStepPastePositionDlg" );

    // {{AFX_DATA_INIT(CPlanStepPastePositionDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepPastePositionDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepPastePositionDlg)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepPastePositionDlg, CDialog)
    // {{AFX_MSG_MAP(CPlanStepPastePositionDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepPastePositionDlg message handlers

BOOL CPlanStepPastePositionDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    GetDlgItem(IDC_PASTE_AFTER_SELECTION_RADIO)->EnableWindow(!m_endStepSelected);
    CheckDlgButton((m_endStepSelected? IDC_PASTE_BEFORE_SELECTION_RADIO : IDC_PASTE_AFTER_SELECTION_RADIO), BST_CHECKED);

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepPastePositionDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // TODO: Add extra validation here
    m_pastePosition = IsDlgButtonChecked(IDC_PASTE_BEFORE_SELECTION_RADIO)? PPT_PASTE_BEFORE : PPT_PASTE_AFTER;

    CDialog::OnOK();

    FUNCTION_EXIT;
}


