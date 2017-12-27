/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoSwitchResource.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * VideoSwitchResource provides access to VideoSwitchResource entity data and a specific framework
  * for VideoSwitchResource entity parameters. It inherits from EntityData.
  *
  * IMPORTANT: This class is used as a base helper class for all entities
  * that are VideoSwitchResources.
  *
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/VideoSwitchResource.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    const std::string VideoSwitchResource::VSR_DEFAULT_ENTITY_TYPE       = "VideoSwitchResource";
	const std::string VideoSwitchResource::ASSET_NAME = "AssetName";

    const std::string VideoSwitchResource::PARAMETER_TRUE   = "1";
    const std::string VideoSwitchResource::PARAMETER_FALSE  = "0";

    //
    // VideoSwitchResource
    //
    VideoSwitchResource::VideoSwitchResource( const std::string& entityType )
        : EntityData(),
          m_entityType( entityType )
    {
    }


    //
    // VideoSwitchResource
    //
    VideoSwitchResource::VideoSwitchResource( unsigned long key, const std::string& entityType ) 
        : EntityData( key, entityType ),
          m_entityType( entityType )
    {
    }

    //
    // setDescription
    //
    void VideoSwitchResource::setDescription( const std::string& description )
    {
        getHelper()->setDescription( description );
        getHelper()->writeEntityData();
    }

	std::string VideoSwitchResource::getAssetName()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter(ASSET_NAME);
	}


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////


    // ADD NEW FEATURES HERE.


    ///////////////////////////////////////////////////////////////////
    // 
    // State Parameters 
    //
    ///////////////////////////////////////////////////////////////////
    

    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string VideoSwitchResource::getType()
    {
        return m_entityType;
    }


    //
    // clone
    //
    //IEntityData* VideoSwitchResource::clone( unsigned long key )
    //{
    //    return new VideoSwitchResource( key, m_entityType );
    //}


    //
    // invalidate
    //
    void VideoSwitchResource::invalidate()
    {
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        getHelper()->invalidate();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Protected methods
    //
    ///////////////////////////////////////////////////////////////////////

    
    //
    // setParameter (for bool parameters)
    //
    void VideoSwitchResource::setParameter( const std::string& name, bool value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        if ( value )
        {
            paramMap[ name ] = PARAMETER_TRUE;
        }
        else
        {
            paramMap[ name ] = PARAMETER_FALSE;
        }
        getHelper()->writeParameters( paramMap );
    }


    //
    // setParameter (for string parameters)
    //
    void VideoSwitchResource::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );
    }


    //
    // setParameter (for unsigned short parameters)
    //
    void VideoSwitchResource::setParameter( const std::string& name, unsigned short value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::stringstream theValue;
        theValue << value;
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = theValue.str();
        getHelper()->writeParameters( paramMap );
    }

} // TA_Base_Core
