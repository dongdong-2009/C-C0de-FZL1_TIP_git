/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NotificationServiceThread.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 * A thread that executes a CORBA Notification Service.
 * Fork/exec code adapted from Manila ModbusAgent's MqeExecutionThread class.
 *
 */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#ifndef WIN32
	#include <sys/wait.h>
	#include <unistd.h>
	#include <signal.h>
	#include <fcntl.h>
#endif 
#include <exception>

#include "app/notification_service/src/NotificationServiceThread.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TA_String.h"
#include "core/corba/src/CorbaUtil.h"
#include "omniNotify.h"

// This is used by the corba_wrappers and is
// specific to the omniNotify guff
#define __OMNIORB4__

#ifdef _MSC_VER
  #include "src/services/include/omniNotify/corba_wrappers.h"
#else
  #include "omniNotify/corba_wrappers.h"
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    NotificationServiceThread::NotificationServiceThread( const std::string& serviceExecutable,
                                                          const std::vector<std::string>& serviceArguments )
        : m_serviceExecutable( serviceExecutable ),
          m_serviceArguments( NULL ),
          m_argumentsSize( 0 ),
          m_terminateEnabled( true )
    {
        FUNCTION_ENTRY( "NotificationServiceThread" );

        TA_ASSERT( serviceExecutable.length() > 0, "No executable name passed to NotificationServiceThread" );

        LOG2( SourceInfo, DebugUtil::DebugDebug,
             "NotificationServiceThread::NotificationServiceThread(): Executable: %s. Args: %s",
             serviceExecutable.c_str(), TA_Base_Core::toString(serviceArguments.begin(), serviceArguments.end()).c_str() );

        initialiseServiceArguments( serviceArguments );

        FUNCTION_EXIT;
    }


    NotificationServiceThread::~NotificationServiceThread()
    {
        FUNCTION_ENTRY( "~NotificationServiceThread" );

        #ifdef WIN32
            //do nothing causes a seg-fault
        #else
            // Clean up the argument list
            for ( int argCounter = 0; argCounter < ( m_argumentsSize - 1) ; ++argCounter )
            {
                delete[] m_serviceArguments[ argCounter ];
                m_serviceArguments[ argCounter ] = NULL;
            }

            delete[] m_serviceArguments;
            m_serviceArguments = NULL;
        #endif

        FUNCTION_EXIT;
    }


    void NotificationServiceThread::run()
    {
        FUNCTION_ENTRY( "run" );

        TA_ASSERT( m_serviceExecutable.length() > 0, "Zero length executable path" );

        // Log the entire execution string
        std::string execString;
        for ( int argCounter = 0; argCounter < ( m_argumentsSize-1 ); ++argCounter )
        {
            execString.append( m_serviceArguments[ argCounter ] );
            execString.append( " " );
        }

        LOG1( SourceInfo, DebugUtil::DebugInfo,
             "Attempting to initialise Omni Notification service with args: %s", execString.c_str() );

        // Use existing orb
        WRAPPED_ORB_OA::_poa_activated = 1;
        WRAPPED_ORB_OA::_one_call_per_connection = 1;
        WRAPPED_ORB_OA::_max_giop_connections = 5;

        // set up all the corba wrappers bollocks
        WRAPPED_ORB_OA::_orb = &TA_Base_Core::CorbaUtil::getInstance().getOrb();
        WRAPPED_ORB_OA::_poa = TA_Base_Core::CorbaUtil::getInstance().getPOA();

        CORBA::Object_var obj2 = WRAPPED_ORB_OA::_orb->resolve_initial_references("DynAnyFactory");
        WRAPPED_ORB_OA::_dynany_fac = DynamicAny::DynAnyFactory::_narrow(obj2);

        // Initialise the Notification Service
        int argc = m_argumentsSize - 1;

        if ( CORBA::is_nil( omniNotify::init_server(argc, m_serviceArguments) ) )
        {
            LOG0(SourceInfo, DebugUtil::DebugFatal, "Unable to inititialise Omni Notification Service. Exiting" );
        }
        else
        {
            LOG0( SourceInfo, DebugUtil::DebugInfo, "Omni Notification service successfully initialised" );

            m_terminateEnabled = true;

            // Wait for the Notification Service to be destroyed
            // This will block until destroy is called by terminate
            omniNotify::wait_for_destroy();

            LOG0(SourceInfo, DebugUtil::DebugInfo, "Omni Notification Service destroyed. Shutting down" );

            //WRAPPED_ORB_OA::cleanup();
        }

        FUNCTION_EXIT;
    }


    void NotificationServiceThread::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        //
        // If we are in the middle of forking and exec'ing the Service, then wait for
        // that to complete
        //
        if ( ! m_terminateEnabled )
        {
            LOG0( SourceInfo, DebugUtil::DebugDebug, "Waiting for service execution to complete" );

            while ( ! m_terminateEnabled )
            {
                sleep( 20 );
            }

            LOG0( SourceInfo, DebugUtil::DebugDebug, "Service execution completed, proceed with termination" );
        }

        AttNotification::Server_var server = omniNotify::get_server();

        if ( CORBA::is_nil( server ) )
        {
            LOG0( SourceInfo, DebugUtil::DebugDebug, "Unable to contact Omni Notification service. Shut down regardless." );
        }
        else
        {
            server->destroy();
        }

        FUNCTION_EXIT;
    }


    void NotificationServiceThread::initialiseServiceArguments( const std::vector<std::string>& arguments )
    {
        FUNCTION_ENTRY( "initialiseServiceArguments" );

        TA_ASSERT( NULL == m_serviceArguments, "Service arguments have already been initialised" );

        try
        {
            // The first argument is the executable path and the last one is a NULL pointer, so add
            // 2 to the total
            m_argumentsSize = arguments.size() + 2;

			if ( m_argumentsSize > 0 )
			{
				m_serviceArguments = new char*[ m_argumentsSize ];
			
				// execv() expects first argument is executable path
				m_serviceArguments[0] = new char[ m_serviceExecutable.length() + 1 ];
				m_serviceArguments[0] = strdup( m_serviceExecutable.c_str() );

				// Add parameter arguments 
				std::vector<std::string>::const_iterator argIter = arguments.begin();
				int argCounter;
				for( argCounter = 1; argIter != arguments.end(); ++argIter, ++argCounter )
				{
					m_serviceArguments[argCounter] = new char[argIter->length() + 1];
					m_serviceArguments[argCounter] = strdup( argIter->c_str() );
				}

                // execv() expects last argument is a NULL
                m_serviceArguments[argCounter + 1] = NULL;
            }
        }
        catch( std::exception& sex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "std::exception", sex.what() );
            throw;
        }

        FUNCTION_EXIT;
    }


}

