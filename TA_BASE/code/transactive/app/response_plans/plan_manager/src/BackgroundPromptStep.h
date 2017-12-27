//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundPromptStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(_BACKGROUND_PROMPT_STEP__INCLUDED_)
#define _BACKGROUND_PROMPT_STEP__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_PROMPT)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class NumericStepParameter;
    class TextualStepParameter;
    class CPlanStepEditParameterPage;

    /////////////////////////////////////////////////////////////////////////////
    // BackgroundPromptStep

    class BackgroundPromptStep : public PlanStep
    {
        // Operations
    public:
        BackgroundPromptStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        BackgroundPromptStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~BackgroundPromptStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        TextualStepParameter* getMessageParameter() { return m_messageParameter;};
        NumericStepParameter* getTimeoutParameter() { return m_timeoutParameter;};
        NumericStepParameter*  getOperatorProfile()     {return m_operatorProfileNumber;}
        TextualStepParameter*  getOperatorProfileName()  { return m_operatorProfileDisplay; }

    protected:
        BackgroundPromptStep(const BackgroundPromptStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        BackgroundPromptStep &operator=(const BackgroundPromptStep &other);


        // Attributes
    private:

        TextualStepParameter* m_messageParameter;
        NumericStepParameter* m_timeoutParameter;
        NumericStepParameter* m_operatorProfileNumber;
        TextualStepParameter* m_operatorProfileDisplay;
    };
}

#endif // defined (STEPTYPE_BACKGROUND_PROMPT)

#endif // !defined(_BACKGROUND_PROMPT_STEP__INCLUDED_)
