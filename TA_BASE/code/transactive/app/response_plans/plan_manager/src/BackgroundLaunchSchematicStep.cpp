/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IAction.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "BackgroundLaunchSchematicStep.h"
#include "Resource.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "PlanStepEditBackgroundLaunchSchematicParameterPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

BackgroundLaunchSchematicStep::BackgroundLaunchSchematicStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent, stepDetail),
    m_dataPoint(NULL),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL),
    m_actionType(NULL),
    m_actionTypeDisplay(NULL),
    m_dataPointDisplay(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLaunchSchematicStep" );
    FUNCTION_EXIT;
}


BackgroundLaunchSchematicStep::BackgroundLaunchSchematicStep(PlanNode* parent, PlanStepId stepId) :
    PlanStep(parent,stepId, TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP),
    m_dataPoint(NULL),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL),
    m_actionType(NULL),
    m_actionTypeDisplay(NULL),
    m_dataPointDisplay(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLaunchSchematicStep" );
    FUNCTION_EXIT;
}


BackgroundLaunchSchematicStep::BackgroundLaunchSchematicStep(const BackgroundLaunchSchematicStep& planStep) :
    PlanStep(planStep),
    m_dataPoint(NULL),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL),
    m_actionType(NULL),
    m_actionTypeDisplay(NULL),
    m_dataPointDisplay(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLaunchSchematicStep" );

    m_schematicNameParameter = new TextualStepParameter(*this, *planStep.m_schematicNameParameter);
    m_commandLineParameter = new TextualStepParameter(*this, *planStep.m_commandLineParameter);
    m_screenNumberParameter = new NumericStepParameter(*this, *planStep.m_screenNumberParameter);
    m_dataPoint = new NumericStepParameter(*this, *planStep.m_dataPoint);
    m_actionType = new NumericStepParameter(*this, *planStep.m_actionType);
    m_actionTypeDisplay = new TextualStepParameter(*this, *planStep.m_actionTypeDisplay);
    m_dataPointDisplay = new TextualStepParameter(*this, *planStep.m_dataPointDisplay);
    m_operatorProfileNumber = new NumericStepParameter(*this, *planStep.m_operatorProfileNumber);
    m_operatorProfileDisplay = new TextualStepParameter(*this, *planStep.m_operatorProfileDisplay);

    addPlanStepParameter(*m_schematicNameParameter);
    addPlanStepParameter(*m_commandLineParameter);
    addPlanStepParameter(*m_screenNumberParameter);
    //addPlanStepParameter(*m_dataPoint);
    //addPlanStepParameter(*m_actionType);
    //addPlanStepParameter(*m_actionTypeDisplay);
    //addPlanStepParameter(*m_dataPointDisplay);
    //addPlanStepParameter(*m_operatorProfileNumber);
    addPlanStepParameter(*m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


BackgroundLaunchSchematicStep::~BackgroundLaunchSchematicStep()
{
    FUNCTION_ENTRY( "~BackgroundLaunchSchematicStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* BackgroundLaunchSchematicStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new BackgroundLaunchSchematicStep(*this);
}


void BackgroundLaunchSchematicStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_schematicNameParameter;
    delete m_commandLineParameter;
    delete m_screenNumberParameter;
    delete m_dataPoint;
    delete m_actionType;
    delete m_actionTypeDisplay;
    delete m_dataPointDisplay;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_schematicNameParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCHEMATIC), "");
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), "");
    m_screenNumberParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCREEN), 1);
    m_dataPoint = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), 0);
    m_actionType = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), 0);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), TA_Base_Core::DatabaseKey::getInvalidKey());
    m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), "");

    stepParameters.clear();
    stepParameters.push_back(m_schematicNameParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_screenNumberParameter);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
    try
    {
        dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(getDataPointParameter()->getNumericValue()));
    }
    catch (...) 
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get entity data, entity key = %d", getDataPointParameter()->getNumericValue());
    }

    m_dataPointDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), dpEntity->getName());
    //stepParameters.push_back(m_dataPointDisplay);

    std::auto_ptr<TA_Base_Core::IAction> actionEntity(NULL);
    try
    {
        actionEntity.reset(TA_Base_Core::ActionAccessFactory::getInstance().getAction(getActionType()->getNumericValue()));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get action data, aciton key = %d", getActionType()->getNumericValue());
        m_actionTypeDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), "Undefined");
        //stepParameters.push_back(m_actionTypeDisplay);
        return;
    }

    m_actionTypeDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), actionEntity->getName());
    //stepParameters.push_back(m_actionTypeDisplay);

    FUNCTION_EXIT;
}


void BackgroundLaunchSchematicStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                               PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for background launch schematic step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP));
    }

    delete m_schematicNameParameter;
    delete m_commandLineParameter;
    delete m_screenNumberParameter;
    delete m_dataPoint;
    delete m_actionType;
    delete m_actionTypeDisplay;
    delete m_dataPointDisplay;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_schematicNameParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCHEMATIC), parameterSet.bgLaunchSchematic().schematic.in());
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), parameterSet.bgLaunchSchematic().commands.in());
    m_screenNumberParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SCREEN), parameterSet.bgLaunchSchematic().screen);
    m_dataPoint = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), parameterSet.bgLaunchSchematic().dataPoint);
    m_actionType = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), parameterSet.bgLaunchSchematic().actionTypeKey);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), parameterSet.bgLaunchSchematic().profile);

    try
    {
        typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
        ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
        for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
        {
            if (parameterSet.bgLaunchSchematic().profile == (*iter)->getKey())
            {
                m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), (*iter)->getName());
            }
            delete *iter;
        }
        allProfile.clear();
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to populate profile name");
        return;
    }

    stepParameters.clear();
    stepParameters.push_back(m_schematicNameParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_screenNumberParameter);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
    try
    {
        dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(parameterSet.bgLaunchSchematic().dataPoint));
    }
    catch (...) 
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get entity data, entity key = %d", parameterSet.bgLaunchSchematic().dataPoint);
        return;
    }

    m_dataPointDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), dpEntity->getName());
    //stepParameters.push_back(m_dataPointDisplay);

    std::auto_ptr<TA_Base_Core::IAction> actionEntity(NULL);
    try
    {
        actionEntity.reset(TA_Base_Core::ActionAccessFactory::getInstance().getAction(parameterSet.bgLaunchSchematic().actionTypeKey));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get action data, aciton key = %d", parameterSet.bgLaunchSchematic().actionTypeKey);
        return ;
    }

    m_actionTypeDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), actionEntity->getName());
    //stepParameters.push_back(m_actionTypeDisplay);
    
    FUNCTION_EXIT;
}


void BackgroundLaunchSchematicStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::BackgroundLaunchSchematicParameters parameters;

    parameters.schematic = CORBA::string_dup(m_schematicNameParameter->getMessageText().c_str());
    parameters.commands = CORBA::string_dup(m_commandLineParameter->getMessageText().c_str());
    parameters.screen = m_screenNumberParameter->getNumericValue();
    parameters.dataPoint = m_dataPoint->getNumericValue();
    parameters.actionTypeKey = m_actionType->getNumericValue();
    parameters.profile = m_operatorProfileNumber->getNumericValue();

    step.parameters.bgLaunchSchematic(parameters);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* BackgroundLaunchSchematicStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditBackgroundLaunchSchematicParameterPage(this);
}


#endif // defined (STEPTYPE_LAUNCH_SCHEMATIC)
