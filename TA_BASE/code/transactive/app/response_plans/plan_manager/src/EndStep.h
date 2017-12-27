//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/EndStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_ENDSTEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_)
#define AFX_ENDSTEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // EndStep

    class EndStep : public PlanStep
    {
    // Operations
    public:
        EndStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        virtual ~EndStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

    protected:
        EndStep(const EndStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);

        virtual bool canDeleteStep();

    private:
        EndStep &operator=(const EndStep &other);


    // Attributes
    private:

    };
}

#endif // !defined(AFX_ENDSTEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_)
