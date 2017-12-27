/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoOutputGroup.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoOutputGroup provides access to VideoOutputGroup entity data and a specific framework
  * for VideoOutputGroup entity parameters.
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/VideoOutputGroup.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    //
    // The entity type is fixed for all instances of this class.
    // If this is changed, please update app/cctv/video_switch_agent/src/EntityTypeConstants.h.
    //

    const std::string VideoOutputGroup::ENTITY_TYPE             = "VideoOutputGroup";

    // 
    // The entity parameters for this entity type.
    //
  
    const std::string VideoOutputGroup::GROUP_TYPE              = "GroupType";
    const std::string VideoOutputGroup::TYPE_SIZE               = "TypeSize";
    const std::string VideoOutputGroup::POSITION                = "Position";
    const std::string VideoOutputGroup::GROUP_DIMENSION_X       = "GroupDimensionX";
    const std::string VideoOutputGroup::GROUP_DIMENSION_Y       = "GroupDimensionY";
    const std::string VideoOutputGroup::CAPABLE_OF_LAYOUTS      = "CapableOfLayouts";
    const std::string VideoOutputGroup::WALLBOARD_AGENT_NAME    = "WallboardAgentName";

    const std::string VideoOutputGroup::PARAMETER_TRUE          = "1";
    const std::string VideoOutputGroup::PARAMETER_FALSE         = "0";

    //
    // Valid sizes.
    //

    const std::string VideoOutputGroup::SMALL                   = "Small";
    const std::string VideoOutputGroup::MEDIUM                  = "Medium";
    const std::string VideoOutputGroup::LARGE                   = "Large";

    //
    // VideoOutputGroup
    //
    VideoOutputGroup::VideoOutputGroup()
        : EntityData()
    {
        // Do nothing.
    }


    //
    // VideoOutputGroup
    //
    VideoOutputGroup::VideoOutputGroup( unsigned long key ) 
        : EntityData( key, ENTITY_TYPE )
    {
    }


    //
    // getGroupType
    //
    std::string VideoOutputGroup::getGroupType()
    {
        // Get the parameters for this entity
		return getHelper()->getParameter(GROUP_TYPE);
    }

       
    //
    // setGroupType
    //
    void VideoOutputGroup::setGroupType( const std::string& groupType )
    {
        setParameter( GROUP_TYPE, groupType);
    }


    //
    // getTypeSize
    //
    std::string VideoOutputGroup::getTypeSize()
    {
		// Get the parameters for this entity
		std::string typeSize = getHelper()->getParameter(TYPE_SIZE);
		if ( ( SMALL != typeSize ) && ( MEDIUM != typeSize ) && ( LARGE != typeSize ) )
		{
			typeSize = SMALL;
		}
        return typeSize;
    }

       
    //
    // setTypeSize
    //
    void VideoOutputGroup::setTypeSize( const std::string& typeSize )
    {
        setParameter( GROUP_TYPE, typeSize);
    }


    //
    // getPosition
    //
    unsigned short VideoOutputGroup::getPosition()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(POSITION);
		unsigned short position = 0;
		if (!value.empty())
		{
            position = static_cast< unsigned short >(EntityHelper::getUnsignedLongData(value));
		}

        return position;
    }


    //
    // setPosition
    //
    void VideoOutputGroup::setPosition( unsigned short position )
    {
        setParameter( POSITION, position);
    }


    //
    // getGroupDimensionX
    //
    unsigned short VideoOutputGroup::getGroupDimensionX()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(GROUP_DIMENSION_X);
		unsigned short groupDimensionX = 0;
		if (!value.empty())
		{
            groupDimensionX = static_cast< unsigned short >(EntityHelper::getUnsignedLongData(value));
		}

        return groupDimensionX;
    }
    

    //
    // setGroupDimensionX
    //
    void VideoOutputGroup::setGroupDimensionX( unsigned short groupDimensionX )
    {
        setParameter( GROUP_DIMENSION_X, groupDimensionX);
    }

    
    //
    // getGroupDimensionY
    //
    unsigned short VideoOutputGroup::getGroupDimensionY()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(GROUP_DIMENSION_Y);
		unsigned short groupDimensionY = 0;
		if (!value.empty())
		{
            groupDimensionY = static_cast< unsigned short >(EntityHelper::getUnsignedLongData(value));
		}

        return groupDimensionY;
    }


    //
    // setGroupDimensionY
    //
    void VideoOutputGroup::setGroupDimensionY( unsigned short groupDimensionY )
    {
        setParameter( GROUP_DIMENSION_Y, groupDimensionY);
    }

    
    //
    // isCapableOfLayouts
    //
    bool VideoOutputGroup::isCapableOfLayouts()
    {
		// Get the parameters for this entity
		bool isCapable = false;
		if( PARAMETER_TRUE == getHelper()->getParameter(CAPABLE_OF_LAYOUTS) )
		{
			isCapable = true;
		}

        return isCapable;
    }


    //
    // setCapableOfLayouts
    //
    void VideoOutputGroup::setCapableOfLayouts( bool isCapableOfLayouts )
    {
        setParameter( CAPABLE_OF_LAYOUTS, isCapableOfLayouts);
    }

    //
    // getVideoWallboardAgentName
    //
    std::string VideoOutputGroup::getVideoWallboardAgentName()
    {
		return getHelper()->getParameter(WALLBOARD_AGENT_NAME);		
    }

    //
    // setVideoWallboardAgentName
    //
    void VideoOutputGroup::setVideoWallboardAgentName( const std::string& wallboardName )
    {
        setParameter( WALLBOARD_AGENT_NAME, wallboardName);
    }
    
    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string VideoOutputGroup::getType()
    {
        return(getStaticType());
    }

    std::string VideoOutputGroup::getStaticType()
    {
        return ENTITY_TYPE;
    }

   
    //
    // clone
    //
    IEntityData* VideoOutputGroup::clone( unsigned long key )
    {
        return new VideoOutputGroup( key );
    }
    
    
    //
    // invalidate
    //
    void VideoOutputGroup::invalidate()
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
    // setParameter (for bool parameters)
    //
    void VideoOutputGroup::setParameter( const std::string& name, bool value)
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
    // setParameter (for unsigned short parameters)
    //
    void VideoOutputGroup::setParameter( const std::string& name, unsigned short value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        
        std::stringstream theValue;
        theValue << value;
 
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = theValue.str();
        getHelper()->writeParameters( paramMap );
    }


    //
    // setParameter (for string parameters)
    //
    void VideoOutputGroup::setParameter( const std::string& name, 
        const std::string& value)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;
        paramMap[ name ] = value;
        getHelper()->writeParameters( paramMap );
    }

} // TA_Base_Core
