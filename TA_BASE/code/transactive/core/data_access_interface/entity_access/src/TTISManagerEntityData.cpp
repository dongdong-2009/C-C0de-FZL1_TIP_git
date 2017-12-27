/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/CODE_TA_BASE_DDB/transactive/core/data_access_interface/entity_access/src/TTISManagerEntityData.cpp $
  * @author zhang hongzhi 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/03/27 15:27:00 $
  * Last modified by: $Author: trainteam $
  * 
  * TTISManagerEntityData provides access to TTISManagerEntityData entity data
  * and a specific framework for TTISManagerEntityData entity parameters.
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/TTISManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DbConnection.h"
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
  
    const std::string MAX_SCHEDULE_MESSAGE        = "MaxScheduleMessage";
}


namespace TA_Base_Core
{
    //
    // The entity type is fixed for all instances of this class.
    // If this is changed, please update app/cctv/video_switch_agent/src/EntityTypeConstants.h.
    //

    const std::string TTISManagerEntityData::ENTITY_TYPE = "TTISManager";
   
    //
    // TTISManagerEntityData
    //
    TTISManagerEntityData::TTISManagerEntityData()
        : EntityData()
    {
        FUNCTION_ENTRY( "TTISManagerEntityData constructor" );
    }


    //
    // TTISManagerEntityData
    //
    TTISManagerEntityData::TTISManagerEntityData( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
        FUNCTION_ENTRY( "TTISManagerEntityData constructor" );
    }


    //
    // getAutoTTISDirectory
    //
    int TTISManagerEntityData::getMaxScheduleMessage()
    {
		int nMaxScheduleMessage = 1;
		std::string value = getHelper()->getParameter(MAX_SCHEDULE_MESSAGE);
		nMaxScheduleMessage = getHelper()->getIntegerData(value);
		return nMaxScheduleMessage;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string TTISManagerEntityData::getType()
    {
        return(getStaticType());
    }


    //
    // getStaticType
    //
    std::string TTISManagerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* TTISManagerEntityData::clone( unsigned long key )
    {
        return new TTISManagerEntityData( key );
    }
    
    
    //
    // invalidate
    //
    void TTISManagerEntityData::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "invalidate" );
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        getHelper()->invalidate();
        LOG_FUNCTION_EXIT( SourceInfo, "invalidate" );
    }

        
    ///////////////////////////////////////////////////////////////////////
    //
    // Protected Methods
    //
    ///////////////////////////////////////////////////////////////////////



    //
    // validateData
    //
    void TTISManagerEntityData::validateData()
    {
    }


    //
    // setParameter (for string parameters)
    //
    void TTISManagerEntityData::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );

        // Set the member variable.
    }

} // TA_Base_Core
