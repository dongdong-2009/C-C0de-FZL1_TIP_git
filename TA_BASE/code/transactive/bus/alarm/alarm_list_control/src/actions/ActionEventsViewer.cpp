/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/actions/ActionEventsViewer.cpp $
 * @author:  Derrick Liew
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class is for the action that launches the Event Viewer displaying all events
 */
// ActionEventsViewer.cpp: implementation of the ActionEventsViewer class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include <memory>

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionEventsViewer.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/ExtAlarmActionChecker.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "bus/generic_gui/src/AppLauncher.h"

#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/ApplicationException.h"
// #include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    ActionEventsViewer::ActionEventsViewer(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu)
    {
    }

    
    ActionEventsViewer::~ActionEventsViewer()
    {
    }


    bool ActionEventsViewer::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

		// Events Viewer at toolbar is always available as it is not linked to any alarms

        FUNCTION_EXIT;
		return true;
    }


    void ActionEventsViewer::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::stringstream commandLine;
        commandLine << "--FilterDefault=0"; // argument is not necessary

        using namespace TA_Base_Bus::TA_GenericGui;

        CPoint pt(GetMessagePos());
        try
        {
            RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                       AREA_SCHEMATIC,
                                                                       pt.x);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Event Viewer (%s) at xPos: %d yPos: %d and it should resize to fit on a single screen.", 
				commandLine.str().c_str(), position.left, position.top);

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
} // namespace end
