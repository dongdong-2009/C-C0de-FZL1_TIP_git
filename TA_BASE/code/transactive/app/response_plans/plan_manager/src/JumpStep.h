//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/JumpStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_JUMPSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_JUMPSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_JUMP)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class StepNumberStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // JumpStep

    class JumpStep : public PlanStep
    {
    // Operations
    public:
        JumpStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        JumpStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~JumpStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return false; }

        StepNumberStepParameter* getJumpStepParameter() { return m_stepParameter;};

    protected:
        JumpStep(const JumpStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        JumpStep &operator=(const JumpStep &other);


    // Attributes
    private:
        StepNumberStepParameter*   m_stepParameter;
    };
}

#endif // defined (STEPTYPE_JUMP)

#endif // !defined(AFX_JUMPSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
