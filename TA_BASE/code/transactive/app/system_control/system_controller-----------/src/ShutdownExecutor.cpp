/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_controller/src/ShutdownExecutor.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Implementation of the IShutdownCorbaDef
  * interface, used by the System Manager.
  *
  */

#include "app/system_control/system_controller/src/ShutdownExecutor.h"
#include "app/system_control/system_controller/src/ProcessUtilities.h"
#include "core/data_access_interface/system_controller/src/ManagedProcessAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::CorbaUtil;
using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{

    ShutdownExecutor::ShutdownExecutor()
        : m_shutdownRetryTimes(0)
    {
        FUNCTION_ENTRY("ShutdownExecutor");

        m_shutdownRetryTimes = TA_Base_Core::getRunParamValue( "ShutdownRetryTimes", 0 );

        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "ShutdownExecutor");
        activateServant();

        FUNCTION_EXIT;
    }


    ShutdownExecutor::~ShutdownExecutor()
    {
    }


    void ShutdownExecutor::shutdown( ::CORBA::Boolean notifyPeer )
    {
        FUNCTION_ENTRY("shutdown");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "execute shutdown with parameters {notifyPeer: %d}", notifyPeer);

        if ( true == notifyPeer )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Notifying peer system controllers to shutdown." );
            notifyPeerToShutdown();

            if ( true == isNeedShutdownCentralDatabaseServer() )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Shutting down central database server." );
                shutdownCentralDatabaseServer();
            }
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Shutting down server." );
        shutdownServer();

        FUNCTION_EXIT;
    }


    bool ShutdownExecutor::isNeedShutdownCentralDatabaseServer()
    {
        FUNCTION_ENTRY("isNeedShutdownCentralDatabaseServer");

        int locationKey = TA_Base_Core::getRunParamValue( RPARAM_LOCATIONKEY, 0 );

        if ( locationKey != 0 )
        {
            TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( locationKey );

            if ( location != NULL )
            {
                TA_Base_Core::ILocation::ELocationType locationType = location->getLocationType();

                delete location;
                location = NULL;

                if ( TA_Base_Core::ILocation::OCC == locationType )
                {
                    FUNCTION_EXIT;
                    return true;
                }
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    void ShutdownExecutor::notifyPeerToShutdown()
    {
        FUNCTION_ENTRY("notifyPeerToShutdown");

        std::map<std::string, TA_Base_Core::IShutdownCorbaDef_var> peerShutdownExecutors = resolvePeerShutdownExecutor();

        for ( std::map<std::string, TA_Base_Core::IShutdownCorbaDef_var>::iterator it = peerShutdownExecutors.begin(); it != peerShutdownExecutors.end(); ++it )
        {
            if ( !CORBA::is_nil( it->second ) )
            {
                for ( size_t i = 0; i <= m_shutdownRetryTimes; ++i )
                {
                    try
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Shutting down peer %s.", it->first.c_str() );
						it->second->shutdown( false );

                        break;
                    }
                    catch (CORBA::SystemException& e)
                    {
                        std::string desc = "System exception occurred while calling peer shutdown executor ";
                        desc += it->first + " - ";
                        desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
                        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", desc);
                    }
                    catch ( ... )
                    {
                        std::string desc = "Error occurred calling peer shutdown executor ";
                        desc += it->first;
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", desc);
                    }
                }
            }
        }

        FUNCTION_EXIT;
    }


    void ShutdownExecutor::shutdownCentralDatabaseServer()
    {
        FUNCTION_ENTRY("shutdownCentralDatabaseServer");

        executeSystemCommand( "./shutdown_central_database_server" );

        FUNCTION_EXIT;
    }


    void ShutdownExecutor::shutdownServer()
    {
        FUNCTION_ENTRY("shutdownServer");

        executeSystemCommand( "pfexec /usr/sbin/init 5" );

        FUNCTION_EXIT;
    }


    std::map<std::string, TA_Base_Core::IShutdownCorbaDef_var> ShutdownExecutor::resolvePeerShutdownExecutor()
    {
        FUNCTION_ENTRY("resolvePeerShutdownExecutor");

        std::map<std::string, TA_Base_Core::IShutdownCorbaDef_var> peerShutdownExecutors;

        int port = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT).c_str());

        std::string myHostname = ProcessUtilities::getHostname();

        // Get the hostnames of my peers from the database.
        std::vector<std::string> peerHostnames = TA_Base_Core::ManagedProcessAccessFactory::getInstance().getPeersOfSystemController(myHostname);

        for ( std::vector<std::string>::iterator it = peerHostnames.begin(); it != peerHostnames.end(); ++it )
        {
            // Create a string in the form: "corbaloc::hostname:port/ShutdownExecutor"
            std::stringstream peerShutdownExecutorIor;
            peerShutdownExecutorIor << "corbaloc::" << *it << ":" << port << "/ShutdownExecutor";

            // Attempt to resolve an IShutdownCorbaDef object
            CORBA::Object_var obj = CorbaUtil::getInstance().stringToObject( peerShutdownExecutorIor.str() );

            if ( !CORBA::is_nil(obj) )
            {
                for ( size_t i = 0; i <= m_shutdownRetryTimes; ++i )
                {
                    try
                    {
						omniORB::setClientCallTimeout(obj, 3000 );
                        TA_Base_Core::IShutdownCorbaDef_var peerShutdownExecutor = TA_Base_Core::IShutdownCorbaDef::_narrow( obj );
                        peerShutdownExecutors.insert( std::make_pair(*it, peerShutdownExecutor) );
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "resolved a peer shutdown executor: %s", peerShutdownExecutorIor.str().c_str());

                        break;
                    }
                    catch (CORBA::SystemException& e)
                    {
                        std::string desc = "System exception occurred while resolving peer shutdown executor ";
                        desc += peerShutdownExecutorIor.str() + " - ";
                        desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
                        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", desc);
                    }
                    catch ( ... )
                    {
                        std::string desc = "Error occurred narrowing ";
                        desc += peerShutdownExecutorIor.str();
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", desc);
                    }
                }
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot resolve peer Shutdown Executor %s", peerShutdownExecutorIor.str().c_str());
            }
        }

        FUNCTION_EXIT;
        return peerShutdownExecutors;
    }


    void ShutdownExecutor::executeSystemCommand( const std::string& commandline )
    {
        FUNCTION_ENTRY("executeSystemCommand");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Executing command %s", commandline.c_str());
        int result = ::system( commandline.c_str() );

        if ( result != 0 )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to execute command [%s] with error code [%d]", commandline.c_str(), result );
        }

        FUNCTION_EXIT;
    }

}
