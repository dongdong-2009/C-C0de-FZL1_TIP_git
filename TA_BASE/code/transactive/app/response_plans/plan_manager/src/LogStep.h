//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LogStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_LOGSTEP_H__90AFD6F7_A1F4_452D_8B3D_48EFC4958974__INCLUDED_)
#define AFX_LOGSTEP_H__90AFD6F7_A1F4_452D_8B3D_48EFC4958974__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_LOG)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class NumericStepParameter;
    class TextualStepParameter;
    class YesNoStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // LogStep

    class LogStep : public PlanStep
    {
    // Operations
    public:
        LogStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        LogStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~LogStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        TextualStepParameter* getMessageParameter()     { return m_messageParameter; }
        NumericStepParameter* getTimeoutParameter()     { return m_timeoutParameter; }
        YesNoStepParameter* getMandatoryLogParameter()  { return m_mandatoryLogParameter; }

    protected:
        LogStep(const LogStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        LogStep &operator=(const LogStep &other);


    // Attributes
    private:
        TextualStepParameter* m_messageParameter;
        NumericStepParameter* m_timeoutParameter;
        YesNoStepParameter* m_mandatoryLogParameter;
    };
}

#endif // defined (STEPTYPE_LOG)

#endif // !defined(AFX_LOGSTEP_H__90AFD6F7_A1F4_452D_8B3D_48EFC4958974__INCLUDED_)
