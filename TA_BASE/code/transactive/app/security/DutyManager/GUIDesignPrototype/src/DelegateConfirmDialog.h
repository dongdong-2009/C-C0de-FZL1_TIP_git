/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/DelegateConfirmDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_DELEGATECONFIRMDIALOG_H__9ABE689C_DC93_45FC_836D_FC90451B45A3__INCLUDED_)
#define AFX_DELEGATECONFIRMDIALOG_H__9ABE689C_DC93_45FC_836D_FC90451B45A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelegateConfirmDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDelegateConfirmDialog dialog

class CDelegateConfirmDialog : public CDialog
{
// Construction
public:
	CDelegateConfirmDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelegateConfirmDialog)
	enum { IDD = IDD_DELEGATECONFIRM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelegateConfirmDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelegateConfirmDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELEGATECONFIRMDIALOG_H__9ABE689C_DC93_45FC_836D_FC90451B45A3__INCLUDED_)
