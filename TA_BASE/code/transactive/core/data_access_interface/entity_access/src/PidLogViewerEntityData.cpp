/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PidLogViewerEntityData.cpp $
  * @author Adam Radics 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PidLogViewerEntityData provides access to PidLogViewerEntityData entity data 
  * and a specific framework for PidLogViewerEntityData entity parameters.
  */


#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/PidLogViewerEntityData.h"
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
    static const std::string REFRESH_INTERVAL_PARAMETER = "RefreshIntervalInSeconds";
}


namespace TA_Base_Core
{
    const std::string PidLogViewerEntityData::ENTITY_TYPE = "PIDLogViewer";
   
    //
    // PidLogViewerEntityData
    //
    PidLogViewerEntityData::PidLogViewerEntityData()
        : EntityData()
    {
	}


    //
    // PidLogViewerEntityData
    //
    PidLogViewerEntityData::PidLogViewerEntityData( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
        
    }


    unsigned long PidLogViewerEntityData::getRefreshIntervalInSecs()
    {
		std::string value = getHelper()->getParameter(REFRESH_INTERVAL_PARAMETER);
		unsigned long refreshInterval = 0;
		try
		{
			if (!value.empty())
			{
				refreshInterval = EntityHelper::getIntegerData(value);
			}
		}
		catch (DataException& de)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", de.what());
		}

        return refreshInterval;
    }

    
    void PidLogViewerEntityData::setRefreshIntervalInSecs( unsigned long interval )
    {
        // convert number to string
        std::ostringstream temp;
        temp << static_cast<unsigned long> (interval);

        setParameter( REFRESH_INTERVAL_PARAMETER, temp.str());
    }


  

    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string PidLogViewerEntityData::getType()
    {
        return(getStaticType());
    }


    //
    // getStaticType
    //
    std::string PidLogViewerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* PidLogViewerEntityData::clone( unsigned long key )
    {
        return new PidLogViewerEntityData( key );
    }
    
    
    //
    // invalidate
    //
    void PidLogViewerEntityData::invalidate()
    {
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        getHelper()->invalidate();
    }


    //
    // setParameter (for string parameters)
    //
    void PidLogViewerEntityData::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );
    }

} // TA_Base_Core
