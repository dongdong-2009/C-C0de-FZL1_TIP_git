/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SiteSummaryDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_SITESUMMARYDIALOG_H__C33C158B_8844_4BE8_8366_669339594913__INCLUDED_)
#define AFX_SITESUMMARYDIALOG_H__C33C158B_8844_4BE8_8366_669339594913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SiteSummaryDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSiteSummaryDialog dialog

class CSiteSummaryDialog : public CDialog
{
// Construction
public:
	CSiteSummaryDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSiteSummaryDialog)
	enum { IDD = IDD_SITESUMMARY };
	CListCtrl	m_summaryList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSiteSummaryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSiteSummaryDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelegate();
	afx_msg void OnTakeDuty();
	//}}AFX_MSG

	CImageList     m_imageList;
	HICON          m_OCCIcon;
	HICON          m_PSCIcon;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SITESUMMARYDIALOG_H__C33C158B_8844_4BE8_8366_669339594913__INCLUDED_)
