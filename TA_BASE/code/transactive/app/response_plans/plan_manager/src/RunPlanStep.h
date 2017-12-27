//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/RunPlanStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_RUNPLANSTEP_H__81809A68_7564_4F2D_8B23_F7B4DDCB695B__INCLUDED_)
#define AFX_RUNPLANSTEP_H__81809A68_7564_4F2D_8B23_F7B4DDCB695B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_RUN_PLAN)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class YesNoStepParameter;
    class PlanPathStepParameter;
    class CPlanStepEditRunPlanPage;

    /////////////////////////////////////////////////////////////////////////////
    // RunPlanStep

    class RunPlanStep : public PlanStep
    {
    // Operations
    public:
        RunPlanStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        RunPlanStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~RunPlanStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return true; }

        PlanPathStepParameter* getPathParameter() { return m_planParameter;};
        YesNoStepParameter* getLaunchParameter() { return m_launchParameter;};
        YesNoStepParameter* getWaitParameter() { return m_waitParameter;};

    protected:
        RunPlanStep(const RunPlanStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        RunPlanStep &operator=(const RunPlanStep &other);


    // Attributes
    private:


        PlanPathStepParameter* m_planParameter;
        YesNoStepParameter* m_launchParameter;
        YesNoStepParameter* m_waitParameter;


    };
}

#endif // defined (STEPTYPE_RUN_PLAN)

#endif // !defined(AFX_RUNPLANSTEP_H__81809A68_7564_4F2D_8B23_F7B4DDCB695B__INCLUDED_)
