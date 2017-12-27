#if !defined(AFX_STEPPARAMETERDLG_H__8701B863_7646_4059_8250_86E3BE8F5996__INCLUDED_)
#define AFX_STEPPARAMETERDLG_H__8701B863_7646_4059_8250_86E3BE8F5996__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StepParameterDlg.h : header file
//

#include "PvCommonDefs.h"


/////////////////////////////////////////////////////////////////////////////
// CStepParameterDlg dialog

class CStepParameterDlg : public CDialog
{
// Construction
public:
    CStepParameterDlg(PV::StepParameter *param, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    // {{AFX_DATA(CStepParameterDlg)
    enum { IDD = IDD_STEPPARAM_DIALOG };
    CComboBox    m_paramValueCombo;
    CEdit    m_paramValueEdit;
    CEdit    m_paramNameEdit;
    // }}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CStepParameterDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    PV::StepParameter *m_param;

    // Generated message map functions
    // {{AFX_MSG(CStepParameterDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEPPARAMETERDLG_H__8701B863_7646_4059_8250_86E3BE8F5996__INCLUDED_)
