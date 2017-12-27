//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureSummaryStep.h $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "PlanStep.h"


namespace TA_Base_App
{
    class FailureCategoryStepParameter;
    class CPlanStepEditParameterPage;
    /////////////////////////////////////////////////////////////////////////////
    // FailureSummaryStep

    class FailureSummaryStep : public PlanStep
    {
    // Operations
    public:
        FailureSummaryStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail);
        FailureSummaryStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~FailureSummaryStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        FailureCategoryStepParameter* getFailureCategoryParameter() { return m_failureCategoryParameter; }

    protected:
        FailureSummaryStep(const FailureSummaryStep &other);

        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        FailureSummaryStep &operator=(const FailureSummaryStep &other);

        FailureCategoryStepParameter* m_failureCategoryParameter;
    };
}

#endif // defined (STEPTYPE_FAILURE_SUMMARY_STEP)

