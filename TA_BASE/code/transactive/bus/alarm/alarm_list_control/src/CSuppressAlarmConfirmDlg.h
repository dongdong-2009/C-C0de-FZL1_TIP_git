/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/CSuppressAlarmConfirmDlg.h $
 * @author:  Derrick Liew
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the user to confirm the create of inhibition rule for system alarm.
 */

#if !defined(AFX_CSUPPRESSALARMCONFIRMDLG_H__67B8673B_6527_4D84_A34E_2326556313C5__INCLUDED_)
#define AFX_CSUPPRESSALARMCONFIRMDLG_H__67B8673B_6527_4D84_A34E_2326556313C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <string>

// CSuppressAlarmConfirmDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSuppressAlarmConfirmDlg dialog
namespace TA_Base_Bus
{

	class CSuppressAlarmConfirmDlg : public CDialog
	{
	// Construction
	public:
		CSuppressAlarmConfirmDlg (CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
		//{{AFX_DATA(CSuppressAlarmConfirmDlg)
		enum { IDD = IDD_INHIBIT_ALARM_CONFIRMATION };
		CString	m_confirmationText;
		//}}AFX_DATA

		void setConfirmationText(std::string confirmText);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSuppressAlarmConfirmDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CSuppressAlarmConfirmDlg)
		virtual void OnCancel();
		virtual void OnOK();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	public:
		virtual BOOL OnInitDialog();
		


	private:
		CFont m_font;
	};
} // end of namespace

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSUPPRESSALARMCONFIRMDLG_H__67B8673B_6527_4D84_A34E_2326556313C5__INCLUDED_)
