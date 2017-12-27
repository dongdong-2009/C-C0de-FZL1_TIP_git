/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmAdjustDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_CONFIRMADJUSTDIALOG_H__90B74E87_58BF_4AED_B13F_F814C7A41133__INCLUDED_)
#define AFX_CONFIRMADJUSTDIALOG_H__90B74E87_58BF_4AED_B13F_F814C7A41133__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfirmAdjustDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfirmAdjustDialog dialog

class CConfirmAdjustDialog : public CDialog
{
// Construction
public:
	CConfirmAdjustDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmAdjustDialog)
	enum { IDD = IDD_CONFIRM_ADJUST_DIALOG };
	CString	m_equipment;
	CString	m_dataPoint;
	CString	m_newValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmAdjustDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmAdjustDialog)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnHelpButton(); //TD15124 azenitha
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIRMADJUSTDIALOG_H__90B74E87_58BF_4AED_B13F_F814C7A41133__INCLUDED_)
