/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/AddProcessDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This dialog is used to add one or more processes to a System Controller.
  */

#if !defined(AFX_ADDPROCESSDIALOG_H__385FD9D0_41C4_452B_B9B8_02562927B759__INCLUDED_)
#define AFX_ADDPROCESSDIALOG_H__385FD9D0_41C4_452B_B9B8_02562927B759__INCLUDED_

#include <string>
#include <vector>

#include "app/configuration/config_plugins/system_controller/src/resource.h"

#include "core/data_access_interface/system_controller/src/IConfigSystemController.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    class AddProcessDialog : public CDialog
    {
    // Construction
    public:
        /**
         * Constructor
         *
         * @param IConfigSystemController& - This is used to retrieve what managed processes the current
         *                                   system controller is already using. The user should not be able
         *                                   to add one twice.
         */
        AddProcessDialog(TA_Base_Core::IConfigSystemController& currentSystemController);   // standard constructor


        struct Process
        {
            unsigned long key;           // A Process entity key from the database
            std::string name;            // The name of the process to display to the user
                                         // The default operating mode of the process
            TA_Base_Core::IConfigSystemController::OperatingMode operatingMode; 
        };


        std::vector<Process>& getProcessesToAdd()
        {
            return m_processes;
        }


    // Dialog Data
	    //{{AFX_DATA(AddProcessDialog)
	enum { IDD = IDD_ADD_PROCESS_DIALOG };
	CListCtrl	m_processList;
    CButton     m_control;
    CButton     m_monitor;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AddProcessDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(AddProcessDialog)
	    virtual BOOL OnInitDialog();
	    afx_msg void onOK();
	afx_msg void onDblclkProcessList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onItemchangedProcessList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        std::vector<Process> m_processes;

        TA_Base_Core::IConfigSystemController& m_currentSystemController;
    };
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDPROCESSDIALOG_H__385FD9D0_41C4_452B_B9B8_02562927B759__INCLUDED_)
