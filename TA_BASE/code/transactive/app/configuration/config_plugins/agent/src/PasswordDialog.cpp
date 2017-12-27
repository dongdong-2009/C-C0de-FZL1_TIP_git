/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/PasswordDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

// PasswordDialog.cpp : implementation file
//

#include "app/configuration/config_plugins/agent/src/StdAfx.h"
#include "app/configuration/config_plugins/agent/src/PasswordDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PasswordDialog dialog

namespace TA_Base_App
{
	const int PasswordDialog::MINIMUM_PASSWORD_SIZE = 0;
    const int PasswordDialog::PASSWORD_FIELD_LENGTH = 50;

	PasswordDialog::PasswordDialog(CWnd* pParent /*=NULL*/)
		: CDialog(PasswordDialog::IDD, pParent)
	{
		//{{AFX_DATA_INIT(PasswordDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void PasswordDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(PasswordDialog)
		DDX_Control(pDX, IDC_EDIT_PASSWORD2, m_password2);
		DDX_Control(pDX, IDC_EDIT_PASSWORD1, m_password1);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(PasswordDialog, CDialog)
		//{{AFX_MSG_MAP(PasswordDialog)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD1, OnChangeEditPassword1)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD2, OnChangeEditPassword2)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	BOOL PasswordDialog::OnInitDialog() 
	{	
		CDialog::OnInitDialog();
		
		// TODO: Add extra initialization here
        GetDlgItem(IDOK)->EnableWindow(FALSE);	    

        m_password1.SetLimitText(PASSWORD_FIELD_LENGTH);
        m_password2.SetLimitText(PASSWORD_FIELD_LENGTH);	

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	/////////////////////////////////////////////////////////////////////////////
	// PasswordDialog message handlers

	void PasswordDialog::OnOK() 
	{
		// TODO: Add extra validation here
        FUNCTION_ENTRY("onOK");

        CString password1;
        CString password2;

        m_password1.GetWindowText(password1);
        m_password2.GetWindowText(password2);

        if( 0 != password1.Compare(password2) )
        {
            // The passwords are not the same
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020045);

            // Now blank the entered values and move the cursor back to the first password field
            m_password1.SetWindowText("");
            m_password2.SetWindowText("");
            GotoDlgCtrl( GetDlgItem(IDC_EDIT_PASSWORD1) );

            FUNCTION_EXIT;
            return;
		}

		if (password1.GetLength() < MINIMUM_PASSWORD_SIZE)
        {
            // The passwords must be greater than 8 characters
            CString password;
            password.LoadString(IDS_PASSWORD);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << password << MINIMUM_PASSWORD_SIZE;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020055);

            // Now blank the entered values and move the cursor back to the first password field
            m_password1.SetWindowText("");
            m_password2.SetWindowText("");
            GotoDlgCtrl( GetDlgItem(IDC_EDIT_PASSWORD1) );

            FUNCTION_EXIT;
            return;
        }

        m_password = password1;
		FUNCTION_EXIT;
		CDialog::OnOK();
	}

	void PasswordDialog::OnChangeEditPassword1() 
	{
		// TODO: If this is a RICHEDIT control, the control will not
		// send this notification unless you override the C???::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.
		
		// TODO: Add your control notification handler code here
        CString password1;
        CString password2;

        m_password1.GetWindowText(password1);
        m_password2.GetWindowText(password2);

        if (password1.IsEmpty() || password2.IsEmpty())
        {
            GetDlgItem(IDOK)->EnableWindow(FALSE);
        }
        else
        {
            GetDlgItem(IDOK)->EnableWindow(TRUE);
        }		
	}

	void PasswordDialog::OnChangeEditPassword2() 
	{
		// TODO: If this is a RICHEDIT control, the control will not
		// send this notification unless you override the C???::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.
		
		// TODO: Add your control notification handler code here
		CString password1;
        CString password2;

        m_password1.GetWindowText(password1);
        m_password2.GetWindowText(password2);

        if (password1.IsEmpty() || password2.IsEmpty())
        {
            GetDlgItem(IDOK)->EnableWindow(FALSE);
        }
        else
        {
            GetDlgItem(IDOK)->EnableWindow(TRUE);
        }
	}
}


