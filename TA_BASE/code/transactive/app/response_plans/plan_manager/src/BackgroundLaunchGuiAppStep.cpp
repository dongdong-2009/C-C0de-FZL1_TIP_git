/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundLaunchGuiAppStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/IAction.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "BackgroundLaunchGuiAppStep.h"
#include "Resource.h"
#include "GuiAppTypeStepParameter.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "YesNoStepParameter.h"
#include "PlanStepEditBackgroundLaunchGuiAppParameterPage.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const unsigned long TA_Base_App::BackgroundLaunchGuiAppStep::DEFAULT_X_POSITION = 0;
const unsigned long TA_Base_App::BackgroundLaunchGuiAppStep::DEFAULT_Y_POSITION = 155;
const unsigned long TA_Base_App::BackgroundLaunchGuiAppStep::DEFAULT_WIDTH      = -1;
const unsigned long TA_Base_App::BackgroundLaunchGuiAppStep::DEFAULT_HEIGHT     = -1;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

BackgroundLaunchGuiAppStep::BackgroundLaunchGuiAppStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent, stepDetail),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL),
    m_addTriggerEntityParameter(NULL),
    m_actionType(NULL),
    m_actionTypeDisplay(NULL),
    m_dataPoint(NULL),
    m_dataPointDisplay(NULL),
    m_TriggerEntityCheckAction(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLaunchGuiAppStep" );
    FUNCTION_EXIT;
}


BackgroundLaunchGuiAppStep::BackgroundLaunchGuiAppStep(PlanNode* parent, PlanStepId stepId) :
PlanStep(parent,stepId, TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL),
    m_addTriggerEntityParameter(NULL),
    m_actionType(NULL),
    m_actionTypeDisplay(NULL),
    m_dataPoint(NULL),
    m_dataPointDisplay(NULL),
    m_TriggerEntityCheckAction(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLaunchGuiAppStep" );
    FUNCTION_EXIT;
}


BackgroundLaunchGuiAppStep::BackgroundLaunchGuiAppStep(const BackgroundLaunchGuiAppStep& planStep) :
    PlanStep(planStep),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL),
    m_addTriggerEntityParameter(NULL),
    m_actionType(NULL),
    m_actionTypeDisplay(NULL),
    m_dataPoint(NULL),
    m_dataPointDisplay(NULL),
    m_TriggerEntityCheckAction(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLaunchGuiAppStep" );

    m_appTypeParameter = new GuiAppTypeStepParameter(*this, *planStep.m_appTypeParameter);
    m_commandLineParameter = new TextualStepParameter(*this, *planStep.m_commandLineParameter);
    m_xPositionParameter = new NumericStepParameter(*this, *planStep.m_xPositionParameter);
    m_yPositionParameter = new NumericStepParameter(*this, *planStep.m_yPositionParameter);
    m_widthParameter = new NumericStepParameter(*this, *planStep.m_widthParameter);
    m_heightParameter = new NumericStepParameter(*this, *planStep.m_heightParameter);
    m_addTriggerEntityParameter = new YesNoStepParameter(*this, *planStep.m_addTriggerEntityParameter);
    m_actionType = new NumericStepParameter(*this, *planStep.m_actionType);
    m_actionTypeDisplay = new TextualStepParameter(*this, *planStep.m_actionTypeDisplay);

    m_dataPoint = new NumericStepParameter(*this, *planStep.m_dataPoint);
    m_dataPointDisplay = new TextualStepParameter(*this, *planStep.m_dataPointDisplay);
    m_TriggerEntityCheckAction = new YesNoStepParameter(*this, *planStep.m_TriggerEntityCheckAction);
    m_operatorProfileNumber = new NumericStepParameter(*this, *planStep.m_operatorProfileNumber);
    m_operatorProfileDisplay = new TextualStepParameter(*this, *planStep.m_operatorProfileDisplay);

    addPlanStepParameter(*m_appTypeParameter);
    addPlanStepParameter(*m_commandLineParameter);
    addPlanStepParameter(*m_xPositionParameter);
    addPlanStepParameter(*m_yPositionParameter);
    addPlanStepParameter(*m_widthParameter);
    addPlanStepParameter(*m_heightParameter);
    addPlanStepParameter(*m_addTriggerEntityParameter);
    //addPlanStepParameter(*m_actionType);
    //addPlanStepParameter(*m_actionTypeDisplay);
    //addPlanStepParameter(*m_dataPoint);
    //addPlanStepParameter(*m_dataPointDisplay);
    //addPlanStepParameter(*m_TriggerEntityCheckAction);
    //addPlanStepParameter(*m_operatorProfileNumber);
    addPlanStepParameter(*m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


BackgroundLaunchGuiAppStep::~BackgroundLaunchGuiAppStep()
{
    FUNCTION_ENTRY( "~BackgroundLaunchGuiAppStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* BackgroundLaunchGuiAppStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new BackgroundLaunchGuiAppStep(*this);
}


void BackgroundLaunchGuiAppStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_appTypeParameter;
    delete m_commandLineParameter;
    delete m_xPositionParameter;
    delete m_yPositionParameter;
    delete m_widthParameter;
    delete m_heightParameter;
    delete m_actionType;
    delete m_actionTypeDisplay;
    delete m_addTriggerEntityParameter;
    delete m_dataPoint;
    delete m_dataPointDisplay;
    delete m_TriggerEntityCheckAction;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_appTypeParameter = new GuiAppTypeStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_APPLICATION), 0, true);
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), "");
    m_xPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_X_POSITION), DEFAULT_X_POSITION);
    m_yPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_Y_POSITION), DEFAULT_Y_POSITION);
    m_widthParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_WIDTH), DEFAULT_WIDTH, true);
    m_heightParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_HEIGHT), DEFAULT_HEIGHT, true);
    m_addTriggerEntityParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRIGGER_ENTITY), false);
    m_actionType = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), 0);
    m_dataPoint = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), 0);
    m_TriggerEntityCheckAction = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRIGGER_ENTITY_CHECK_ACTION), true);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), TA_Base_Core::DatabaseKey::getInvalidKey());
    m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), "");

    //try
    //{
    //    typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
    //    ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
    //    for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
    //    {
    //        if (1 == (*iter)->getKey())
    //        {
    //            m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), (*iter)->getName());
    //        }
    //        delete *iter;
    //    }
    //    allProfile.clear();
    //}
    //catch (...)
    //{
    //    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to populate profile name");
    //    return;
    //}

    stepParameters.clear();
    stepParameters.push_back(m_appTypeParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_xPositionParameter);
    stepParameters.push_back(m_yPositionParameter);
    stepParameters.push_back(m_widthParameter);
    stepParameters.push_back(m_heightParameter);
    stepParameters.push_back(m_addTriggerEntityParameter);
    //stepParameters.push_back(m_TriggerEntityCheckAction);
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


void BackgroundLaunchGuiAppStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                               PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for background launch GUI app step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP));
    }

    delete m_appTypeParameter;
    delete m_commandLineParameter;
    delete m_xPositionParameter;
    delete m_yPositionParameter;
    delete m_widthParameter;
    delete m_heightParameter;
    delete m_addTriggerEntityParameter;
    delete m_actionType;
    delete m_actionTypeDisplay;
    delete m_dataPoint;
    delete m_dataPointDisplay;
    delete m_TriggerEntityCheckAction;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_appTypeParameter = new GuiAppTypeStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_APPLICATION), parameterSet.bgLaunchGuiApp().app);
    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), parameterSet.bgLaunchGuiApp().args.in());

    m_xPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_X_POSITION), parameterSet.bgLaunchGuiApp().pos.left);
    m_yPositionParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_Y_POSITION), parameterSet.bgLaunchGuiApp().pos.top);

    // Flag the value as hidden if window width is set to default (-1)
    m_widthParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_WIDTH),
                                                parameterSet.bgLaunchGuiApp().pos.width,
                                                parameterSet.bgLaunchGuiApp().pos.width == DEFAULT_WIDTH);

    // Flag the value as hidden if window height is set to default (-1)
    m_heightParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_HEIGHT),
                                                 parameterSet.bgLaunchGuiApp().pos.height,
                                                 parameterSet.bgLaunchGuiApp().pos.height == DEFAULT_HEIGHT);

    m_addTriggerEntityParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRIGGER_ENTITY), 
                                                 parameterSet.bgLaunchGuiApp().bAddTriggerEntity);

    m_actionType = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), parameterSet.bgLaunchGuiApp().actionTypeKey);

    m_dataPoint = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), parameterSet.bgLaunchGuiApp().dataPoint);

    m_TriggerEntityCheckAction = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRIGGER_ENTITY_CHECK_ACTION), 
        parameterSet.bgLaunchGuiApp().bTriggerEntityCheckAction);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), parameterSet.bgLaunchGuiApp().profile);

    try
    {
        typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
        ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
        for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
        {
            if (parameterSet.bgLaunchGuiApp().profile == (*iter)->getKey())
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
    stepParameters.push_back(m_appTypeParameter);
    stepParameters.push_back(m_commandLineParameter);
    stepParameters.push_back(m_xPositionParameter);
    stepParameters.push_back(m_yPositionParameter);
    stepParameters.push_back(m_widthParameter);
    stepParameters.push_back(m_heightParameter);
    stepParameters.push_back(m_addTriggerEntityParameter);
    //stepParameters.push_back(m_TriggerEntityCheckAction);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
    try
    {
        dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(parameterSet.bgLaunchGuiApp().dataPoint));
    }
    catch (...) 
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get entity data, entity key = %d", parameterSet.bgLaunchGuiApp().dataPoint);
        return;
    }

    m_dataPointDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), dpEntity->getName());
    //stepParameters.push_back(m_dataPointDisplay);

    std::auto_ptr<TA_Base_Core::IAction> actionEntity(NULL);
    try
    {
        actionEntity.reset(TA_Base_Core::ActionAccessFactory::getInstance().getAction(parameterSet.bgLaunchGuiApp().actionTypeKey));
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception for get action data, aciton key = %d", parameterSet.bgLaunchGuiApp().actionTypeKey);
        return ;
    }

    m_actionTypeDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ACTION_TYPE), actionEntity->getName());
    //stepParameters.push_back(m_actionTypeDisplay);
    

    FUNCTION_EXIT;
}


void BackgroundLaunchGuiAppStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::BackgroundLaunchGuiAppParameters parameters;

    parameters.app = m_appTypeParameter->getNumericValue();
    parameters.args = CORBA::string_dup(m_commandLineParameter->getMessageText().c_str());
    parameters.pos.left = m_xPositionParameter->getNumericValue();
    parameters.pos.top = m_yPositionParameter->getNumericValue();
    parameters.pos.width = m_widthParameter->getNumericValue();
    parameters.pos.height = m_heightParameter->getNumericValue();
    parameters.bAddTriggerEntity = m_addTriggerEntityParameter->getAffirmative();
    parameters.actionTypeKey = m_actionType->getNumericValue();
    parameters.dataPoint = m_dataPoint->getNumericValue();
    parameters.bTriggerEntityCheckAction = m_TriggerEntityCheckAction->getAffirmative();
    parameters.profile = m_operatorProfileNumber->getNumericValue();

    step.parameters.bgLaunchGuiApp(parameters);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* BackgroundLaunchGuiAppStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditBackgroundLaunchGuiAppParameterPage(this);
}


#endif // defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)
