/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedPlanInstance.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * This class is for the action that allows the originating plan instance to be controlled.
 */

#pragma warning(disable:4786)

#include <sstream>

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedPlanInstance.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/PlanAssociationDialog.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"

#include "core/message/types/PlanAgentAlarms_MessageTypes.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionRelatedPlanInstance::ActionRelatedPlanInstance(bool isAvailableInContextMenu /*=true*/)
        : m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper()),
          m_isAvailableInContextMenu(isAvailableInContextMenu),
          m_planManagerKey(0)
    {

    }

    
    ActionRelatedPlanInstance::~ActionRelatedPlanInstance()
    {
        try
        {
            TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
            TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().cleanUp();
        }
        catch( ... )
        {
        }
    }


    bool ActionRelatedPlanInstance::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            // Already in the process of an action like this so this is not available
            FUNCTION_EXIT;
            return false;
        }

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return false;
        }
        
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm;
		//ATS Alarm check
		if (alarmRetriever.getNumberAttr(selectedAlarms[0], IS_EXT_ALARM))
		{
			alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
			if (alarm.get() == NULL) return false;

			FUNCTION_EXIT;
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), RELATED_PLAN_INSTANCE);
		}

        // Now determine if we have the rights to do this
        if (alarmRetriever.getRightsChecker().canPerformAction(getPlanManagerKey(),TA_Base_Bus::aca_RUN_APPLICATION))
        {
            // First retrieve the entity details from the alarm            
            try
            {
				alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));
				if (alarm.get() == NULL) return false;

                // The messageTypeKey member actually holds the alarm type name
				// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name
				/*
                std::string alarmTypeKey = alarmRetriever.getDatabaseCache().getAlarmTypeKey(alarm->messageTypeKey.in());
                if (alarmTypeKey.empty())
                {
                    delete alarm;
                    return false;
                }
				*/

				std::stringstream alarmTypeKey;
				alarmTypeKey << alarm->messageTypeKey;
                
                //delete alarm;
					 //only enable for alarms of type PlanStepExecutionFailure
                return ( alarmTypeKey.str().compare (TA_Base_Core::PlanAgentAlarms::PlanStepExecutionFailure.getTypeKey()) == 0);

            }
            catch( const TA_Base_Core::AlarmListCtrlException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine associated plan instance");
            }
        }

        FUNCTION_EXIT;
		return false;
    }


    void ActionRelatedPlanInstance::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return;
        }

        // First retrieve the entity details from the alarm
        
		std::auto_ptr<TA_Base_Core::AlarmModifyItem> alarm;
        try
        {
			std::auto_ptr<TA_Base_Core::AlarmModifyItem> pTemp(alarmRetriever.getAlarmModifyItem(selectedAlarms[0]));
			alarm = pTemp;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine plan associations");

            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_FIND_ASSOCIATED_PLAN) );
        }
       
				// Plan Manager only needs alarmID in the following format:
				// --assoc-alarm-id=<alarm ID>
				// The planID and plan instance will be obtained by Plan Manager
				// from the Plan Agent which will have a local map of unique 
				// alarmIDs with planIDs and plan instances
				std::ostringstream commandLine;
				commandLine << "--assoc-alarm-id=" << selectedAlarms[0] << "@" << alarm->locationKey;
				

            using namespace TA_Base_Bus::TA_GenericGui;

            CPoint pt(GetMessagePos());

            try
            {
                RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                           AREA_SCHEMATIC,
                                                                           pt.x);

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Plan manager at xPos: %d yPos: %d and it should resize to fit on a single screen --assoc-alarm-id %s", position.left, position.top, commandLine.str().c_str());   
                TA_Base_Bus::AppLauncher::getInstance().launchApplication(PLAN_MANAGER_GUI_APPTYPE,
                                                                     commandLine.str(),
                                                                     POS_BOUNDED|POS_RESIZABLE,
                                                                     ALIGN_FIT|ALIGN_LEFT|ALIGN_TOP,
                                                                     NULL,
                                                                     &position);
            }
            catch(TA_Base_Core::ApplicationException&)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch plan manager");

         //       if (alarm != NULL)
//                {
//                    delete alarm;
//                    alarm = NULL;
     //           }

                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_RUN_ASSOCIATED_PLAN) );
            }

            try
            {
                // Let the alarm agent know the plan has been run
                m_alarmHelper.setAssociatedPlanWasRun(alarm.get());
            }
            catch( const TA_Base_Core::NoAlarmAgentException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", "Could not set that the plan was run on the agent");

                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_RUN_ASSOCIATED_PLAN) );
             }
       
        FUNCTION_EXIT;
    }

	void ActionRelatedPlanInstance::preformPreAction()
	{
		getPlanManagerKey();
	}

    unsigned long ActionRelatedPlanInstance::getPlanManagerKey()
    {
        FUNCTION_ENTRY("getPlanManagerKey");

        if (0 == m_planManagerKey)
        {
            try
            {
                TA_Base_Core::IEntityDataList planManagers = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::PlanManagerEntityData::getStaticType(), 0); //for fast query who knows the reason

                if (1 == planManagers.size())
                {
                    m_planManagerKey = planManagers[0]->getKey();
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "There are %lu plan managers entities in the database. There should be one only.",
                        planManagers.size());
                    if ( planManagers.size() > 1)
                    {
                        // use the first one
                        m_planManagerKey = planManagers[0]->getKey();
                    }
                }

                // free the entity data
                while (! planManagers.empty())
                {
                    delete planManagers.back();
                    planManagers.pop_back();
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Could not look up the Plan Managers entity key");
            }
        }

        FUNCTION_EXIT;
        return m_planManagerKey;
    }
}
