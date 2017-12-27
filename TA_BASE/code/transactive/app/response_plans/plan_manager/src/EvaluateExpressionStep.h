//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/EvaluateExpressionStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_EVALUATEEXPRESSIONSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_)
#define AFX_EVALUATEEXPRESSIONSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_EVALUATE_EXPRESSION)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class StepNumberStepParameter;
    class TextualStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // EvaluateExpressionStep

    class EvaluateExpressionStep : public PlanStep
    {
         // Operations
    public:
         EvaluateExpressionStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
         EvaluateExpressionStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
         virtual ~EvaluateExpressionStep();

         virtual PlanStep* clone();
         virtual bool isStepBranching() const { return true; }
         virtual bool canIgnoreFailure() const { return false; }

         TextualStepParameter* getEvaluateExpressionValueParameter () {return m_evaluateExpressionParameter;}
         StepNumberStepParameter*         getSuccessParameter() { return m_successParameter;}
         StepNumberStepParameter*         getFailParameter() { return m_failureParameter;}

    protected:
         EvaluateExpressionStep(const EvaluateExpressionStep &other);
         virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
         virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                            PlanStepParameterList &stepParameters);

         virtual void getUpdatedParameters(TA_Base_Core::Step& step);
         virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
         EvaluateExpressionStep &operator=(const EvaluateExpressionStep &other);


         // Attributes
    private:

         TextualStepParameter*   m_evaluateExpressionParameter;
         StepNumberStepParameter*                m_successParameter;
         StepNumberStepParameter*                m_failureParameter;
    };
}

#endif // defined (STEPTYPE_EVALUATE_EXPRESSION)

#endif // !defined(AFX_EVALUATEEXPRESSIONSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_)
