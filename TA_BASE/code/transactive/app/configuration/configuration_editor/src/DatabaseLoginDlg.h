/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/DatabaseLoginDlg.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the dialog used to allow the user to enter their login details to connect directly to a database.
  */


#if !defined(AFX_DATABASELOGINDLG_H__5524C4AC_1C0F_403B_B86F_BED18EB68CB2__INCLUDED_)
#define AFX_DATABASELOGINDLG_H__5524C4AC_1C0F_403B_B86F_BED18EB68CB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/configuration_editor/src/resource.h"

namespace TA_Base_App
{
    class DatabaseLoginDlg : public CDialog
    {
    // Construction
    public:
        /**
          * Constructor
          *
          * @param CString - The reason message to display at the top of the dialog
          */
	    DatabaseLoginDlg(const CString& reasonMessage);   // standard constructor


    // Dialog Data
	    //{{AFX_DATA(DatabaseLoginDlg)
	enum { IDD = IDD_DATABASELOGIN_DIALOG };
	CEdit	m_user;
	CEdit	m_password;
	CEdit	m_database;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(DatabaseLoginDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
    	HICON m_hIcon;

	    // Generated message map functions
	    //{{AFX_MSG(DatabaseLoginDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void onChangeDatabase();
	    afx_msg void onChangePassword();
	    afx_msg void onChangeUsername();
	    afx_msg void onOK();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        /**
          * refresh
          *
          * This checks the status of the controls and enables or disables the OK button.
          */
        void refresh();

        
        CString m_reasonMessage; // This is the reason message to show at the top of the dialog to explain
                                 // why the user must login like this
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_DATABASELOGINDLG_H__5524C4AC_1C0F_403B_B86F_BED18EB68CB2__INCLUDED_)
