//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/AbortStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_ABORTSTEP_H__E40AE63B_4A40_4DC8_992E_240935C7BBA9__INCLUDED_)
#define AFX_ABORTSTEP_H__E40AE63B_4A40_4DC8_992E_240935C7BBA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ABORT)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // AbortStep

    class AbortStep : public PlanStep
    {
    // Operations
    public:
        AbortStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        AbortStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~AbortStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

    protected:
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);

    private:
        AbortStep(const AbortStep &other);
        AbortStep &operator=(const AbortStep &other);

    };
}

#endif // defined (STEPTYPE_ABORT)

#endif // !defined(AFX_ABORTSTEP_H__E40AE63B_4A40_4DC8_992E_240935C7BBA9__INCLUDED_)
