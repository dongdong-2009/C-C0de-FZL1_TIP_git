/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/ActivePlanAgency.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2013/11/14 14:36:03 $
  * Last modified by:  $Author: huangjian $
  *
  */
#include <afxwin.h> 
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "app/response_plans/plan_mft_processor/src/RightsManager.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/LaunchSchematicActiveStep.h"

namespace TA_Base_App
{


    LaunchSchematicActiveStep::LaunchSchematicActiveStep( const TA_Base_Core::CurrentPlanStepDetail& stepDetail ):
    ActivePlanStep(stepDetail)
    {
        FUNCTION_ENTRY( "LaunchSchematicActiveStep" );

        FUNCTION_EXIT;
    }

    void LaunchSchematicActiveStep::execute()
    {
        launch();
    }

    void LaunchSchematicActiveStep::stop()
    {
        FUNCTION_ENTRY( "stop" );

        FUNCTION_EXIT;
    }

    LaunchSchematicActiveStep::~LaunchSchematicActiveStep()
    {
        FUNCTION_ENTRY( "~LaunchSchematicActiveStep" );

        FUNCTION_EXIT;
    }

    void LaunchSchematicActiveStep::launch()
    {
        FUNCTION_ENTRY( "launch" );

        if ( !RightsManager::getInstance().isProfileMatched( m_stepDetail.data.bgLaunchSchematic().profile ))
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not lunch the Schematic, request profile = %d", m_stepDetail.data.bgLaunchSchematic().profile);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            ActivePlanAgency::instance()->getPlanAgent()->processBackgroundLaunchSchematicStepResponse( m_stepDetail, TA_Base_Core::PSCS_SUCCESS);
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute MftMessageActiveStep fail");
        }

        //if (!RightsManager::getInstance().isRightsPermittedOnResource( m_stepDetail.data.bgLaunchSchematic().dataPoint, m_stepDetail.data.bgLaunchSchematic().actionTypeKey ))
        //{
        //    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Action %d on Data Point %lu not allowed.", 
        //        m_stepDetail.data.bgLaunchSchematic().actionTypeKey, m_stepDetail.data.bgLaunchSchematic().dataPoint );
        //    return;
        //}

        // Launch schematic
        try
        {
            // Get the target screen number as selected in the screen list.

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "Target screen for schematic: %d", m_stepDetail.data.bgLaunchSchematic().screen);

            // Determine the coordinate of the left side of the target screen.
            RECT screenBounds(TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC,
                TA_Base_Bus::TA_GenericGui::AREA_SCREEN,
                m_stepDetail.data.bgLaunchSchematic().screen));

            // If additional schematic commands have been configured for this step, append them
            // to the schematic's path. The protocol tag to be used here is PLANS.
            
            std::string schematic(m_stepDetail.data.bgLaunchSchematic().schematic.in());
            std::string commands(m_stepDetail.data.bgLaunchSchematic().schematic.in());

            if (!commands.empty())
            {
                if (commands.find("#PLANS,") != 0)
                {
                    commands = "#PLANS," + commands;
                }

                schematic += commands;
            }

            // Now launch the schematic.
            TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(schematic, "", screenBounds.left);
        }
        catch (TA_Base_Core::TransactiveException &ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,    "TransactiveException", ex.what());

        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,    "UnknownException", "");
        }

        FUNCTION_EXIT;
    }

}