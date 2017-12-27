/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Camera.cpp $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Camera is a concrete implementation of ICamera, which is in turn an implementation of
  * IEntityData. It provides access to Camera entity data and a specific framework
  * for Camera entity parameters.
  *
  * When adding new camera features, code needs to be added everywhere
  * the phrase "ADD NEW FEATURES HERE" appears.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "core/data_access_interface/entity_access/src/Camera.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    // 
    // The entity type is fixed for all instances of this class.
    // If this is changed, please update app/cctv/video_switch_agent/src/EntityTypeConstants.h.
    // 
    const std::string Camera::ENTITY_TYPE = "Camera";

    // 
    // Supported Features
    //
    const std::string Camera::MOVEMENT                  = "MovementSupported";
    const std::string Camera::PROPORTIONAL_MOVEMENT     = "ProportionalMovementSupported";
    const std::string Camera::AUTO_FOCUS                = "AutoFocusSupported";
    const std::string Camera::MANUAL_FOCUS              = "ManualFocusSupported";
    const std::string Camera::AUTO_IRIS                 = "AutoIrisSupported";
    const std::string Camera::MANUAL_IRIS               = "ManualIrisSupported";
    const std::string Camera::IRIS_PEAK_WHITE_INVERSION = "IrisPeakWhiteInversion";
    const std::string Camera::WASH                      = "WashSupported";
    const std::string Camera::WIPE                      = "WipeSupported";
    const std::string Camera::PRESETS                   = "PresetsSupported";
    // ADD NEW FEATURES HERE.


    //
    // Camera
    //
    Camera::Camera()
        : VideoInput( ENTITY_TYPE )
    {
    }


    //
    // Camera
    //
    Camera::Camera( unsigned long key ) 
        : VideoInput( key, ENTITY_TYPE )
    {
    }


    ///////////////////////////////////////////////////////////////////
    // 
    // Supported Features
    //
    ///////////////////////////////////////////////////////////////////


    //
    // isMovementSupported
    //
    bool Camera::isMovementSupported()
    {
		m_isMovementSupported = false;
		std::string value = getHelper()->getParameter(MOVEMENT);
		m_isMovementSupported = EntityHelper::getBooleanData(value);

        return m_isMovementSupported;
    }


    //
    // setMovementSupported
    //
    void Camera::setMovementSupported( bool isMovementSupported )
    {
        setParameter( MOVEMENT, isMovementSupported);
    }


    //
    // isProportionalMovementSupported
    //
    bool Camera::isProportionalMovementSupported()
    {
		m_isProportionalMovementSupported = false;
		std::string value = getHelper()->getParameter(PROPORTIONAL_MOVEMENT);
		m_isProportionalMovementSupported = EntityHelper::getBooleanData(value);

        return m_isProportionalMovementSupported;
    }


    //
    // setProportionalMovementSupported
    //
    void Camera::setProportionalMovementSupported( bool isProportionalMovementSupported )
    {
        setParameter( PROPORTIONAL_MOVEMENT, isProportionalMovementSupported );
    }


    //
    // isAutoFocusSupported
    //
    bool Camera::isAutoFocusSupported()
    {
		m_isAutoFocusSupported = false;
		std::string value = getHelper()->getParameter(AUTO_FOCUS);
		m_isAutoFocusSupported = EntityHelper::getBooleanData(value);

        return m_isAutoFocusSupported;
    }


    //
    // setAutoFocusSupported
    //
    void Camera::setAutoFocusSupported( bool isAutoFocusSupported )
    {
        setParameter( AUTO_FOCUS, isAutoFocusSupported );
    }


    //
    // isManualFocusSupported
    //
    bool Camera::isManualFocusSupported()
    {
		m_isManualFocusSupported = false;
		std::string value = getHelper()->getParameter(MANUAL_FOCUS);
		m_isManualFocusSupported = EntityHelper::getBooleanData(value);

        return m_isManualFocusSupported;
    }


    //
    // setManualFocusSupported
    //
    void Camera::setManualFocusSupported( bool isManualFocusSupported )
    {
        setParameter( MANUAL_FOCUS, isManualFocusSupported );
    }


    //
    // isAutoIrisSupported
    //
    bool Camera::isAutoIrisSupported()
    {
		m_isAutoIrisSupported = false;
		std::string value = getHelper()->getParameter(AUTO_IRIS);
		m_isAutoIrisSupported = EntityHelper::getBooleanData(value);

        return m_isAutoIrisSupported;
    }


    //
    // setAutoIrisSupported
    //
    void Camera::setAutoIrisSupported( bool isAutoIrisSupported )
    {
        setParameter( AUTO_IRIS, isAutoIrisSupported );
    }


    //
    // isManualIrisSupported
    //
    bool Camera::isManualIrisSupported()
    {
		m_isManualIrisSupported = false;
		std::string value = getHelper()->getParameter(MANUAL_IRIS);
		m_isManualIrisSupported = EntityHelper::getBooleanData(value);

        return m_isManualIrisSupported;
    }


    //
    // setManualIrisSupported
    //
    void Camera::setManualIrisSupported( bool isManualIrisSupported )
    {
        setParameter( MANUAL_IRIS, isManualIrisSupported );
    }


    //
    // isIrisPeakWhiteInversionSupported
    //
    bool Camera::isIrisPeakWhiteInversionSupported()
    {
		m_isIrisPeakWhiteInversionSupported = false;
		std::string value = getHelper()->getParameter(IRIS_PEAK_WHITE_INVERSION);
		m_isIrisPeakWhiteInversionSupported = EntityHelper::getBooleanData(value);

        return m_isIrisPeakWhiteInversionSupported;
    }


    //
    // setIrisPeakWhiteInversionSupported
    //
    void Camera::setIrisPeakWhiteInversionSupported( bool isIrisPeakWhiteInversionSupported )
    {
        setParameter( MANUAL_IRIS, isIrisPeakWhiteInversionSupported );
    }


    //
    // isWashSupported
    //
    bool Camera::isWashSupported()
    {
		m_isWashSupported = false;
		std::string value = getHelper()->getParameter(WASH);
		m_isWashSupported = EntityHelper::getBooleanData(value);
		
        return m_isWashSupported;
    }


    //
    // setWashSupported
    //
    void Camera::setWashSupported( bool isWashSupported )
    {
        setParameter( WASH, isWashSupported );
    }


    //
    // isWipeSupported
    //
    bool Camera::isWipeSupported()
    {
		m_isWipeSupported = false;
		std::string value = getHelper()->getParameter(WIPE);
		m_isWipeSupported = EntityHelper::getBooleanData(value);

        return m_isWipeSupported;
    }


    //
    // setWipeSupported
    //
    void Camera::setWipeSupported( bool isWipeSupported )
    {
        setParameter( WASH, isWipeSupported );
    }


    //
    // arePresetsSupported
    //
    bool Camera::arePresetsSupported()
    {
		m_arePresetsSupported = false;
		std::string value = getHelper()->getParameter(PRESETS);
		m_arePresetsSupported = EntityHelper::getBooleanData(value);

        return m_arePresetsSupported;
    }


    //
    // setPresetsSupported
    //
    void Camera::setPresetsSupported( bool arePresetsSupported )
    {
        setParameter( PRESETS, arePresetsSupported );
    }

    // ADD NEW FEATURES HERE.


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getType
    //
    std::string Camera::getType()
    {
        return(getStaticType());
    }

    std::string Camera::getStaticType()
    {
        return(ENTITY_TYPE);
    }

    //
    // clone
    //
    IEntityData* Camera::clone( unsigned long key )
    {
        return new Camera( key );        
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Protected methods
    //
    ///////////////////////////////////////////////////////////////////////




    ///////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // initialiseParameters
    //
    void Camera::initialiseParameters()
    {
        // Initialise all bool parameters to false, so we don't assume something is on/supported
        // when it isn't.

        m_isMovementSupported = false;               
        m_isProportionalMovementSupported = false;  
        m_isAutoFocusSupported = false;            
        m_isManualFocusSupported = false;         
        m_isAutoIrisSupported = false;           
        m_isManualIrisSupported = false;        
        m_isWashSupported = false;             
        m_isWipeSupported = false;            
        m_arePresetsSupported = false;       
        // ADD NEW FEATURES HERE.
    }

} // TA_Base_Core
