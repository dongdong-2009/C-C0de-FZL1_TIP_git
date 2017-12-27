//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundLogStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(_BACKGROUND_LOG__INCLUDED_)
#define _BACKGROUND_LOG__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_LOG)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class NumericStepParameter;
    class TextualStepParameter;
    class YesNoStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // BackgroundLogStep

    class BackgroundLogStep : public PlanStep
    {
        // Operations
    public:
        BackgroundLogStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        BackgroundLogStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~BackgroundLogStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        TextualStepParameter* getMessageParameter()     { return m_messageParameter; }
        NumericStepParameter* getTimeoutParameter()     { return m_timeoutParameter; }
        YesNoStepParameter* getMandatoryLogParameter()  { return m_mandatoryLogParameter; }
        NumericStepParameter*  getOperatorProfile()     {return m_operatorProfileNumber;}
        TextualStepParameter*  getOperatorProfileName()  { return m_operatorProfileDisplay; }

    protected:
        BackgroundLogStep(const BackgroundLogStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        BackgroundLogStep &operator=(const BackgroundLogStep &other);


        // Attributes
    private:
        TextualStepParameter* m_messageParameter;
        NumericStepParameter* m_timeoutParameter;
        YesNoStepParameter* m_mandatoryLogParameter;
        NumericStepParameter* m_operatorProfileNumber;
        TextualStepParameter* m_operatorProfileDisplay;
    };
}

#endif // defined (STEPTYPE_BACKGROUND_LOG)

#endif // !defined(_BACKGROUND_LOG__INCLUDED_)
