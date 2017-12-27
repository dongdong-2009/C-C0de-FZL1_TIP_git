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
#include "app/response_plans/plan_manager/src/PlanStepParameterEditDlg.h"
#include "app/response_plans/plan_manager/src/PlanStepParameter.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{

const unsigned int WM_ADJUST_FONT = WM_USER + 0x1001;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterEditDlg dialog

CPlanStepParameterEditDlg::CPlanStepParameterEditDlg(PlanStepParameter& parameter,bool canEdit)
    : CDialog(CPlanStepParameterEditDlg::IDD, NULL), m_canEdit(canEdit),m_parameter(parameter)
{
    FUNCTION_ENTRY( "CPlanStepParameterEditDlg" );

    m_parameterName = parameter.getName().c_str();

    FUNCTION_EXIT;
}


// Called by derived classes to specify dialog template
CPlanStepParameterEditDlg::CPlanStepParameterEditDlg(PlanStepParameter& parameter,UINT templateID,bool canEdit)
    : CDialog(templateID, NULL), m_canEdit(canEdit),m_parameter(parameter)
{
    FUNCTION_ENTRY( "CPlanStepParameterEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterEditDlg)
    m_parameterName = parameter.getName().c_str();
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterEditDlg)
    DDX_Text(pDX, IDC_PARAMETER_NAME, m_parameterName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterEditDlg, CDialog)
    // {{AFX_MSG_MAP(CPlanStepParameterEditDlg)
    ON_MESSAGE_VOID(WM_ADJUST_FONT, OnAdjustFont)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterEditDlg message handlers

BOOL CPlanStepParameterEditDlg::OnInitDialog()
{
    PostMessage(WM_ADJUST_FONT, 0, 0);

    return CDialog::OnInitDialog();
}

void CPlanStepParameterEditDlg::OnAdjustFont()
{
    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );
}

}