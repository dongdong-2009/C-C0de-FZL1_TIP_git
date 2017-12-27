/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoMonitor.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoMonitor provides access to VideoMonitor entity data and a specific framework
  * for VideoMonitor entity parameters. 
  * Inherits from the EntityData and VideoOutput classes.
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    //
    // The entity type is fixed for all instances of this class.
    // If this is changed, please update app/cctv/video_switch_agent/src/EntityTypeConstants.h.
    //

    const std::string VideoMonitor::ENTITY_TYPE = "VideoMonitor";
    
    // 
    // The entity parameters for this entity type.
    //

    const std::string VideoMonitor::SIZE = "Size";
    const std::string VideoMonitor::DISPLAYORDER = "DisplayOrder";
	const std::string VideoMonitor::CONSOLE = "Console";
	const std::string VideoMonitor::ALARMMONITOR = "AlarmMonitor";
	const std::string VideoMonitor::ISOVERALLMONITOR = "IsOverallMonitor";

    //
    // Valid sizes.
    //

    const std::string VideoMonitor::SMALL  = "Small";
    const std::string VideoMonitor::MEDIUM = "Medium";
    const std::string VideoMonitor::LARGE  = "Large";

    //
    // VideoMonitor
    //
    VideoMonitor::VideoMonitor()
        : VideoOutput( ENTITY_TYPE )
    {
        // Do nothing.
    }


    //
    // VideoMonitor
    //
    VideoMonitor::VideoMonitor( unsigned long key )
        : VideoOutput( key, ENTITY_TYPE )
    {
    }
    
        
    //
    // getSize
    //
    std::string VideoMonitor::getSize()
    {
		// Get the parameters for this entity
		std::string size = SMALL;
		std::string value("");
		value = getHelper()->getParameter(SIZE);
		if ( ( SMALL  != value ) &&
			( MEDIUM != value ) &&
			( LARGE  != value )  )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Invalid VideoMonitor size: %s. Defaulting to %s", value.c_str(), SMALL.c_str() );
			size = SMALL;       // Default to "Small"
		}
		else
		{
			size = value;
		}

        return size;
    }

    
    //
    // setSize
    //
    void VideoMonitor::setSize( const std::string& size )
    {  
        if ( ( SMALL  != size ) &&
             ( MEDIUM != size ) &&
             ( LARGE  != size )  )
        {
            TA_ASSERT( false, "The size is invalid." );
			/*
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Invalid VideoMonitor size: %s. Defaulting to %s", size.c_str(), SMALL.c_str() );
            setParameter( SIZE, SMALL);
			*/
        }
        else
        {
            setParameter( SIZE, size);
        }
    }

          
    //
    // getConsoleKey
    //
    unsigned long VideoMonitor::getConsoleKey()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(CONSOLE);
		unsigned long consoleKey = 0;
		if (!value.empty())
		{
            consoleKey = EntityHelper::getUnsignedLongData(value);
		}

        return consoleKey;
    }

	//
	// getDisplayOrder
	//
	unsigned int VideoMonitor::getDisplayOrder()
	{
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(DISPLAYORDER);
		unsigned long consoleKey = 0;
		if (!value.empty())
		{
			consoleKey = EntityHelper::getUnsignedLongData(value);
		}

		return consoleKey;
	}

	bool VideoMonitor::isAlarmMonitor()
	{
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(ALARMMONITOR);
		bool isAlarmMonitor = false;
		if (!value.empty())
		{
			isAlarmMonitor = EntityHelper::getBooleanData(value);
		}

		return isAlarmMonitor;
	}

	bool VideoMonitor::isOverallMonitor()
	{
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(ISOVERALLMONITOR);
		bool isOverallMonitor = false;
		if (!value.empty())
		{
			isOverallMonitor = EntityHelper::getBooleanData(value);
		}

		return isOverallMonitor;
	}

	unsigned int VideoMonitor::getAlarmType()
	{
		std::string value("");
		value = getHelper()->getParameter(ALARMMONITOR);
		unsigned int alarmType = 0;
		if (!value.empty())
		{
			alarmType = EntityHelper::getUnsignedLongData(value);
		}

		return alarmType;
	}
    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string VideoMonitor::getType()
    {
        return(getStaticType());
    }

    std::string VideoMonitor::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* VideoMonitor::clone( unsigned long key )
    {
        return new VideoMonitor( key );
    }

} // TA_Base_Core
