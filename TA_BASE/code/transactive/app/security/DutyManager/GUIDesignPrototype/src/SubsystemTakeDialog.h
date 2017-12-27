/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SubsystemTakeDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_SUBSYSTEMTAKEDIALOG_H__88336AB7_F173_4CA0_8901_218471EAD839__INCLUDED_)
#define AFX_SUBSYSTEMTAKEDIALOG_H__88336AB7_F173_4CA0_8901_218471EAD839__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubsystemTakeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubsystemTakeDialog dialog

class CSubsystemTakeDialog : public CDialog
{
// Construction
public:
	CSubsystemTakeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSubsystemTakeDialog)
	enum { IDD = IDD_SUBSYSTEMTAKE };
	CListCtrl	m_subsystemList;
	CComboBox	m_siteCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubsystemTakeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSubsystemTakeDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBSYSTEMTAKEDIALOG_H__88336AB7_F173_4CA0_8901_218471EAD839__INCLUDED_)
