/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/NotesDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_NOTESDIALOG_H__EF9B4082_E73A_41B6_9D03_CB711A561E20__INCLUDED_)
#define AFX_NOTESDIALOG_H__EF9B4082_E73A_41B6_9D03_CB711A561E20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NotesDialog.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CNotesDialog dialog

class CNotesDialog : public CDialog
{
// Construction
public:
	CNotesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNotesDialog)
	enum { IDD = IDD_NOTES_DLG };
	CEdit	m_NotesEdit;
	CButton	m_UpdateButton;
	CButton	m_ClearButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNotesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClearButton();
	afx_msg void OnUpdateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTESDIALOG_H__EF9B4082_E73A_41B6_9D03_CB711A561E20__INCLUDED_)
