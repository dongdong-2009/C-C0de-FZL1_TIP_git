//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PromptStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PROMPTSTEP_H__536C6226_7267_4C75_AD72_8D5637FEBD02__INCLUDED_)
#define AFX_PROMPTSTEP_H__536C6226_7267_4C75_AD72_8D5637FEBD02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_PROMPT)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class NumericStepParameter;
    class TextualStepParameter;
    class CPlanStepEditParameterPage;

    /////////////////////////////////////////////////////////////////////////////
    // PromptStep

    class PromptStep : public PlanStep
    {
    // Operations
    public:
        PromptStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        PromptStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~PromptStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        TextualStepParameter* getMessageParameter() { return m_messageParameter;};
        NumericStepParameter* getTimeoutParameter() { return m_timeoutParameter;};

    protected:
        PromptStep(const PromptStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        PromptStep &operator=(const PromptStep &other);


    // Attributes
    private:

     TextualStepParameter* m_messageParameter;
     NumericStepParameter* m_timeoutParameter;

    };
}

#endif // defined (STEPTYPE_PROMPT)

#endif // !defined(AFX_PROMPTSTEP_H__536C6226_7267_4C75_AD72_8D5637FEBD02__INCLUDED_)
