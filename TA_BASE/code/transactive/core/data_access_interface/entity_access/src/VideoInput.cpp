/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoInput.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoInput provides access to VideoInput entity data and a specific framework
  * for VideoInput entity parameters. It inherits from EntityData.
  *
  * IMPORTANT: This class is used as a base helper class for all entities
  * that are VideoInputs.
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/VideoInput.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    const std::string VideoInput::VI_DEFAULT_ENTITY_TYPE    = "VideoInput";
    const std::string VideoInput::VIDEO_INPUT_GROUP_NAME    = "VideoInputGroupName";


    // 
    // Supported Features
    //
    const std::string VideoInput::SUPERIMPOSE_DATE_SUPPORTED     = "SuperimposeDateSupported";
    const std::string VideoInput::SUPERIMPOSE_NAME_SUPPORTED     = "SuperimposeNameSupported";
    const std::string VideoInput::SUPERIMPOSE_TEXT_SUPPORTED     = "SuperimposeTextSupported";
    // ADD NEW FEATURES HERE.

    // 
    // State Parameters 
    //
    const std::string VideoInput::PUBLIC_VIEWING_ALLOWED = "PublicViewingAllowed";
    const std::string VideoInput::IN_SERVICE             = "InService";
    const std::string VideoInput::DATE_SUPERIMPOSED      = "DateSuperimposed";
    const std::string VideoInput::NAME_SUPERIMPOSED      = "NameSuperimposed";
    const std::string VideoInput::SUPERIMPOSED_TEXT      = "SuperimposedText";


    //
    // VideoInput
    //
    VideoInput::VideoInput( const std::string& entityType )
        : VideoSwitchResource( entityType )
    {
    }


    //
    // VideoInput
    //
    VideoInput::VideoInput( unsigned long key, const std::string& entityType ) 
        : VideoSwitchResource( key, entityType )
    {
    }


    //
    // getVideoInputGroupName
    //
    std::string VideoInput::getVideoInputGroupName()
    {
        // Get the parameters for this entity
		return getHelper()->getParameter(VIDEO_INPUT_GROUP_NAME);
    }


    //
    // setVideoInputGroupName
    //
    void VideoInput::setVideoInputGroupName( const std::string& name )
    {
        setParameter( VIDEO_INPUT_GROUP_NAME, name);
    }


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////


    //
    // isSuperimposeDateSupported
    //
    bool VideoInput::isSuperimposeDateSupported()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(SUPERIMPOSE_DATE_SUPPORTED);
		bool isSuperimposeDateSupported = false;
		if (!value.empty())
		{
			isSuperimposeDateSupported = EntityHelper::getBooleanData(value);
		}

		return isSuperimposeDateSupported;
    }


    //
    // setSuperimposeDateSupported
    //
    void VideoInput::setSuperimposeDateSupported( bool isSuperimposeDateSupported )
    {
        setParameter( SUPERIMPOSE_DATE_SUPPORTED, isSuperimposeDateSupported);
    }


    //
    // isSuperimposeNameSupported
    //
    bool VideoInput::isSuperimposeNameSupported()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(SUPERIMPOSE_NAME_SUPPORTED);
		bool isSuperimposeNameSupported = false;
		if (!value.empty())
		{
			isSuperimposeNameSupported = EntityHelper::getBooleanData(value);
		}

		return isSuperimposeNameSupported;
    }


    //
    // setSuperimposeNameSupported
    //
    void VideoInput::setSuperimposeNameSupported( bool isSuperimposeNameSupported )
    {
        setParameter( SUPERIMPOSE_NAME_SUPPORTED, isSuperimposeNameSupported);
    }


    //
    // isSuperimposeTextSupported
    //
    bool VideoInput::isSuperimposeTextSupported()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(SUPERIMPOSE_TEXT_SUPPORTED);
		bool isSuperimposeTextSupported = false;
		if (!value.empty())
		{
			isSuperimposeTextSupported = EntityHelper::getBooleanData(value);
		}

		return isSuperimposeTextSupported;
    }


    //
    // setSuperimposeTextSupported
    //
    void VideoInput::setSuperimposeTextSupported( bool isSuperimposeTextSupported )
    {
        setParameter( SUPERIMPOSE_TEXT_SUPPORTED, isSuperimposeTextSupported);
    }


    // ADD NEW FEATURES HERE.


    ///////////////////////////////////////////////////////////////////
    // 
    // State Parameters 
    //
    ///////////////////////////////////////////////////////////////////
    

    //
    // isPublicViewingAllowed
    //
    bool VideoInput::isPublicViewingAllowed()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(PUBLIC_VIEWING_ALLOWED);
		bool isPublicViewingAllowed = false;
		if (!value.empty())
		{
			isPublicViewingAllowed = EntityHelper::getBooleanData(value);
		}

        return isPublicViewingAllowed;
    }


    //
    // setPublicViewingAllowed
    //
    void VideoInput::setPublicViewingAllowed( bool isPublicViewingAllowed )
    {
        setParameter( PUBLIC_VIEWING_ALLOWED, isPublicViewingAllowed);
    }


    //
    // isInService
    //
    bool VideoInput::isInService()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(IN_SERVICE);
		bool isInService = false;
		if (!value.empty())
		{
			isInService = EntityHelper::getBooleanData(value);
		}

        return isInService;
    }


    //
    // setInService
    //
    void VideoInput::setInService( bool isInService )
    {
        setParameter( IN_SERVICE, isInService);
    }

    //
    // isDateSuperimposedOnVideoInput
    // 
    bool VideoInput::isDateSuperimposedOnVideoInput()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(DATE_SUPERIMPOSED);
		bool isDateSuperimposedOnVideoInput = false;
		if (!value.empty())
		{
			isDateSuperimposedOnVideoInput = EntityHelper::getBooleanData(value);
		}

        return isDateSuperimposedOnVideoInput;
    }


    //
    // setDateSuperimposedOnVideoInput
    //
    void VideoInput::setDateSuperimposedOnVideoInput( bool isDateSuperimposedOnVideoInput )
    {
        setParameter( DATE_SUPERIMPOSED, isDateSuperimposedOnVideoInput);
    }


    //
    // isNameSuperimposedOnVideoInput
    // 
    bool VideoInput::isNameSuperimposedOnVideoInput()
    {
        // Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(NAME_SUPERIMPOSED);
		bool isNameSuperimposedOnVideoInput = false;
		if (!value.empty())
		{
			isNameSuperimposedOnVideoInput = EntityHelper::getBooleanData(value);
		}

        return isNameSuperimposedOnVideoInput;
    }


    //
    // setNameSuperimposedOnVideoInput
    //
    void VideoInput::setNameSuperimposedOnVideoInput( bool isNameSuperimposedOnVideoInput )
    {
        setParameter( NAME_SUPERIMPOSED, isNameSuperimposedOnVideoInput);
    }


    //
    // getTextSuperimposedOnVideoInput
    //
    std::string VideoInput::getTextSuperimposedOnVideoInput()
    {
		// Get the parameters for this entity
		return getHelper()->getParameter(NAME_SUPERIMPOSED);
    }


    //
    // setTextSuperimposedOnVideoInput
    //
    void VideoInput::setTextSuperimposedOnVideoInput( std::string superimposedText )
    {
        setParameter( SUPERIMPOSED_TEXT, superimposedText);
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string VideoInput::getType()
    {
        return m_entityType;
    }


    //
    // clone
    //
    //IEntityData* VideoInput::clone( unsigned long key )
    //{
    //    return new VideoInput( key, m_entityType );
    //}


    ///////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ///////////////////////////////////////////////////////////////////////

} // TA_Base_Core
