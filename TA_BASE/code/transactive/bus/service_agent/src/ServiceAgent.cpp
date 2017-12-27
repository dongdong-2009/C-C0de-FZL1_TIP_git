/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/service_agent/src/ServiceAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#include "ServiceAgent.h"
#include <new.h>

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/UtilitiesException.h"
#include "core/exceptions/src/ManagedProcessException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"
#include "core/exceptions/src/InvalidCommandLineException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/process_management/src/ManagedProcess.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/generic_agent/src/IEntity.h"
#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/Naming.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmPublicationManager.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"

#include <winsock2.h>  // Used by getHostName 

using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::ConfigUpdateDetails;
using TA_Base_Core::EOperationMode;
using TA_Base_Core::CorbaUtil;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;
using TA_Base_Core::ManagedProcess;
using TA_Base_Core::UtilityInitialiser;
using TA_Base_Core::ManagedProcessException;
using TA_Base_Core::ProcessAlreadyRunningException;
using TA_Base_Core::UtilitiesException;
using TA_Base_Core::InvalidCommandLineException;
using TA_Base_Core::OnlineUpdateListener;

namespace TA_Base_Bus
{

    const unsigned int ServiceAgent::NUM_RETRIES = 10;

    //
    // Constructor
    //
    ServiceAgent::ServiceAgent(int argc, char* argv[], IGenericAgentUser* registeredUser, int portNum)
        : m_agentStatus(TA_Base_Core::Startup),
          m_registeredUser(registeredUser),
          m_agentEntityData(0)//,
          //m_managedProcess(0)
    {
        try
        {
            #if defined(_MSC_VER)
                _set_new_handler(TA_Base_Bus::ServiceAgent::outOfMemoryHandler);
            #endif // defined(_MSC_VER)

			//UtilityInitialiser::initialiseUtilities(argc, argv);
			intitialiseUtilities(argc, argv, portNum);
            registerAsRunParamUser();
			//m_managedProcess = new ManagedProcess(this);
            // Tell the System Controller we have started starting up
            //m_managedProcess->changeState(m_agentStatus);

            doEntityConfiguration();
            doAgentSetup();
			//m_managedProcess->registerManagedApplication(_this());
            activateServant();
			// Service Agent is always in control mode
			m_registeredUser->agentSetControl();
        }
        catch( const InvalidCommandLineException& ice )
        {
            // UtilityInitialiser failed to initialise, re-throw as a GenericAgentException
            TA_THROW(GenericAgentException(GenericAgentException::INITIALISATION_ERROR, ice.what()));
        }
        catch( const UtilitiesException& ue )
        {
            // UtilityInitialiser failed to initialise, re-throw as a GenericAgentException
            TA_THROW(GenericAgentException(GenericAgentException::INITIALISATION_ERROR, ue.what()));
        }
        catch(const ManagedProcessException& mpe)
        {
            // The servant has not yet been activated, tell the process monitor that we are
            // terminating and then re-throw as a GenericAgentException
            //if (m_managedProcess)
            //{
            //    m_managedProcess->notifyTerminating(TA_Base_Core::InitError, mpe.what());
            //}
            TA_THROW(GenericAgentException(GenericAgentException::INITIALISATION_ERROR, mpe.what()));
        }
        catch(const ProcessAlreadyRunningException&)
        {
            // The servant has not yet been activated, tell the process monitor that we are
            // terminating and then re-throw as a GenericAgentException
            //m_managedProcess->notifyTerminating(TA_Base_Core::InitError, "Process already running");
            TA_THROW(GenericAgentException(GenericAgentException::INITIALISATION_ERROR, "Process already running"));
        }
        catch(const GenericAgentException&)
        {
            // Tell the process monitor we are terminating, deactivate the servant, and re-throw
            //m_managedProcess->notifyTerminating(TA_Base_Core::InitError, gae.what());
			deactivateServant();
            throw;
        }
        catch( ... ) // Ensure we only throw GenericAgentExceptions
        {
            //m_managedProcess->notifyTerminating(TA_Base_Core::InitError, "Generic Agent failed to initialise");
			deactivateServant();
            TA_THROW(GenericAgentException(GenericAgentException::INITIALISATION_ERROR));
        }
    }


    //
    // Destructor
    //
    ServiceAgent::~ServiceAgent()
    {
        cleanup();
        LOG ( SourceInfo, DebugUtil::FunctionExit, "Destructor");
    }


    //
    // onTerminate
    //
    void ServiceAgent::onTerminate()
    {
        //m_managedProcess->notifyTerminating(
        //    TA_Base_Core::RequestedTerminate,
        //    "Terminating due to request from System Controller");
        
        // Update of this state already sent in the call above
        m_agentStatus = TA_Base_Core::Terminating;

        // Deactivate managed process
        //m_managedProcess->deactivateServant();

        // Deactivate servant
        this->deactivateServant();

        std::map<unsigned long, IEntity*>::iterator entityPtr = m_entities.begin();
        while( entityPtr != m_entities.end() )
        {
            if( entityPtr->second != NULL )
            {
               entityPtr->second->stop();
            }
            entityPtr++;
        }

        // Allow the derived agent to do any house keeping
        if( m_registeredUser != NULL )
        {
            m_registeredUser->agentTerminate();
        }

        // This will result in the CorbaUtil run method returning
        // inside our own run method.
        CorbaUtil::getInstance().stop();
    }


    //
    // addEntity
    //
    void ServiceAgent::addEntity( unsigned long p_entityKey, IEntity* p_entity )
    {
        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        m_entities.insert(std::pair<unsigned long, IEntity*>(p_entityKey, p_entity));
        p_entity->start();
        OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, p_entityKey);
    }


    //
    // deleteEntity
    //
    void ServiceAgent::deleteEntity( unsigned long p_entityKey )
    {
        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        std::map<unsigned long,IEntity*>::iterator p = m_entities.find(p_entityKey);

        if( p == m_entities.end() )
        {
            return;
        }

        if( p->second != NULL )
        {
            p->second->stop();
            delete p->second;
        }
        m_entities.erase(p);
    }


    //
    // modifyEntity
    //
    void ServiceAgent::modifyEntity( unsigned long p_entityKey, IEntity* p_entity )
    {
        deleteEntity(p_entityKey);
        addEntity(p_entityKey, p_entity);
    }


    //
    // handleUpdateForAgentEntity
    //
    void ServiceAgent::handleUpdateForAgentEntity(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "handleUpdateForAgentEntity" );

        if ( updateEvent.getModifications() == TA_Base_Core::Delete )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a 'Delete' update for the agent entity. Ignoring." );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            return;
        }

        if ( updateEvent.getModifications() == TA_Base_Core::Create )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a 'Create' update for a key that should already exist. Ignoring" );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            return;
        }

        if ( updateEvent.getModifications() == TA_Base_Core::Update )
        {
            if ( updateEvent.getChangedParams().empty() )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a configuration update with an empty list of changes. Ignoring" );
                LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
                return;
            }

            TA_ASSERT( m_registeredUser != NULL, "Could not update agent entity as agent pointer is NULL");

            m_registeredUser->processOnlineUpdate(updateEvent);
        }
    }


    //
    // handleUpdateForChildEntity
    //
    void ServiceAgent::handleUpdateForChildEntity(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "handleUpdateForChildEntity" );

        if ( updateEvent.getModifications() == TA_Base_Core::Create )
        {
			try
			{
				IEntityData* entityData = EntityAccessFactory::getInstance().getEntity(updateEvent.getKey());

				if( entityData->getAgent() == m_agentEntityData->getKey() )
				{
					m_childEntityData.push_back(entityData);

					IEntity* entity = m_registeredUser->createEntity( *entityData );
					if( entity != NULL )
					{
						addEntity((entityData)->getKey(), entity);
						m_registeredUser->processOnlineUpdate(updateEvent);
					}
				}
			}
			catch(TA_Base_Core::DataException& ex)
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", ex.what() );
			}
			catch(TA_Base_Core::DatabaseException& ex)
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", ex.what() );
			}

            LOG( SourceInfo, DebugUtil::FunctionExit, "handleUpdateForChildEntity" );
            return;
        }

        if ( updateEvent.getModifications() == TA_Base_Core::Delete )
        {
            m_registeredUser->processOnlineUpdate(updateEvent);
            LOG( SourceInfo, DebugUtil::FunctionExit, "handleUpdateForChildEntity" );
            return;
        }

        if ( updateEvent.getModifications() == TA_Base_Core::Update )
        {
			if ( updateEvent.getChangedParams().empty() )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a configuration update with an empty list of changes. Ignoring" );
                LOG( SourceInfo, DebugUtil::FunctionExit, "handleUpdateForChildEntity" );
            }
            else
            {
                m_registeredUser->processOnlineUpdate(updateEvent);
            }
        }
    }


    //
    // processUpdate
    //
    void ServiceAgent::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );

        TA_ASSERT( m_agentEntityData != NULL, "Could not update entity as agent entity data is NULL");
    
        if ( updateEvent.getType() != TA_Base_Core::ENTITY_OWNER )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Type of configuration update does not match the update we are interested in. Ignoring" );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            return;
        }

        if ( updateEvent.getKey() == m_agentEntityData->getKey() )
        {
            handleUpdateForAgentEntity(updateEvent);
        }
        else
        {
            handleUpdateForChildEntity(updateEvent);
        }
        
        LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
    }


    //
    // getOperationMode
    //
	EOperationMode ServiceAgent::getOperationMode()
    {
        return m_mode;
    }


    //
    // setOperationMode
    //
    void ServiceAgent::setOperationMode(EOperationMode mode)
    {
		FUNCTION_ENTRY("setOperationMode");

        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

		if (m_mode == TA_Base_Core::Control && mode == TA_Base_Core::Control)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"In Control mode and told to change to Control again. Rebroadcasting named objects.");

			// Agent told to go into Control when it is already in Control.
			// Just need NamedObject references to be re-resolved.
			TA_Base_Core::Naming::getInstance().rebroadcastNamedObjects();

			// Update the System Controller state.
            m_agentStatus = TA_Base_Core::RunningControl;
            //m_managedProcess->changeState(m_agentStatus);

			FUNCTION_EXIT;
			return;
		}

        m_mode = mode;

        if( mode == TA_Base_Core::Monitor )
        {
            // Let the System Controller know we are about to change state
            m_agentStatus = TA_Base_Core::GoingToMonitor;
            //m_managedProcess->changeState(m_agentStatus);

            if( m_registeredUser != NULL )
            {
                try
                {
                    m_registeredUser->agentSetMonitor();
                    std::map<unsigned long, IEntity*>::iterator entityPtr = m_entities.begin();
                    while( entityPtr != m_entities.end() )
                    {
                        if( entityPtr->second != NULL )
                        {
                           entityPtr->second->setToMonitorMode();
                        }
                        entityPtr++;
                    }
                }
                catch(...)
                {
                    // Agent threw an exception and didn't handle it.
                    TA_ASSERT(false, "Unhandled exception in agent when changing to Monitor mode");
                }
            }
            TA_Base_Core::RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_MONITOR);

            // Let the System Controller know we have successfully changed state
            m_agentStatus = TA_Base_Core::RunningMonitor;
            //m_managedProcess->changeState(m_agentStatus);
        }
        else
        {
            if( mode == TA_Base_Core::Control )
            {
                // Let the System Controller know we are about to change state
                m_agentStatus = TA_Base_Core::GoingToControl;
                //m_managedProcess->changeState(m_agentStatus);

                if( m_registeredUser != NULL )
                {
                    try
                    {
                        m_registeredUser->agentSetControl();
                        std::map<unsigned long, IEntity*>::iterator entityPtr = m_entities.begin();
                        while( entityPtr != m_entities.end() )
                        {
                            if( entityPtr->second != NULL )
                            {
                               entityPtr->second->setToControlMode();
                            }
                            entityPtr++;
                        }
                    }
                    catch(...)
                    {
                        // Agent threw an exception and didn't handle it.
                        TA_ASSERT(false, "Unhandled exception in agent when changing to Control mode");
                    }
                }
                TA_Base_Core::RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_CONTROL);

                // Let the System Controller know we have successfully changed state
                m_agentStatus = TA_Base_Core::RunningControl;
                //m_managedProcess->changeState(m_agentStatus);
            }
        }        
		
		FUNCTION_EXIT;
    }


	void ServiceAgent::invalidateNotifyHosts(const TA_Base_Core::IManagedAgentCorbaDef::NotifyHostSequence& notifyHosts)
	{
        FUNCTION_ENTRY( "invalidateNotifyHosts" );

		// Go through the list of hosts and invalidate in the Messaging Library
		for ( unsigned int i = 0; i < notifyHosts.length(); i++)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
				"Invalidating notify host: %s", notifyHosts[i].in() );

			TA_Base_Core::ConnectionChecker::getInstance().invalidateService( notifyHosts[i].in() );
		}

        FUNCTION_EXIT;
	}


    //
    // notifyGroupOffline
    //
    void ServiceAgent::notifyGroupOffline(const char* group)
    {
		FUNCTION_ENTRY( "notifyGroupOffline" );

		m_registeredUser->notifyGroupOffline( std::string(group) );

		FUNCTION_EXIT;
    }


    //
    // notifyGroupOnline
    //
    void ServiceAgent::notifyGroupOnline(const char* group)
    {
		FUNCTION_ENTRY( "notifyGroupOnline" );

		m_registeredUser->notifyGroupOnline( std::string(group) );

		FUNCTION_EXIT;
    }


    //
    // run
    //
    void ServiceAgent::run()
    {
        //m_managedProcess->startMonitoringThread();

        // This should be called after all startup has been performed.
        // Therefore we can now pass our state to the system controller
        if (m_mode == TA_Base_Core::Control)
        {
            m_agentStatus = TA_Base_Core::RunningControl;
        }
        else if (m_mode == TA_Base_Core::Monitor)
        {
            m_agentStatus = TA_Base_Core::RunningMonitor;
        }

        //m_managedProcess->changeState(m_agentStatus);

        // This call blocks while servicing CORBA requests
        CorbaUtil::getInstance().run();
    }


    //
    // onRunParamChange
    //
    void ServiceAgent::onRunParamChange(const std::string& name,const std::string& value)
    {
        // New debug file name
        if (name.compare(RPARAM_DEBUGFILE) == 0)
        {
            DebugUtil::getInstance().setFile(value.c_str());
        }
        // New debug level
        else if (name.compare(RPARAM_DEBUGLEVEL) == 0)
        {
            DebugUtil::getInstance().setLevel( DebugUtil::getDebugLevelFromString(value.c_str()));
        }
        // New max file size
        else if (name.compare(RPARAM_DEBUGFILEMAXSIZE) == 0)
        {
            DebugUtil::getInstance().setMaxSize(atoi(value.c_str()));
        }
        // New max number of debug files
        else if (name.compare(RPARAM_DEBUGMAXFILES) == 0)
        {
            DebugUtil::getInstance().setMaxFiles(atoi(value.c_str()));
        }
		// Use pid in filenames
		else if (name.compare(RPARAM_DEBUGPIDFILENAMES) == 0)
		{
			DebugUtil::getInstance().encryptPidInFilenames(value);
		}
		// Turn off logging for these debug levels
		else if (name.compare(RPARAM_DEBUGLEVELOFF) == 0)
		{
			DebugUtil::getInstance().decodeDisabledLevels(value);
		}
		// Filenames for debug levels
		else if (name.compare(RPARAM_DEBUGLEVELFILE) == 0)
		{
			DebugUtil::getInstance().decodeLevelFilenames(value);
		}
		else if (name.compare(RPARAM_GROUPSOFFLINE) == 0)
		{
			notifyGroupOffline(value.c_str());
		}
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                "Received change notification for unsupported parameter %s", name.c_str());
        }
    }


    //
    // getAgentEntityConfiguration
    //
    IEntityData* ServiceAgent::getAgentEntityConfiguration()
    {
        return m_agentEntityData;
    }


    //
    // getEntityMap
    //
    const std::map<unsigned long, IEntity*>* ServiceAgent::getEntityMap()
    {
        return &m_entities;
    }


    //
    // doEntityConfiguration
    //
    void ServiceAgent::doEntityConfiguration()
    {
        try
        {
            if( !RunParams::getInstance().isSet(RPARAM_ENTITYNAME) )
            {
                TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR,
                                            "Entity name RunParam has not been set") );
            }
            // Get the entity name of the agent
            std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);

            m_agentEntityData = EntityAccessFactory::getInstance().getEntity(entityName);
        
            // Get all the entities that the agent is responsible for
            m_childEntityData = EntityAccessFactory::getInstance().getDescendantsOfAgent(entityName);

            std::vector<IEntityData*>::iterator p = m_childEntityData.begin();
            while( p != m_childEntityData.end() )
            {
                if( m_registeredUser != NULL )
                {
                    // Populate our vector
                    IEntity* child = m_registeredUser->createEntity( **p );
                    if( child != NULL )
                    {
                        addEntity((*p)->getKey(), child);
                    }
                }
                p++;
            }

            //
            // Register interest for online updates of the agent entity
            // 
            OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, m_agentEntityData->getKey());
        }
        catch(const TA_Base_Core::DatabaseException&)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"DatabaseException", 
                 "Throwing this as a GenericAgent exception to reduce the amount of exceptions the client has to catch" );
                TA_THROW( GenericAgentException(GenericAgentException::CANNOT_CONNECT_TO_DB) );
        }
        catch(const TA_Base_Core::DataException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"DatabaseException", 
                 "Throwing this as a GenericAgent exception to reduce the amount of exceptions the client has to catch" );
            switch ( dbe.getFailType() )
            {
                case ( TA_Base_Core::DataException::WRONG_TYPE ):
                        TA_THROW ( GenericAgentException(GenericAgentException::INVALID_ENTITY_TYPE) );
                        break;
                case ( TA_Base_Core::DataException::NO_VALUE ):
                        TA_THROW ( GenericAgentException(GenericAgentException::ENTITY_NOT_CONFIGURED) );
                        break;
                case ( TA_Base_Core::DataException::NOT_UNIQUE ):
                        TA_THROW ( GenericAgentException(GenericAgentException::ENTITY_NAME_NOT_UNIQUE) );
                        break;
                default:
                    TA_ASSERT(false, "Do not recognise the DataException type");
            }
        }
        catch( ... )
        {
            // We are only expecting database and data exceptions. Throw anything else as a 
            // Generic Agent Exception
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"Unknown", 
                 "Throwing this as a GenericAgent exception to reduce the amount of exceptions the client has to catch" );
            TA_THROW( GenericAgentException(GenericAgentException::ENTITY_CONFIG_FAILURE) );
        }
    }


    //
    // doAgentSetup()
    //
    void ServiceAgent::doAgentSetup()
    {
        // Get the agents initial operation mode - always control for a service agent
        std::string operationMode = RunParams::getInstance().get(RPARAM_OPERATIONMODE);
        if( 0 == operationMode.compare(RPARAM_MONITOR) )
        {
            m_mode = TA_Base_Core::Monitor;
        }
        else
        {
            if( 0 == operationMode.compare(RPARAM_CONTROL) )
            {
                m_mode = TA_Base_Core::Control;
            }
            else
            {
                TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR,
                                                "Operation Mode RunParam has not been set") );
            }
        }

        if( m_registeredUser != NULL )
        {
            m_registeredUser->registerForStateUpdates();
        }
    }


    //
    // registerAsRunParamUser
    //
    void ServiceAgent::registerAsRunParamUser()
    {
        // Register for run param updates
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILE);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVEL);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILEMAXSIZE);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGMAXFILES);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGPIDFILENAMES);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELOFF);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELFILE);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_GROUPSOFFLINE);
    }


    //
    // cleanup
    //
    void ServiceAgent::cleanup()
    {
        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        //if( m_managedProcess != NULL )
        //{
        //    m_managedProcess->deactivateAndDeleteServant();
        //    m_managedProcess = 0;
        //}

        if( m_agentEntityData != NULL )
        {
            delete m_agentEntityData;
            m_agentEntityData = 0;
        }

        OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::ENTITY_OWNER, *this);

        std::vector<TA_Base_Core::IEntityData*>::iterator entityDataPtr = m_childEntityData.begin();
        while( entityDataPtr != m_childEntityData.end() )
        {
            // Clean up the m_childEntityData vector
            if( *entityDataPtr != NULL )
            {
                delete *entityDataPtr;
                *entityDataPtr = 0;
            }
            entityDataPtr++;
        }
        m_childEntityData.clear();

        std::map<unsigned long, IEntity*>::iterator entityPtr = m_entities.begin();
        while( entityPtr != m_entities.end() )
        {
            // Clean up the m_entities map
            if( entityPtr->second != NULL )
            {
                delete entityPtr->second;
                entityPtr->second = 0;
            }
            entityPtr++;
        }
        m_entities.clear();

		// Clean up the alarm subsystem
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();	
		TA_Base_Core::AlarmPublicationManager::getInstance().cleanUpSuppliers();
		TA_Base_Core::AlarmSubscriptionManager::getInstance().cleanUp();

		// Clean up naming
        TA_Base_Core::Naming::cleanUp();

        // Clean up the Event Consumers
        TA_Base_Core::MessageSubscriptionManager::getInstance().cleanUp();
        TA_Base_Core::MessagePublicationManager::getInstance().cleanUpSuppliers();

        CorbaUtil::getInstance().shutdown();

        CorbaUtil::cleanup();

        // Deregister ourselves 
        RunParams::getInstance().deregisterRunParamUser(this);
    }


    //
    // outOfMemoryHandler
    //
    int ServiceAgent::outOfMemoryHandler( size_t size )
    {
        TA_THROW(std::bad_alloc());
        // Shouldn't reach here but if it does, this should tell new to stop allocation attempts
        return 0;  
    }

	void ServiceAgent::intitialiseUtilities(int argc, char* argv[], int portNum)
	{
		if( false == RunParams::getInstance().parseCmdLine(argc, argv) )
		{
			TA_THROW (GenericAgentException(GenericAgentException::INITIALISATION_ERROR, 
											"Command line arguments are in invalid format"));
		}
		initialiseDebugUtil();
		initialiseCorba(portNum);
		locateSystemController();
		retrieveRunParams();
		RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_CONTROL);
	}

	void ServiceAgent::initialiseDebugUtil()
    {
        // Set the debug file name
        std::string param = RunParams::getInstance().get(RPARAM_DEBUGFILE);
        if (!param.empty())
        {
            DebugUtil::getInstance().setFile(param.c_str());
        }

        // Set the debug level
        param = RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
        if (!param.empty())
        {
            DebugUtil::getInstance().setLevel(DebugUtil::getDebugLevelFromString(param.c_str()));
        }

        // Set the maximum size of a debug file in bytes
        param = RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
        if (!param.empty())
        {
            DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
        }

        // Set the maximum number of log files
        param = RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
        if (!param.empty())
        {
            DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
        }

		// Use Pid encoding in filenames
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGPIDFILENAMES);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().encryptPidInFilenames(param);
        }

		// Use debug level specific log files
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELFILE);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().decodeLevelFilenames(param);
        }

		// Turn off logging for debug levels
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELOFF);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().decodeDisabledLevels(param);
        }
    }


    //
    // initialiseCorba
    //
    void ServiceAgent::initialiseCorba(int portNum)
    {
		int port = portNum;

		// If a different port is set we should use that
		if ( RunParams::getInstance().isSet( "PortNumber" ) )
		{
			port = atoi( RunParams::getInstance().get( "PortNumber" ).c_str());
		}

		// Initialise on the default, or specified, port
        if ( false == CorbaUtil::getInstance().initialise( port ) )
        {
			TA_THROW( GenericAgentException(GenericAgentException::INITIALISATION_ERROR, "Failed to initialise corba") );
        }

        try
        {
            CorbaUtil::getInstance().activate();
        }
        catch( ... )
        {
			TA_THROW( GenericAgentException(GenericAgentException::INITIALISATION_ERROR, "Failed to initialise corba") );
        }
    }

	void ServiceAgent::locateSystemController()
	{
		using TA_Base_Core::RunParams;
		using TA_Base_Core::CorbaUtil;

		static const std::string HOST_NAMES = "HostNames";
		static const std::string PORT_NUMBER = "MgrPort";
        static const std::string DEFAULT_HOST = "localhost";
        static const std::string NUM_RETRIES_PARAM = "NumRetries";

		LOG( SourceInfo, DebugUtil::FunctionEntry, "locateSystemController" );

		std::string hosts = RunParams::getInstance().get(HOST_NAMES.c_str());

		//
		// Convert the portnumber into an unsigned long
		//
		unsigned long portNumber = TA_Base_Core::DEFAULT_MGR_PORT;

		std::string portNumberString = RunParams::getInstance().get(PORT_NUMBER.c_str()).c_str();
		if ( !portNumberString.empty() )
		{
			if (-1 == portNumberString.find_first_not_of("0123456789") )
			{
				//all valid characters
				portNumber = atol( portNumberString.c_str() );
				if (0 == portNumber)
				{
					portNumber = TA_Base_Core::DEFAULT_MGR_PORT;
				}
			}
		}

		// Generate a list of hostnames from the command line argument
   		std::vector<std::string> hostNames;
        hostNames = convertCSVStringToVectorOfStrings(hosts);
		
        // If there are no hostnames at all, then set it to the default value
        if (hostNames.size() == 0)
        {
            hostNames.push_back(DEFAULT_HOST);
        }

		bool haveFoundSystemController = false;

        // Find out how many times to retry the connection
        std::string retriesString(TA_Base_Core::RunParams::getInstance().get(NUM_RETRIES_PARAM.c_str()));

        // Set the number to the default value first
        int retries = NUM_RETRIES;
        
        // But if there was a different value specified, attempt to parse it
        if (retriesString.length() > 0)
        {
            std::stringstream inputString;
            inputString << retriesString;

            inputString >> retries;
        }

        if (retries == 0)
        {
            // 0 is bad, make it at least 1
            retries = 1;
        }

        for (int i = 0;i < retries;i++)
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "Making attempt %lu to resolve host names",
                i+1);
		    std::vector<std::string>::iterator iter = hostNames.begin();

		    while( (iter != hostNames.end()) && (!haveFoundSystemController) )
		    {
			    char procManString [100];
			    CORBA::Object_var obj;
			    sprintf(procManString, "corbaloc::%s:%lu/ProcessManager", iter->c_str(), portNumber);

			    try
			    {
				    // Attempt to get Process Manager
				    obj = CorbaUtil::getInstance().stringToObject(procManString);
				    if (!CORBA::is_nil(obj))
				    {
					    m_systemController = TA_Base_Core::IProcessManagerCorbaDef::_narrow(obj);
					    if (!CORBA::is_nil(m_systemController))
					    {
						    haveFoundSystemController = true;
					    }
				    }   
			    }
			    catch ( ... )
			    {
				    // Nothing to do as we just move onto the next host
			    }
			    ++iter;
		    }

            if (haveFoundSystemController)
            {
                // break out of the for loop
                break;
            }
            else
            {
                LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn, "Failed to find System Controller on pass %lu",
                i+1);
            }
        }

		if (!haveFoundSystemController)
		{
			std::string errorMsg = "No system controller was located on any of the hosts specifed: ";
			errorMsg += hosts;
			TA_THROW( GenericAgentException(GenericAgentException::INITIALISATION_ERROR, errorMsg.c_str()) );
		}

		LOG( SourceInfo, DebugUtil::FunctionExit, "locateSystemController" );
	}


	void ServiceAgent::retrieveRunParams()
	{
		using TA_Base_Core::RunParams;

		LOG( SourceInfo, DebugUtil::FunctionEntry, "retrieveRunParams" );

		char hostName[512];
		int hNameLength = 512;

		if( gethostname(hostName, hNameLength) != 0 )
        {
			std::string errorMsg = "Host name could not be retrieved so the console cannot be found";
			TA_THROW( GenericAgentException(GenericAgentException::INITIALISATION_ERROR, errorMsg.c_str()) );
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "This console is: %s",hostName );

		TA_Base_Core::RunParamSeq* tmpParams = NULL;

		try
		{
			tmpParams = m_systemController->getParams(0,hostName);

			// For each RunParam we have retrieved. If it is not already set then
			// set it.
			for (unsigned long i = 0; i < tmpParams->length(); ++i)
			{
				if( !RunParams::getInstance().isSet((*tmpParams)[i].name) )
				{
					RunParams::getInstance().set((*tmpParams)[i].name, (*tmpParams)[i].value);
				}
			}
		}
		catch (const CORBA::Exception& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),"");
			TA_THROW( GenericAgentException(GenericAgentException::INITIALISATION_ERROR, "Could not retrieve a list of RunParams from the System Controller. Since we won't have most of the information we need then there is no point continuing") );
		}

		if (tmpParams != NULL)
		{
			delete tmpParams;
		}

		LOG( SourceInfo, DebugUtil::FunctionExit, "retrieveRunParams" );
	}


	std::vector<std::string> ServiceAgent::convertCSVStringToVectorOfStrings(const std::string& inputString)
	{
		// Check the input string is not empty or NULL
		TA_ASSERT(!inputString.empty(), "Input string is empty");
		TA_ASSERT(inputString.c_str() != NULL, "Input string is is NULL");

		std::vector<std::string> returnVector;
	
		std::string::size_type pos = 0;
		std::string::size_type startPos = 0;

		while (pos != std::string::npos)
		{
			// Find the first comma
			pos = inputString.find(',', startPos);

            std::string result;
			if (pos != std::string::npos)
			{
				// Still more to come
				result = inputString.substr(startPos, (pos - startPos));
			}
			else
			{
				// End of string
				result = inputString.substr(startPos, inputString.length());				
			}

			returnVector.push_back(result);

			// Start next search after current comma position
			startPos = pos + 1;
		}

		return returnVector;
	}

};
