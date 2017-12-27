/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/PlanAssociationDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class holds the plan association dialog that allows the user to run an associated
  * plan for an alarm
  */

#if !defined(AFX_PLANASSOCIATIONDIALOG_H__1F315645_BB40_493F_A637_F73C99D309F7__INCLUDED_)
#define AFX_PLANASSOCIATIONDIALOG_H__1F315645_BB40_493F_A637_F73C99D309F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/alarm_list_control/src/resource.h"

#include "core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h"

namespace TA_Base_Bus
{
    class PlanAssociationDialog : public CDialog
    {
    // Construction
    public:
        /**
         * Constructor
         *
         * @param PlanDetailsList& - This is a list of all plans including their id and name
         * @param CString - This is the name of the task being performed (eg run, postpone the running of) so
         *                  that it can be added to message boxes.
         */
        PlanAssociationDialog(TA_Base_Core::PlanDetailsList& plans,
                              CWnd* pParent = NULL);   // standard constructor



        /**
         * getSelectedPlanPath
         *
         * This will return the full path of plan the user selected. It will be set after OnOK has been called
         *
         * @return std::string - The path of the selected plan
         */
        std::string getSelectedPlanPath() const
        {
            return m_selectedPlanPath;
        }


    // Dialog Data
	    //{{AFX_DATA(PlanAssociationDialog)
	    enum { IDD = IDD_ALARMPLANASSOC };
	    CButton	m_okButton;
	    CListBox	m_planList;
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(PlanAssociationDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(PlanAssociationDialog)
	    virtual BOOL OnInitDialog();
	    afx_msg void onPlanlistboxOk();
	    afx_msg void onSelchangeAlarmplanlistbox();
	    afx_msg void onPlanlistboxCancel();
	    afx_msg void onDblclkAlarmplanlistbox();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:
        TA_Base_Core::PlanDetailsList& m_plans;

        std::string m_selectedPlanPath;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_PLANASSOCIATIONDIALOG_H__1F315645_BB40_493F_A637_F73C99D309F7__INCLUDED_)
