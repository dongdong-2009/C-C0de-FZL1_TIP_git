/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_service/src/ServiceAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  */

#include "ServiceAgent.h"
#include <new.h>
#include <cstdlib>
#include "ace/OS.h"
#include <memory>
#include <winsock2.h>  // Used by getHostName 

#include "bus/response_plans/event_triggered/src/EventTriggeredPlanProcessor.h"
#include "bus/event/src/SessionHelper.h"

#include "core/data_access_interface/entity_access/src/system.h"
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
#include "core/process_management/src/PollListener.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/Naming.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/ConnectionChecker.h"
#include "core\message\types\DatabaseStatusComms_MessageTypes.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmPublicationManager.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/message/types/GenericAgentAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/utilities/src/Hostname.h"
#include "core/database/src/DbServerLocator.h"
#include "core/event_processing/src/RealTimeEventSender.h"
#include "core/event_processing/src/EventProcessingLibrary.h"
#include "core/event_processing/src/ISessionHelper.h"


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


#define RPARAM_DISABLE_MESSAGE "DisableMessage"

namespace TA_Base_Bus
{
    const std::string ServiceAgent::GROUP_ONLINE = "GroupOnline";
    const unsigned int ServiceAgent::NUM_RETRIES = 10;

    //
    // Constructor
    //
    ServiceAgent::ServiceAgent(int argc, char* argv[], IGenericAgentUser* const registeredUser,
		int portNum,
        TA_Base_Core::PollListener* pollListener /* = NULL */)
        : m_agentStatus(TA_Base_Core::Startup),
          m_registeredUser(registeredUser),
          //m_agentEntityData(0),
          //m_managedProcess(0),
          m_hostname(TA_Base_Core::Hostname::getHostname())
    {
        try
        {
            #if defined(_MSC_VER)
                _set_new_handler(TA_Base_Bus::ServiceAgent::outOfMemoryHandler);
            #endif // defined(_MSC_VER)

            if (!intitialiseUtilities(argc, argv, portNum))
            {
                // No need to continue execution.
                exit(0);
            }
            registerAsRunParamUser();
			//m_managedProcess = new ManagedProcess(this);
            // Tell the System Controller we have started starting up
            //m_managedProcess->changeState(m_agentStatus);

            doEntityConfiguration();
            doAgentSetup();
			//m_managedProcess->registerManagedApplication(_this());
            activateServant();

            if ( false == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DISABLE_MESSAGE ) )
            {
                // TD8684: Ensure plans are triggered from events.
                EventTriggeredPlanProcessor::registerWithProcessingLibrary();
            }
			//set SessionHelper for RealtimeEventSender for sending real time events.
			TA_Base_Core::ISessionHelper * sessionHelper = new TA_Base_Bus::SessionHelper();
			TA_Base_Core::RealTimeEventSenderInstance::instance()->setSessionHelper(sessionHelper);

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
        catch(GenericAgentException& ex)
        {
            switch (ex.getType())
            {
                // Configuration errors that indicate the agent will likely never start up, until
                // config changes are made. 
                case GenericAgentException::CONFIGURATION_ERROR:
                case GenericAgentException::ENTITY_CONFIG_FAILURE:
                case GenericAgentException::INVALID_ENTITY_TYPE:
                case GenericAgentException::ENTITY_NOT_CONFIGURED:
                case GenericAgentException::ENTITY_NAME_NOT_UNIQUE:

                    // Tell the Process Monitor that we are terminating and should not be restarted.
                    terminateWithoutRestart();
                    break;
            }

            // Deactivate the servant, and re-throw
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

        IEntityMap::iterator entityPtr = m_entities.begin();
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

        submitAuditMessage(TA_Base_Core::GenericAgentAudit::AgentShutdown);

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

        m_entities.insert(IEntityMap::value_type(p_entityKey, p_entity));
        p_entity->start();

        if ( false == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DISABLE_MESSAGE ) )
        {
            OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, p_entityKey);
        }
    }


    void ServiceAgent::addEntityList( const IEntityList& entityList )
    {
        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        // add to map
        IEntityList::const_iterator iter;
        for (iter = entityList.begin(); iter != entityList.end(); ++iter )
        {
            if (NULL != iter->second)
            {
                m_entities.insert( IEntityMap::value_type( iter->first, iter->second ) );
                iter->second->start();

                if ( false == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DISABLE_MESSAGE ) )
                {
                    OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, iter->first);
                }
            }
        }
    }


    //
    // deleteEntity
    //
    void ServiceAgent::deleteEntity( unsigned long p_entityKey )
    {
        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        IEntityMap::iterator p = m_entities.find(p_entityKey);

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
    // terminateWithoutRestart
    //
    void ServiceAgent::terminateWithoutRestart()
    {
        // Tell the Process Monitor that we should not be restarted. This is generally
        // due to a configuration error.
        //m_managedProcess->notifyTerminating(TA_Base_Core::ConfigError, "Configuration problem with agent");
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
				TA_Base_Core::IEntityDataPtr entityData(EntityAccessFactory::getInstance().getEntity(updateEvent.getKey()));

				if( entityData->getAgent() == m_agentEntityData->getKey() )
				{
					IEntity* entity = m_registeredUser->createEntity( entityData );
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

        TA_ASSERT( m_agentEntityData.get() != NULL, "Could not update entity as agent entity data is NULL");
    
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
                    IEntityMap::iterator entityPtr = m_entities.begin();
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

            submitAuditMessage(TA_Base_Core::GenericAgentAudit::AgentChangeToMonitor);

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
                        IEntityMap::iterator entityPtr = m_entities.begin();
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

                submitAuditMessage(TA_Base_Core::GenericAgentAudit::AgentChangeToControl);

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

			TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().invalidateConnection( notifyHosts[i].in() );
		}

        FUNCTION_EXIT;
	}


    //
    // notifyGroupOffline
    //
    void ServiceAgent::notifyGroupOffline(const char* group)
    {
		FUNCTION_ENTRY( "notifyGroupOffline" );

		// TD10093 Check if the entry is in the vector
        std::set<std::string>::iterator matching = m_offlineGroups.find(std::string(group));

        if (matching == m_offlineGroups.end())
        {
            // Haven't yet processed this offline group so do it now
			m_registeredUser->notifyGroupOffline( std::string(group) );
            
            m_offlineGroups.insert(std::string(group));

        }

		FUNCTION_EXIT;
    }


    //
    // notifyGroupOnline
    //
    void ServiceAgent::notifyGroupOnline(const char* group)
    {
		FUNCTION_ENTRY( "notifyGroupOnline" );

		// TD10093 Check if the entry is in the vector
        std::set<std::string>::iterator matching = m_offlineGroups.find(std::string(group));

        if (matching != m_offlineGroups.end())
        {
            // We have this in our list so we need to ensure it becomes offline
    		m_registeredUser->notifyGroupOnline( std::string(group) );

            m_offlineGroups.erase(matching);
        }

		FUNCTION_EXIT;
    }


    //
    // run
    //
    void ServiceAgent::run()
    {

        try
        {

            // now subscribe to the system controller dbstate change comms messages
    /*TD18257, to use the below block
                    std::auto_ptr<IEntityData> me(EntityAccessFactory::getInstance().getEntity(m_hostname));
        		    TA_Base_Core::IEntityDataList systemControllers = EntityAccessFactory::getInstance().
        			    getEntitiesOfTypeAtLocation( TA_Base_Core::System().getStaticType(),  me->getLocation());
        
                    TA_Base_Core::IEntityDataList::iterator systemController = systemControllers.begin();
                    while (systemController != systemControllers.end())
                    {
        			    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                            TA_Base_Core::DatabaseStatusComms::DatabaseStatusChange, 
        				    this, 
        				    (*systemController)->getKey(), 
        				    (*systemController)->getSubsystem(), 
        				    (*systemController)->getLocation(), 
        				    NULL);
        
                        delete(*systemController);
                        systemController = NULL;
                        systemController++;
                    }*/
        
            if ( false == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DISABLE_MESSAGE ) )
            {
		        for (std::vector<std::string>::iterator it= m_systemcontrollers.begin(); it != m_systemcontrollers.end(); ++it)
		        {
			        TA_Base_Core::IEntityData* systemControllerEntity = EntityAccessFactory::getInstance().getEntity((*it));
			        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                        "systemController key is %d, location is %d", systemControllerEntity->getKey(),systemControllerEntity->getLocation());
			        
			        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                        TA_Base_Core::DatabaseStatusComms::DatabaseStatusChange, 
				        this, 
				        systemControllerEntity->getKey(), 
				        0, 
				        systemControllerEntity->getLocation(), 
				        NULL);
			        
			        delete systemControllerEntity;
			        systemControllerEntity = NULL;
		        }
            }

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

		    // TD10093 - See if the group is offline and notify accordingly
		    // This will catch any notification on agent start up
		    if (m_agentStatus == TA_Base_Core::RunningControl ||
			    m_agentStatus == TA_Base_Core::RunningMonitor)
		    {
			    // Get the offline group
			    std::string offlineGroups = RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);

			    // If group is not online
			    if( (!offlineGroups.empty()) &&
                    (offlineGroups.compare(ServiceAgent::GROUP_ONLINE) != 0) )
			    {
                    // Break up the comma separated list of groups and set each group offline
                    std::string::size_type startPos = 0;
                    std::string::size_type commaPos = offlineGroups.find_first_of(",",startPos);

                    while (commaPos != std::string::npos)
                    {
                        std::string offlineGroup = offlineGroups.substr(startPos, commaPos - startPos);
                        notifyGroupOffline(offlineGroup.c_str());

                        // Search for the next comma
                        startPos = commaPos + 1;
                        commaPos = offlineGroups.find_first_of(",",startPos);
                    }

                    // Now get the last one.
                    std::string offlineGroup = offlineGroups.substr(startPos, offlineGroups.length() - startPos);
				    notifyGroupOffline(offlineGroup.c_str());
			    }
		    }
        }
        catch ( const CORBA::Exception & ex )
        {
            LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
        }
        catch ( const TA_Base_Core::TransactiveException & ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", ex.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");
        }

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
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                "Received change notification for unsupported parameter %s", name.c_str());
        }
    }


    void ServiceAgent::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
        //
        // messages from the system controller
        //
        std::string mesgTypeKey = message.messageTypeKey;

        // if we got an dbStatus message, update the matching runparam
        if (0 == mesgTypeKey.compare(TA_Base_Core::DatabaseStatusComms::DatabaseStatusChange.getTypeKey()))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                "Received a DatabaseStatusChange message");
            TA_Base_Bus::DatabaseStatusCorbaDef::DatabaseStatus *status = NULL;
            message.messageState >>= status;

            std::string dbName (status->database);
            std::string dbStatus;
            switch (status->status)
            {
            case TA_Base_Bus::DatabaseStatusCorbaDef::statusUnknown:
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					"DatabaseStatusChange message, database %s is Unknown", dbName.c_str());
                dbStatus = RPARAM_DBUNKNOWN;
                break;
            case TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable:
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					"DatabaseStatusChange message, database %s is online", dbName.c_str());
                dbStatus = RPARAM_DBONLINE;
                break;
            case TA_Base_Bus::DatabaseStatusCorbaDef::statusUnAvailable:
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					"DatabaseStatusChange message, database %s is offline", dbName.c_str());
                dbStatus = RPARAM_DBOFFLINE;
                break;
            default:
                TA_ASSERT(false, "Unknown or new TA_Base_Bus::DatabaseStatusCorbaDef::State");
            }

           
            TA_Base_Core::RunParams::getInstance().set(dbName.c_str(), dbStatus.c_str());


        }
    }

    //
    // getAgentEntityConfiguration
    //
    TA_Base_Core::IEntityDataPtr ServiceAgent::getAgentEntityConfiguration()
    {
        return m_agentEntityData;
    }


    //
    // getEntityMap
    //
    const IEntityMap* ServiceAgent::getEntityMap()
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

			TA_Base_Core::IEntityDataPtr entityData(EntityAccessFactory::getInstance().getEntity(entityName));
            m_agentEntityData = entityData;
        
            // Get all the entities that the agent is responsible for
            TA_Base_Core::SharedIEntityDataList childEntityData =
                EntityAccessFactory::getInstance().getDescendantsOfAgent(m_agentEntityData->getKey());

            // try to do bulk creation
            IEntityList createdEntities;
            if (true == m_registeredUser->createAllEntities(m_agentEntityData, childEntityData, createdEntities) )
            {
                // bulk add entities
                addEntityList( createdEntities );
            }
            else
            {
                TA_Base_Core::SharedIEntityDataList::iterator p = childEntityData.begin();
                while( p != childEntityData.end() )
                {
                    if( m_registeredUser != NULL )
                    {
                        // Populate our vector
                        IEntity* child = m_registeredUser->createEntity( *p );
                        if( child != NULL )
                        {
                            addEntity((*p)->getKey(), child);
                        }
                    }
                    p++;
                }
            }

            // let the agent know that entity creation is complete
            m_registeredUser->entityCreationComplete();

            if ( false == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DISABLE_MESSAGE ) )
            {
                //
                // Register interest for online updates of the agent entity
                // 
                OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, m_agentEntityData->getKey());
            }
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
    // submitAuditMessage
    //
    void ServiceAgent::submitAuditMessage(const TA_Base_Core::MessageType& messageType)
    {
        if ( false == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DISABLE_MESSAGE ) )
        {
            TA_Base_Core::AuditMessageSender* auditSender = 
                TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::GenericAgentAudit::Context);
        
            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair param1("AgentName", RunParams::getInstance().get(RPARAM_ENTITYNAME));
            TA_Base_Core::NameValuePair param2("HostName", m_hostname);

            dp.push_back(&param1);
            dp.push_back(&param2);
        
            auditSender->sendAuditMessage(
                messageType,                                             // Message Type
                m_agentEntityData->getKey(),                             // Entity key
                dp,                                                      // Description
                "",                                                      // Additional details
                TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID), // SessionID
                "",                                                      // AlarmID if an alarm associated with event
                "",                                                      // IncidentKey if incident associated with event
                "");                                                     // EventID of a parent event, used to link events
        
            delete auditSender;
        }
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

        //if( m_agentEntityData != NULL )
        //{
        //    delete m_agentEntityData;
        //    m_agentEntityData = 0;
        //}

        OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::ENTITY_OWNER, *this);

        IEntityMap::iterator entityPtr = m_entities.begin();
        while( entityPtr != m_entities.end() )
        {
            // Clean up the m_entities map
            if( entityPtr->second != NULL )
            {
	            entityPtr->second->remove();
                delete entityPtr->second;
                entityPtr->second = 0;
            }
            entityPtr++;
        }
        m_entities.clear();

		// Clean up the alarm subsystem
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();	

		//Clean up Real Time Event Sender Quene
		TA_Base_Core::RealTimeEventSenderInstance::instance()->cleanUp();

		//Clean up EventProcessLibray (for insert event into DB)
		TA_Base_Core::EventProcessingLibrary::instance()->cleanUp();

		// Clean up naming
        TA_Base_Core::Naming::cleanUp();

        // Clean up the Event Consumers
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        TA_Base_Core::gDestroySubscriptionManager();
        TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
        TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();

        LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "shutdowning corba utility");

        CorbaUtil::getInstance().shutdown();

        LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "cleanning up corba utility");

        CorbaUtil::cleanup();

        // Deregister ourselves 
        RunParams::getInstance().deregisterRunParamUser(this);

        LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "removing oci access instance");

        TA_Base_Core::DbServerLocator::removeInstance();

        LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "service agent cleanup exits");
    }


    //
    // outOfMemoryHandler
    //
    int ServiceAgent::outOfMemoryHandler( size_t size )
    {
        LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugFatal, "outOfMemory!");
        TA_THROW(std::bad_alloc());
        // Shouldn't reach here but if it does, this should tell new to stop allocation attempts
        return 0;  
    }

	bool ServiceAgent::intitialiseUtilities(int argc, char* argv[], int portNum)
	{
		try
        {
			if( false == RunParams::getInstance().parseCmdLine(argc, argv) )
			{
				if (RunParams::getInstance().isSet(RPARAM_VERSION))
                {
                    // Versioning info printed to stdout. Not an exceptional condition,
                    // but there's no need to continue.
                    return false;
                }
 
				TA_THROW (TA_Base_Core::InvalidCommandLineException
                    ("Command line arguments are in invalid format"));
			}

			initialiseDebugUtil();
			initialiseCorba(portNum);
		}
        catch( const TA_Base_Core::UtilitiesException& )
        {
            throw; // Ensuring we only throw the exceptions listed in the documentation
        }
        catch( const TA_Base_Core::InvalidCommandLineException& )
        {
            throw; // Ensuring we only throw the exceptions listed in the documentation
        }
        catch( ... )
        {
            TA_THROW( TA_Base_Core::UtilitiesException("One or more utilities failed to initialise") );
        }

		locateSystemController();
		retrieveRunParams();
		RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_CONTROL);

		return true;
	}

	void ServiceAgent::initialiseDebugUtil()
    {
        // Set the debug file name - update this to look like an Agent log file
		// which is usually set by the system controller
        std::string param = RunParams::getInstance().get(RPARAM_DEBUGFILE);
        if (param.empty())
        {
			param = RunParams::getInstance().get(RPARAM_ENTITYNAME);
		}

		if (!param.empty())
		{
			std::ostringstream filename;
        
			std::string::size_type dotPos = param.rfind('.');

			if (dotPos != std::string::npos)
			{
				// Found a '.'. Take a copy of what's after it.
				std::string extension = param.substr(dotPos + 1, param.length());
	
				if (atoi(extension.c_str()) == 0 && extension != "0")
				{
					// Extension is a normal string, not a number.
					// (This is most likely the case for Windows.)
					
					// Add everything before the '.' to the filename.
					filename << param.substr(0, dotPos);
					
					// Add the '.' and the date.
					filename << "." << getDateAndTime();
					
					// Add the extension.
					filename << "." << extension;
				}
				else
				{
					// Extension is a number. (Most likely Solaris)
					// Just add the log number to the end.   
					filename << param << "." << getDateAndTime();
				}
			}
			else
			{
				// No extension at all. Add the date to the end.
				filename << param << "." << getDateAndTime();
			}

            DebugUtil::getInstance().setFile(filename.str().c_str());
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
			TA_THROW( TA_Base_Core::UtilitiesException("Failed to initialise corba") );
        }

        try
        {
            CorbaUtil::getInstance().activate();
        }
        catch( ... )
        {
			TA_THROW( TA_Base_Core::UtilitiesException("Failed to initialise corba") );
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
			if (std::string::npos == portNumberString.find_first_not_of("0123456789") )
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
		m_systemcontrollers = hostNames;
		
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
			    char procManString [100] = {0};
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

	//
    // getDate
    //
    std::string ServiceAgent::getDateAndTime() const
    {
        // Get the current time.
        time_t currentTime;
        struct tm timeStruct;
        time(&currentTime);
        ACE_OS::localtime_r(&currentTime, &timeStruct);

        // Convert it to a string.
        std::ostringstream timeStr;
        timeStr << timeStruct.tm_mday << ".";
        timeStr << (timeStruct.tm_mon + 1) << ".";
        timeStr << (timeStruct.tm_year + 1900) << ".";
		if (timeStruct.tm_hour < 10)
		{
			timeStr << '0';
		}
		timeStr << (timeStruct.tm_hour);
		if (timeStruct.tm_min < 10)
		{
			timeStr << '0';
		}
		timeStr << (timeStruct.tm_min);

        return timeStr.str();
    }

};
