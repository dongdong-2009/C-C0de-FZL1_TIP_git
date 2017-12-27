/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/ChangePlanDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to change a plan.
  */

#if !defined(AFX_CHANGEPLANDIALOG_H__E80F44CC_671E_4390_B499_6EFF3C2F5266__INCLUDED_)
#define AFX_CHANGEPLANDIALOG_H__E80F44CC_671E_4390_B499_6EFF3C2F5266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/event_plan_association/src/resource.h"

#define IMPORT_EXTENSIONS
#include "bus/response_plans/plan_tree/src/PlanTreeCtrl.h"

namespace TA_Base_App
{
    class ChangePlanDialog : public CDialog
    {
    // Construction
    public:

        /**
         * Constructor
         *
         * @param const std::string& - This is the full path of the plan the user currently has selected so we can
         *                             set the current selection in the tree control to be this string.
         * @param CWnd* - The parent of this dialog
         */
	    ChangePlanDialog(const std::string& currentPlanPath, CWnd* pParent = NULL);   // standard constructor


        /**
         * getSelectedPlanPath
         *
         * After the user has selected Ok on the dialog and a response has been returned to the 
         * caller then it can call this to retrieve the path of the plan the user selected.
         *
         * @return CString - The path of the plan the user selected in the plan tree.
         */
        CString getSelectedPlanPath() const
        {
            return m_planPath;
        }

       
    // Dialog Data
	    //{{AFX_DATA(ChangePlanDialog)
	    enum { IDD = IDD_CHANGE_PLAN_DIALOG };
	    //}}AFX_DATA
	    TA_Base_Bus::CPlanTreeCtrl	m_planTree;


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ChangePlanDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(ChangePlanDialog)
	    afx_msg void onOK();
	    virtual BOOL OnInitDialog();
	    afx_msg void onSelchangedPlanTree(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void onDblclkPlanTree(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

        // This is the full path of the plan the user selected. This will be set when the
        // user selects Ok.
        CString m_planPath;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_CHANGEPLANDIALOG_H__E80F44CC_671E_4390_B499_6EFF3C2F5266__INCLUDED_)
