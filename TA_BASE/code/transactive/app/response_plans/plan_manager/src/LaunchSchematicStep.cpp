/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchSchematicStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_LAUNCH_SCHEMATIC)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "LaunchSchematicStep.h"
#include "Resource.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "PlanStepEditLaunchSchematicParameterPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

LaunchSchematicStep::LaunchSchematicStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent, stepDetail),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL)
{
    FUNCTION_ENTRY( "LaunchSchematicStep" );
    FUNCTION_EXIT;
}


LaunchSchematicStep::LaunchSchematicStep(PlanNode* parent, PlanStepId stepId) :
    PlanStep(parent,stepId, TA_Base_Core::LAUNCH_SCHEMATIC_STEP),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL)
{
    FUNCTION_ENTRY( "LaunchSchematicStep" );
    FUNCTION_EXIT;
}


LaunchSchematicStep::LaunchSchematicStep(const LaunchSchematicStep& planStep) :
    PlanStep(planStep),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL)
{
    FUNCTION_ENTRY( "LaunchSchematicStep" );

    m_schematicNameParameter = new TextualStepParameter(*this, *planStep.m_schematicNameParameter);
    m_commandLineParameter = new TextualStepParameter(*this, *planStep.m_commandLineParameter);
    m_screenNumberParameter = new NumericStepParameter(*this, *planStep.m_screenNumberParameter);

    addPlanStepParameter(*m_schematicNameParameter);
    addPlanStepParameter(*m_commandLineParameter);
    addPlanStepParameter(*m_screenNumberParameter);

    FUNCTION_EXIT;
}


LaunchSchematicStep::~LaunchSchematicStep()
{
    FUNCTION_ENTRY( "~LaunchSchematicStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* LaunchSchematicStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new LaunchSchematicStep(*this);
}


void LaunchSchematicStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_schematicNameParameter;
    delete m_commandLineParameter;
    delete m_screenNumberParameter;

    m_schematicNameParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCHEMATIC), "");
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), "");
    m_screenNumberParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCREEN), 1);

    stepParameters.clear();
    stepParameters.push_back(m_schematicNameParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_screenNumberParameter);

    FUNCTION_EXIT;
}


void LaunchSchematicStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                               PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::LAUNCH_SCHEMATIC_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for launch schematic step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::LAUNCH_SCHEMATIC_STEP));
    }

    delete m_schematicNameParameter;
    delete m_commandLineParameter;
    delete m_screenNumberParameter;

    m_schematicNameParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCHEMATIC), parameterSet.launchSchematic().schematic.in());
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), parameterSet.launchSchematic().commands.in());
    m_screenNumberParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCREEN), parameterSet.launchSchematic().screen);

    stepParameters.clear();
    stepParameters.push_back(m_schematicNameParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_screenNumberParameter);

    FUNCTION_EXIT;
}


void LaunchSchematicStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::LaunchSchematicParameters parameters;

    parameters.schematic = CORBA::string_dup(m_schematicNameParameter->getMessageText().c_str());
    parameters.commands = CORBA::string_dup(m_commandLineParameter->getMessageText().c_str());
    parameters.screen = m_screenNumberParameter->getNumericValue();

    step.parameters.launchSchematic(parameters);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* LaunchSchematicStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditLaunchSchematicParameterPage(this);
}


#endif // defined (STEPTYPE_LAUNCH_SCHEMATIC)
