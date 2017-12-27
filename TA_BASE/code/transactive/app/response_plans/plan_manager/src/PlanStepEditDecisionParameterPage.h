//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDecisionParameterPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPEDITDECISIONPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
#define AFX_PLANSTEPEDITDECISIONPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DECISION)

#include "PlanStepEditParameterPage.h"
#include "StepNumberStepParameter.h"


namespace TA_Base_App
{
    class DecisionStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditDecisionParameterPage dialog

    class CPlanStepEditDecisionParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditDecisionParameterPage)

    // Construction
    public:
        CPlanStepEditDecisionParameterPage(DecisionStep* parameter);
        ~CPlanStepEditDecisionParameterPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Attributes
    private:
        StepNumberStepParameter  m_noStepParameter;
        StepNumberStepParameter  m_yesStepParameter;


    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditDecisionParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditDecisionParameterPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnNoPickStepButton();
        afx_msg void OnYesPickStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDecisionParameterPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_DECISION_DLG };
        CString    m_decisionMessage;
        CSpinButtonCtrl    m_responseTimeoutSpin;
        UINT    m_responseTimeout;
        CString    m_yesStep;
        CString    m_noStep;
        // lizettejl++ (DP-changes)
        CString m_yesBtnLabel;
        CString m_noBtnLabel;
        CString m_pauseBtnLabel;
        BOOL m_noBtnEnabled;
        BOOL m_pauseBtnEnabled;
        // }}AFX_DATA
    };
}

#endif // defined (STEPTYPE_DECISION)

#endif // !defined(AFX_PLANSTEPEDITDECISIONPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
