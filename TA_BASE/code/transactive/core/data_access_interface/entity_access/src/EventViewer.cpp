/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EventViewer.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * EventViewer provides access to the Event Viewer entity for loading configuration data.
  * It inherits from EntityData.
  */

#if defined(_MSC_VER)
#pragma warning( disable: 4786 )
#endif // defined (_MSC_VER)

#include "core/data_access_interface/entity_access/src/EventViewer.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    // 
    // The entity type is fixed for all instances of this class.
    // If this is changed, please update app/cctv/video_switch_agent/src/EntityTypeConstants.h.
    // 
    const std::string EventViewer::ENTITY_TYPE = "EventViewer";

    // 
    // Supported Features
    //
    const std::string EventViewer::COLOUR_CODE_ALARM_EVENTS = "ColourCodeAlarmEvents";
    const std::string EventViewer::EXPORT_FILE_PATH = "ExportFilePath";
    const std::string EventViewer::DISABLE_CLOSE = "DisableClose";
    const std::string EventViewer::MAX_EVENTS = "MaxEvents";
    const std::string EventViewer::REFRESH_TIME = "EventRefreshTime";
	const std::string EventViewer::REFRESH_TIME_AGENT = "EventRefreshTimeAgent";
	const std::string EventViewer::DEFAULT_EVENT_VISIBILITY_LEVEL = "DefaultEventVisibilityLevel";
	//TD17888 marc_bugfix
	const std::string EventViewer::EXPORT_DIRECTORY = "ExportDirectory";



    //
    // EventViewer
    //
    EventViewer::EventViewer()
    {
    }


    //
    // EventViewer
    //
    EventViewer::EventViewer( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
        FUNCTION_ENTRY( "EventViewer constructor" );
    }


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////


    bool EventViewer::areAlarmEventsColourCoded()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        bool areAlarmsEventsColourCoded = false;
		std::string value = getHelper()->getParameter(COLOUR_CODE_ALARM_EVENTS);
		if (!value.empty())
		{
			areAlarmsEventsColourCoded = EntityHelper::getBooleanData(value);
		}
        return areAlarmsEventsColourCoded;
    }


    std::string EventViewer::getDefaultExportLocation()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		return getHelper()->getParameter(EXPORT_FILE_PATH);
    }


    bool EventViewer::isCloseDisabled()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		bool isCloseDisabled = false;
		std::string value = getHelper()->getParameter(DISABLE_CLOSE);
		if (!value.empty())
		{
			isCloseDisabled = EntityHelper::getBooleanData(value);
		}
        return isCloseDisabled;
    }


    unsigned long EventViewer::getEventLimit()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		unsigned long maxEvents = 0;
		std::string value = getHelper()->getParameter(MAX_EVENTS);
		if (!value.empty())
		{
			maxEvents = EntityHelper::getUnsignedLongData(value);
		}
        return maxEvents;
    }


    unsigned long EventViewer::getEventRefreshTime()
    {
        TA_ASSERT( getHelper() != NULL, "The EntityHelper pointer is null" );
        unsigned long refreshTime = 2;
		std::string value = getHelper()->getParameter(REFRESH_TIME);
		if (!value.empty())
		{
			refreshTime = EntityHelper::getUnsignedLongData(value);
		}
        return refreshTime;
    }

	unsigned long EventViewer::getEventRefreshTimeAgent()
	{
		TA_ASSERT( getHelper() != NULL, "The EntityHelper pointer is null" );
		unsigned long refreshTime = 200;
		std::string value = getHelper()->getParameter(REFRESH_TIME_AGENT);
		if (!value.empty())
		{
			refreshTime = EntityHelper::getUnsignedLongData(value);
		}
		return refreshTime;
	}
	unsigned long EventViewer::getDefaultEventVisibilityLevel()
    {
        TA_ASSERT( getHelper() != NULL, "The EntityHelper pointer is null" );
		std::string value = getHelper()->getParameter(DEFAULT_EVENT_VISIBILITY_LEVEL);
		unsigned long defaultEventVisibilityLevel = 0;
		if (!value.empty())
		{
			defaultEventVisibilityLevel = EntityHelper::getUnsignedLongData(value);
		}
        return defaultEventVisibilityLevel;
    }

    std::string EventViewer::getExportDirectory()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		return getHelper()->getParameter(EXPORT_DIRECTORY);
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string EventViewer::getType()
    {
        return(getStaticType());
    }


    std::string EventViewer::getStaticType()
    {
        return(ENTITY_TYPE);
    }

	
    //
    // clone
    //
    IEntityData* EventViewer::clone( unsigned long key )
    {
        return new EventViewer( key );        
    }


    //
    // invalidate
    //
    void EventViewer::invalidate()
    {
        getHelper()->invalidate();
    }

} // TA_Base_Core
