/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchGuiAppStep.cpp $
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

#if defined (STEPTYPE_LAUNCH_GUI_APP)

#include "core/utilities/src/TAAssert.h"
#include "LaunchGuiAppStep.h"
#include "Resource.h"
#include "GuiAppTypeStepParameter.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "PlanStepEditLaunchGuiAppParameterPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_X_POSITION = 0;
const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_Y_POSITION = 155;
const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_WIDTH      = -1;
const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_HEIGHT     = -1;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

LaunchGuiAppStep::LaunchGuiAppStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent, stepDetail),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL)
{
    FUNCTION_ENTRY( "LaunchGuiAppStep" );
    FUNCTION_EXIT;
}


LaunchGuiAppStep::LaunchGuiAppStep(PlanNode* parent, PlanStepId stepId) :
    PlanStep(parent,stepId, TA_Base_Core::LAUNCH_GUI_APP_STEP),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL)
{
    FUNCTION_ENTRY( "LaunchGuiAppStep" );
    FUNCTION_EXIT;
}


LaunchGuiAppStep::LaunchGuiAppStep(const LaunchGuiAppStep& planStep) :
    PlanStep(planStep),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL)
{
    FUNCTION_ENTRY( "LaunchGuiAppStep" );

    m_appTypeParameter = new GuiAppTypeStepParameter(*this, *planStep.m_appTypeParameter);
    m_commandLineParameter = new TextualStepParameter(*this, *planStep.m_commandLineParameter);
    m_xPositionParameter = new NumericStepParameter(*this, *planStep.m_xPositionParameter);
    m_yPositionParameter = new NumericStepParameter(*this, *planStep.m_yPositionParameter);
    m_widthParameter = new NumericStepParameter(*this, *planStep.m_widthParameter);
    m_heightParameter = new NumericStepParameter(*this, *planStep.m_heightParameter);

    addPlanStepParameter(*m_appTypeParameter);
    addPlanStepParameter(*m_commandLineParameter);
    addPlanStepParameter(*m_xPositionParameter);
    addPlanStepParameter(*m_yPositionParameter);
    addPlanStepParameter(*m_widthParameter);
    addPlanStepParameter(*m_heightParameter);

    FUNCTION_EXIT;
}


LaunchGuiAppStep::~LaunchGuiAppStep()
{
    FUNCTION_ENTRY( "~LaunchGuiAppStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* LaunchGuiAppStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new LaunchGuiAppStep(*this);
}


void LaunchGuiAppStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_appTypeParameter;
    delete m_commandLineParameter;
    delete m_xPositionParameter;
    delete m_yPositionParameter;
    delete m_widthParameter;
    delete m_heightParameter;

    m_appTypeParameter = new GuiAppTypeStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_APPLICATION), 0, true);
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), "");
    m_xPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_X_POSITION), DEFAULT_X_POSITION);
    m_yPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_Y_POSITION), DEFAULT_Y_POSITION);
    m_widthParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_WIDTH), DEFAULT_WIDTH, true);
    m_heightParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_HEIGHT), DEFAULT_HEIGHT, true);

    stepParameters.clear();
    stepParameters.push_back(m_appTypeParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_xPositionParameter);
    stepParameters.push_back(m_yPositionParameter);
    stepParameters.push_back(m_widthParameter);
    stepParameters.push_back(m_heightParameter);

    FUNCTION_EXIT;
}


void LaunchGuiAppStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                               PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::LAUNCH_GUI_APP_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for launch GUI app step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::LAUNCH_GUI_APP_STEP));
    }

    delete m_appTypeParameter;
    delete m_commandLineParameter;
    delete m_xPositionParameter;
    delete m_yPositionParameter;
    delete m_widthParameter;
    delete m_heightParameter;

    m_appTypeParameter = new GuiAppTypeStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_APPLICATION), parameterSet.guiAppParameters().app);
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), parameterSet.guiAppParameters().args.in());

    m_xPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_X_POSITION), parameterSet.guiAppParameters().pos.left);
    m_yPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_Y_POSITION), parameterSet.guiAppParameters().pos.top);

    // Flag the value as hidden if window width is set to default (-1)
    m_widthParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_WIDTH),
                                                parameterSet.guiAppParameters().pos.width,
                                                parameterSet.guiAppParameters().pos.width == DEFAULT_WIDTH);

    // Flag the value as hidden if window height is set to default (-1)
    m_heightParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_HEIGHT),
                                                 parameterSet.guiAppParameters().pos.height,
                                                 parameterSet.guiAppParameters().pos.height == DEFAULT_HEIGHT);

    stepParameters.clear();
    stepParameters.push_back(m_appTypeParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_xPositionParameter);
    stepParameters.push_back(m_yPositionParameter);
    stepParameters.push_back(m_widthParameter);
    stepParameters.push_back(m_heightParameter);

    FUNCTION_EXIT;
}


void LaunchGuiAppStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::LaunchGuiAppParameters parameters;

    parameters.app = m_appTypeParameter->getNumericValue();
    parameters.args = CORBA::string_dup(m_commandLineParameter->getMessageText().c_str());
    parameters.pos.left = m_xPositionParameter->getNumericValue();
    parameters.pos.top = m_yPositionParameter->getNumericValue();
    parameters.pos.width = m_widthParameter->getNumericValue();
    parameters.pos.height = m_heightParameter->getNumericValue();

    step.parameters.guiAppParameters(parameters);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* LaunchGuiAppStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditLaunchGuiAppParameterPage(this);
}


#endif // defined (STEPTYPE_LAUNCH_GUI_APP)
