/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoOutput.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoOutput provides access to VideoOutput entity data and a specific framework
  * for VideoOutput entity parameters.
  *
  * When adding new video input features, code needs to be added everywhere
  * the phrase "ADD NEW FEATURES HERE" appears.
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/VideoOutput.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    const std::string VideoOutput::VO_DEFAULT_ENTITY_TYPE   = "VideoOutput";
    const std::string VideoOutput::VIDEO_OUTPUT_GROUP_NAME  = "VideoOutputGroupName";
    
    //
    // VideoOutput
    //
    VideoOutput::VideoOutput( const std::string& entityType )
        : VideoSwitchResource( entityType )
    {
        // Do nothing.
    }


    //
    // VideoOutput
    //
    VideoOutput::VideoOutput( unsigned long key, const std::string& entityType ) 
        : VideoSwitchResource( key, entityType )
    {
    }


    //
    // getVideoOutputGroupName
    //
    std::string VideoOutput::getVideoOutputGroupName()
    {
        // Get the parameters for this entity
		return getHelper()->getParameter(VIDEO_OUTPUT_GROUP_NAME);
    }


    //
    // setVideoOutputGroupName
    //
    void VideoOutput::setVideoOutputGroupName( const std::string& name )
    {
        setParameter( VIDEO_OUTPUT_GROUP_NAME, name);
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string VideoOutput::getType()
    {
        return m_entityType;
    }

   
    //
    // clone
    //
    //IEntityData* VideoOutput::clone( unsigned long key )
    //{
    //    return new VideoOutput( key, m_entityType );
    //}
    
} // TA_Base_Core
