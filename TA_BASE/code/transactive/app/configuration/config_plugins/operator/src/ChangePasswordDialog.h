/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/ChangePasswordDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */


#if !defined(AFX_CHANGEPASSWORDDIALOG_H__DBDF47A8_F9D1_43F5_AFAB_28B151CE20B6__INCLUDED_)
#define AFX_CHANGEPASSWORDDIALOG_H__DBDF47A8_F9D1_43F5_AFAB_28B151CE20B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/operator/src/resource.h"

namespace TA_Base_App
{
    class ChangePasswordDialog : public CDialog
    {
    // Construction
    public:
	    ChangePasswordDialog(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
	    //{{AFX_DATA(ChangePasswordDialog)
	enum { IDD = IDD_PASSWORD_DIALOG };
	CEdit	m_password2;
	CEdit	m_password1;
	//}}AFX_DATA


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


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ChangePasswordDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(ChangePasswordDialog)
	    afx_msg void onOK();
	afx_msg void onChangeEditPassword1();
	afx_msg void onChangeEditPassword2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:
        static const int MINIMUM_PASSWORD_SIZE;
        static const int PASSWORD_FIELD_LENGTH;

        CString m_password;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_CHANGEPASSWORDDIALOG_H__DBDF47A8_F9D1_43F5_AFAB_28B151CE20B6__INCLUDED_)
