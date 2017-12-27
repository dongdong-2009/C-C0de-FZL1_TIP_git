//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TerminatePlanStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_TERMINATEPLANSTEP_H__8F6BCAA3_B30D_4FF0_8025_A7264924E915__INCLUDED_)
#define AFX_TERMINATEPLANSTEP_H__8F6BCAA3_B30D_4FF0_8025_A7264924E915__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_TERMINATE_PLAN)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class PlanPathStepParameter;
    class CPlanStepEditParameterPage;

    /////////////////////////////////////////////////////////////////////////////
    // TerminatePlanStep

    class TerminatePlanStep : public PlanStep
    {
    // Operations
    public:
        TerminatePlanStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        TerminatePlanStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~TerminatePlanStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        PlanPathStepParameter* getPathParameter() { return m_planParameter;};

    protected:
        TerminatePlanStep(const TerminatePlanStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        TerminatePlanStep &operator=(const TerminatePlanStep &other);


    // Attributes
    private:
        PlanPathStepParameter* m_planParameter;
    };
}

#endif // defined (STEPTYPE_TERMINATE_PLAN)

#endif // !defined(AFX_TERMINATEPLANSTEP_H__8F6BCAA3_B30D_4FF0_8025_A7264924E915__INCLUDED_)
