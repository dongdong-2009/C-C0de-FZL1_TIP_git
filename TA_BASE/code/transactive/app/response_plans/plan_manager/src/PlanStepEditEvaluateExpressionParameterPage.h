//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditEvaluateExpressionParameterPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class facilitates editing of parameters associated with a Evaluate Expression step.

#if !defined(AFX_PLANSTEPEDITEVALUATEEXPRESSIONPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)
#define AFX_PLANSTEPEDITEVALUATEEXPRESSIONPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_EVALUATE_EXPRESSION)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "PlanStepEditParameterPage.h"
#include "StepNumberStepParameter.h"
#include "TextualStepParameter.h"

namespace TA_Base_App
{
    class EvaluateExpressionStep;
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditEvaluateExpressionPage dialog
    class CPlanStepEditEvaluateExpressionParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditEvaluateExpressionParameterPage)

    // Construction
    public:
        CPlanStepEditEvaluateExpressionParameterPage(EvaluateExpressionStep* step);

        ~CPlanStepEditEvaluateExpressionParameterPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
          virtual void DisplayParameters();
          virtual void UpdateParameters();

        void ResetSuccessAndFailureStepDisplay();


    // Attributes
    private:
        TextualStepParameter                    m_expressionValue;
        StepNumberStepParameter                m_successParameter;
        StepNumberStepParameter                m_failureParameter;

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditEvaluateExpressionPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_EXPRESSION_DLG };
        CEdit    m_expression;
        CString    m_failStepLabel;
        CString    m_successStepLabel;
        CString    m_dataPointLabel;
        CString    m_dataPointTypeLabel;
    // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditEvaluateExpressionPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
         virtual BOOL OnInitDialog ();
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditEvaluateExpressionPage)
        afx_msg void OnPickFailStepButton();
        afx_msg void OnPickSuccessStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // defined (STEPTYPE_EVALUATE_EXPRESSION)

#endif // !defined(AFX_PLANSTEPEDITEVALUATEEXPRESSIONPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)
