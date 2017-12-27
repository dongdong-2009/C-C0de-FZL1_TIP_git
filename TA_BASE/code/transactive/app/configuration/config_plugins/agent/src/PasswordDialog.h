/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/PasswordDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#if !defined(AFX_PASSWORDDIALOG_H__02F3DDEB_C0F9_42E2_B004_E87882C09F30__INCLUDED_)
#define AFX_PASSWORDDIALOG_H__02F3DDEB_C0F9_42E2_B004_E87882C09F30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDialog.h : header file
//

#include "app/configuration/config_plugins/agent/src/resource.h"


/////////////////////////////////////////////////////////////////////////////
// PasswordDialog dialog
namespace TA_Base_App
{

	class PasswordDialog : public CDialog
	{
	// Construction
	public:
		PasswordDialog(CWnd* pParent = NULL);   // standard constructor

		/**
         * getPassword
         *
         * This should be called after the user has Ok'd the dialog to retrieve the new password
         *
         * @return CString - The password. This will return an empty string if no password is set.
         */
        CString getPassword() const
        {
            return m_password;
        }

	// Dialog Data
		//{{AFX_DATA(PasswordDialog)
		enum { IDD = IDD_PASSWORD_DIALOG };
		CEdit	m_password2;
		CEdit	m_password1;
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(PasswordDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(PasswordDialog)
	virtual void OnOK();
	afx_msg void OnChangeEditPassword1();
	afx_msg void OnChangeEditPassword2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		
		static const int MINIMUM_PASSWORD_SIZE;
		static const int PASSWORD_FIELD_LENGTH;

		CString m_password;
	};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDIALOG_H__02F3DDEB_C0F9_42E2_B004_E87882C09F30__INCLUDED_)
