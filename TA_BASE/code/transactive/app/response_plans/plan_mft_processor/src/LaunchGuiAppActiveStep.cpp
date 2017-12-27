/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/LaunchGuiAppActiveStep.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */
#include <afxwin.h> 
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "app/response_plans/plan_mft_processor/src/RightsManager.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/LaunchGuiAppActiveStep.h"

namespace TA_Base_App
{


LaunchGuiAppActiveStep::LaunchGuiAppActiveStep( const TA_Base_Core::CurrentPlanStepDetail& stepDetail ):
ActivePlanStep(stepDetail)
{
    FUNCTION_ENTRY( "LaunchGuiAppActiveStep" );

    FUNCTION_EXIT;
}

void LaunchGuiAppActiveStep::execute()
{
    launch();
}

void LaunchGuiAppActiveStep::stop()
{
    FUNCTION_ENTRY( "stop" );

    FUNCTION_EXIT;
}

LaunchGuiAppActiveStep::~LaunchGuiAppActiveStep()
{
    FUNCTION_ENTRY( "~LaunchGuiAppActiveStep" );

    FUNCTION_EXIT;
}

void LaunchGuiAppActiveStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Process Launch Gui App request");

    if ( !RightsManager::getInstance().isProfileMatched( m_stepDetail.data.bgLaunchGuiApp().profile ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not lunch the Schematic, request profile = %d", m_stepDetail.data.bgLaunchGuiApp().profile);
        FUNCTION_EXIT;
        return;
    }

    try
    {
        ActivePlanAgency::instance()->getPlanAgent()->processBackgroundLaunchGuiAppStepResponse( m_stepDetail, TA_Base_Core::PSCS_SUCCESS);
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute MftMessageActiveStep fail");
    }

    unsigned long checkActionEntity = m_stepDetail.data.bgLaunchGuiApp().bTriggerEntityCheckAction ? m_stepDetail.activePlan.triggerEntity : m_stepDetail.data.bgLaunchGuiApp().dataPoint;

    //if (!RightsManager::getInstance().isRightsPermittedOnResource( checkActionEntity, m_stepDetail.data.bgLaunchGuiApp().actionTypeKey ))
    //{
    //    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Action %d on Data Point %lu not allowed.", 
    //        m_stepDetail.data.bgLaunchGuiApp().actionTypeKey, checkActionEntity );
    //    return;
    //}

    try
    {
        // Determine app position and size. Want the app on the same screen as the main Plan Manager window.
        RECT appBounds(TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
            TA_Base_Bus::TA_GenericGui::AREA_SCREEN, 0));

        unsigned long appPosType(TA_Base_Bus::TA_GenericGui::POS_BOUNDED);
        TA_Base_Core::BackgroundLaunchGuiAppParameters parameters(m_stepDetail.data.bgLaunchGuiApp());

        appBounds.left += parameters.pos.left;
        appBounds.top = parameters.pos.top;

        // Width and height are optional parameters of the Launch GUI App step.
        // If not specified, the default values for the app will be used.
        if (parameters.pos.width != static_cast<unsigned long>(-1) &&
            parameters.pos.height != static_cast<unsigned long>(-1))
        {
            appPosType |= TA_Base_Bus::TA_GenericGui::POS_RESIZABLE;

            appBounds.right = appBounds.left + parameters.pos.width;
            appBounds.bottom = appBounds.top + parameters.pos.height;
        }
        else
        {
            appBounds.right = appBounds.bottom = 0;
        }

        // Construct command line parameter
        std::stringstream commandline;
        commandline << parameters.args.in(); 

        if (parameters.bAddTriggerEntity)
        {
            commandline << "  --trigger-entity=";
            commandline << m_stepDetail.activePlan.triggerEntity;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start Launch Gui App request: app type:%lu, command line:%s", parameters.app, commandline.str().c_str());

        // Now launch the app
        TA_Base_Bus::AppLauncher::getInstance().launchApplication(parameters.app,
            commandline.str(),
            appPosType,
            0,
            NULL,
            &appBounds);
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "");
    }

    FUNCTION_EXIT;
}

}