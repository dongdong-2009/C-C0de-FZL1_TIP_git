/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TisLogViewerEntityData.cpp $
  * @author Adam Radics 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * TisLogViewerEntityData provides access to TisLogViewerEntityData entity data 
  * and a specific framework for TisLogViewerEntityData entity parameters.
  */


#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/TisLogViewerEntityData.h"
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
    const std::string TisLogViewerEntityData::ENTITY_TYPE = "TISLogViewer";
   
    //
    // TisLogViewerEntityData
    //
    TisLogViewerEntityData::TisLogViewerEntityData()
        : EntityData()
    {
    }


    //
    // TisLogViewerEntityData
    //
    TisLogViewerEntityData::TisLogViewerEntityData( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
    }


    unsigned long TisLogViewerEntityData::getRefreshIntervalInSecs()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(REFRESH_INTERVAL_PARAMETER);
		unsigned long refreshInterval = 0;
		if (!value.empty())
		{
            refreshInterval = EntityHelper::getUnsignedLongData(value);
		}
		
        return refreshInterval;
    }

    
    void TisLogViewerEntityData::setRefreshIntervalInSecs( unsigned long interval )
    {
        // convert number to string
        std::ostringstream temp;
        temp << interval;

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
    std::string TisLogViewerEntityData::getType()
    {
        return(getStaticType());
    }


    //
    // getStaticType
    //
    std::string TisLogViewerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* TisLogViewerEntityData::clone( unsigned long key )
    {
        return new TisLogViewerEntityData( key );
    }
    
    
    //
    // invalidate
    //
    void TisLogViewerEntityData::invalidate()
    {
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        getHelper()->invalidate();
    }

        
    ///////////////////////////////////////////////////////////////////////
    //
    // Protected Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // setParameter (for string parameters)
    //
    void TisLogViewerEntityData::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );
    }

} // TA_Base_Core
