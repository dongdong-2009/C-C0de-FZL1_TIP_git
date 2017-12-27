/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedAvalancheEvents.cpp $
 * @author:  Ben Robbins
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that displays related avalanche events
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedAvalancheEvents.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/generic_gui/src/AppLauncher.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EventViewer.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/AlarmActionException.h"

namespace TA_Base_Bus
{
    ActionRelatedAvalancheEvents::ActionRelatedAvalancheEvents(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu),
        m_eventViewerKey(0)
    {
    }

    
    bool ActionRelatedAvalancheEvents::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.size() != 1)
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
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), RELATED_AVALANCHE_EVENTS);
		}

        // Before we can check if this alarm has related child alarms, 
        // need to work out if the user can acutally use the event viewer.
        // Now determine if we have the rights to do this
        if (alarmRetriever.getRightsChecker().canPerformAction(getEventViewerKey(),TA_Base_Bus::aca_RUN_APPLICATION))
        {
            // First retrieve the entity details from the alarm
            try
            {   
				alarm.reset(alarmRetriever.getAlarmItem(selectedAlarms[0]));

				if (alarm.get() == NULL) return false;
				char avalanche= alarm->avalanche[0];
				if('H' == avalanche)
				{
                    //delete alarm;
			        FUNCTION_EXIT;
                    return true;
				}
            }
            catch( const TA_Base_Core::AlarmListCtrlException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine plan associations");
            }
            catch( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the alarm plan associations from the database");
            }
            catch( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the alarm plan associations from the database");
            }
        }

        FUNCTION_EXIT;
		return false;
    }


    void ActionRelatedAvalancheEvents::performAction(IAlarmRetriever& alarmRetriever)
    {
		FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // Function only available when one alarm is selected
            FUNCTION_EXIT;
            return;
        }

		// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name
		// And the alarmType is actually not used in this function

		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm(alarmRetriever.getAlarmItem(selectedAlarms[0]));
        std::stringstream commandLine;
        commandLine << "--AlarmId=" << alarm->alarmID; // append the avalanche alarm-id
	
		// Check whether this is the avalanche head		
		if ('H' == alarm->avalanche[0])
		{
			std::string headAlarmID( alarm->alarmID );
			// Retrieve the children id, if any and pass in as commandLine
			std::vector<std::string> childrenId; // Alarm ID of children
			
			alarmRetriever.getAvalanchedChildAlarms(headAlarmID, childrenId);

			if (childrenId.size() > 0)
			{
				commandLine << " --AlarmChild=";
				for ( int i=0; i<childrenId.size(); ++i )
				{
					commandLine << childrenId[i].c_str();
					// check whether it is the last id. If not, add a comma separator
					if ( i < childrenId.size()-1 )
					{
						commandLine << ",";
					}
				}
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Command Line string: %s", commandLine.str().c_str() );
		}
        
        using namespace TA_Base_Bus::TA_GenericGui;

        CPoint pt(GetMessagePos());
        try
        {
            RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                       AREA_SCHEMATIC,
                                                                       pt.x);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Launching Event Viewer (%s) at xPos: %d yPos: %d and it should resize to fit on a single screen.", commandLine.str().c_str(), position.left, position.top);
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(EVENT_GUI_APPTYPE,
                                                                 commandLine.str(),
                                                                 POS_BOUNDED|POS_RESIZABLE,
                                                                 ALIGN_FIT|ALIGN_LEFT|ALIGN_TOP,
                                                                 NULL,
                                                                 &position);
        }
        catch(TA_Base_Core::ApplicationException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch event viewer");
            TA_THROW( TA_Base_Core::AlarmActionException(TA_Base_Core::AlarmActionException::CANNOT_LAUNCH_EVENT_VIEWER) );
        }

        FUNCTION_EXIT;
    }


	void ActionRelatedAvalancheEvents::preformPreAction()
	{
		getEventViewerKey();
	}

    unsigned long ActionRelatedAvalancheEvents::getEventViewerKey()
    {
        FUNCTION_ENTRY("getEventViewerKey");

        if (0 == m_eventViewerKey)
        {
            try
            {
                TA_Base_Core::IEntityDataList eventViewers = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::EventViewer::getStaticType(),0);//for fast query who knows the reason

                if (1 == eventViewers.size())
                {
                    m_eventViewerKey = eventViewers[0]->getKey();
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "There are %lu event viewer entities in the database. There should be one only.",
                        eventViewers.size());
                    if ( eventViewers.size() > 1)
                    {
                        // use the first one
                        m_eventViewerKey = eventViewers[0]->getKey();
                    }
                }

                // free the entity data
                while (! eventViewers.empty())
                {
                    delete eventViewers.back();
                    eventViewers.pop_back();
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Could not look up the Event Viewer entity key");
            }
        }

        FUNCTION_EXIT;
        return m_eventViewerKey;
    }
}
