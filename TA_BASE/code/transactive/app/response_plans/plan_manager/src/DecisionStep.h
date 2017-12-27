//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DecisionStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_DECISIONSTEP_H__4623E196_CA3D_4BAC_B9C3_9A80949E9007__INCLUDED_)
#define AFX_DECISIONSTEP_H__4623E196_CA3D_4BAC_B9C3_9A80949E9007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DECISION)

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
    // DecisionStep

    class DecisionStep : public PlanStep
    {
    // Operations
    public:
        DecisionStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        DecisionStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~DecisionStep();

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

    protected:
        DecisionStep(const DecisionStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        DecisionStep &operator=(const DecisionStep &other);


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

    };
}

#endif // defined (STEPTYPE_DECISION)

#endif // !defined(AFX_DECISIONSTEP_H__4623E196_CA3D_4BAC_B9C3_9A80949E9007__INCLUDED_)
