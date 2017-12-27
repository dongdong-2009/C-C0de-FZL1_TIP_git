/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_agent/src/GenericAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2016/10/17 17:53:57 $
  * Last modified by:  $Author: limin.zhu $
  *
  */

#include "GenericAgent.h"

#if defined(_WIN32)
  #include <new.h>
#else
  #include <new>
#endif

#include <cstdlib>

#include "bus/response_plans/event_triggered/src/EventTriggeredPlanProcessor.h"
#include "bus/event/src/SessionHelper.h"

#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.h"
#include "core/data_access_interface/src/DataPointStateChangeAccessFactory.h"
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
#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/Naming.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/ConnectionChecker.h"
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
using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    const std::string GenericAgent::GROUP_ONLINE = "GroupOnline";

    //
    // Constructor
    //
    GenericAgent::GenericAgent(int argc, char* argv[], IGenericAgentUser* const registeredUser, 
        TA_Base_Core::PollListener* pollListener /* = NULL */,
		const unsigned int agentUserType /*= AGENT_USER_TYPE_SCADA*/)
        : m_agentStatus(TA_Base_Core::Startup),
          m_registeredUser(registeredUser),
          //m_agentEntityData(0),
          m_managedProcess(0), 
          m_hostname(TA_Base_Core::Hostname::getHostname())
	{
        try
        {
            #if defined(_MSC_VER)
                _set_new_handler(TA_Base_Bus::GenericAgent::outOfMemoryHandler);
            #endif // defined(_MSC_VER)

			// yanrong++: CL-21230
			// set hostname run parameter
			TA_Base_Core::RunParams::getInstance().set(RPARAM_NSHOSTNAME, m_hostname.c_str());
			// ++yanrong

            if (!UtilityInitialiser::initialiseUtilities(argc, argv))
            {
                // No need to continue execution.
                exit(0);
            }

            registerAsRunParamUser();
            m_managedProcess = new ManagedProcess(this, pollListener);

            std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
            std::string mgrProcessName = entityName + "_" + "ManagedProcess" ;
            TA_Base_Core::CorbaUtil::getInstance().setServantKey(m_managedProcess, mgrProcessName);
            m_managedProcess->activateServantWithName(mgrProcessName.c_str());

            // Tell the System Controller we have started starting up
            m_managedProcess->changeState(m_agentStatus);

            doEntityConfiguration(agentUserType);
            doAgentSetup();
            m_managedProcess->registerManagedApplication(_this());
            activateServantWithName( "GenericAgent" );

            // TD8684: Ensure plans are triggered from events.
            EventTriggeredPlanProcessor::registerWithProcessingLibrary();

			m_agentInfo.FailOverFromStationToOcc = false;

			//set SessionHelper for RealtimeEventSender for sending real time events.
			ISessionHelper * sessionHelper = new TA_Base_Bus::SessionHelper();
			TA_Base_Core::RealTimeEventSenderInstance::instance()->setSessionHelper(sessionHelper);

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
            if (m_managedProcess)
            {
                m_managedProcess->notifyTerminating(TA_Base_Core::InitError, mpe.what());
            }
            TA_THROW(GenericAgentException(GenericAgentException::INITIALISATION_ERROR, mpe.what()));
        }
        catch(const ProcessAlreadyRunningException&)
        {
            // The servant has not yet been activated, tell the process monitor that we are
            // terminating and then re-throw as a GenericAgentException
            m_managedProcess->notifyTerminating(TA_Base_Core::InitError, "Process already running");
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
            m_managedProcess->notifyTerminating(TA_Base_Core::InitError, "Generic Agent failed to initialise");
            deactivateServant();
            TA_THROW(GenericAgentException(GenericAgentException::INITIALISATION_ERROR));
        }
    }


    //
    // Destructor
    //
    GenericAgent::~GenericAgent()
    {
        cleanup();
        LOG ( SourceInfo, DebugUtil::FunctionExit, "Destructor");
    }


    //
    // onTerminate
    //
    void GenericAgent::onTerminate()
    {
        m_managedProcess->notifyTerminating(
            TA_Base_Core::RequestedTerminate,
            "Terminating due to request from System Controller");

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "GenericAgent::onTerminate - Terminating due to request from System Controller" );

        submitAuditMessage(TA_Base_Core::GenericAgentAudit::AgentShutdown);
        TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().shutdownInProgress();

        // Update of this state already sent in the call above
        m_agentStatus = TA_Base_Core::Terminating;

        // Deactivate managed process
        m_managedProcess->deactivateServant();

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

        // This will result in the CorbaUtil run method returning
        // inside our own run method.
        CorbaUtil::getInstance().stop();
    }


    //
    // addEntity
    //
    void GenericAgent::addEntity( unsigned long p_entityKey, IEntity* p_entity )
    {
        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        m_entities.insert(IEntityMap::value_type(p_entityKey, p_entity));
        p_entity->start();
        OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, p_entityKey);
    }


    void GenericAgent::addEntityList( const IEntityList& entityList )
    {
        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        // add to map
        IEntityList::const_iterator iter;
        for (iter = entityList.begin(); iter != entityList.end(); ++iter )
        {
            if( iter->second != NULL )
            {
                m_entities.insert( IEntityMap::value_type( iter->first, iter->second ) );
                iter->second->start();
                OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, iter->first);
            }
        }
    }


    //
    // deleteEntity
    //
    void GenericAgent::deleteEntity( unsigned long p_entityKey )
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
    void GenericAgent::modifyEntity( unsigned long p_entityKey, IEntity* p_entity )
    {
        deleteEntity(p_entityKey);
        addEntity(p_entityKey, p_entity);
    }


    //
    // terminateWithoutRestart
    //
    void GenericAgent::terminateWithoutRestart()
    {
        // Tell the Process Monitor that we should not be restarted. This is generally
        // due to a configuration error.
        m_managedProcess->notifyTerminating(TA_Base_Core::ConfigError, "Configuration problem with agent");
    }


    //
    // handleUpdateForAgentEntity
    //
    void GenericAgent::handleUpdateForAgentEntity(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
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
    void GenericAgent::handleUpdateForChildEntity(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
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
    void GenericAgent::processUpdate(const ConfigUpdateDetails& updateEvent)
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
    EOperationMode GenericAgent::getOperationMode()
    {
        return m_mode;
    }


    //
    // setOperationMode
    //
    void GenericAgent::setOperationMode(EOperationMode mode)
    {
		FUNCTION_ENTRY("setOperationMode");

        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

		if((mode == TA_Base_Core::Control || mode == TA_Base_Core::Monitor) 
			&& m_mode == TA_Base_Core::Standby)
		{
			m_agentInfo.FailOverFromStationToOcc = true;
		}
		else
		{
			m_agentInfo.FailOverFromStationToOcc = false;
		}
		if (m_mode == TA_Base_Core::Control && mode == TA_Base_Core::Control)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"In Control mode and told to change to Control again. Rebroadcasting named objects.");

			// Agent told to go into Control when it is already in Control.
			// Just need NamedObject references to be re-resolved.
			TA_Base_Core::Naming::getInstance().rebroadcastNamedObjects();

			// Update the System Controller state.
            m_agentStatus = TA_Base_Core::RunningControl;
            m_managedProcess->changeState(m_agentStatus);

			FUNCTION_EXIT;
			return;
		}

        m_mode = mode;

        if( mode == TA_Base_Core::Monitor )
        {
            // NOTE: this is different from Control
            //       set first to ensure that it will throw OperationModeException
            //       while transforming to Monitor mode
            TA_Base_Core::RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_MONITOR);

            // Let the System Controller know we are about to change state
            m_agentStatus = TA_Base_Core::GoingToMonitor;
            m_managedProcess->changeState(m_agentStatus);

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

            //TA_Base_Core::RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_MONITOR);

            submitAuditMessage(TA_Base_Core::GenericAgentAudit::AgentChangeToMonitor);

            // Let the System Controller know we have successfully changed state
            m_agentStatus = TA_Base_Core::RunningMonitor;
            m_managedProcess->changeState(m_agentStatus);
        }
        else
        {
            if( mode == TA_Base_Core::Control )
            {
                // Let the System Controller know we are about to change state
                m_agentStatus = TA_Base_Core::GoingToControl;
                m_managedProcess->changeState(m_agentStatus);

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
                m_managedProcess->changeState(m_agentStatus);
            }
			else if( mode == TA_Base_Core::Standby )
			{
				// Let the System Controller know we are about to change state
				m_agentStatus = TA_Base_Core::GoingToStandby;
				m_managedProcess->changeState(m_agentStatus);

				if( m_registeredUser != NULL )
				{
					try
					{
						m_registeredUser->agentSetStandby();
						IEntityMap::iterator entityPtr = m_entities.begin();
						while( entityPtr != m_entities.end() )
						{
							if( entityPtr->second != NULL )
							{
								entityPtr->second->setToStandbyMode();
							}
							entityPtr++;
						}
					}
					catch(...)
					{
						// Agent threw an exception and didn't handle it.
						TA_ASSERT(false, "Unhandled exception in agent when changing to Standby mode");
					}
				}

				TA_Base_Core::RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_STANDBY);

				//submitAuditMessage(TA_Base_Core::GenericAgentAudit::AgentChangeToControl);

				// Let the System Controller know we have successfully changed state
				m_agentStatus = TA_Base_Core::RunningStandby;
				m_managedProcess->changeState(m_agentStatus);
			}
        }        
		
		FUNCTION_EXIT;
    }


	void GenericAgent::invalidateNotifyHosts(const TA_Base_Core::IManagedAgentCorbaDef::NotifyHostSequence& notifyHosts)
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
    void GenericAgent::notifyGroupOffline(const char* group)
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
    void GenericAgent::notifyGroupOnline(const char* group)
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
    void GenericAgent::run()
    {
        m_managedProcess->startMonitoringThread();

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
		else if(m_mode == TA_Base_Core::Standby)
		{
			m_agentStatus = TA_Base_Core::RunningStandby;
        }

        m_managedProcess->changeState(m_agentStatus);

		// TD10093 - See if the group is offline and notify accordingly
		// This will catch any notification on agent start up
		if (m_agentStatus == TA_Base_Core::RunningControl ||
			m_agentStatus == TA_Base_Core::RunningMonitor)
		{
			// Get the offline group
			std::string offlineGroups = RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);

			// If group is not online
			if( (!offlineGroups.empty()) &&
                (offlineGroups.compare(GenericAgent::GROUP_ONLINE) != 0) )
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

        // This call blocks while servicing CORBA requests
        CorbaUtil::getInstance().run();
    }


    //
    // onRunParamChange
    //
    void GenericAgent::onRunParamChange(const std::string& name,const std::string& value)
    {
        // New debug file name
        if (name.compare(RPARAM_DEBUGFILE) == 0)
        {
            DebugUtil::getInstance().setFile(value.c_str(), TA_Base_Core::getRunParamValue(RPARAM_DEBUGFILEMAXSIZE, 0));
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
        // double agents control restored
        else if (name.compare(RPARAM_DOUBLEAGENTSCONTROLRESTORED) == 0)
        {
            if ( m_registeredUser )
            {
                m_registeredUser->handleDoubleAgentsControlRestored();
            }
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
    IEntityDataPtr GenericAgent::getAgentEntityConfiguration()
    {
        return m_agentEntityData;
    }


    //
    // getEntityMap
    //
    const IEntityMap* GenericAgent::getEntityMap()
    {
        return &m_entities;
    }


    //
    // doEntityConfiguration
    //
    void GenericAgent::doEntityConfiguration(const unsigned int agentUserType)
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
        
			m_registeredUser->beforeGetDescendantsOfAgent(entityData);
            // Get all the entities that the agent is responsible for
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "Loading agent entities" );
            
            TA_Base_Core::SharedIEntityDataList childEntityData =
                EntityAccessFactory::getInstance().getDescendantsOfAgent(m_agentEntityData->getKey());
            
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "Loaded %d agent entities", childEntityData.size());

            MessageTypeTableAccessor::getInstance();
			
			//xinsong ++
			//if it is scada agent, then we need load some scada info due to failover
			if ( agentUserType == AGENT_USER_TYPE_SCADA )
			{
				// intialize DataPointStateChangeAccessFactory first
				TA_Base_Core::DataPointStateChangeAccessFactory::getInstance();

				TA_Base_Core::DerivedDataPointAccessFactory::getInstance().bulkLoadByAgentKey(m_agentEntityData->getKey());
			}

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
    void GenericAgent::doAgentSetup()
    {
        // Get the agents initial operation mode
        // Raymond Pau++ TD17345
        std::string operationMode = RunParams::getInstance().get(RPARAM_OPERATIONMODE);
        if ( true == operationMode.empty() )
        {
            TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR,
                "Operation Mode RunParam has not been set") );
        }

        if( 0 == operationMode.compare(RPARAM_MONITOR) )
        {
            m_mode = TA_Base_Core::Monitor;
        }
        else if( 0 == operationMode.compare(RPARAM_CONTROL) )
        {
            m_mode = TA_Base_Core::Control;
        }
		else if( 0 == operationMode.compare(RPARAM_STANDBY) )
		{
			m_mode = TA_Base_Core::Standby;
		}
        else
        {
            std::ostringstream desc;
            desc << "Invalid Operation Mode [" << operationMode << "];"
                << " valid operation mode are " << RPARAM_MONITOR << ", "
                << RPARAM_CONTROL ;

            TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
        }
        // ++Raymond Pau TD17345

        if( m_registeredUser != NULL )
        {
            m_registeredUser->registerForStateUpdates();
        }
    }


    //
    // registerAsRunParamUser
    //
    void GenericAgent::registerAsRunParamUser()
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
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DOUBLEAGENTSCONTROLRESTORED);
   }

    
    //
    // submitAuditMessage
    //
    void GenericAgent::submitAuditMessage(const TA_Base_Core::MessageType& messageType)
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


    //
    // cleanup
    //
    void GenericAgent::cleanup()
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup begin");

        TA_Base_Core::ThreadGuard guard( m_entityMapLock );

        if( m_managedProcess != NULL )
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - m_managedProcess->deactivateAndDeleteServant");
            m_managedProcess->deactivateAndDeleteServant();
            m_managedProcess = 0;
        }

        //if( m_agentEntityData != NULL )
        //{
        //    delete m_agentEntityData;
        //    m_agentEntityData = 0;
        //}

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - OnlineUpdateListener::deregisterAllInterests");
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
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - AlarmHelperManager::cleanUp");
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();	

		//Clean up Real Time Event Sender Quene
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - RealTimeEventSenderInstance::cleanUp");
		TA_Base_Core::RealTimeEventSenderInstance::instance()->cleanUp();

		//Clean up EventProcessLibray (for insert event into DB)
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - EventProcessingLibrary::cleanUp");
		TA_Base_Core::EventProcessingLibrary::instance()->cleanUp();

		// Clean up naming
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - Naming::cleanUp");
        TA_Base_Core::Naming::cleanUp();

        // Clean up the Event Consumers
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - gDestroySubscriptionManager");
        TA_Base_Core::gDestroySubscriptionManager();

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - gTerminateAndDeleteStructuredEventSuppliers");
        TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - ChannelLocatorConnectionMgr::removeInstance");
 		TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - CorbaUtil::shutdown");
        CorbaUtil::getInstance().shutdown();

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - CorbaUtil::cleanup");
        CorbaUtil::cleanup();

        // Deregister ourselves 
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - RunParams::deregisterRunParamUser this");
        RunParams::getInstance().deregisterRunParamUser(this);

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup - DbServerLocator::removeInstance");
        DbServerLocator::removeInstance();

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "GenericAgent::cleanup end");
    }


    //
    // outOfMemoryHandler
    //
    int GenericAgent::outOfMemoryHandler( size_t size )
    {
        TA_THROW(std::bad_alloc());
        // Shouldn't reach here but if it does, this should tell new to stop allocation attempts
        return 0;  
    }
};
