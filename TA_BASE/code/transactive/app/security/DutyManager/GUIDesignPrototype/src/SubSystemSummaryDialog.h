/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SubSystemSummaryDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_SUBSYSTEMSUMMARYDIALOG_H__82A7C532_7650_4CA2_8F3F_8E2B319382BF__INCLUDED_)
#define AFX_SUBSYSTEMSUMMARYDIALOG_H__82A7C532_7650_4CA2_8F3F_8E2B319382BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubSystemSummaryDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubSystemSummaryDialog dialog

class CSubSystemSummaryDialog : public CDialog
{
// Construction
public:
	CSubSystemSummaryDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSubSystemSummaryDialog)
	enum { IDD = IDD_SUBSYSTEMSUMMARY };
	CListCtrl	m_summaryList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubSystemSummaryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSubSystemSummaryDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGiveDuty();
	afx_msg void OnTakeDuty();
	//}}AFX_MSG


	HICON   m_tickIcon;
	HICON   m_crossIcon;
	CImageList  m_imageList;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBSYSTEMSUMMARYDIALOG_H__82A7C532_7650_4CA2_8F3F_8E2B319382BF__INCLUDED_)
