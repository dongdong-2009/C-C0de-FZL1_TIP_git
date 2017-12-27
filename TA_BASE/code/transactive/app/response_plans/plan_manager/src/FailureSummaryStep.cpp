/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureSummaryStep.cpp $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)

#include "FailureSummaryStep.h"
#include "Resource.h"
#include "FailureCategoryStepParameter.h"
#include "PlanStepEditParameterPage.h"
#include "PlanStepEditFailureSummaryParameterPage.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// PlanStep

FailureSummaryStep::FailureSummaryStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent, stepDetail),
    m_failureCategoryParameter(NULL)
{
    FUNCTION_ENTRY( "FailureSummaryStep" );
    FUNCTION_EXIT;
}


FailureSummaryStep::FailureSummaryStep(PlanNode* parent, PlanStepId stepId) :
    PlanStep(parent,stepId, TA_Base_Core::FAILURE_SUMMARY_STEP),
    m_failureCategoryParameter(NULL)
{
    FUNCTION_ENTRY( "FailureSummaryStep" );
    FUNCTION_EXIT;
}


FailureSummaryStep::FailureSummaryStep(const FailureSummaryStep& planStep) :
    PlanStep(planStep),
    m_failureCategoryParameter(NULL)
{
    FUNCTION_ENTRY( "FailureSummaryStep" );

    m_failureCategoryParameter = new FailureCategoryStepParameter(*this, *planStep.m_failureCategoryParameter);
    addPlanStepParameter(*m_failureCategoryParameter);

    FUNCTION_EXIT;
}


FailureSummaryStep::~FailureSummaryStep()
{
    FUNCTION_ENTRY( "~FailureSummaryStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* FailureSummaryStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new FailureSummaryStep(*this);
}


void FailureSummaryStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_failureCategoryParameter;

    m_failureCategoryParameter= new FailureCategoryStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_FAILURE_CATEGORY), TA_Base_Core::SFC_SET_DATA_POINT_STEPS);
    stepParameters.clear();
    stepParameters.push_back(m_failureCategoryParameter);

    FUNCTION_EXIT;
}


void FailureSummaryStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                               PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::FAILURE_SUMMARY_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for Failure Summary step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::FAILURE_SUMMARY_STEP));
    }

    delete m_failureCategoryParameter;

    m_failureCategoryParameter= new FailureCategoryStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_FAILURE_CATEGORY), parameterSet.failureSummary().failureCategory);
    stepParameters.clear();
    stepParameters.push_back(m_failureCategoryParameter);

    FUNCTION_EXIT;
}


void FailureSummaryStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::FailureSummaryParameters parameter;
    parameter.failureCategory = TA_Base_Core::SFC_UNDEFINED;

    if (NULL != m_failureCategoryParameter)
    {
        parameter.failureCategory = m_failureCategoryParameter->getCategory();
    }
    step.parameters.failureSummary(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* FailureSummaryStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditFailureSummaryParameterPage(this);
}


#endif // defined (STEPTYPE_FAILURE_SUMMARY_STEP)
