//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundDecisionStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(_BACKGROUND_DECISION_STEP__INCLUDED_)
#define _BACKGROUND_DECISION_STEP__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_DECISION)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class StepNumberStepParameter;
    class TextualStepParameter;
    class NumericStepParameter;
    class YesNoStepParameter;
    /////////////////////////////////////////////////////////////////////////////
    // BackgroundDecisionStep
          
    class BackgroundDecisionStep : public PlanStep
    {
        // Operations
    public:
        BackgroundDecisionStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        BackgroundDecisionStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~BackgroundDecisionStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return false; }

        TextualStepParameter*     getMessageParameter() { return m_messageParameter;};
        NumericStepParameter*     getTimeoutParameter() { return m_timeoutParameter;};
        StepNumberStepParameter*  getYesParameter() { return m_yesParameter; }
        StepNumberStepParameter*  getNoParameter() { return m_noParameter; };
        // lizettejl++ (DP-changes)
        TextualStepParameter* getYesLabelParameter() { return m_yesLabelParameter; }
        TextualStepParameter* getNoLabelParameter()  { return m_noLabelParameter; }
        TextualStepParameter* getPauseLabelParameter() { return m_pauseLabelParameter; }
        YesNoStepParameter*   getEnableNoParameter()  { return m_enableNoParameter; }
        YesNoStepParameter*   getEnablePauseParameter()  { return m_enablePauseParameter; }
        // ++lizettejl
        NumericStepParameter*  getOperatorProfile()     {return m_operatorProfileNumber;}
        TextualStepParameter*  getOperatorProfileName()  { return m_operatorProfileDisplay; }

    protected:
        BackgroundDecisionStep(const BackgroundDecisionStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        BackgroundDecisionStep &operator=(const BackgroundDecisionStep &other);


        // Attributes
    private:
        TextualStepParameter* m_messageParameter;
        NumericStepParameter* m_timeoutParameter;
        StepNumberStepParameter* m_yesParameter;
        StepNumberStepParameter* m_noParameter;
        // lizettejl++ (DP-changes)
        TextualStepParameter* m_yesLabelParameter;
        TextualStepParameter* m_noLabelParameter;
        TextualStepParameter* m_pauseLabelParameter;
        YesNoStepParameter*   m_enableNoParameter;
        YesNoStepParameter*   m_enablePauseParameter;
        // ++lizettejl
        NumericStepParameter* m_operatorProfileNumber;
        TextualStepParameter* m_operatorProfileDisplay;
    };
}

#endif // defined (STEPTYPE_BACKGROUND_DECISION)

#endif // !defined(_BACKGROUND_DECISION_STEP__INCLUDED_)
