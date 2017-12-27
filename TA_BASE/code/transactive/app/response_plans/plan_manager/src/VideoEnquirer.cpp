/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VideoEnquirer.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Light-weight singleton class to communicate with the VideoSwitchAgent
  * for selected enquiry functions only :
  *     - get camera name
  *
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 )
#endif // defined( WIN32 )

#include "stdafx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET) || defined (STEPTYPE_VIDEOWALL)

#include <memory>
#include "VideoEnquirer.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"

#if defined (STEPTYPE_VIDEOWALL)
#include "core/data_access_interface/video_wall_layout_agent/src/WallboardAccessFactory.h"
#include "core/data_access_interface/video_wall_layout_agent/src/IWallboardData.h"
#endif

#ifdef _MSC_VER
    #pragma warning(disable : 4786)
#endif

namespace TA_Base_App
{
#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

    std::string VideoEnquirer::getCameraName( unsigned long cameraEntityKey )
    {
        FUNCTION_ENTRY( "getCameraName" );

        std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject > camera;
        std::string name = "";
        try
        {
            camera = std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject >(
                TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCamera( cameraEntityKey ) );
            if ( 0 != camera.get() )
            {
                CORBA_CALL_RETURN( name, (*camera), getName, () );//limin
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera object is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get Camera." ));
            }
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            TA_THROW( TA_Base_Core::TransactiveException( static_cast< const char* >( e.what ) ));
        }
        catch( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            TA_THROW( TA_Base_Core::TransactiveException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() ));
        }
        catch( TA_Base_Core::TransactiveException& )
        {
            // Catch this and rethrow so that we don't lose the info when we catch ...
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get Camera name" );
            TA_THROW( TA_Base_Core::TransactiveException( "Unknown exception caught." ));
        }

        FUNCTION_EXIT;
        return name;
    }


#endif

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

    std::string VideoEnquirer::getMonitorName( unsigned long monitorEntityKey )
    {
        FUNCTION_ENTRY( "getMonitorName" );

        std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject > monitor;
        std::string name = "";
        try
        {
            monitor = std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject >(
                TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoMonitorNamedObject( monitorEntityKey ) );
            if ( 0 != monitor.get() )
            {
                CORBA_CALL_RETURN( name, (*monitor), getName, () );//limin
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Monitor object is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get Monitor." ));
            }
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            TA_THROW( TA_Base_Core::TransactiveException( static_cast< const char* >( e.what ) ));
        }
        catch( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            TA_THROW( TA_Base_Core::TransactiveException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() ));
        }
        catch( TA_Base_Core::TransactiveException& )
        {
            // Catch this and rethrow so that we don't lose the info when we catch ...
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get Monitor name" );
            TA_THROW( TA_Base_Core::TransactiveException( "Unknown exception caught." ));
        }

        FUNCTION_EXIT;
        return name;
    }


#endif

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

    std::string VideoEnquirer::getPresetName( unsigned long cameraEntityKey, unsigned long presetKey )
    {
        FUNCTION_ENTRY( "getPresetName" );

        std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject > camera;
        try
        {
            camera = std::auto_ptr< TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject >(
                TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCamera( cameraEntityKey ) );
            if ( 0 == camera.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera object is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get camera." ));
            }

            MovementControlCorbaDef_var control;
            CORBA_CALL_RETURN( control, (*camera), getMovementControl, () );//limin
            MovementControlCorbaDef::CameraPresetSequence* presets = control->getAllPresets();
            if ( 0 == presets )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Preset sequence is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get presets." ));
            }
            for ( unsigned int i = 0; i < presets->length(); i++ )
            {
                if ( presetKey == (*presets)[i].index )
                {
                    FUNCTION_EXIT;
                    return (*presets)[i].name;
                }
            }
        }
        catch( TA_Base_App::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            TA_THROW( TA_Base_Core::TransactiveException( static_cast< const char* >( e.what ) ));
        }
        catch( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            TA_THROW( TA_Base_Core::TransactiveException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() ));
        }
        catch( TA_Base_Core::TransactiveException& )
        {
            // Catch this and rethrow so that we don't lose the info when we catch ...
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get Camera presets" );
            TA_THROW( TA_Base_Core::TransactiveException( "Unknown exception caught." ));
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Preset id not found." );
        TA_THROW( TA_Base_Core::TransactiveException( "The preset was not found, so it's name could not be determined." ));

        FUNCTION_EXIT;
        return "";
    }


    MovementControlCorbaDef::CameraPresetSequence* VideoEnquirer::getPresets( unsigned long cameraEntityKey )
    {
        FUNCTION_ENTRY( "getPresets" );

        std::auto_ptr< VideoSwitchAgentFactory::CameraNamedObject > camera;
        try
        {
            camera = std::auto_ptr< VideoSwitchAgentFactory::CameraNamedObject >(
                VideoSwitchAgentFactory::getInstance().getCamera( cameraEntityKey ) );
            if ( 0 == camera.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera object is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get camera." ));
            }

            MovementControlCorbaDef_var control;
            CORBA_CALL_RETURN( control, (*camera), getMovementControl, () );//limin

            FUNCTION_EXIT;
            return control->getAllPresets();
        }
        catch( TA_Base_App::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            TA_THROW( TA_Base_Core::TransactiveException( static_cast< const char* >( e.what ) ));
        }
        catch( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            TA_THROW( TA_Base_Core::TransactiveException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() ));
        }
        catch( TA_Base_Core::TransactiveException& )
        {
            // Catch this and rethrow so that we don't lose the info when we catch ...
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get Camera presets" );
            TA_THROW( TA_Base_Core::TransactiveException( "Unknown exception caught." ));
        }

        FUNCTION_EXIT;
        return 0;
    }


#endif

#if defined (STEPTYPE_VIDEOWALL)

    std::string VideoEnquirer::getWallboardName( unsigned long wallboardEntityKey )
    {
        FUNCTION_ENTRY( "getWallboardName" );

        std::auto_ptr< VideoSwitchAgentFactory::VideoOutputGroupNamedObject > wallboard;
        std::string name = "";
        try
        {
            wallboard = std::auto_ptr< VideoSwitchAgentFactory::VideoOutputGroupNamedObject >(
                VideoSwitchAgentFactory::getInstance().getVideoOutputGroup( wallboardEntityKey ) );
            if ( 0 != wallboard.get() )
            {
                CORBA_CALL_RETURN( name, (*wallboard), getName, () );//limin
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Wallboard object is 0." );
                TA_THROW( TA_Base_Core::TransactiveException( "Failed to get Wallboard." ));
            }
        }
        catch( TA_Base_App::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            TA_THROW( TA_Base_Core::TransactiveException( static_cast< const char* >( e.what ) ));
        }
        catch( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            TA_THROW( TA_Base_Core::TransactiveException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() ));
        }
        catch( TA_Base_Core::TransactiveException& )
        {
            // Catch this and rethrow so that we don't lose the info when we catch ...
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get Wallboard name" );
            TA_THROW( TA_Base_Core::TransactiveException( "Unknown exception caught." ));
        }

        FUNCTION_EXIT;
        return name;
    }


    std::string VideoEnquirer::getLayoutName( unsigned long layoutId )
    {
        FUNCTION_ENTRY( "getLayoutName" );

        // This is cheating - it should come from the appropriate agent, but this is
        // heaps quicker and less complex.

        TA_Base_Core::IWallboardData* data = TA_Base_Core::WallboardAccessFactory::getInstance()->getWallboardData();
        if ( 0 == data )
        {
            TA_THROW( TA_Base_Core::TransactiveException( "Failed to retrieve wallboard data from the database." ));
        }
        TA_Base_Core::Layout* layout = data->getLayout( layoutId );
        if ( 0 == layout )
        {
            TA_THROW( TA_Base_Core::TransactiveException( "Failed to retrieve layout data from the database." ));
        }

        FUNCTION_EXIT;
        return layout->layoutName;
    }


#endif

}

#endif // STEPTYPE_ASSIGN_CAMERA_TO_MONITOR || STEPTYPE_ACTIVATE_CAMERA_PRESET || STEPTYPE_VIDEOWALL
