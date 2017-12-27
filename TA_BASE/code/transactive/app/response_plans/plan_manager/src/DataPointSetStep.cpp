/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointSetStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_DATA_POINT_SET)

#include "core/utilities/src/TAAssert.h"
#include "DataPointSetStep.h"
#include "DataPointStepParameter.h"
#include "DataPointValueStepParameter.h"
#include "resource.h"
#include "PlanStepEditDataPointSetPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

DataPointSetStep::DataPointSetStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_dataPoint(NULL),
    m_dataPointValue(NULL)
{
    FUNCTION_ENTRY( "DataPointSetStep" );
    FUNCTION_EXIT;
}


DataPointSetStep::DataPointSetStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::DATA_POINT_SET_STEP),
    m_dataPoint(NULL),
    m_dataPointValue(NULL)
{
    FUNCTION_ENTRY( "DataPointSetStep" );
    FUNCTION_EXIT;
}


DataPointSetStep::DataPointSetStep(const DataPointSetStep& planStep) :
    PlanStep(planStep),
    m_dataPoint(NULL),
    m_dataPointValue(NULL)
{
    FUNCTION_ENTRY( "DataPointSetStep" );

    m_dataPoint = new DataPointStepParameter(*this, *planStep.m_dataPoint);
    m_dataPointValue = new DataPointValueStepParameter(*this, *planStep.m_dataPointValue);

    addPlanStepParameter(*m_dataPoint);
    addPlanStepParameter(*m_dataPointValue);

    FUNCTION_EXIT;
}


DataPointSetStep::~DataPointSetStep()
{
    FUNCTION_ENTRY( "~DataPointSetStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* DataPointSetStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new DataPointSetStep(*this);
}


void DataPointSetStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_dataPoint;
    delete m_dataPointValue;

    m_dataPoint = new DataPointStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), "");
    m_dataPointValue = new DataPointValueStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NEW_VALUE), TA_Base_Core::DataPointValue());

    stepParameters.clear();
    stepParameters.push_back(m_dataPoint);
    stepParameters.push_back(m_dataPointValue);

    FUNCTION_EXIT;
}


void DataPointSetStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::DATA_POINT_SET_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for data point set step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::DATA_POINT_SET_STEP));
    }

    delete m_dataPoint;
    delete m_dataPointValue;

    m_dataPoint = new DataPointStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), parameterSet.dataPointSet().dataPoint.in());
    m_dataPointValue = new DataPointValueStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NEW_VALUE), parameterSet.dataPointSet().newValue);

    stepParameters.clear();
    stepParameters.push_back(m_dataPoint);
    stepParameters.push_back(m_dataPointValue);

    FUNCTION_EXIT;
}


void DataPointSetStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::DataPointSetParameters  parameter;

    parameter.dataPoint = CORBA::string_dup(m_dataPoint->getDataPoint().c_str());
    parameter.newValue = m_dataPointValue->getDataPointValue();

    step.parameters.dataPointSet(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* DataPointSetStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDataPointSetPage(this);
}


#endif // defined (STEPTYPE_DATA_POINT_SET)
