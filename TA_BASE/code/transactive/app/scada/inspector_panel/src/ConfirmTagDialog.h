#if !defined(AFX_CONFIRMTAGDIALOG_H__6AEDBB05_7070_11D7_B170_0050BAB1D931__INCLUDED_)
#define AFX_CONFIRMTAGDIALOG_H__6AEDBB05_7070_11D7_B170_0050BAB1D931__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmTagDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which confirms that the tag is to be updated.
  * It provides details of the tag and the new setting.
  * The return from DoModal() is used to indicate if the update should proceed (IDOK) or
  * be cancelled (IDCANCEL).
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfirmTagDialog.h : header file
//

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CConfirmTagDialog dialog

	class CConfirmTagDialog : public CDialog
	{
		// Construction
		public:
			CConfirmTagDialog(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CConfirmTagDialog)
		enum { IDD = IDD_CONFIRM_TAG_DIALOG };
		CString	m_equipment;
		CString	m_tag;
		CString	m_inhibit;
		//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CConfirmTagDialog)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CConfirmTagDialog)
				// NOTE: the ClassWizard will add member functions here
			afx_msg void OnHelpButton(); //TD15124 azenitha
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_CONFIRMTAGDIALOG_H__6AEDBB05_7070_11D7_B170_0050BAB1D931__INCLUDED_)
