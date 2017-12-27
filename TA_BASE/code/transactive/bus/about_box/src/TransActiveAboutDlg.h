/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/about_box/src/TransActiveAboutDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_ABOUTDLG_H__14D3629B_67FA_4222_802E_B7212C18B935__INCLUDED_)
#define AFX_ABOUTDLG_H__14D3629B_67FA_4222_802E_B7212C18B935__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransActiveAboutDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTransActiveAboutDlg dialog

class CTransActiveAboutDlg : public CDialog
{
// Construction
public:
	CTransActiveAboutDlg(const char buildVersion[],	 const char buildDate[],
						 const char componentName[], const char componentVersion[],
						 const int headerArraySize,  char* headerArray[],
						 CWnd* pParent = NULL);   // Constructor

// Dialog Data
	//{{AFX_DATA(CTransActiveAboutDlg)
	enum { IDD = IDD_ABOUT_DLG };
	CAnimateCtrl	m_Movie;
	CString	m_AppName;
	CString	m_BuildTime;
	CString	m_RelNumber;
	CString	m_ProjectName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransActiveAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransActiveAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Used to paint the background white
	CBrush m_brush;

private:
	CString m_DialogTitle;
	const char* m_ComponentName;
	const char* m_AppVersion;
	int m_TimerID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDLG_H__14D3629B_67FA_4222_802E_B7212C18B935__INCLUDED_)

