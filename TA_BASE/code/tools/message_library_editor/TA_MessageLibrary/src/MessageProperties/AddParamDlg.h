#if !defined(AFX_ADDPARAMDLG_H__B75D8B1A_7262_4032_8E86_E9E21B294B05__INCLUDED_)
#define AFX_ADDPARAMDLG_H__B75D8B1A_7262_4032_8E86_E9E21B294B05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddParamDlg.h : header file
//

#include "..\SizingDialog\cdxCDynamicDialog.h"

/////////////////////////////////////////////////////////////////////////////
// AddParamDlg dialog

class AddParamDlg : public cdxCDynamicDialog
{
// Construction
public:
	AddParamDlg(CWnd* pParent, CStringArray* pMsgParams);

// Dialog Data
	//{{AFX_DATA(AddParamDlg)
	enum { IDD = IDD_ADDPARAMDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CListCtrl m_listParams;
	CStringArray* m_pMsgParams;
	HICON m_hIcon;

	bool CheckIfParamInMessage(LPCTSTR pszParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AddParamDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEditParameters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool PopulateListCtrl();
	bool InitialiseListCtrl();
	bool SaveSelection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDPARAMDLG_H__B75D8B1A_7262_4032_8E86_E9E21B294B05__INCLUDED_)
