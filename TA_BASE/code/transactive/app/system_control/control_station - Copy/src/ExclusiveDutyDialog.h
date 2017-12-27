/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ExclusiveDutyDialog.h $
  * @author:  Azenith Arcilla
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A dialog which lists the exclusive duties of the current session
  *
  */

#if !defined(AFX_EXCLUSIVEDUTYDIALOG_H__35F5FEF2_BEA5_444F_B075_8D880E6FF7D8__INCLUDED_)
#define AFX_EXCLUSIVEDUTYDIALOG_H__35F5FEF2_BEA5_444F_B075_8D880E6FF7D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExclusiveDutyDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ExclusiveDutyDialog dialog

class ExclusiveDutyDialog : public CDialog
{
// Construction
public:
	BOOL OnInitDialog();
	//ExclusiveDutyDialog(CWnd* pParent = NULL);   // standard constructor
	ExclusiveDutyDialog(CString subsystemsAtLocations, CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(ExclusiveDutyDialog)
	enum { IDD = IDD_EXCLUSIVE_DUTY_DIALOG };
	CRichEditCtrl	m_textCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ExclusiveDutyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ExclusiveDutyDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_detail;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXCLUSIVEDUTYDIALOG_H__35F5FEF2_BEA5_444F_B075_8D880E6FF7D8__INCLUDED_)
