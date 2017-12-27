/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/Naming.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is a locally created object that wrappers the sending
  * and receiving of BindingRequestMessages
  * The executing thread will block querying this object.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ace/Synch.h>
#include <ace/Guard_T.h> 
#include "core/naming/src/Naming.h"
#include "core/naming/src/BindingRequestListener.h"
#include "core/naming/src/BindingHandler.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/NamingService_MessageTypes.h"
#include "core/naming/IDL/src/RebindNameCorbaDef.h"
#include "core/naming/src/AbstractNamedObject.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/naming/src/NamingThreadPoolSingleton.h"


namespace TA_Base_Core
{
    NamingUser::NamingUser()
    {
    }


    NamingUser::~NamingUser()
    {
        FUNCTION_ENTRY( "~NamingUser" );

        // De-registered from destructor of NamedObject

        FUNCTION_EXIT;
    }


    // initialise Naming statics
    //TA_Base_Core::NonReEntrantThreadLockable Naming::m_singletonLock;
    ACE_Mutex Naming::m_singletonLock;
    Naming* Naming::m_theClassInstance = NULL;

    Naming& Naming::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        //TA_Base_Core::ThreadGuard guard(m_singletonLock);
        ACE_Guard< ACE_Mutex > guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL
        if ( m_theClassInstance == NULL )
        {
            // Create the one & only object
            m_theClassInstance = new Naming();
        }

        FUNCTION_EXIT;
        return *m_theClassInstance;
    }


    void Naming::cleanUp()
    {
        FUNCTION_ENTRY( "cleanUp" );

        ACE_Guard< ACE_Mutex > guard( m_singletonLock );
        if ( m_theClassInstance != NULL )
        {
            delete m_theClassInstance;
            m_theClassInstance = NULL;
        }

        FUNCTION_EXIT;
    }


    Naming::Naming()
        : m_theListener( NULL ),
          m_terminating( false ),
          m_namingThreadPoolSingleton( TA_Base_Core::NamingThreadPoolSingleton::getInstance() )
    {
        FUNCTION_ENTRY( "Naming" );
        FUNCTION_EXIT;
    }


    Naming::~Naming()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Naming::~Naming begin");

        m_terminating = true;

        // call clearUp first, ensure that there is no work item callback to ObjectRepositoryInfo
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Naming::~Naming - NamingThreadPoolSingleton::cleanUp");
        m_namingThreadPoolSingleton->cleanUp();

        // Unsubscribe the message subscriber
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Naming::~Naming - MessageSubscriptionManager::unsubscribeToMessages");
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        if ( NULL != m_theListener )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Naming::~Naming - m_theListener->deactivateAndDeleteServant");
            m_theListener->deactivateAndDeleteServant();
            m_theListener = NULL;
        }

        // Guard this function
        TA_THREADGUARD( m_handlerLock );

        // Clean up the ObjectRepositoryObjects
        for ( RepositoryMap::iterator it = m_repositoryMap.begin(); it != m_repositoryMap.end(); it++ )
        {
            delete it->second;
            it->second = NULL;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Naming::~Naming - NamingThreadPoolSingleton::removeInstance");
        TA_Base_Core::NamingThreadPoolSingleton::removeInstance( m_namingThreadPoolSingleton );

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "~Naming::~Naming end");
    }


    void Naming::registerName( CORBA::Object_ptr obj, const std::string& objectName )
    {
        FUNCTION_ENTRY( "registerName" );

        TA_ASSERT( !objectName.empty(), "Object name to register was empty" );
        TA_ASSERT( !CORBA::is_nil( obj ), "Object to register was not valid" );

        Naming& theNaming = getInstance();

        // Guard this function
        TA_Base_Core::ThreadGuard guard( theNaming.m_listenerLock );

        // get the listener, it will be created if it does not exist already
        BindingRequestListener* listener = theNaming.getBindingListener();

        // bind the object to the listener
        listener->bind( obj, objectName );

        FUNCTION_EXIT;
    }


    void Naming::removeNameRegistration( const std::string& objectName )
    {
        FUNCTION_ENTRY( "removeNameRegistration" );

        TA_ASSERT( !objectName.empty(), "Object name to register was empty" );

        Naming& theNaming = getInstance();

        // Guard this function
        TA_Base_Core::ThreadGuard guard( theNaming.m_listenerLock );

        // get the listener, it will be created if it does not exist already
        BindingRequestListener* listener = theNaming.getBindingListener();

        // bind the object to the listener
        listener->unbind( objectName );

        FUNCTION_EXIT;
    }


    CORBA::Object_var Naming::resolveName( const std::string& agentName, const std::string& objectName )
    {
        FUNCTION_ENTRY( "resolveName" );

        if ( true == m_terminating )
        {
            FUNCTION_EXIT;
            return CORBA::Object::_nil();
        }

        TA_ASSERT( !agentName.empty(), "Agent name to resolve was empty" );
        TA_ASSERT( !objectName.empty(), "Object name to resolve was empty" );

        // find the repository
        ObjectRepositoryInfo* repository = getObjectRepository( agentName );

        // This will throw an exception if the object is not found
        // Hope your ready to catch!
        unsigned short retryCount = 0;
        while ( retryCount < 2 ) // We should never hit the second loop anyways as an exception will be thrown
        {
            try
            {
                // increment the retry
                retryCount++;

                FUNCTION_EXIT;
                return repository->getObject( objectName );
            }
            catch ( const TA_Base_Core::NamedObjectRepositoryCorbaDef::NameNotFound& e )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::NamedObjectRepositoryCorbaDef", e.reason.in());

                repository->invalidate(); //chenlei++

                TA_THROW( TA_Base_Core::ObjectResolutionException( TA_Base_Core::NO_OBJECT, agentName, objectName ) );
            }
            catch ( const TA_Base_Core::OperationModeException& e )
            {
                // If the second request fails we give up and let the call go through to AbstractNamedObject
                if ( 2 == retryCount )
                {
                    throw;
                }

                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in());

                // We have received an OperationModeException, clear the cache so
                // we can resolve the new control agent
                repository->invalidate();
            }
            catch ( const CORBA::Exception& cex )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", CorbaUtil::getInstance().exceptionToString(cex).c_str());

                // invalidate the repository because we have received a CORBA exception on an known, cached
                // object
                repository->invalidate();

                // Throw on an object resolution error
                TA_THROW( TA_Base_Core::ObjectResolutionException( TA_Base_Core::UNKNOWN_ERROR,
                                                                   agentName,
                                                                   objectName,
                                                                   CorbaUtil::getInstance().exceptionToString( cex ) ) );
            }
        }

        // We should never get here!
        FUNCTION_EXIT;
        return CORBA::Object::_nil();
    }


    ObjectRepositoryInfo* Naming::getObjectRepository( const std::string& agentName )
    {
        FUNCTION_ENTRY( "getObjectRepository" );

        TA_ASSERT( !agentName.empty(), "Agent name to resolve was empty" );

        // yanrong++: CL-21207
        ObjectRepositoryInfo* repository = NULL;
        // ++yanrong
        {
            // Guard this function
            TA_THREADGUARD( m_handlerLock );

            // See if the repository is cached
            RepositoryMap::iterator it = m_repositoryMap.find( agentName );

            if ( it != m_repositoryMap.end() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "NamedObjectRepository on [ %s ] fond in cache.", agentName.c_str() );

                // Return the repository, even if it is nil
                FUNCTION_EXIT;
                return it->second;
            }
            // yanrong++: CL-21207 -- create a new ObjectRepositoryInfo within the same lock scope, 2010-01-21
            else
            {
                // Create a new repository and insert into map,
                repository = new ObjectRepositoryInfo( agentName );
                m_repositoryMap[ agentName ] = repository;
            }
            // ++yanrong
        }

        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "First request for NamedObjectRepository on [ %s ]. Creating new object.", agentName.c_str() );

            // Subscribe for this agent, we need to subscribe for updates now
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBindingRequestMessage( TA_Base_Core::NamingService::RebindName,
                                                                                                       agentName,
                                                                                                       this );

            // yanrong++: CL-21207 -- this operation should be done within the same lock scope of m_handlerLock, 2010-01-21
            /*
            // Create a new repository and insert into map
            ObjectRepositoryInfo* repository = new ObjectRepositoryInfo( agentName );

            {
                TA_THREADGUARD( m_handlerLock );
                // Cache the object
                m_repositoryMap[ agentName ] = repository;
            }
            */
            // ++yanrong

            // Return the resolved object
            FUNCTION_EXIT;
            return repository;
        }
    }


    NamedObjectRepositoryCorbaDef_var Naming::getObjectRepositoryCorbaDef( const std::string& agentName )
    {
        ObjectRepositoryInfo* repositoryInfo = getObjectRepository(agentName);

        if (repositoryInfo)
        {
            return repositoryInfo->getObjectRepositoryCorbaDef();
        }

        return NamedObjectRepositoryCorbaDef::_nil();
    }


    void Naming::setObjectRepository( NamedObjectRepositoryCorbaDef_var repository,
                                      const std::string& agentName,
                                      CORBA::ULong timeToLive )
    {
        FUNCTION_ENTRY( "setObjectRepository" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "Naming::setObjectRepository() - received RebindName Message for object repository on [ %s ]", agentName.c_str() );

        // Guard this function
        TA_THREADGUARD( m_handlerLock );

        // See if the repository is cached
        RepositoryMap::iterator it = m_repositoryMap.find( agentName );

        if ( it == m_repositoryMap.end() )
        {
            // Create a new repository and insert into map
            ObjectRepositoryInfo* objectRepository = new ObjectRepositoryInfo( agentName );

            // Set the repository pointer and the timeout
            objectRepository->setObjectRepository_r( repository, timeToLive );

            // Cache the object
            m_repositoryMap[ agentName ] = objectRepository;
        }
        else
        {
            // Replace with the new
            it->second->setObjectRepository_r( repository, timeToLive );
            //it->second->invalidate();
        }

        FUNCTION_EXIT;
    }


    BindingRequestListener* Naming::getBindingListener( )
    {
        FUNCTION_ENTRY( "getBindingListener" );

        // Don't need to threadguard this as the top level functions that call this method
        // will be sufficient to protect the listener
        if ( NULL == m_theListener )
        {
            m_theListener = new TA_Base_Core::BindingRequestListener();
        }

        // Make sure that a listener exists
        TA_ASSERT(NULL != m_theListener, "A BindingRequestListener could not be created or found");

        FUNCTION_EXIT;
        return m_theListener;
    }


    void Naming::registerNamingUser( NamingUser* user )
    {
        FUNCTION_ENTRY( "registerNamingUser" );

        ThreadGuard guard( m_namingUserLock );

        // Add the user to the list
        m_namingUsers.push_back( user );
    }


    void Naming::deregisterNamingUser( NamingUser* user )
    {
        FUNCTION_ENTRY( "deregisterNamingUser" );

        ThreadGuard guard( m_namingUserLock );

        // Find the user and erase
        for ( NamingUserList::iterator it = m_namingUsers.begin(); it != m_namingUsers.end(); ++it )
        {
            // check to see if we have the correct user
            if ( (*it) == user )
            {
                // erase it
                m_namingUsers.erase( it );

                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    void Naming::rebroadcastNamedObjects( )
    {
        FUNCTION_ENTRY( "rebroadcastNamedObjects" );

        // Get the listener
        BindingRequestListener* listener = getBindingListener();
		
        // tell it to rebroadcast the objects

        // gongzong++ CL-21311 for video switch agent.
        //listener->broadcastBoundObjects();
        listener->broadcastBoundObjects(true);
        // ++gongzong

        FUNCTION_EXIT;
    }
	
	

    void Naming::receiveMessage( const CORBA::Any& message )
    {
        FUNCTION_ENTRY( "receiveMessage" );

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Naming::receiveSpecialisedMessage() - Received Message" );

        // We have just received a message so it is possible that our object reference is no longer valid
        // NOTE: We will only receive messages in the correct context because that is how we have registered
        // The data structure i want
        RebindNameCorbaDef* rebindMessage;

        // Extract to the messageState
        try
        {
            if ( ( message >>= rebindMessage ) != CORBA::Boolean(true) )
            {
                // invalid message
                FUNCTION_EXIT;
                return;
            }
        }
        catch ( ... )
        {
            // We have caught an exception in the extraction, ignore this message
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Invalid RebindMessage received, ignoring." );

            FUNCTION_EXIT;
            return;
        }

        std::string agentName( rebindMessage->agentName );

        {
            NamedObjectRepositoryCorbaDef_var objectRepository = getObjectRepositoryCorbaDef(agentName);
            if (!CORBA::is_nil(objectRepository))
            {
                try
                {
                    if (objectRepository->__is_equivalent(rebindMessage->objectRepository))
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                            "Naming::receiveMessage() - received RebindName Message for same object repository on [ %s ], do nothing", agentName.c_str() );
                        return;
                    }
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "failed to call __is_equivalent, ignoring." );
                }
            }
        }

        getObjectRepository(agentName)->clearIds();        

        // update the objectRepository in the cache
        setObjectRepository( rebindMessage->objectRepository, agentName, rebindMessage->timeToLive );

        // If this is a context only message we must clear our cache as we don't know what is and isn't valid
        if ( rebindMessage->contextOnly == CORBA::Boolean(true) )
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "Naming::receiveSpecialisedMessage() - Context only NameRebind for [ %s ]", agentName.c_str() );
        }

        // for failover issue, naming library no need to update each name object,
        // client will re-resolve for Agent if there is corba exception.
        // Go through the list of NamedObjects
        // Make sure no users can register/de-register while this is happening
        ThreadGuard guard( m_namingUserLock );

        NamingUserList::iterator it = m_namingUsers.begin();
        for ( ; it != m_namingUsers.end(); it++ )
        {
            (*it)->handleNamingUpdate( agentName, rebindMessage->boundObjects );
        }

        FUNCTION_EXIT;
    }


    void Naming::invalidateAgent( const std::string& agentName )
    {
        FUNCTION_ENTRY( "invalidateAgent" );

        // Go through the list of NamedObjects
        // Make sure no users can register/de-register while this is happening
        ThreadGuard guard( m_namingUserLock );

        for ( NamingUserList::iterator it = m_namingUsers.begin(); it != m_namingUsers.end(); ++it )
        {
            (*it)->invalidateObject( agentName );
        }

        FUNCTION_EXIT;
    }


    // yanrong++: CL-21230
    std::string Naming::getServantKey(const std::string& agentName)
    {
        FUNCTION_ENTRY( "getServantKey" );

        std::string servantKey = agentName;
        servantKey += "Repo";

        FUNCTION_EXIT;
        return servantKey;
    }
    // ++yanrong


    std::string Naming::getHostname(const std::string& agentName)
    {
        ObjectRepositoryInfo* repository = getObjectRepository(agentName);

        if (repository != NULL)
        {
            return repository->getHostname();
        }

        return "";
    }

}; // TA_Base_Core
