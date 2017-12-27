/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedEvents.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that launches the Event Viewer displaying the related events
 */

#pragma warning(disable:4786)

#include <memory>

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedEvents.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/generic_gui/src/AppLauncher.h"

#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionRelatedEvents::ActionRelatedEvents(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu)
    {
    }

    
    ActionRelatedEvents::~ActionRelatedEvents()
    {
    }


    bool ActionRelatedEvents::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

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
			return ExtAlarmActionChecker::getInstance().isActionEnabled(alarm.get(), RELATED_EVENTS);
		}

        FUNCTION_EXIT;
		return true;
    }


    void ActionRelatedEvents::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // Function only available when one alarm is selected
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the alarm information that needs to be passed to the event viewer.
		AlarmDataItem * pdataTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
		if (pdataTemp == NULL) return;

		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm (pdataTemp);
        //std::auto_ptr<TA_Base_Core::IAlarmTypeData> alarmType( TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmType(
        //    alarm->messageTypeKey.in(), false ) ); // Why does 'messageTypeKey' contain the type name??
		// The messageTypeKey of AlarmDetailCorbaDef should be the alarm type key instead of the alarm type name


        // TD7538: Events are now filter based on asset and alarm type.
        std::stringstream commandLine;
        commandLine << "--FilterAsset=" << alarm->assetName;
        commandLine << " --FilterAlarmtype=" << alarm->messageTypeKey;

        using namespace TA_Base_Bus::TA_GenericGui;

        CPoint pt(GetMessagePos());
        try
        {
            RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                       AREA_SCHEMATIC,
                                                                       pt.x);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Event Viewer (%s) at xPos: %d yPos: %d and it should resize to fit on a single screen.", commandLine.str().c_str(), position.left, position.top);
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
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_EVENT_VIEWER) );
        }

        FUNCTION_EXIT;
    }
}
