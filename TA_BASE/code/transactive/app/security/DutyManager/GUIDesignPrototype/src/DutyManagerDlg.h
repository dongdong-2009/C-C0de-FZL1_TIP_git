/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/DutyManagerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// DutyManagerDlg.h : header file
//

#if !defined(AFX_DUTYMANAGERDLG_H__F86D73D9_BF6A_4EF6_8F06_E15AC96DA0ED__INCLUDED_)
#define AFX_DUTYMANAGERDLG_H__F86D73D9_BF6A_4EF6_8F06_E15AC96DA0ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MainTab.h"


/////////////////////////////////////////////////////////////////////////////
// CDutyManagerDlg dialog

class CDutyManagerDlg : public CDialog
{
// Construction
public:
	CDutyManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDutyManagerDlg)
	enum { IDD = IDD_DUTYMANAGER_DIALOG };
	CMainTab	m_MainTab;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDutyManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDutyManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUTYMANAGERDLG_H__F86D73D9_BF6A_4EF6_8F06_E15AC96DA0ED__INCLUDED_)
