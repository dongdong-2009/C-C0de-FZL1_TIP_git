/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/corba/src/CorbaUtil.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2016/06/27 15:34:22 $
  * Last modified by:  $Author: Ouyang $
  * 
  * A singleton that is used to store much of the
  * CORBA related code.
  */

/*
* Disable STL warnings when building on Windows.
*/
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/corba/src/CorbaUtil.h"
#include "core/corba/src/ServantBase.h"
#include "core/corba/src/ServantActivator.h"
#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/StdPubUtility.h"
#include "omniORB4/internal/orbParameters.h"

#include <sys/timeb.h>
#include <time.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>


namespace TA_Base_Core
{
	
	CorbaUtil* CorbaUtil::m_theCorbaUtil = NULL;
	TA_Base_Core::NonReEntrantThreadLockable CorbaUtil::m_singletonLock;
	std::fstream CorbaUtil::m_omniLogFile;

	CorbaUtil& CorbaUtil::getInstance()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );

		if ( m_theCorbaUtil == NULL )
		{
			m_theCorbaUtil = new CorbaUtil();
		}

		return *m_theCorbaUtil;
	}


    // KT 8 Jul 03: DLLs need to use the main application's CorbaUtil instance
    // because it can't create its own "mypoa" in activate since there is already
    // one active for this process and it can't resolve the main application's 
    // "mypoa" (it gets an InvalidName exception).  
    //
    // IMPORTANT:  Don't call CorbaUtil::cleanup() if you initialise the DLL
    // CorbaUtil instance using this function, because you will clean up the
    // main application's instance.
    void CorbaUtil::setInstance( CorbaUtil& corbaUtil )
    {
        //TA_ASSERT( NULL == m_theCorbaUtil, "CorbaUtil has already been initialised" );
        if (m_theCorbaUtil != NULL )
        {
            return;
        }
        m_theCorbaUtil = &corbaUtil;
    }


	CorbaUtil::CorbaUtil()
		: m_orbVar( CORBA::ORB::_nil() ), m_running( false ), m_activated( false )
	{
	}


	CorbaUtil::~CorbaUtil()
	{
		// close the logFile if it is open
		if ( m_omniLogFile.is_open() )
		{
			omniORB::traceLevel = 0;
			omniORB::traceInvocations = false;

			m_omniLogFile.flush();
			m_omniLogFile.close();
		}
	}

	//xinsong++ no need to lock the method, but need avoid initialize repeatedly
	bool CorbaUtil::initialise(int port)
	{
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"CorbaUtil::initialise(): will attempt to pin port %d", port);

		if (! CORBA::is_nil(m_orbVar))
		{
			return true;
		}
		
		// Bruce Fountain PR540 30-Aug-2001:
		//      Set CORBA request timeout so that clients don't hang forever.
		//      RunParam has priority over environment variable has priority over
		//      default (20 seconds)
		std::string timeout;
        if ( TA_Base_Core::RunParams::getInstance().isSet(RPARAM_CORBATIMEOUT) )
		{
				timeout = TA_Base_Core::RunParams::getInstance().get(RPARAM_CORBATIMEOUT);
		}
		if ( timeout.empty() )
		{
			char* env = getenv(RPARAM_CORBATIMEOUT);
			if (env != NULL)
			{
				timeout = env;
			}
		}
		if ( timeout.empty() )
		{
			timeout = "20";
		}

		// timeouts in milliseconds
		timeout += "000";

		// The following code is likely to raise an exception if we are
		// not configured properly
		try
		{
			// Dummy the runtime parameters and use to initialise the ORB
			if (CORBA::is_nil(m_orbVar))
			{
				int     argc = 0;
				char*   argv[1];
				argv[0] = "NULL";

				char portstr[32] = {0};
				if (port > 0)           // pin the port
				{
					sprintf(portstr, "giop:tcp::%d", port);
				}
				else
				{
					sprintf(portstr, "giop:tcp::");
				}

				//TD14082
				//marvin++
				//TODO:Need to verify with derrick on how to properly handle agent synchronization with high volume of corba messages being sent
				//probably need to send it in batches of 10K alarms
				//TODO:Make GIOP_MAX_MSG_SIZE a runtime parameter
                const char* GIOP_MAX_MSG_SIZE = "104857600";	// 100 MB message size
				//TD14082
				//++marvin

                const char* options[][2] =
                    { { "endPoint", portstr },
                      { "clientCallTimeOutPeriod", timeout.c_str() },
                      { "serverCallTimeOutPeriod", timeout.c_str() },
                      { "giopMaxMsgSize", GIOP_MAX_MSG_SIZE },
                      { 0, 0 },
                      { 0, 0 } };

                const std::string END_POINT_PUBLISH = "EndPointPublish";

				std::string publishString;

                if ( true == TA_Base_Core::RunParams::getInstance().isSet( END_POINT_PUBLISH.c_str() ) )
                {
                    options[4][0] = "endPointPublish";

                    publishString = TA_Base_Core::RunParams::getInstance().get( END_POINT_PUBLISH.c_str() );

                    if ( true == publishString.empty() )
                    {
                        options[4][1] = "all(hostname|addr)";
                    }
                    else
                    {
                        options[4][1] = publishString.c_str();
                    }
                    LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                        "CorbaUtil::initialise(): calling CORBA::ORB_init() with options:\n"
                        " endPoint = %s\n"
                        " clientCallTimeOutPeriod = serverCallTimeOutPeriod = %s\n"
                        " giopMaxMsgSize = %s\n"
                        " endPointPublish = %s", 
                        portstr, timeout.c_str(), GIOP_MAX_MSG_SIZE,options[4][1] );
                }
                else
                {
                    LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                        "CorbaUtil::initialise(): calling CORBA::ORB_init() with options:\n"
                        " endPoint = %s\n"
                        " clientCallTimeOutPeriod = serverCallTimeOutPeriod = %s\n"
                        " giopMaxMsgSize = %s\n"
                        " endPointPublish = %s", 
                        portstr, timeout.c_str(), GIOP_MAX_MSG_SIZE,"unset" );
                }


				m_orbVar = CORBA::ORB_init(argc, argv,"omniORB4",options);

				// Check to see if we should be using the omni log
				if ( RunParams::getInstance().isSet( "WithOmniLog" ) )
				{
                    // If OmniToDbgLog is ALSO set, omni logging will be written to the debug log,
                    // interleaved with other LOG statements, otherwise it will be written to
                    // a separate log file.
                    if ( RunParams::getInstance().isSet( "OmniToDbgLog" ) )
                    {
                        // NOTE that the DebugLevel must be at least DebugNorm.
					    LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					        "CorbaUtil::initialise(): omni will write to the debug log file " );

                        omniORB::setLogFunction( writeOmniToDbgLog );
                    }
                    else
                    {
                        // The file seems to be put in the same directory as the omni configuration file.

					    LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					        "CorbaUtil::initialise(): omni will write to a separate log file "
                            "with the prefix %s_OMNILog_",
                            RunParams::getInstance().get( RPARAM_ENTITYNAME ).c_str() );

					    omniORB::setLogFunction( writeOmniToSeparateLog );
                    }
                                    
                    // Set the OmniOrb trace level. Refer to the OmniOrb documentation, section 
                    // 4.2 Tracing options. The following levels are defined in OmniOrb 4.0.5:
                    // level 0 critical errors only 
                    // level 1 informational messages only 
                    // level 2 configuration information and warnings 
                    // level 5 notifications when server threads are created and communication endpoints are shutdown 
                    // level 10 execution and exception traces 
                    // level 25 trace each send or receive of a giop message 
                    // level 30 dump up to 128 bytes of each giop message 
                    // level 40 dump complete contents of each giop message 
					CORBA::ULong traceLevel = 15;     // Set the default to 15
					if ( RunParams::getInstance().isSet( "OmniLogLevel" ) )
					{
						traceLevel = strtoul( RunParams::getInstance().get( "OmniLogLevel" ).c_str(), NULL, 10 );
					}

                    // Set whether local and remote invocations are logged (in addition to any logging that 
                    // may have been selected with traceLevel).
					CORBA::Boolean traceInvocations( false );
					if ( RunParams::getInstance().isSet( "OmniLogTraceInvocations" ) )
					{
						traceInvocations = true;
					}

					// set the logging level
					omniORB::traceLevel = traceLevel;
					omniORB::traceInvocations = traceInvocations;
				}

				//xinsong++ add a runparm to output omnilog easily

				if ( RunParams::getInstance().isSet("CollectCorbaLog"))
				{
					omniORB::setLogFunction( writeOmniToDbgLog );
					omniORB::traceLevel = 40;
					omniORB::traceInvocations = true;
					//omniORB::traceExceptions = true;
					//DebugUtil::getInstance().setMaxSize(10000000); //10,000,000
				}
				//end xinsong++

			}
			else // the orb is already ativated
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				    "CorbaUtil::Initialise() - Orb already initialised. Unable to initialise on port %d", port);
			}
		}
		catch(CORBA::Exception& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			exceptionToString(ex).c_str(),
			"Thrown while initialising CORBA");
			return false;
		}
		catch(...)
		{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"Unrecognised exception",
				"Thrown while initialising CORBA");
			return false;
		}

		return true;
	}


	void CorbaUtil::activate()
	{
		TA_ASSERT(!CORBA::is_nil( m_orbVar ), "Attempted to activate orb before initialising");

		if ( ! m_activated)
		{
		    LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CorbaUtil::activate(): activating ORB" );

			try
			{
				LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CorbaUtil::activate(): Locating RootPOA" );

				// Get a reference to the Root POA
				CORBA::Object_var obj = m_orbVar->resolve_initial_references("RootPOA");
				PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj);

				// Activate the POA manager
				PortableServer::POAManager_var mgr = rootPOA->the_POAManager();
				mgr->activate();

				// Create the policy list for the new POA
				CORBA::PolicyList pl;
				createPolicyList(rootPOA, pl);

				// Create a new poa to hold our objects.
				m_thePOA = rootPOA->create_POA("mypoa", mgr, pl);

				// Activate the ServantActivator...
				ServantActivator* activator = new ServantActivator();
				PortableServer::ObjectId_var id = rootPOA->activate_object(activator);
				PortableServer::ServantActivator_var saref = activator->_this();
				activator->_remove_ref();

				// Set the servant activator
				m_thePOA->set_servant_manager(saref);

				LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CorbaUtil::activate(): Activating POA manager" );

				// install the exception handlers
				omniORB::installCommFailureExceptionHandler( NULL, commFailureExceptionHandler );

				m_activated = true;
			}
			catch( const CORBA::Exception & lvEx )
			{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
						TA_Base_Core::DebugUtil::DebugError, "Orb activation failed.  Reason: '%s'",
					exceptionToString( lvEx ).c_str());
				throw;
			}
			catch (...)
			{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
						TA_Base_Core::DebugUtil::DebugError, "Unrecognised exception during activation");
				throw;
			}
		}
	}


	void CorbaUtil::createPolicyList(PortableServer::POA_ptr poa, CORBA::PolicyList& list)
	{
		list.length(4);

		// Use a specific servant manager
		list[0] = poa->create_request_processing_policy(
							 PortableServer::USE_SERVANT_MANAGER);

		// use a thread pool
		list[1] = poa->create_thread_policy(
			PortableServer::ORB_CTRL_MODEL);

		// servants have a transient life span
		list[2] = poa->create_implicit_activation_policy(
			PortableServer::IMPLICIT_ACTIVATION);

		// servants have a transient life span
		list[3] = poa->create_lifespan_policy(
			PortableServer::TRANSIENT);

	}

	void CorbaUtil::run()
	{
		activate();

		m_running = true;

		while ( m_running )
		{
				TA_Base_Core::Thread::sleep( 100 );
		}
		//getOrb().run();
	}


	void CorbaUtil::stop()
	{
		m_running = false;
	}


	void CorbaUtil::shutdown()
	{
		try
		{
		    LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CorbaUtil::shutdown(): shutting ORB down" );

			m_orbVar->shutdown( CORBA::Boolean(true) );

			// Cleanup the POA object reference, and flag as un-activated
			m_activated = false;
			m_thePOA = PortableServer::POA::_nil();

			// Clean up the orb entirely
			m_orbVar->destroy();
			m_orbVar = CORBA::ORB::_nil();
		}
		catch (const CORBA::Exception& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
				exceptionToString(ex).c_str(),
				"Thrown while shutting down the ORB");
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
				TA_Base_Core::DebugUtil::DebugWarn,
				"shutdown(): exception while shutting down the ORB");
		}
	}


	void CorbaUtil::cleanup()
	{
		if (m_theCorbaUtil != NULL)
		{
			delete m_theCorbaUtil;
			m_theCorbaUtil = NULL;
		}
	}


	CORBA::ORB& CorbaUtil::getOrb()
	{
		return *(m_orbVar.in());
	}

    CORBA::Boolean CorbaUtil::commFailureExceptionHandler(void* cookie, CORBA::ULong n_retries, const CORBA::COMM_FAILURE& ex)
    {
        // Check to see if we have a COMM failure exception that should be retried!
		if ( ex.minor() == omni::COMM_FAILURE_WaitingForReply )
		{
			// Only retry the exception once
			if ( n_retries != 0 )
			{
				// Don't retry more than once
				return CORBA::Boolean( false );
			}

			// We want to retry this call
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"CorbaUtil::commFailureExceptionHandler(): COMM_FAILURE_WaitingForReply caught, retrying last call" );

			return CORBA::Boolean( true );
		}

		// Propogate exception back to the client
		return CORBA::Boolean( false );
	}


    CORBA::Object_ptr CorbaUtil::stringToObject(const std::string& ior)
    {
		TA_ASSERT(!ior.empty(), "IOR was empty");

        return getOrb().string_to_object(ior.c_str());
    }
					

	std::string CorbaUtil::exceptionToString(const CORBA::Exception& ex)
	{
		// get the minor code and append. This is proprietary
		try
		{
			// Attemp to cast to a systemException
			const CORBA::SystemException* sysEx = CORBA::SystemException::_downcast( &ex );

            // Must check the the possibility of a NULL sysEx which can lead to 
            // core dumps runtime.   
            if (sysEx == NULL)
            {
                // We will throw this to be handled by the catch (...) below.
                throw 0;
            }

			// Extract the result string
			const char* buf = sysEx->NP_minorString();
			if ( NULL != buf )
			{
				return buf;
			}
		}
		catch ( ... )
		{
			// This is not actually proprietory at all - it should work for any ORB.
			// However the string you get is pretty terse - the proprietory exception
			// message methods (which omniORB doesn't seem to have) are usually much
			// more verbosecatch (CORBA::Exception& ex)
			try
			{
				CORBA::Any any;
				any <<= ex;
				CORBA::TypeCode_var tc = any.type();
				return tc->name();
			}
			catch ( ... )
			{
				return "Unable to extract string from exception";
			}
		}
		
		// Should never get here
		return "Unable to extract string from exception";
	}

    void CorbaUtil::setServantKey(ServantBase* theServant, const std::string& key)
    {
        TA_ASSERT(NULL != theServant, "ServantBase was NULL");
        TA_ASSERT(!key.empty(), "Servant key was was empty");

        LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CorbaUtil::setServantKey(): setting servant key of %s to %s", gPtrToStr(theServant).c_str(), key.c_str());

        try
        {
            // Resolve the omnibootstrapping POA and activate it
            CORBA::Object_var obj = getOrb().resolve_initial_references("omniINSPOA");
            PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
            PortableServer::POAManager_var mgr = poa->the_POAManager();
            mgr->activate();

            // Activate the servant with the bootstrapping POA
            PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(key.c_str());

            poa->activate_object_with_id(oid, theServant);

            // Add the bootstrapping POA to the list of POAs the
            // servant is activated with.
            theServant->addPOA(poa);

            CORBA::Object_var o = poa->servant_to_reference(theServant);
            CORBA::String_var s = TA_Base_Core::CorbaUtil::getInstance().getOrb().object_to_string(o);
            LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CorbaUtil::setServantKey(): %s activated with key %s %s", gPtrToStr(theServant).c_str(), key.c_str(), s.in());
        }
        catch (const CORBA::SystemException& ex)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, exceptionToString(ex).c_str(), "Thrown while Setting Servant Key");
            throw;
        }
        catch (const PortableServer::POA::ServantAlreadyActive)
        {
        }
        catch (const PortableServer::POA::ObjectAlreadyActive)
        {
        }
        catch (const PortableServer::POA::WrongPolicy)
        {
        }
    }

	PortableServer::POA_ptr CorbaUtil::getPOA()
	{	
			TA_ASSERT(!CORBA::is_nil(m_thePOA), "The ServantManaged POA was not active");
		return PortableServer::POA::_duplicate(m_thePOA);
	}


	void CorbaUtil::writeOmniToDbgLog( const char* logMessage )
    {
        TA_Base_Core::DebugUtil::getInstance().logLargeString(SourceInfo, TA_Base_Core::DebugUtil::DebugCORBA, logMessage);
    }


	void CorbaUtil::writeOmniToSeparateLog( const char* logMessage )
	{
		// get the current time
		struct timeb timebuffer;
		ftime ( &timebuffer );

		struct tm newtime;

	#if defined ( SOLARIS )
		newtime = *localtime_r ( &timebuffer.time, &newtime );
	#else
		newtime = *localtime ( &timebuffer.time );
	#endif

		// check to see if the file is open
		if ( ! m_omniLogFile.is_open() )
		{       
			// create the log file name
			char fileName[100] = {0};
			if ( ( &newtime ) != NULL )
			{
				sprintf ( fileName, "%s_OMNILog_%02d.%02d.%02d_%02d.%02d",
					RunParams::getInstance().get( RPARAM_ENTITYNAME ).c_str(),
					newtime.tm_mday, 
					newtime.tm_mon + 1, 
					newtime.tm_year + 1900, 
					newtime.tm_hour, 
					newtime.tm_min );
			}

			// open the log file
			m_omniLogFile.open( fileName, std::ios::out ); 
			
            // Make sure the open didn't fail
            if (m_omniLogFile.fail())
            {
                std::cerr << "CorbaUtil::writeOmniLog(): error opening file!" << std::endl;

                m_omniLogFile.clear();
            }
		}

        // Log the time of the message
        char buf[100] = {0};

        if ((&newtime) != NULL)
        {
            sprintf(buf, "[%02d:%02d:%02d:%03d] ", newtime.tm_hour, newtime.tm_min, newtime.tm_sec, timebuffer.millitm);
        }

		// write the log
		m_omniLogFile << buf << logMessage;
		m_omniLogFile.flush();
	}

#if defined( SOLARIS )
    #define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif
    
    void CorbaUtil::setCorbaParam(const char * paramNameValuePair)
    {
        char tmpBuffer[256];
        strcpy(tmpBuffer, paramNameValuePair );
        char * paramValue = tmpBuffer;
        char * paramName = TA_Base_Core::CStdPubUtility::seperateText(&paramValue, '=');

        if ( NULL == paramValue || *paramValue == 0x0 ) 
        {
            throw TA_Base_Core::RunParamFailed("the format of name value pair is wrong");
        }

        if ( NULL == paramName || *paramName == 0x0 )
        {
            throw TA_Base_Core::RunParamFailed("the format of name value pair is wrong");
        }


        int nParamValue = atoi(paramValue); 

        // the following is omniORB parameter
        //traceLevel
        if (_stricmp(paramName, "traceLevel")==0)
        {
            omniORB::setLogFunction( TA_Base_Core::CorbaUtil::writeOmniToDbgLog );
            omniORB::traceLevel = nParamValue;
        }
        //traceInvocations
        else if (_stricmp(paramName, "traceInvocations")==0)
        {
           omniORB::setLogFunction( TA_Base_Core::CorbaUtil::writeOmniToDbgLog );
          omniORB::traceInvocations = nParamValue > 0;
        }
        //traceExceptions
        else if (_stricmp(paramName, "traceExceptions")==0)
        {
            omniORB::setLogFunction( TA_Base_Core::CorbaUtil::writeOmniToDbgLog );
            omniORB::traceExceptions = nParamValue > 0;
        }
        //traceInvocationReturns
        else if (_stricmp(paramName, "traceInvocationReturns")==0)
        {
            omniORB::setLogFunction( TA_Base_Core::CorbaUtil::writeOmniToDbgLog );
            omniORB::traceInvocationReturns = nParamValue > 0;
        }
        //traceThreadId
        else if (_stricmp(paramName, "traceThreadId")==0)
        {
            omniORB::setLogFunction( TA_Base_Core::CorbaUtil::writeOmniToDbgLog );
            omniORB::traceThreadId = nParamValue > 0;
        }
        //traceTime
        else if (_stricmp(paramName, "traceTime")==0)
        {
            omniORB::setLogFunction( TA_Base_Core::CorbaUtil::writeOmniToDbgLog );
            omniORB::traceTime = nParamValue > 0;
        }

        // the following is omniParameter
        //maxSocketSend
        else if (_stricmp(paramName, "maxSocketSend")==0)
        {
            omni::orbParameters::maxSocketSend = nParamValue;
        }
        //maxSocketRecv
        else if (_stricmp(paramName, "maxSocketRecv")==0)
        {
            omni::orbParameters::maxSocketRecv = nParamValue;
        }
        //socketSendBuffer
        else if (_stricmp(paramName, "socketSendBuffer")==0)
        {
            omni::orbParameters::socketSendBuffer = nParamValue;
        }
        //scanGranularity
        else if (_stricmp(paramName, "scanGranularity")==0)
        {
            omni::orbParameters::scanGranularity = nParamValue;
        }
        //clientCallTimeOutPeriod
        else if (_stricmp(paramName, "clientCallTimeOutPeriod")==0)
        {
            omniORB::setClientCallTimeout(nParamValue);
        }
        //clientConnectTimeOutPeriod
        else if (_stricmp(paramName, "clientConnectTimeOutPeriod")==0)
        {
            omniORB::setClientConnectTimeout(nParamValue);
        }
        //inConScanPeriod
        else if (_stricmp(paramName, "inConScanPeriod")==0)
        {
            omni::orbParameters::inConScanPeriod = nParamValue;
        }
        else
        {
            throw TA_Base_Core::RunParamFailed("no such parameter");
        }
    }


    std::string CorbaUtil::getObjectAddress( CORBA::Object_ptr obj )
    {
		std::string strRetEmpty;
		return strRetEmpty;
#if 0  // disable the function due to the omniORB upgrade to 4.2.1
        IOP::IOR ior;

        try
        {
            CORBA::String_var iorstr =  getOrb().object_to_string( obj );
            size_t s = ( iorstr ? strlen(iorstr) : 0 );

            if ( s < 4 )
            {
                throw CORBA::MARSHAL(0, CORBA::COMPLETED_NO);
            }

            const char *p = iorstr;

            if ( p[0] != 'I' || p[1] != 'O' || p[2] != 'R' || p[3] != ':')
            {
                throw CORBA::MARSHAL(0,CORBA::COMPLETED_NO);
            }

            s = (s - 4) / 2;  // how many octets are there in the string
            p += 4;

            cdrMemoryStream buf((CORBA::ULong)s, 0);

            for ( int i = 0; i < (int)s; ++i )
            {
                int j = i * 2;
                CORBA::Octet v;

                if ( p[j] >= '0' && p[j] <= '9')
                {
                    v = ((p[j] - '0') << 4);
                }
                else if ( p[j] >= 'a' && p[j] <= 'f')
                {
                    v = ((p[j] - 'a' + 10) << 4);
                }
                else if ( p[j] >= 'A' && p[j] <= 'F')
                {
                    v = ((p[j] - 'A' + 10) << 4);
                }
                else
                {
                    throw CORBA::MARSHAL(0,CORBA::COMPLETED_NO);
                }

                if ( p[j+1] >= '0' && p[j+1] <= '9')
                {
                    v += (p[j+1] - '0');
                }
                else if ( p[j+1] >= 'a' && p[j+1] <= 'f')
                {
                    v += (p[j+1] - 'a' + 10);
                }
                else if ( p[j+1] >= 'A' && p[j+1] <= 'F')
                {
                    v += (p[j+1] - 'A' + 10);
                }
                else
                {
                    throw CORBA::MARSHAL(0,CORBA::COMPLETED_NO);
                }

                buf.marshalOctet(v);
            }

            buf.rewindInputPtr();
            CORBA::Boolean b = buf.unmarshalBoolean();
            buf.setByteSwapFlag(b);
            ior.type_id = IOP::IOR::unmarshaltype_id(buf);
            ior.profiles <<= buf;
        }
        catch ( CORBA::MARSHAL& )
        {
        }
        catch ( ... )
        {
        }

        if ( ior.profiles.length() == 0 && strlen(ior.type_id) == 0 )
        {
            return "";
        }

        std::stringstream address;

        try
        {
            for ( unsigned long i = 0; i < ior.profiles.length(); ++i )
            {
                if ( IOP::TAG_INTERNET_IOP == ior.profiles[i].tag )
                {
                    if ( false == address.str().empty() )
                    {
                        address << ",";
                    }

                    IIOP::ProfileBody body;
                    IIOP::unmarshalProfile( ior.profiles[i], body );
                    address << static_cast<const char*>( body.address.host ) << ":" << body.address.port;

                    for ( size_t j = 0; j < body.components.length(); ++j )
                    {
                        CORBA::String_var content = IOP::dumpComponent( body.components[j] );
                        std::string s = content.in();

                        if ( boost::starts_with( s, "TAG_ALTERNATE_IIOP_ADDRESS" ) )
                        {
                            std::vector<std::string> vs;
                            boost::algorithm::split( vs, s, boost::algorithm::is_space(), boost::algorithm::token_compress_on );

                            if ( 3 == vs.size() )
                            {
                                address << "," << vs[1] << ":" << vs[2];
                            }
                        }
                    }
                }
            }

            address << ":" << ior.type_id;
        }
        catch ( CORBA::MARSHAL& )
        {
        }
        catch ( ... )
        {
        }

        return address.str();
#endif // #if 0
    }


}; // TA_Base_Core
