/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DisplayManagerEntityData.cpp $
  * @author Adam Radics 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * DisplayManagerEntityData provides access to DisplayManagerEntityData entity data 
  * and a specific framework for DisplayManagerEntityData entity parameters.
  */


#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

namespace
{
    // 
    // The entity parameters for this entity type.
    //
    static const std::string DEFAULT_DWELL_TIME = "DefaultDwellTime";

    static const std::string LEFT_TOP_MONITOR = "LeftTopWallMonitor";
    static const std::string LEFT_MIDDLE_MONITOR = "LeftMiddleWallMonitor";
    static const std::string LEFT_BOTTOM_MONITOR = "LeftBottomWallMonitor";
    static const std::string RIGHT_TOP_MONITOR = "RightTopWallMonitor";
    static const std::string RIGHT_MIDDLE_MONITOR = "RightMiddleWallMonitor";
    static const std::string RIGHT_BOTTOM_MONITOR = "RightBottomWallMonitor";
    static const std::string ODS_MONITOR = "ODSWallMonitor";
	static const std::string LEFT_MIDDLE_TOP_MONITOR = "LeftMidTopWallMonitor";
	static const std::string RIGHT_MIDDLE_TOP_MONITOR = "RightMidTopWallMonitor";
	static const std::string LEFT_MIDDLE_BOTTOM_MONITOR = "LeftMidBottomWallMonitor";
	static const std::string RIGHT_MIDDLE_BOTTOM_MONITOR = "RightMidBottomWallMonitor";
}


namespace TA_Base_Core
{
    const std::string DisplayManagerEntityData::ENTITY_TYPE = "DisplayManager";
   
    //
    // DisplayManagerEntityData
    //
    DisplayManagerEntityData::DisplayManagerEntityData()
        : EntityData()
    {
    }


    //
    // DisplayManagerEntityData
    //
    DisplayManagerEntityData::DisplayManagerEntityData( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
    }


    unsigned short DisplayManagerEntityData::getDefaultDwellTime()
    {
		std::string value = getHelper()->getParameter(DEFAULT_DWELL_TIME);
		unsigned short defaultDwellTime = 5;
		try
		{
			if (!value.empty())
			{
				defaultDwellTime = static_cast< unsigned short > (EntityHelper::getUnsignedLongData(value));
			}

			if (defaultDwellTime < 1)
			{
				defaultDwellTime =1;
			}
			else if (defaultDwellTime > 99)
			{
				defaultDwellTime = 99;
			}
		}
		catch (DataException& de)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", de.what());
			// invalid data - not critical so dont throw on
			// TES 725 - default value of 5 seconds.
			defaultDwellTime = 5;
		}
		
        return defaultDwellTime;
    }

    
    void DisplayManagerEntityData::setDefaultDwellTime( unsigned short defaultDwellTime )
    {
        // cap values at limits
        if (defaultDwellTime < 1)
        {
            defaultDwellTime = 1;
        }
        else if (defaultDwellTime > 99)
        {
            defaultDwellTime = 99;
        }

        // convert number to string
        std::ostringstream temp;
        temp << static_cast<unsigned long> (defaultDwellTime);

        setParameter( DEFAULT_DWELL_TIME, temp.str());
    }


    unsigned long DisplayManagerEntityData::getLeftTopWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(LEFT_TOP_MONITOR);
		unsigned long leftTopWallMonitorKey = 0;
		if (!value.empty())
		{
			leftTopWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return leftTopWallMonitorKey;
    }

    
    unsigned long DisplayManagerEntityData::getLeftMiddleWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(LEFT_MIDDLE_MONITOR);
		unsigned long leftMiddleWallMonitorKey = 0;
		if (!value.empty())
		{
			leftMiddleWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}
		
        return leftMiddleWallMonitorKey;
    }

    
    unsigned long DisplayManagerEntityData::getLeftBottomWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(LEFT_BOTTOM_MONITOR);
		unsigned long leftBottomWallMonitorKey = 0;
		if (!value.empty())
		{
			leftBottomWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return leftBottomWallMonitorKey;
    }

    
    unsigned long DisplayManagerEntityData::getRightTopWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(RIGHT_TOP_MONITOR);
		unsigned long rightTopWallMonitorKey = 0;
		if (!value.empty())
		{
			rightTopWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return rightTopWallMonitorKey;
    }

    
    unsigned long DisplayManagerEntityData::getRightMiddleWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(RIGHT_MIDDLE_MONITOR);
		unsigned long rightMiddleWallMonitorKey = 0;
		if (!value.empty())
		{
			rightMiddleWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return rightMiddleWallMonitorKey;
    }

    
    unsigned long DisplayManagerEntityData::getRightBottomWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(RIGHT_BOTTOM_MONITOR);
		unsigned long rightBottomWallMonitorKey  = 0;
		if (!value.empty())
		{
			rightBottomWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return rightBottomWallMonitorKey;
    }


	unsigned long DisplayManagerEntityData::getLeftMidTopWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(LEFT_MIDDLE_TOP_MONITOR);
		unsigned long rightBottomWallMonitorKey  = 0;
		if (!value.empty())
		{
			rightBottomWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return rightBottomWallMonitorKey;
    }
   

    unsigned long DisplayManagerEntityData::getRightMidTopWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(RIGHT_MIDDLE_TOP_MONITOR);
		unsigned long rightBottomWallMonitorKey  = 0;
		if (!value.empty())
		{
			rightBottomWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return rightBottomWallMonitorKey;
    }


    unsigned long DisplayManagerEntityData::getLeftMidBottomWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(LEFT_MIDDLE_BOTTOM_MONITOR);
		unsigned long rightBottomWallMonitorKey  = 0;
		if (!value.empty())
		{
			rightBottomWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return rightBottomWallMonitorKey;
    }


    unsigned long DisplayManagerEntityData::getRightMidBottomWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(RIGHT_MIDDLE_BOTTOM_MONITOR);
		unsigned long rightBottomWallMonitorKey  = 0;
		if (!value.empty())
		{
			rightBottomWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return rightBottomWallMonitorKey;
    }

	
    unsigned long DisplayManagerEntityData::getOdsWallMonitorKey()
    {
		std::string value = getHelper()->getParameter(ODS_MONITOR);
		unsigned long odsWallMonitorKey = 0;
		if (!value.empty())
		{
			odsWallMonitorKey = EntityHelper::getUnsignedLongData(value);
		}

        return odsWallMonitorKey;
    }

    

    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string DisplayManagerEntityData::getType()
    {
        return(getStaticType());
    }


    //
    // getStaticType
    //
    std::string DisplayManagerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* DisplayManagerEntityData::clone( unsigned long key )
    {
        return new DisplayManagerEntityData( key );
    }
    
    
    //
    // invalidate
    //
    void DisplayManagerEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

        
    ///////////////////////////////////////////////////////////////////////
    //
    // Protected Methods
    //
    ///////////////////////////////////////////////////////////////////////



    //
    // validateData
    //
    void DisplayManagerEntityData::validateData()
    {
		getHelper()->reload();
    }


    //
    // setParameter (for string parameters)
    //
    void DisplayManagerEntityData::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );
    }

} // TA_Base_Core
