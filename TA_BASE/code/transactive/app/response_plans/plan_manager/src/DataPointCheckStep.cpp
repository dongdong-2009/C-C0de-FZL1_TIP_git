/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointCheckStep.cpp $
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

#if defined (STEPTYPE_DATA_POINT_CHECK)

#include "core/utilities/src/TAAssert.h"
#include "resource.h"
#include "DataPointCheckStep.h"
#include "DataPointStepParameter.h"
#include "DataPointTestTypeStepParameter.h"
#include "DataPointValueStepParameter.h"
#include "StepNumberStepParameter.h"
#include "PlanStepEditDataPointCheckPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

DataPointCheckStep::DataPointCheckStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_dataPoint(NULL),
    m_dataPointTestType(NULL),
    m_dataPointValue(NULL),
    m_successParameter(NULL),
    m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "DataPointCheckStep" );
    FUNCTION_EXIT;
}


DataPointCheckStep::DataPointCheckStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::DATA_POINT_CHECK_STEP),
    m_dataPoint(NULL),
    m_dataPointTestType(NULL),
    m_dataPointValue(NULL),
    m_successParameter(NULL),
    m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "DataPointCheckStep" );
    FUNCTION_EXIT;
}


DataPointCheckStep::DataPointCheckStep(const DataPointCheckStep& planStep) :
    PlanStep(planStep),
    m_dataPoint(NULL),
    m_dataPointTestType(NULL),
    m_dataPointValue(NULL),
    m_successParameter(NULL),
    m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "DataPointCheckStep" );

    m_dataPoint = new DataPointStepParameter(*this, *planStep.m_dataPoint);
    m_dataPointTestType = new DataPointTestTypeStepParameter(*this, *planStep.m_dataPointTestType);
    m_dataPointValue = new DataPointValueStepParameter(*this, *planStep.m_dataPointValue);
    m_successParameter = new StepNumberStepParameter(*this, *planStep.m_successParameter);
    m_failureParameter = new StepNumberStepParameter(*this, *planStep.m_failureParameter);

    addPlanStepParameter(*m_dataPoint);
    addPlanStepParameter(*m_dataPointTestType);
    addPlanStepParameter(*m_dataPointValue);
    addPlanStepParameter(*m_successParameter);
    addPlanStepParameter(*m_failureParameter);

    FUNCTION_EXIT;
}


DataPointCheckStep::~DataPointCheckStep()
{
    FUNCTION_ENTRY( "~DataPointCheckStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* DataPointCheckStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new DataPointCheckStep(*this);
}


void DataPointCheckStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_dataPoint;
    delete m_dataPointValue;
    delete m_dataPointTestType;
    delete m_successParameter;
    delete m_failureParameter;

    m_dataPoint = new DataPointStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), "");
    m_dataPointTestType = new DataPointTestTypeStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_TEST_TYPE), TA_Base_Core::UNDEFINED_TEST);
    m_dataPointValue = new DataPointValueStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_TEST_VALUE), TA_Base_Core::DataPointValue());
    m_successParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), 0);
    m_failureParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), 0);

    stepParameters.clear();
    stepParameters.push_back(m_dataPoint);
    stepParameters.push_back(m_dataPointTestType);
    stepParameters.push_back(m_dataPointValue);
    stepParameters.push_back(m_successParameter);
    stepParameters.push_back(m_failureParameter);

    FUNCTION_EXIT;
}


void DataPointCheckStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                  PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::DATA_POINT_CHECK_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for data point check step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::DATA_POINT_CHECK_STEP));
    }
    delete m_dataPoint;
    delete m_dataPointValue;
    delete m_dataPointTestType;
    delete m_successParameter;
    delete m_failureParameter;

    m_dataPoint = new DataPointStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), parameterSet.dataPointCheck().dataPoint.in());
    m_dataPointTestType = new DataPointTestTypeStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_TEST_TYPE), parameterSet.dataPointCheck().testType);
    m_dataPointValue = new DataPointValueStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_TEST_VALUE), parameterSet.dataPointCheck().value);
    m_successParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), parameterSet.dataPointCheck().successStep);
    m_failureParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), parameterSet.dataPointCheck().failureStep);

    stepParameters.clear();
    stepParameters.push_back(m_dataPoint);
    stepParameters.push_back(m_dataPointTestType);
    stepParameters.push_back(m_dataPointValue);
    stepParameters.push_back(m_successParameter);
    stepParameters.push_back(m_failureParameter);

    FUNCTION_EXIT;
}


void DataPointCheckStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::DataPointCheckParameters  parameter;

    parameter.dataPoint = CORBA::string_dup(m_dataPoint->getDataPoint().c_str());
    parameter.value = m_dataPointValue->getDataPointValue();
    parameter.testType = m_dataPointTestType->getDataPointTestType();
    parameter.successStep = m_successParameter->getStepId();
    parameter.failureStep = m_failureParameter->getStepId();

    step.parameters.dataPointCheck(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* DataPointCheckStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDataPointCheckPage(this);
}


#endif // defined (STEPTYPE_DATA_POINT_CHECK)
