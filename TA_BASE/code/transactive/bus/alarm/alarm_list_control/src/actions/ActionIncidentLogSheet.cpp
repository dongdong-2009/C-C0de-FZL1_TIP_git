/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionIncidentLogSheet.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that launched the related incident log sheet for the alarm
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionIncidentLogSheet.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/IncidentManagerData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

namespace TA_Base_Bus
{
    ActionIncidentLogSheet::ActionIncidentLogSheet(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu),
          m_incidentManagerKey(0)
    {
    }

    
    ActionIncidentLogSheet::~ActionIncidentLogSheet()
    {
    }


    bool ActionIncidentLogSheet::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            FUNCTION_EXIT;
            return false;
        }

		//ATS Alarm check
		if (alarmRetriever.getNumberAttr(selectedAlarms[0], IS_EXT_ALARM))
		{
			std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm(alarmRetriever.getAlarmItem(selectedAlarms[0]));
			if (alarm.get() == NULL)
				return false;

			FUNCTION_EXIT;
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), INCIDENT_LOG_SHEET);
		}

        FUNCTION_EXIT;
		return alarmRetriever.getRightsChecker().canPerformAction(getIncidentManagerKey(), 
                    TA_Base_Bus::aca_RUN_APPLICATION);
    }


    void ActionIncidentLogSheet::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // Function only available when one alarm is selected
            FUNCTION_EXIT;
            return;
        }

        //std::string alarmId = selectedAlarms[0];
        std::ostringstream commandLine;
		
		//TD15870 xinsong++
		AlarmDataItem * pTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
		if (pTemp == NULL) return;

		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm(pTemp);
//		TA_Assert( alarm != NULL );
        commandLine << "--alarm-id=" << alarm->alarmID;
		CString strAlarmValue = alarm->alarmValue.c_str();
		strAlarmValue.Replace(" ", "{%Blank%}");	// replace blank in planpath with {%Blank%} to accept the Plan Categories with blanks, 28.Mar.2007, hongzhi
        commandLine << "  --alarm-value=" << (LPCTSTR)strAlarmValue;

		LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "xxs test++: alarmID=%s, alarmValue=%s", 
			alarm->alarmID.c_str(), alarm->alarmValue.c_str());
        
        using namespace TA_Base_Bus::TA_GenericGui;

        CPoint pt(GetMessagePos());
        try
        {
            RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                       AREA_SCHEMATIC,
                                                                       pt.x);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Launching Incident Manager at xPos: %d yPos: %d and it should resize to fit on a single screen.", position.left, position.top);
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(INCIDENT_MANAGER_GUI_APPTYPE,
                                                                 commandLine.str(),
                                                                 POS_BOUNDED|POS_RESIZABLE,
                                                                 ALIGN_FIT|ALIGN_LEFT|ALIGN_TOP,
                                                                 NULL,
                                                                 &position);
        }
        catch(TA_Base_Core::ApplicationException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Incident Manager");
            TA_THROW( TA_Base_Core::AlarmActionException(TA_Base_Core::AlarmActionException::CANNOT_LAUNCH_INCIDENT_MANAGER) );
        }

        FUNCTION_EXIT;
    }
	void ActionIncidentLogSheet::performPreAction()
	{
		getIncidentManagerKey();
	}

    unsigned long ActionIncidentLogSheet::getIncidentManagerKey(void)
    {
        FUNCTION_ENTRY("getIncidentManagerKey");
        // if it's not already cached we need to look it up
        if (0 == m_incidentManagerKey)
        {
            try
            {
                TA_Base_Core::IEntityDataList incidentManagers = 
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::IncidentManagerData::getStaticType(),0);//for fast query who knows the reason

                if (1 == incidentManagers.size())
                {
                    m_incidentManagerKey = incidentManagers[0]->getKey();
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "There are %lu incident manager entities in the database. There should be one only.",
                        incidentManagers.size());
                    if ( incidentManagers.size() > 1)
                    {
                        // use the first one
                        m_incidentManagerKey = incidentManagers[0]->getKey();
                    }
                }

                // free the entity data
                while (! incidentManagers.empty())
                {
                    delete incidentManagers.back();
                    incidentManagers.pop_back();
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Could not look up the Incident Manager entity key");
            }
        }

        FUNCTION_EXIT;
        return m_incidentManagerKey;
    }


}
