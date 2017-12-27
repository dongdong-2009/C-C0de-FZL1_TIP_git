/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/PasswordDlg.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Dialog box which allows the current operator to change password
  */

#if !defined(AFX_PASSWORDDLG_H__69DD4959_81D2_449E_BC14_6CDA8330AD0A__INCLUDED_)
#define AFX_PASSWORDDLG_H__69DD4959_81D2_449E_BC14_6CDA8330AD0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDlg.h : header file
//

// Forward declare classes
namespace TA_Base_Core
{
    class IOperator;
    class IProfile;
	class ILocation;
}

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor


    /**
     * getSelectedDetails
     *
     * This is called to retrieve the details the user has entered into the password dialog.
     * This should be called after the user has clicked OK.
     * 
     * @param IOperator*& - A reference to an IOperator pointer. This is because we want to update
     *                      the pointer passed in to be the same as the operator pointer we have
     *                      stored. (This is an OUT parameter)
     * @param std::string& oldPassword - Old password that user has entered
	 * @param std::string& newPassword1 - New password that user has entered the first time
	 * @param std::string& newPassword2 - New password that user has entered the second time
     */
	void getSelectedDetails(
		TA_Base_Core::IOperator*& selectedOperator, CString& oldPassword, 
		CString& newPassword1, CString& newPassword2) const;


    /**
     * useExistingDataOnNextDoModal
     *
     * This method should be called if the next time the dialog is shown it should
     * automatically populate itself with the old password that was entered the
     * last time the dialog was shown
     */
    void useExistingDataOnNextDoModal()
    {
        m_useExistingData = true;
    }


// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_CHANGE_PASSWORD_DIALOG };
	CEdit	        m_oldPasswordEdit;
	CComboBox	    m_nameCombo;
	CEdit	        m_newPasswordEdit;
	CButton	        m_applyCtrl;
	CEdit	        m_confirmPasswordEdit;
	CAnimateCtrl	m_introMovie;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	afx_msg void OnChangeNewPasswordEdit();
	afx_msg void OnChangePasswordEdit();
	afx_msg void OnChangeConfirmPasswordEdit();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelpButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT OnCancel(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
    /**
     * populateNameComboBox
     *
     * This will populate the operator name combo box with a list of operator descriptions
     * retrieved from the SessionDetails object.
     */
    void populateNameComboBox();

    // old password string 
    CString m_oldPassword;

    // new password string entered the first time
    CString m_newPassword1;

    // new password string entered the second time
    CString m_newPassword2;

    // When the dialog is displayed to the user it may need to be displayed using the 
    // old password (when there is a mismatch in the new password).  We then
    // display the dialog with the old password previously typed in.
    bool m_useExistingData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__69DD4959_81D2_449E_BC14_6CDA8330AD0A__INCLUDED_)
