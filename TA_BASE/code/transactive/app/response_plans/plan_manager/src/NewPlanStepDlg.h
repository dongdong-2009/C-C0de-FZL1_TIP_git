/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/NewPlanStepDlg.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents a simple dialog displayed to the operator when creating new steps within a plan.
  * It enables the operator to pick the step's type and insert position wrt currently selected step(s),
  * i.e. before or after. If the currently selected step is the End Step, the insert after option is disabled.
  *
  **/

#if !defined(AFX_NEWPLANSTEPDLG_H__95629202_DF3A_49D6_84D1_11A05E9AE330__INCLUDED_)
#define AFX_NEWPLANSTEPDLG_H__95629202_DF3A_49D6_84D1_11A05E9AE330__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CNewPlanStepDlg dialog

    class CNewPlanStepDlg : public CDialog
    {
    // Public Types
    public:
        enum EInsertPositionType
        {
            IPT_INSERT_BEFORE,
            IPT_INSERT_AFTER
        };

    // Construction/destruction
    public:
        CNewPlanStepDlg(bool endStepSelected, CWnd* pParent = NULL);
        ~CNewPlanStepDlg();

    // Operations
    public:
        EInsertPositionType getInsertPosition() { return m_insertPosition; }
        TA_Base_Core::EStep getStepType()       { return m_selectedStepType; }

    // Attributes
    private:
        bool m_endStepSelected;
        EInsertPositionType m_insertPosition;
        TA_Base_Core::EStep m_selectedStepType;


    // Dialog Data
    private:
        // {{AFX_DATA(CNewPlanStepDlg)
        enum { IDD = IDD_PLAN_STEP_NEW_DLG };
        CComboBox    m_stepTypeCombo;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CNewPlanStepDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CNewPlanStepDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_NEWPLANSTEPDLG_H__95629202_DF3A_49D6_84D1_11A05E9AE330__INCLUDED_)
