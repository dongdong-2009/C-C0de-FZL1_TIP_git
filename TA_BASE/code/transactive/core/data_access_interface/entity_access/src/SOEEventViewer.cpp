

#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "SOEEventViewer.h"



namespace TA_Base_Core
{

	const std::string SOEEventViewer::ENTITY_TYPE = "SoeEventViewer";

	// 
	// Supported Features
	//
	const std::string SOEEventViewer::COLOUR_CODE_ALARM_EVENTS = "ColourCodeAlarmEvents";
	const std::string SOEEventViewer::EXPORT_FILE_PATH = "ExportFilePath";
	const std::string SOEEventViewer::DISABLE_CLOSE = "DisableClose";
	const std::string SOEEventViewer::MAX_EVENTS = "MaxEvents";
	const std::string SOEEventViewer::REFRESH_TIME = "EventRefreshTime";
	const std::string SOEEventViewer::DEFAULT_EVENT_VISIBILITY_LEVEL = "DefaultEventVisibilityLevel";
	//TD17888 marc_bugfix
	const std::string SOEEventViewer::EXPORT_DIRECTORY = "ExportDirectory";

	SOEEventViewer::SOEEventViewer(void)
	{
	}
	
	//
	// EventViewer
	//
	SOEEventViewer::SOEEventViewer( unsigned long key ) 
		: EntityData( key, ENTITY_TYPE )
	{
		FUNCTION_ENTRY( "EventViewer constructor" );
	}

	bool SOEEventViewer::areAlarmEventsColourCoded()
	{
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		bool areAlarmsEventsColourCoded = false;		
		return areAlarmsEventsColourCoded;
	}

	std::string SOEEventViewer::getDefaultExportLocation()
	{
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		std::string value = getHelper()->getParameter(EXPORT_FILE_PATH);
		std::string exportdirectory = "C:\\";
		if(!value.empty())
			exportdirectory = value;
		return exportdirectory;
	}


	bool SOEEventViewer::isCloseDisabled()
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

	unsigned long SOEEventViewer::getEventLimit()
	{
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		unsigned long maxEvents = 10000;
		std::string value = getHelper()->getParameter(MAX_EVENTS);
		if (!value.empty())
		{
			maxEvents = EntityHelper::getUnsignedLongData(value);
		}
		return maxEvents;
	}


	unsigned long SOEEventViewer::getEventRefreshTime()
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

	unsigned long SOEEventViewer::getDefaultEventVisibilityLevel()
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

	std::string SOEEventViewer::getExportDirectory()
	{
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		std::string value = getHelper()->getParameter(EXPORT_DIRECTORY);
		std::string exportdirectory = "C:\\";
		if(!value.empty())
			exportdirectory = value;
		return exportdirectory;
		//return getHelper()->getParameter(EXPORT_DIRECTORY);
	}

	///////////////////////////////////////////////////////////////////////
	//
	// IEntityData methods
	//
	///////////////////////////////////////////////////////////////////////

	//
	// getType
	//
	std::string SOEEventViewer::getType()
	{
		return(getStaticType());
	}


	std::string SOEEventViewer::getStaticType()
	{
		return(ENTITY_TYPE);
	}


	//
	// clone
	//
	IEntityData* SOEEventViewer::clone( unsigned long key )
	{
		return new SOEEventViewer( key );        
	}


	//
	// invalidate
	//
	void SOEEventViewer::invalidate()
	{
		getHelper()->invalidate();
	}

}