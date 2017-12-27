//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterStepNumberEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERSTEPNUMBEREDITDLG_H__82599E1A_18A1_4724_9215_FED8D062A8F6__INCLUDED_)
#define AFX_PLANSTEPPARAMETERSTEPNUMBEREDITDLG_H__82599E1A_18A1_4724_9215_FED8D062A8F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanStepParameterEditDlg.h"
#include "PlanStepListCtrl.h"


namespace TA_Base_App
{
    class StepNumberStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterStepNumberEditDlg dialog

    class CPlanStepParameterStepNumberEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterStepNumberEditDlg(StepNumberStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterStepNumberEditDlg)
        enum { IDD = IDD_PLAN_STEP_PARAMETER_STEP_NUMBER_EDIT_DLG };
        CPlanStepListCtrl    m_stepList;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterStepNumberEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterStepNumberEditDlg)
    virtual BOOL OnInitDialog();
    afx_msg void onOK();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
} // End namespace
#endif // !defined(AFX_PLANSTEPPARAMETERSTEPNUMBEREDITDLG_H__82599E1A_18A1_4724_9215_FED8D062A8F6__INCLUDED_)
