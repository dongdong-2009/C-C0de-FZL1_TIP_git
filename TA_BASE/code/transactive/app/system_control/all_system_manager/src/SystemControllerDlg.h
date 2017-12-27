/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemControllerDlg.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  * 
  * This is the dialog used to allow the user to enter the System Controller host name and
  * port number.
  */

#if !defined(AFX_SYSTEMCONTROLLERDLG_H__54994F59_29DF_4CB4_9708_38E805786495__INCLUDED_)
#define AFX_SYSTEMCONTROLLERDLG_H__54994F59_29DF_4CB4_9708_38E805786495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "resource.h"

namespace TA_Base_App
{
    class SystemControllerDlg : public CDialog
    {
    public:
        /**
          * Constructor
          */
	    SystemControllerDlg(CWnd* pParent = NULL);   // standard constructor

        /**
          * Destructor
          */
        virtual ~SystemControllerDlg(){ };

		/**
		 * getHostnames
		 *
		 * This gets the host names
		 */
		std::vector<std::string> getHostnames();

    // Dialog Data
	    //{{AFX_DATA(SystemControllerDlg)
		enum { IDD = IDD_SYSTEMCONTROLLER_DIALOG };
		CEdit	m_hostnameEdit;
	    CEdit	m_portNumber;
	    CComboBox	m_hostnameCombo;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(SystemControllerDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
    	HICON m_hIcon;

	    // Generated message map functions
	    //{{AFX_MSG(SystemControllerDlg)
	    afx_msg void onOK();
		virtual BOOL OnInitDialog();
	    afx_msg void onChangePortNumber();
		afx_msg void OnSelectHostName();
		afx_msg void OnChangeHostNameEdit();
	    afx_msg void OnDropdownHostName();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:

        /**
         * refresh
         *
         * This checks the status of the controls and enables or disables the OK button.
         */
        void refresh();

		/**
		 * populateHostnamesCombo
		 *
		 * This populates the hostname combobox with host from the database
		 */
		void populateHostnamesCombo();

		/**
		 * populateHostnames
		 *
		 * This populates the host names with host from the database
		 */
		void populateHostnames();

		std::vector<std::string> m_vectorOfHostnames;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_SYSTEMCONTROLLERDLG_H__54994F59_29DF_4CB4_9708_38E805786495__INCLUDED_)
