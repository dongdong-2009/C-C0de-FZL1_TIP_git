//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterPlanPathEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERPLANPATHEDITDLG_H__41AE5E17_5935_4507_AC2A_4973278C59E7__INCLUDED_)
#define AFX_PLANSTEPPARAMETERPLANPATHEDITDLG_H__41AE5E17_5935_4507_AC2A_4973278C59E7__INCLUDED_

#include "bus/response_plans/plan_tree/src/PlanTreeCtrl.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStepListCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class PlanPathStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterPlanPathEditDlg dialog

    class CPlanStepParameterPlanPathEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterPlanPathEditDlg(PlanPathStepParameter& parameter,bool canEdit);
        ~CPlanStepParameterPlanPathEditDlg();

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterPlanPathEditDlg)
        enum { IDD = IDD_PLAN_STEP_PARAMETER_PLAN_PATH_EDIT_DLG };
        CString    m_planPath;
        // }}AFX_DATA
        TA_Base_Bus::CPlanTreeCtrl* m_planTree;


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterPlanPathEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterPlanPathEditDlg)
        afx_msg void OnPlanTreeItemSelected(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}; // Namespace

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANSTEPPARAMETERPLANPATHEDITDLG_H__41AE5E17_5935_4507_AC2A_4973278C59E7__INCLUDED_)
