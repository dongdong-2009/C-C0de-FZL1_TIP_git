/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/SummaryDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_SUMMARYDIALOG_H__F44D6388_A254_478F_A1F7_30E99A36B7F0__INCLUDED_)
#define AFX_SUMMARYDIALOG_H__F44D6388_A254_478F_A1F7_30E99A36B7F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SummaryDialog.h : header file
//
#include "resource.h"
#include "SummaryListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSummaryDialog dialog

class CSummaryDialog : public CDialog
{
// Construction
public:
	CSummaryDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSummaryDialog)
	enum { IDD = IDD_SUMMARY_DLG };
	CSummaryListCtrl	m_SummaryListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSummaryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSummaryDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUMMARYDIALOG_H__F44D6388_A254_478F_A1F7_30E99A36B7F0__INCLUDED_)
