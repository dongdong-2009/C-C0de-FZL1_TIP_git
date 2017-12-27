//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditBackgroundDecisionParameterPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(_PLANSTEPEDIT_BACKGROUND_DECISIONPARAMETERPAGE_H__)
#define _PLANSTEPEDIT_BACKGROUND_DECISIONPARAMETERPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_DECISION)

#include "PlanStepEditParameterPage.h"
#include "StepNumberStepParameter.h"
#include "NumericStepParameter.h"

namespace TA_Base_App
{
    class BackgroundDecisionStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditBackgroundDecisionParameterPage dialog

    class CPlanStepEditBackgroundDecisionParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditBackgroundDecisionParameterPage)

        // Construction
    public:
        CPlanStepEditBackgroundDecisionParameterPage(BackgroundDecisionStep* parameter);
        ~CPlanStepEditBackgroundDecisionParameterPage();


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
        // {{AFX_VIRTUAL(CPlanStepEditBackgroundDecisionParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditBackgroundDecisionParameterPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnNoPickStepButton();
        afx_msg void OnYesPickStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        NumericStepParameter m_operatorProfileParameter;

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditBackgroundDecisionParameterPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_BACKGROUND_DECISION_DLG };
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
        CComboBox    m_operatorProfileCombo;
        // }}AFX_DATA
    };
}

#endif // defined (STEPTYPE_BACKGROUND_DECISION)

#endif // !defined(_PLANSTEPEDIT_BACKGROUND_DECISIONPARAMETERPAGE_H__)
