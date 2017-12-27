/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/ActionCreateIncidentReport.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class launches the Incident Manager with parameters that specify
  * the alarms to populate the report with.
  */


#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/ActionIds.h"
#include "app/event/event_viewer/src/ActionCreateIncidentReport.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ApplicationException.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

// Ctor
ActionCreateIncidentReport::ActionCreateIncidentReport()
{
}

// Dtor
ActionCreateIncidentReport::~ActionCreateIncidentReport()
{
}

unsigned int ActionCreateIncidentReport::getActionId()
{
    return ACTION_CREATE_INCIDENT_REPORT;
}

void ActionCreateIncidentReport::executeAction( Filter* filter )
{
    // Get the index of the first selected item.
    const std::vector<unsigned long>& selectedItems = filter->getSelectedItems();

    // The number of selected items has already been checked, but there is no harm
    // checking again.
    if(selectedItems.size() == 1)
    {
        // Get the ID of the alarm associated with the even.
        std::string alarmId( filter->getItemData( selectedItems[0], CombinedEventFilter::HIDDEN_COLUMN_ALARMID ) );

		//TD15870 xinsong++
       const std::string & eventValue = filter->getItemData( selectedItems[0], CombinedEventFilter::COLUMN_VALUE ) ;

        std::ostringstream commandLine;
		
		//TD15870 xinsong++
        commandLine << "--alarm-id=" << alarmId;
		CString strAlarmValue = eventValue.c_str();
		strAlarmValue.Replace(" ", "{%Blank%}");	// replace blank in planpath with {%Blank%} to accept the Plan Categories with blanks, 28.Mar.2007, hongzhi
        commandLine << "  --alarm-value=" << (LPCTSTR)strAlarmValue;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "xxs test++: alarmID=%s, eventValue=%s", 
			alarmId.c_str(), eventValue.c_str());

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

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launch Incident Manager for alarm: %s",alarmId.c_str());
        }
        catch(TA_Base_Core::ApplicationException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch Incident Manager");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_040017);
        }
    }
  
}
