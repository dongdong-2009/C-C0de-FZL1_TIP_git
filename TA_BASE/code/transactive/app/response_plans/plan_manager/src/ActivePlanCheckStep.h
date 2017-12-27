//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanCheckStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_ACTIVEPLANCHECKSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_)
#define AFX_ACTIVEPLANCHECKSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ACTIVE_PLAN_CHECK)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class StepNumberStepParameter;
    class PlanPathStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // ActivePlanCheckStep

    class ActivePlanCheckStep : public PlanStep
    {
    // Operations
    public:
        ActivePlanCheckStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        ActivePlanCheckStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~ActivePlanCheckStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return false; }

        PlanPathStepParameter*   getPathParameter() { return m_planPathParameter;};
        StepNumberStepParameter* getYesParameter() { return m_yesStepParameter;};
        StepNumberStepParameter* getNoParameter() { return m_noStepParameter;};

    protected:
        ActivePlanCheckStep(const ActivePlanCheckStep &other);

        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        ActivePlanCheckStep &operator=(const ActivePlanCheckStep &other);


    // Attributes
    private:
        PlanPathStepParameter*   m_planPathParameter;
        StepNumberStepParameter* m_yesStepParameter;
        StepNumberStepParameter* m_noStepParameter;

    };
}

#endif // defined (STEPTYPE_ACTIVE_PLAN_CHECK)

#endif // !defined(AFX_ACTIVEPLANCHECKSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_)
