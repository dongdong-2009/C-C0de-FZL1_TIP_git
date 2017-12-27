// StepParameterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "StepParameterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStepParameterDlg dialog


CStepParameterDlg::CStepParameterDlg(PV::StepParameter *param, CWnd* pParent /*=NULL*/)
    : CDialog(CStepParameterDlg::IDD, pParent), m_param(param)
{
    FUNCTION_ENTRY( "CStepParameterDlg" );

    // {{AFX_DATA_INIT(CStepParameterDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CStepParameterDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CStepParameterDlg)
    DDX_Control(pDX, IDC_PARAMVALUE_COMBO, m_paramValueCombo);
    DDX_Control(pDX, IDC_PARAMVALUE_EDIT, m_paramValueEdit);
    DDX_Control(pDX, IDC_PARAMNAME_EDIT, m_paramNameEdit);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CStepParameterDlg, CDialog)
    // {{AFX_MSG_MAP(CStepParameterDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStepParameterDlg message handlers

BOOL CStepParameterDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    m_paramNameEdit.SetWindowText(m_param->m_name.c_str());

    if (m_param->m_isSelection)
    {
        for (std::vector<std::string>::iterator iter = m_param->m_valueList.begin(); iter != m_param->m_valueList.end(); iter++)
        {
            std::string value = *iter;
            m_paramValueCombo.AddString(value.c_str());
        }
        m_paramValueCombo.SelectString(0, m_param->m_value.c_str());
        m_paramValueEdit.ShowWindow(SW_HIDE);
        m_paramValueCombo.SetFocus();
    }
    else
    {
        m_paramValueEdit.SetWindowText(m_param->m_value.c_str());
        m_paramValueCombo.ShowWindow(SW_HIDE);
        m_paramValueEdit.SetFocus();
    }

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CStepParameterDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // TODO: Add extra validation here
    CString paramValue;
    if (m_paramValueEdit.IsWindowVisible())
    {
        m_paramValueEdit.GetWindowText(paramValue);
    }
    else
    {
        m_paramValueCombo.GetLBText(m_paramValueCombo.GetCurSel(), paramValue);
    }

    m_param->m_value = (LPCTSTR)paramValue;

    CDialog::OnOK();

    FUNCTION_EXIT;
}


