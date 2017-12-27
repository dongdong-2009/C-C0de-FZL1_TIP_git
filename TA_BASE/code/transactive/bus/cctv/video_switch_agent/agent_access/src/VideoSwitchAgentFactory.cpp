/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class is a singleton that provides access to the object monitored
  * by the Video Switch Agent.
  *
  * IMPORTANT: When using this class, you must make sure cleanup() is called
  * before Corba is cleaned up.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

/*#if defined( WIN32 )
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif // WIN32*/

#include <sstream>

#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoInputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/QuadCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/BVSStageCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/naming/src/Naming.h"
#include "core/naming/src/INamedObject.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using CORBA::String_var;

namespace
{
	const std::string KRTC_PROJECT_SETTING("4677");
	const std::string PROJECT_SETTING("ProjectName");
	const std::string PTZ_CAMERA("PT");
}

namespace TA_Base_Bus
{
    //
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable VideoSwitchAgentFactory::m_singletonLock;
	VideoSwitchAgentFactory* VideoSwitchAgentFactory::m_theClassInstance = NULL;


    //
    // getInstance
    //
	VideoSwitchAgentFactory& VideoSwitchAgentFactory::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new VideoSwitchAgentFactory();
		}

		return *m_theClassInstance;
    }


    //
    // cleanup
    //
    void VideoSwitchAgentFactory::cleanup()
    {
        delete m_theClassInstance;
        m_theClassInstance = NULL;
    }


    //
    // getVideoOutputGroups
    //
    VideoSwitchAgentFactory::VideoOutputGroupVector VideoSwitchAgentFactory::getVideoOutputGroups()
    {
		VideoOutputGroupVector a;
		return a;
    }


    //
    // getVideoOutputGroup
    //
    VideoSwitchAgentFactory::VideoOutputGroupNamedObject* VideoSwitchAgentFactory::getVideoOutputGroup( unsigned long entityKey )
    {
		return NULL;
    }

   
    //
    // getVideoOutputsForGroup
    //
    VideoSwitchAgentFactory::VideoOutputVector VideoSwitchAgentFactory::getVideoOutputsForGroup( unsigned long entityKey )
    {
		VideoOutputVector a;
		return a;
    }


    //
    // getVideoMonitors
    //
    VideoSwitchAgentFactory::VideoMonitorVector VideoSwitchAgentFactory::getVideoMonitors()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the VideoMonitors from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence_var videoMonitorSeq;
        CORBA_CALL_RETURN( videoMonitorSeq, m_switchManager, getVideoMonitors, () );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        VideoMonitorVector videoMonitors;
        for ( unsigned int i = 0; i < videoMonitorSeq->length(); i++ )
        {
            // Get the Corba name of the VideoMonitor.

            VideoMonitorNamedObject* videoMonitorNamedObject = 0;
            try
            {
                String_var name = videoMonitorSeq[ i ]->getName();
                videoMonitorNamedObject = new VideoMonitorNamedObject( m_agentName, name.in() );
                if ( 0 == videoMonitorNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new VideoMonitorNamedObject." );
                }
                try
                {
                    videoMonitorNamedObject->assignObject( videoMonitorSeq[ i ] );
		        }
		        catch (...) 
		        {
			        videoMonitorNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete videoMonitorNamedObject;
                videoMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = videoMonitors.begin();
                for( ; it != videoMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete videoMonitorNamedObject;
                videoMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = videoMonitors.begin();
                for( ; it != videoMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete videoMonitorNamedObject;
                videoMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = videoMonitors.begin();
                for( ; it != videoMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            videoMonitors.push_back( videoMonitorNamedObject );
        }
        return videoMonitors;
    }


    //
    // getVideoMonitorNamedObject
    //
    VideoSwitchAgentFactory::VideoMonitorNamedObject* VideoSwitchAgentFactory::getVideoMonitorNamedObject( unsigned long entityKey )
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the VideoMonitor from the agent.

        TA_Base_Bus::VideoMonitorCorbaDef_var videoMonitor;
		VideoMonitorNamedObject* videoMonitorNamedObject = 0;
        try
        { 
			CORBA_CALL_RETURN( videoMonitor, m_switchManager, getVideoMonitor, ( entityKey ) );//limin
			if(videoMonitor ==0)
			{
				throw TA_Base_Bus::VideoSwitchAgentException( 
					"No videoMonitor with this entityKey." );
			}
        // Get the Corba name of the VideoMonitor and create a named obj ref.

			
            String_var name = videoMonitor->getName();
            videoMonitorNamedObject = new VideoMonitorNamedObject( m_agentName, name.in() );
            if ( 0 == videoMonitorNamedObject )
            {
                throw TA_Base_Bus::VideoSwitchAgentException( 
                    "Failed to create new VideoMonitorNamedObject." );
            }

            try
            {
                videoMonitorNamedObject->assignObject( videoMonitor );
		    }
		    catch (...) 
		    {
			    videoMonitorNamedObject->clear();
		    }
        }
        catch( const TA_Base_Bus::VideoSwitchAgentException& )
        {
            delete videoMonitorNamedObject;
            videoMonitorNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const CORBA::Exception& )
        {
            delete videoMonitorNamedObject;
            videoMonitorNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const TA_Base_Core::ObjectResolutionException& )
        {
            delete videoMonitorNamedObject;
            videoMonitorNamedObject = 0; 
            throw;  // Throw it on.
        }

        return videoMonitorNamedObject;
    }


    //
    // getConsoleMonitors
    //
    VideoSwitchAgentFactory::VideoMonitorVector VideoSwitchAgentFactory::getConsoleMonitors()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the VideoMonitors from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence_var consoleMonitorSeq;
        CORBA_CALL_RETURN( consoleMonitorSeq, m_switchManager, getAllConsoleMonitors, () );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        VideoMonitorVector consoleMonitors;
        for ( unsigned int i = 0; i < consoleMonitorSeq->length(); i++ )
        {
            // Get the Corba name of the VideoMonitor.
            VideoMonitorNamedObject* consoleMonitorNamedObject = 0;
            try
            {
                String_var name = consoleMonitorSeq[ i ]->getName();
                consoleMonitorNamedObject = new VideoMonitorNamedObject( m_agentName, name.in() );
                if ( 0 == consoleMonitorNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new VideoMonitorNamedObject." );
                }

                try
                {
                    consoleMonitorNamedObject->assignObject( consoleMonitorSeq[ i ] );
		        }
		        catch (...) 
		        {
			        consoleMonitorNamedObject->clear();
                }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete consoleMonitorNamedObject;
                consoleMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = consoleMonitors.begin();
                for( ; it != consoleMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete consoleMonitorNamedObject;
                consoleMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = consoleMonitors.begin();
                for( ; it != consoleMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete consoleMonitorNamedObject;
                consoleMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = consoleMonitors.begin();
                for( ; it != consoleMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            consoleMonitors.push_back( consoleMonitorNamedObject );
        }
        return consoleMonitors;
    }

    //
    // getLocalConsoleMonitors
    //
    VideoSwitchAgentFactory::VideoMonitorVector VideoSwitchAgentFactory::getLocalConsoleMonitors()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        unsigned long consoleKey = getLocalConsoleKey();

        // Get the VideoMonitors from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence_var consoleMonitorSeq;
        CORBA_CALL_RETURN( consoleMonitorSeq , m_switchManager, getConsoleMonitors, ( consoleKey ) );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        VideoMonitorVector consoleMonitors;
        for ( unsigned int i = 0; i < consoleMonitorSeq->length(); i++ )
        {
            // Get the Corba name of the VideoMonitor.

            VideoMonitorNamedObject* consoleMonitorNamedObject = 0;
            try
            {
                String_var name = consoleMonitorSeq[ i ]->getName();
                consoleMonitorNamedObject = new VideoMonitorNamedObject( m_agentName, name.in() );
                if ( 0 == consoleMonitorNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new VideoMonitorNamedObject." );
                }
                try
                {
                    consoleMonitorNamedObject->assignObject( consoleMonitorSeq[ i ] );
		        }
		        catch (...) 
		        {
			        consoleMonitorNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete consoleMonitorNamedObject;
                consoleMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = consoleMonitors.begin();
                for( ; it != consoleMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete consoleMonitorNamedObject;
                consoleMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = consoleMonitors.begin();
                for( ; it != consoleMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete consoleMonitorNamedObject;
                consoleMonitorNamedObject = 0;

                VideoMonitorVector::iterator it = consoleMonitors.begin();
                for( ; it != consoleMonitors.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            consoleMonitors.push_back( consoleMonitorNamedObject );
        }
        return consoleMonitors;
    }


    //
    // getRecordingUnits
    //
    VideoSwitchAgentFactory::RecordingUnitVector VideoSwitchAgentFactory::getRecordingUnits()
    {
		RecordingUnitVector a;
		return a;
    }


    //
    // getLocalRecordingUnits
    //
    VideoSwitchAgentFactory::RecordingUnitVector VideoSwitchAgentFactory::getLocalConsoleRecordingUnits()
    {
		RecordingUnitVector a;
		return a;
    }


    //
    // getRecordingUnit
    //
    VideoSwitchAgentFactory::RecordingUnitNamedObject* VideoSwitchAgentFactory::getRecordingUnit( unsigned long entityKey )
    {
		return NULL;
    }


    //
    // getCameras
    //
    VideoSwitchAgentFactory::CameraVector VideoSwitchAgentFactory::getCameras()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the Cameras from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence_var cameraSeq;
		try
		{
			CORBA_CALL_RETURN( cameraSeq, m_switchManager, getCameras, () );//limin
		}
		catch(const TA_Base_Core::TransactiveException&)
		{
            CORBA_CALL_RETURN( cameraSeq, m_switchManager, getCameras, () );//limin
		}
		catch(const CORBA::Exception&)
		{
            CORBA_CALL_RETURN( cameraSeq, m_switchManager, getCameras, () );//limin
		}
		catch(...)
		{
		}

        // Convert each group into a NamedObject and then put it in a vector.

        CameraVector cameras;
        for ( unsigned int i = 0; i < cameraSeq->length(); i++ )
        {
            // Get the Corba name of the Camera.

            CameraNamedObject* cameraNamedObject = 0;
            try
            {
                String_var name = cameraSeq[ i ]->getName();
                cameraNamedObject = new CameraNamedObject( m_agentName, name.in() );
                if ( 0 == cameraNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new CameraNamedObject." );
                }
                try
                {
                    cameraNamedObject->assignObject( cameraSeq[ i ] );
		        }
		        catch (...) 
		        {
			        cameraNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete cameraNamedObject;
                cameraNamedObject = 0;

                CameraVector::iterator it = cameras.begin();
                for( ; it != cameras.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete cameraNamedObject;
                cameraNamedObject = 0;

                CameraVector::iterator it = cameras.begin();
                for( ; it != cameras.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete cameraNamedObject;
                cameraNamedObject = 0;

                CameraVector::iterator it = cameras.begin();
                for( ; it != cameras.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            cameras.push_back( cameraNamedObject );
        }
        return cameras;
    }


    VideoSwitchAgentFactory::CameraVector VideoSwitchAgentFactory::getPTZCameras()
    {
        // Make sure the Switch Manager has been resolved.
		
        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }
		
        // Get the Cameras from the agent.
		
        TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence_var cameraSeq;
        CORBA_CALL_RETURN( cameraSeq, m_switchManager, getCameras, () );//limin
		
        // Convert each group into a NamedObject and then put it in a vector.
		
        CameraVector cameras;
        for ( unsigned int i = 0; i < cameraSeq->length(); i++ )
        {
            // Get the Corba name of the Camera.
			std::string cameraName;
			cameraName = cameraSeq[i]->getName();

			std::string::size_type tPosition = cameraName.find(PTZ_CAMERA, 0);
			if (tPosition == std::string::npos)
			{
				continue;
			}
			
            CameraNamedObject* cameraNamedObject = 0;
            try
            {
                cameraNamedObject = new CameraNamedObject( m_agentName, cameraName );
                
                if ( 0 == cameraNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new CameraNamedObject." );
                }

                try
                {
                    cameraNamedObject->assignObject( cameraSeq[ i ] );
		        }
		        catch (...) 
		        {
			        cameraNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete cameraNamedObject;
                cameraNamedObject = 0;
				
                CameraVector::iterator it = cameras.begin();
                for( ; it != cameras.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete cameraNamedObject;
                cameraNamedObject = 0;
				
                CameraVector::iterator it = cameras.begin();
                for( ; it != cameras.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete cameraNamedObject;
                cameraNamedObject = 0;
				
                CameraVector::iterator it = cameras.begin();
                for( ; it != cameras.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
			
            cameras.push_back( cameraNamedObject );
        }
        return cameras;
    }
    //
    // getCamera
    //
    VideoSwitchAgentFactory::CameraNamedObject* VideoSwitchAgentFactory::getCamera( unsigned long entityKey )
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the Camera from the agent.

        TA_Base_Bus::CameraCorbaDef_var camera;
        CORBA_CALL_RETURN( camera, m_switchManager, getCamera, ( entityKey ) );

        // Get the Corba name of the Camera and create a named obj ref.

        CameraNamedObject* cameraNamedObject = 0;
        try
        {
            String_var name = camera->getName();
            cameraNamedObject = new CameraNamedObject( m_agentName, name.in() );
            if ( 0 == cameraNamedObject )
            {
                throw TA_Base_Bus::VideoSwitchAgentException( 
                    "Failed to create new CameraNamedObject." );
            }
            
            try
            {
                cameraNamedObject->assignObject( camera );
		    }
		    catch (...) 
		    {
			    cameraNamedObject->clear();
		    }
        }
        catch( const TA_Base_Bus::VideoSwitchAgentException& )
        {
            delete cameraNamedObject;
            cameraNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const CORBA::Exception& )
        {
            delete cameraNamedObject;
            cameraNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const TA_Base_Core::ObjectResolutionException& )
        {
            delete cameraNamedObject;
            cameraNamedObject = 0; 
            throw;  // Throw it on.
        }

        return cameraNamedObject;
    }


    //
    // getQuads
    //
    /*VideoSwitchAgentFactory::QuadVector VideoSwitchAgentFactory::getQuads()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the Quads from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::QuadSequence_var quadSeq;
        CORBA_CALL_RETURN( quadSeq, m_switchManager, getQuads, () );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        QuadVector quads;
        for ( unsigned int i = 0; i < quadSeq->length(); i++ )
        {
            // Get the Corba name of the Quad.

            QuadNamedObject* quadNamedObject = 0;
            try
            {
                String_var name = quadSeq[ i ]->getName();
                quadNamedObject = new QuadNamedObject( m_agentName, name.in() );
                if ( 0 == quadNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new QuadNamedObject." );
                }
                try
                {
                    quadNamedObject->assignObject( quadSeq[ i ] );
		        }
		        catch (...) 
		        {
			        quadNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete quadNamedObject;
                quadNamedObject = 0;

                QuadVector::iterator it = quads.begin();
                for( ; it != quads.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete quadNamedObject;
                quadNamedObject = 0;

                QuadVector::iterator it = quads.begin();
                for( ; it != quads.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete quadNamedObject;
                quadNamedObject = 0;

                QuadVector::iterator it = quads.begin();
                for( ; it != quads.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            quads.push_back( quadNamedObject );
        }
        return quads;
    }


    //
    // getBVSStages
    //
    VideoSwitchAgentFactory::BVSStageVector VideoSwitchAgentFactory::getBVSStages()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the BVSStages from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::BVSStageSequence_var bvsStageSeq;
        CORBA_CALL_RETURN( bvsStageSeq, m_switchManager, getBVSStages, () );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        BVSStageVector bvsStages;
        for ( unsigned int i = 0; i < bvsStageSeq->length(); i++ )
        {
            // Get the Corba name of the BVSStage.

            BVSStageNamedObject* bvsStageNamedObject = 0;
            try
            {
                String_var name = bvsStageSeq[ i ]->getName();
                bvsStageNamedObject = new BVSStageNamedObject( m_agentName, name.in() );
                
                if ( 0 == bvsStageNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new BVSStageNamedObject." );
                }
                try
                {
                    bvsStageNamedObject->assignObject( bvsStageSeq[ i ] );
		        }
		        catch (...) 
		        {
			        bvsStageNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete bvsStageNamedObject;
                bvsStageNamedObject = 0;

                BVSStageVector::iterator it = bvsStages.begin();
                for( ; it != bvsStages.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete bvsStageNamedObject;
                bvsStageNamedObject = 0;

                BVSStageVector::iterator it = bvsStages.begin();
                for( ; it != bvsStages.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete bvsStageNamedObject;
                bvsStageNamedObject = 0;

                BVSStageVector::iterator it = bvsStages.begin();
                for( ; it != bvsStages.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            bvsStages.push_back( bvsStageNamedObject );
        }

/*        if (m_locationType != TA_Base_Core::ILocation::OCC)
        {
            // get the stages from the OCC agent
            bvsStageSeq = m_occSwitchManager->getBVSStages();

            // Convert each group into a NamedObject and then put it in a vector.

            for ( unsigned int i = 0; i < bvsStageSeq->length(); i++ )
            {
                // Get the Corba name of the BVSStage.

                BVSStageNamedObject* bvsStageNamedObject = 0;
                try
                {
                    bvsStageNamedObject = new BVSStageNamedObject();
                    if ( 0 == bvsStageNamedObject )
                    {
                        throw TA_Base_Bus::VideoSwitchAgentException( 
                            "Failed to create new BVSStageNamedObject." );
                    }
                    bvsStageNamedObject->setEntityName( bvsStageSeq[ i ]->getName() );
                }
                catch( const TA_Base_Bus::VideoSwitchAgentException& )
                {
                    delete bvsStageNamedObject;
                    bvsStageNamedObject = 0;

                    BVSStageVector::iterator it = bvsStages.begin();
                    for( ; it != bvsStages.end(); ++it )
                    {
                        delete *it;
                        *it = 0;
                    }
                    throw;  // Throw it on.
                }
                catch( const CORBA::Exception& )
                {
                    delete bvsStageNamedObject;
                    bvsStageNamedObject = 0;

                    BVSStageVector::iterator it = bvsStages.begin();
                    for( ; it != bvsStages.end(); ++it )
                    {
                        delete *it;
                        *it = 0;
                    }
                    throw;  // Throw it on.
                }
                catch( const TA_Base_Core::ObjectResolutionException& )
                {
                    delete bvsStageNamedObject;
                    bvsStageNamedObject = 0;

                    BVSStageVector::iterator it = bvsStages.begin();
                    for( ; it != bvsStages.end(); ++it )
                    {
                        delete *it;
                        *it = 0;
                    }
                    throw;  // Throw it on.
                }

                bvsStages.push_back( bvsStageNamedObject );
            }
        }


        return bvsStages;
    }


    //
    // getQuad
    //
    VideoSwitchAgentFactory::QuadNamedObject* VideoSwitchAgentFactory::getQuad( unsigned long entityKey )
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the Quad from the agent.

        TA_Base_Bus::QuadCorbaDef_var quad;
        CORBA_CALL_RETURN( quad , m_switchManager, getQuad, ( entityKey ) );//limin

        // Get the Corba name of the Quad and create a named obj ref.

        QuadNamedObject* quadNamedObject = 0;
        try
        {
            String_var name = quad->getName();
            quadNamedObject = new QuadNamedObject( m_agentName, name.in() );
            if ( 0 == quadNamedObject )
            {
                throw TA_Base_Bus::VideoSwitchAgentException( 
                    "Failed to create new QuadNamedObject." );
            }
            try
            {
                quadNamedObject->assignObject( quad );
		    }
		    catch (...) 
		    {
			    quadNamedObject->clear();
		    }
        }
        catch( const TA_Base_Bus::VideoSwitchAgentException& )
        {
            delete quadNamedObject;
            quadNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const CORBA::Exception& )
        {
            delete quadNamedObject;
            quadNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const TA_Base_Core::ObjectResolutionException& )
        {
            delete quadNamedObject;
            quadNamedObject = 0; 
            throw;  // Throw it on.
        }

        return quadNamedObject;
    }
*/

    //
    // getSequences
    //
    VideoSwitchAgentFactory::SequenceVector VideoSwitchAgentFactory::getSequences()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the Sequences from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence_var sequenceSeq;
        CORBA_CALL_RETURN( sequenceSeq, m_switchManager, getSequences, () );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        SequenceVector sequences;
        for ( unsigned int i = 0; i < sequenceSeq->length(); i++ )
        {
            // Get the Corba name of the Sequence.

            SequenceNamedObject* sequenceNamedObject = 0;
            try
            {
                String_var name = sequenceSeq[ i ]->getName();
                sequenceNamedObject = new SequenceNamedObject( m_agentName, name.in() );

                if ( 0 == sequenceNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new SequenceNamedObject." );
                }
                try
                {
                    sequenceNamedObject->assignObject( sequenceSeq[ i ] );
		        }
		        catch (...) 
		        {
			        sequenceNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete sequenceNamedObject;
                sequenceNamedObject = 0;

                SequenceVector::iterator it = sequences.begin();
                for( ; it != sequences.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete sequenceNamedObject;
                sequenceNamedObject = 0;

                SequenceVector::iterator it = sequences.begin();
                for( ; it != sequences.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete sequenceNamedObject;
                sequenceNamedObject = 0;

                SequenceVector::iterator it = sequences.begin();
                for( ; it != sequences.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            sequences.push_back( sequenceNamedObject );
        }
        return sequences;
    }


    //
    // getSequence
    //
    VideoSwitchAgentFactory::SequenceNamedObject* VideoSwitchAgentFactory::getSequence( unsigned long entityKey )
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the Sequence from the agent.

        TA_Base_Bus::SequenceCorbaDef_var sequence;
        CORBA_CALL_RETURN( sequence , m_switchManager, getSequence, ( entityKey ) );//limin

        // Get the Corba name of the Sequence and create a named obj ref.

        SequenceNamedObject* sequenceNamedObject = 0;
        try
        {
            String_var name = sequence->getName();
            sequenceNamedObject = new SequenceNamedObject( m_agentName, name.in() );
            if ( 0 == sequenceNamedObject )
            {
                throw TA_Base_Bus::VideoSwitchAgentException( 
                    "Failed to create new SequenceNamedObject." );
            }
            try
            {
                sequenceNamedObject->assignObject( sequence );
		    }
		    catch (...) 
		    {
			    sequenceNamedObject->clear();
		    }
        }
        catch( const TA_Base_Bus::VideoSwitchAgentException& )
        {
            delete sequenceNamedObject;
            sequenceNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const CORBA::Exception& )
        {
            delete sequenceNamedObject;
            sequenceNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const TA_Base_Core::ObjectResolutionException& )
        {
            delete sequenceNamedObject;
            sequenceNamedObject = 0; 
            throw;  // Throw it on.
        }

        return sequenceNamedObject;
    }


    //
    // getVideoOutputs
    //
    VideoSwitchAgentFactory::VideoOutputVector VideoSwitchAgentFactory::getVideoOutputs()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the VideoOutputs from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence_var videoOutputSeq;
        CORBA_CALL_RETURN( videoOutputSeq , m_switchManager, getVideoOutputs, () );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        VideoOutputVector videoOutputs;
        for ( unsigned int i = 0; i < videoOutputSeq->length(); i++ )
        {
            // Get the Corba name of the VideoOutput.

            VideoOutputNamedObject* videoOutputNamedObject = 0;
            try
            {
                String_var name = videoOutputSeq[ i ]->getName();
                videoOutputNamedObject = new VideoOutputNamedObject( m_agentName, name.in() );
                if ( 0 == videoOutputNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new VideoOutputNamedObject." );
                }
                try
                {
                    videoOutputNamedObject->assignObject( videoOutputSeq[ i ] );
		        }
		        catch (...) 
		        {
			        videoOutputNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete videoOutputNamedObject;
                videoOutputNamedObject = 0;

                VideoOutputVector::iterator it = videoOutputs.begin();
                for( ; it != videoOutputs.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete videoOutputNamedObject;
                videoOutputNamedObject = 0;

                VideoOutputVector::iterator it = videoOutputs.begin();
                for( ; it != videoOutputs.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete videoOutputNamedObject;
                videoOutputNamedObject = 0;

                VideoOutputVector::iterator it = videoOutputs.begin();
                for( ; it != videoOutputs.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            videoOutputs.push_back( videoOutputNamedObject );
        }
        return videoOutputs;
    }


    //
    // getVideoOutput
    //
    VideoSwitchAgentFactory::VideoOutputNamedObject* VideoSwitchAgentFactory::getVideoOutput( unsigned long entityKey )
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the VideoOutput from the agent.

        TA_Base_Bus::VideoOutputCorbaDef_var videoOutput;
        CORBA_CALL_RETURN( videoOutput, m_switchManager, getVideoOutput, ( entityKey ) );//limin

        // Get the Corba name of the VideoOutput and create a named obj ref.

        VideoOutputNamedObject* videoOutputNamedObject = 0;
        try
        {
            String_var name = videoOutput->getName();
            videoOutputNamedObject = new VideoOutputNamedObject( m_agentName, name.in() );
            if ( 0 == videoOutputNamedObject )
            {
                throw TA_Base_Bus::VideoSwitchAgentException( 
                    "Failed to create new VideoOutputNamedObject." );
            }
            try
            {
                videoOutputNamedObject->assignObject( videoOutput );
		    }
		    catch (...) 
		    {
			    videoOutputNamedObject->clear();
		    }
        }
        catch( const TA_Base_Bus::VideoSwitchAgentException& )
        {
            delete videoOutputNamedObject;
            videoOutputNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const CORBA::Exception& )
        {
            delete videoOutputNamedObject;
            videoOutputNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const TA_Base_Core::ObjectResolutionException& )
        {
            delete videoOutputNamedObject;
            videoOutputNamedObject = 0; 
            throw;  // Throw it on.
        }

        return videoOutputNamedObject;
    }


    //
    // getVideoInputs
    //
    VideoSwitchAgentFactory::VideoInputVector VideoSwitchAgentFactory::getVideoInputs()
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the VideoInputs from the agent.

        TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence_var videoInputSeq;
        CORBA_CALL_RETURN( videoInputSeq, m_switchManager, getVideoInputs, () );//limin

        // Convert each group into a NamedObject and then put it in a vector.

        VideoInputVector videoInputs;
        for ( unsigned int i = 0; i < videoInputSeq->length(); i++ )
        {
            // Get the Corba name of the VideoInput.

            VideoInputNamedObject* videoInputNamedObject = 0;
            try
            {
                String_var name = videoInputSeq[ i ]->getName();
                videoInputNamedObject = new VideoInputNamedObject( m_agentName, name.in() );
                if ( 0 == videoInputNamedObject )
                {
                    throw TA_Base_Bus::VideoSwitchAgentException( 
                        "Failed to create new VideoInputNamedObject." );
                }
                try
                {
                    videoInputNamedObject->assignObject( videoInputSeq[ i ] );
		        }
		        catch (...) 
		        {
			        videoInputNamedObject->clear();
		        }
            }
            catch( const TA_Base_Bus::VideoSwitchAgentException& )
            {
                delete videoInputNamedObject;
                videoInputNamedObject = 0;

                VideoInputVector::iterator it = videoInputs.begin();
                for( ; it != videoInputs.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const CORBA::Exception& )
            {
                delete videoInputNamedObject;
                videoInputNamedObject = 0;

                VideoInputVector::iterator it = videoInputs.begin();
                for( ; it != videoInputs.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }
            catch( const TA_Base_Core::ObjectResolutionException& )
            {
                delete videoInputNamedObject;
                videoInputNamedObject = 0;

                VideoInputVector::iterator it = videoInputs.begin();
                for( ; it != videoInputs.end(); ++it )
                {
                    delete *it;
                    *it = 0;
                }
                throw;  // Throw it on.
            }

            videoInputs.push_back( videoInputNamedObject );
        }
        return videoInputs;
    }


    //
    // getVideoInput
    //
    VideoSwitchAgentFactory::VideoInputNamedObject* VideoSwitchAgentFactory::getVideoInput( unsigned long entityKey )
    {
        // Make sure the Switch Manager has been resolved.

        if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

        // Get the VideoInput from the agent.

        TA_Base_Bus::VideoInputCorbaDef_var videoInput;
        CORBA_CALL_RETURN( videoInput, m_switchManager, getVideoInput, ( entityKey ) );//limin

        // Get the Corba name of the VideoInput and create a named obj ref.

        VideoInputNamedObject* videoInputNamedObject = 0;
        try
        {
            String_var name = videoInput->getName();
            videoInputNamedObject = new VideoInputNamedObject( m_agentName, name.in() );
            if ( 0 == videoInputNamedObject )
            {
                throw TA_Base_Bus::VideoSwitchAgentException( 
                    "Failed to create new VideoInputNamedObject." );
            }
            try
            {
                videoInputNamedObject->assignObject( videoInput );
		    }
		    catch (...) 
		    {
			    videoInputNamedObject->clear();
		    }
        }
        catch( const TA_Base_Bus::VideoSwitchAgentException& )
        {
            delete videoInputNamedObject;
            videoInputNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const CORBA::Exception& )
        {
            delete videoInputNamedObject;
            videoInputNamedObject = 0; 
            throw;  // Throw it on.
        }
        catch( const TA_Base_Core::ObjectResolutionException& )
        {
            delete videoInputNamedObject;
            videoInputNamedObject = 0; 
            throw;  // Throw it on.
        }

        return videoInputNamedObject;
    }


    //
    // VideoSwitchAgentFactory
    //
    VideoSwitchAgentFactory::VideoSwitchAgentFactory()
        : m_isNameSet( false )
    {
        //resolveSwitchManager();
    }


    //
    // resolveSwitchManager
    //
    void VideoSwitchAgentFactory::resolveSwitchManager()
    {
        if ( m_isNameSet )
        {
            return;
        }

        // Get the Corba name of the agent.

        TA_Base_Core::CorbaNameList agents;
        TA_Base_Core::IConsole* console = 0;
        try
        {
            // get the local agent

            if(m_location=="")
			{
				// Determine the local agent for the physical Location of this Console Session.
				std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
				console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
				m_location = console->getLocationName();

                m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_location);
			}
			agents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( 
            TA_Base_Core::VideoSwitchAgent::getStaticType(), m_location,true );
            delete console;
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            delete console;
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                "The Corba name for the Video Switch Agent could not be determined." );
            //throw VideoSwitchAgentException( "The Corba name for the Video Switch Agent could not be determined." );
			throw VideoSwitchAgentException( "Agent cannot be contacted." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            delete console;
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", 
                "The Corba name for the Video Switch Agent could not be determined." );
            //throw VideoSwitchAgentException( "The Corba name for the Video Switch Agent could not be determined." );
			throw VideoSwitchAgentException( "Agent cannot be contacted." );
        }

        if ( 1 != agents.size() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "The Corba name for the Video Switch Agent could not be uniquely determined, because there were %d names returned.",
                agents.size() );
            //throw VideoSwitchAgentException( "The Corba name for the Video Switch Agent could not be determined." );
			throw VideoSwitchAgentException( "Agent cannot be contacted." );
        }

        m_agentName = agents[ 0 ].getAgentName();
        m_switchManager.setCorbaName( agents[ 0 ] );

        agents.clear();

        // if this is not at the OCC, then get a reference to the OCC agent
        if (m_locationType != TA_Base_Core::ILocation::OCC)
        {
            try
            {
                // ++ TD15639 AZ : cameras not loaded in KRTC station 
                if ( TA_Base_Core::RunParams::getInstance().get( PROJECT_SETTING.c_str() ) == KRTC_PROJECT_SETTING )
                {
			        agents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
                        TA_Base_Core::VideoSwitchAgent::getStaticType(),
                        TA_Base_Core::LocationAccessFactory::getInstance().getOccComLocationKey(),true);
                }
                else
                {
			        agents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
                        TA_Base_Core::VideoSwitchAgent::getStaticType(),
                        TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey(),true);
                }
            }
            catch( const TA_Base_Core::DatabaseException& e )
            {
                delete console;
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                    "The Corba name for the OCC Video Switch Agent could not be determined." );
                //throw VideoSwitchAgentException( "The Corba name for the Video Switch Agent could not be determined." );
				throw VideoSwitchAgentException( "Agent cannot be contacted." );
            }
            catch( const TA_Base_Core::DataException& e )
            {
                delete console;
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", 
                    "The Corba name for the OCC Video Switch Agent could not be determined." );
                //throw VideoSwitchAgentException( "The Corba name for the Video Switch Agent could not be determined." );
				throw VideoSwitchAgentException( "Agent cannot be contacted." );

            }

            if ( 1 != agents.size() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "The Corba name for the OCC Video Switch Agent could not be uniquely determined, because there were %d names returned.",
                    agents.size() );
                //throw VideoSwitchAgentException( "The Corba name for the OCC Video Switch Agent could not be determined." );
				throw VideoSwitchAgentException( "Agent cannot be contacted." );
            }
            // TD15639 ++

            m_occSwitchManager.setCorbaName( agents[ 0 ] );
        }
        

        m_isNameSet = true;
    }


    //
    // getLocalConsoleKey
    //
    unsigned long VideoSwitchAgentFactory::getLocalConsoleKey()
    {

        // Determine the physical console key from session.
        TA_Base_Core::IConsole* console = 0;
        unsigned long consoleKey( 0 );
        try
        {
            // Determine the local agent for the physical Location of this Console Session.
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
            console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
            consoleKey = console->getKey();
            delete console;
            console = 0;
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            delete console;
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                "The console name could not be determined." );
        throw TA_Base_Bus::VideoSwitchAgentException( "The key of this console could not be determined." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            delete console;
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", 
                "The Corba name for the Video Switch Agent could not be determined." );
            throw TA_Base_Bus::VideoSwitchAgentException( "The key of this console could not be determined." );
        }

        return consoleKey;
    }

	VideoSwitchAgentFactory::SwitchManagerNamedObject& VideoSwitchAgentFactory::getSwitchManager()
	{
		if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

		return m_switchManager;
	}

	VideoSwitchAgentFactory::SwitchManagerNamedObject& VideoSwitchAgentFactory::getOccSwitchManager()
	{
		if( !m_isNameSet )
		{
			resolveSwitchManager();
		}

		return m_occSwitchManager;
	}

	bool VideoSwitchAgentFactory::testAgentConnection()
	{
		return true;
	}

	void VideoSwitchAgentFactory::setLocationName(std::string location)
	{
		m_location=location;
	}
	

	void VideoSwitchAgentFactory::setLocationNameFromSession(std::string sessionID)
	{
		TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
		if (console)
		{
			std::string projectSetting = TA_Base_Core::RunParams::getInstance().get(PROJECT_SETTING.c_str());
			unsigned long locatinKey = console->getLocation();
			if ( projectSetting == KRTC_PROJECT_SETTING && 
				( locatinKey == TA_Base_Core::LocationAccessFactory::getInstance().getOccIntLocationKey() || 
				  locatinKey == TA_Base_Core::LocationAccessFactory::getInstance().getOccScadaLocationKey() ) ) 
			{
				m_location = TA_Base_Core::LocationAccessFactory::getInstance().getOccComLocationName();
			}
			else
			{
				m_location = console->getLocationName();
			}
		}
		else
		{
			m_location = "";
		}
		
	}
	// TD15853
	VideoSwitchAgentFactory::SwitchManagerNamedObject& VideoSwitchAgentFactory::getSwitchManager(unsigned long consoleId)
	{
		if( !m_isNameSet )
        {
            resolveSwitchManager();
        }

		TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(consoleId);
		if ((entity != NULL) && (m_locationType != TA_Base_Core::ILocation::OCC))
		{
			TA_Base_Core::ILocation::ELocationType locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(entity->getLocationName());
			
			if (locationType == TA_Base_Core::ILocation::OCC)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15853] Return OCC SwitchManager" );
				return m_occSwitchManager;
			}
		}

		delete entity;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15853] Return STATION SwitchManager" );
		return m_switchManager;
	}
	
} // TA_Base_Bus

