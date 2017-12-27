//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterTextualEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"
#include "planmanager.h"
#include "PlanStepParameterTextualEditDlg.h"
#include "TextualStepParameter.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameterTextualEditDlg dialog

CPlanStepParameterTextualEditDlg::CPlanStepParameterTextualEditDlg(TextualStepParameter& parameter,bool canEdit)
    : CPlanStepParameterEditDlg(parameter,CPlanStepParameterTextualEditDlg::IDD,canEdit)
{
    FUNCTION_ENTRY( "CPlanStepParameterTextualEditDlg" );
    FUNCTION_EXIT;
}


void CPlanStepParameterTextualEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterTextualEditDlg)
    DDX_Control(pDX, IDC_PARAMETER_VALUE, m_textValue);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterTextualEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterTextualEditDlg)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterTextualEditDlg message handlers


BOOL CPlanStepParameterTextualEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    TextualStepParameter* textualParameter = dynamic_cast<TextualStepParameter*>(GetStepParameter());
    TA_ASSERT(textualParameter!=NULL,"Not a textual step parameter");

    m_textValue.SetWindowText(textualParameter->getMessageText().c_str());

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterTextualEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    CString text;
    m_textValue.GetWindowText(text);

    TextualStepParameter* textualParameter = dynamic_cast<TextualStepParameter*>(GetStepParameter());
    TA_ASSERT(textualParameter!=NULL,"Not a textual step parameter");
    textualParameter->setText((LPCSTR)text);

    // Normal exit
    EndDialog(IDOK);

    FUNCTION_EXIT;
}


