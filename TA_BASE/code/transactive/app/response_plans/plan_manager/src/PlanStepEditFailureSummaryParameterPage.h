//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditFailureSummaryParameterPage.h $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>


#if !defined(AFX_PLANSTEPEDITFAILURESUMMARYPARAMETERPAGE_H__12C1A40A_6BB7_43D4_93D4_91C2CA4653E9__INCLUDED_)
#define AFX_PLANSTEPEDITFAILURESUMMARYPARAMETERPAGE_H__12C1A40A_6BB7_43D4_93D4_91C2CA4653E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanStepEditFailureSummaryParameterPage.h : header file
//

#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)

#include "PlanStepEditParameterPage.h"


namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditFailureSummaryParameterPage dialog
    class FailureSummaryStep;

    class CPlanStepEditFailureSummaryParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditFailureSummaryParameterPage)

        // Construction
    public:
        CPlanStepEditFailureSummaryParameterPage(FailureSummaryStep* step);
        ~CPlanStepEditFailureSummaryParameterPage() {}

        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    // Dialog Data
    private:
        // Dialog Data
        // {{AFX_DATA(CPlanStepEditFailureSummaryParameterPage)
    enum { IDD = IDD_PLAN_STEP_EDIT_FAILURE_SUMMARY_DLG };
    int        m_failureStepCategory;
    // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditFailureSummaryParameterPage)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditFailureSummaryParameterPage)
    virtual BOOL OnInitDialog();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        enum { UNDEFINED_STEPS = -1,
               ALL_STEPS = 0,
               BRC_STEPS_ONLY = 1};
    };
}
#endif //STEPTYPE_FAILURE_SUMMARY_STEP

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANSTEPEDITFAILURESUMMARYPARAMETERPAGE_H__12C1A40A_6BB7_43D4_93D4_91C2CA4653E9__INCLUDED_)
