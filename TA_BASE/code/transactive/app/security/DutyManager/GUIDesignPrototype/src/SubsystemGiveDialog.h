/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SubsystemGiveDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_SUBSYSTEMGIVEDIALOG_H__B730E375_EC97_499A_B0DE_613EC9DE6F7F__INCLUDED_)
#define AFX_SUBSYSTEMGIVEDIALOG_H__B730E375_EC97_499A_B0DE_613EC9DE6F7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubsystemGiveDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubsystemGiveDialog dialog

class CSubsystemGiveDialog : public CDialog
{
// Construction
public:
	CSubsystemGiveDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSubsystemGiveDialog)
	enum { IDD = IDD_SUBSYSTEMGIVE };
	CListCtrl	m_subsystemList;
	CComboBox  	m_siteCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubsystemGiveDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSubsystemGiveDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBSYSTEMGIVEDIALOG_H__B730E375_EC97_499A_B0DE_613EC9DE6F7F__INCLUDED_)
