/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/02/11 16:14:07 $
  * Last modified by:  $Author: haijun.li $
  *
  *	DataNodeAgentCorbaDef_Impl is an abstract base class that implements
  * the DataNodeAgentCorbaDef interface, providing DataNode configurations
  * and updates to remote clients.
  *
  */

#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeObserver.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentSynHandler.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeDisplayClient.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/naming/src/Naming.h"

using TA_Base_Core::DebugUtil;
//TD15618, use timer to trigger if the observers are timeout
namespace TA_Base_Bus
{

	DataNodeAgentCorbaDef_Impl::DataNodeAgentCorbaDef_Impl( const std::string & agentName, TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent )
	:
	m_agentName ( agentName ),
	m_operationMode ( TA_Base_Core::NotApplicable ),
    m_specificAgent( specificDataNodeAgent ),
	m_generatedObservedID ( time( NULL ) ),
    m_generatedClientID( time( NULL ) ),
    m_stateContainer(NULL),
	m_synHandler(NULL),
    m_synClientID(0),
    m_synClientExisted(false),
	m_servantActivated(false)
	{
        m_listOfDisplayClients.clear();
		m_listOfObservers.clear();

        m_stateContainer = new DataNodeStateContainer(m_specificAgent);
        
        m_synHandler = new DataNodeAgentSynHandler(m_agentName,m_specificAgent);
	}


	DataNodeAgentCorbaDef_Impl::~DataNodeAgentCorbaDef_Impl()
	{
		m_operationMode = TA_Base_Core::Monitor;
		// CL-17309 ++
		if (m_servantActivated)
		{
			deactivateServant();
		}
		// ++ CL-17309
		//deactivateAndDeleteServant();

		if(NULL != m_synHandler)
		{
			m_synHandler->stopSynchronize();
			delete m_synHandler;
			m_synHandler = NULL;
		}

        // ensure the timer is no longer scheduled to call back on a NULL object
        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        
		// clear the reference to the data node agent, without actually deleting
		// the object
		m_specificAgent = NULL;
        {
			// destroy all observers
        	TA_THREADGUARD( m_observerListLock );
		    // destroy all observers
		    std::map < unsigned long, DataNodeObserver * >::iterator itr;
		    for ( itr = m_listOfObservers.begin();
			      itr != m_listOfObservers.end();
			      itr++ )
		    {
			    delete itr->second;
				itr->second = NULL;
		    }
		    m_listOfObservers.clear();
        }
        cleanupDisplayContainer();
	}


	void DataNodeAgentCorbaDef_Impl::threadedTimerExpired(long timerId, void* userData)
	{
        // flag to indicate if all observers are gone
        bool allObserversEmpty = true;
        
        {
            // stop multi threads adding/removing observers while we are doing this
            TA_THREADGUARD( m_observerListLock );

		    // for each of the existing observers in the list
            std::map< unsigned long, DataNodeObserver * >::iterator itObserver = m_listOfObservers.begin();
            while ( itObserver != m_listOfObservers.end() )
            {
                // if the observer indicates that the latest updates from the DataNode
                // instances that assigned to it have not been fetched by a remote client
                // for the idle time
                if ( TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS < itObserver->second->getIdleTime() )
                {
                    // if this happens, destroy the observer
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
                        "Observer [%lu] timeout [%lu ms] expired and its updates will be deleted as well as itself", 
                        itObserver->second->getObserverID(), TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS);

					// destroy the existing observer from the list
                    delete itObserver->second;
                    m_listOfObservers.erase( itObserver++ );
                }
                else
                {
                    ++itObserver;
                }
            }
            
            allObserversEmpty = m_listOfObservers.empty();
        }
              
        {
		    TA_THREADGUARD( m_clientListLock );
              
            if (m_stateContainer != NULL) 
            {				    
		        std::map < unsigned long, DataNodeDisplayClient * >::iterator itDisplayClient = m_listOfDisplayClients.begin();
                while ( itDisplayClient != m_listOfDisplayClients.end() )
                {
			        if ( TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS < itDisplayClient->second->getIdleTime() )
			        {
				        // if this happens, destroy the observer
				        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
							"Clinet [%lu] timeout [%lu ms] expired and its updates will be deleted as well as itself", 
							itDisplayClient->second->getClientID(), TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS);

                        m_stateContainer->removeDisplayClient( itDisplayClient->first );

				        delete itDisplayClient->second;
				        m_listOfDisplayClients.erase( itDisplayClient++ );
			        }
					else
					{
						++itDisplayClient;
					}
				}
			}
            
            allObserversEmpty = allObserversEmpty && m_listOfDisplayClients.empty();
		}
        
        if ( ( TA_Base_Core::Control != m_operationMode ) && 
             ( true == allObserversEmpty ) )
        {
            TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        }
	}


	unsigned long DataNodeAgentCorbaDef_Impl::generateNextObserverID()
	{
		// increment and return the ID
		return ++m_generatedObservedID;
	}

	unsigned long DataNodeAgentCorbaDef_Impl::generateNextClientID()
	{
		// increment and return the ID
		m_generatedClientID = m_generatedClientID + 1;
		return m_generatedClientID;
	}

	DataNodeDisplayClient * DataNodeAgentCorbaDef_Impl::findDisplayClient ( unsigned long clientId )
	{
		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		DataNodeDisplayClient * existingClient = 0;

		// find if the specified observerId is already in the internal list
		std::map < unsigned long, DataNodeDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( clientId );

		// if found one
		if ( itr != m_listOfDisplayClients.end() )
		{
			existingClient = itr->second;
		}

		return existingClient;
	}

	CORBA::ULong DataNodeAgentCorbaDef_Impl::regiesterDisplayDataNodeClient() 
    {
		FUNCTION_ENTRY("regiesterDisplayDataNodeClient");

        // stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		unsigned long cleintID = generateNextClientID();
        if (m_stateContainer != NULL) 
        {				    
		    // create an instance of DataNodeObserver and put into the list for further
		    // references
		    m_listOfDisplayClients [ cleintID ] = new DataNodeDisplayClient ( cleintID );

            m_stateContainer->addDisplayClient(cleintID, m_listOfDisplayClients [ cleintID ]);

            std::stringstream ss;
		    ss << m_agentName << "-->register display datanode client: has created Client ID " << cleintID;
		    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str() );
        }
		return cleintID;

		FUNCTION_EXIT;
    }


    void DataNodeAgentCorbaDef_Impl::unregiesterDisplayDataNodeClient(CORBA::ULong cleintId)
	{
		FUNCTION_ENTRY("unregiesterDisplayDataNodeClient");

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

        if (m_stateContainer != NULL) 
        {
		    // find if the specified observerId is already in the internal list
		    std::map < unsigned long, DataNodeDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( cleintId );

		    // if found one
		    if ( itr != m_listOfDisplayClients.end() )
		    {
			    // destroy the existing observer from the list
                m_stateContainer->removeDisplayClient(cleintId);
			    delete itr->second;
			    m_listOfDisplayClients.erase ( itr );
		    }
		    // if found none
		    else
		    {
			    throw TA_Base_Core::BadParameterException();
		    }
        }

		FUNCTION_EXIT;
	}

    
    DataNodeCorbaTypes::DataNodeDisplayStateSequence* DataNodeAgentCorbaDef_Impl::getDisplayDataNodeUpdate(CORBA::ULong clientId, CORBA::Boolean getall)
    {
		std::stringstream ss;
		ss << m_agentName << "-->getUpdates for Client ID "  << clientId;
		FUNCTION_ENTRY (ss.str().c_str() );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

        if (m_stateContainer != NULL) 
        {
            // find if the specified clientid is already in the internal list
		    DataNodeDisplayClient * existingClient = 0;
		    existingClient = findDisplayClient ( clientId );

		    // if found one
		    if ( 0 != existingClient )
		    {
			    // get and return the data point updates from the observer
                if (getall)
                {
                    return m_stateContainer->getAllDataNodeDisplayState();
                }
                else
                {
                    return existingClient->getCurrrentDisplayUpdates();
                }
		    }
		    // if found none
		    else
		    {
			    throw TA_Base_Core::BadParameterException();
		    }
        }
        else
        {
			throw TA_Base_Core::OperationModeException();
        }
		FUNCTION_EXIT;
    }

    void DataNodeAgentCorbaDef_Impl::cleanupDisplayContainer()
    {
        if (m_stateContainer != NULL)
        {
		    TA_Base_Core::ThreadGuard guard( m_clientListLock );
		    // destroy all observers
		    std::map < unsigned long, DataNodeDisplayClient * >::iterator itrc;
		    for ( itrc = m_listOfDisplayClients.begin();
			      itrc != m_listOfDisplayClients.end();
			      ++itrc )
		    {
                m_stateContainer->removeDisplayClient(itrc->first);
			    delete itrc->second;
		    }
		    m_listOfDisplayClients.clear();
            //clean up the state container too
            delete m_stateContainer;
            m_stateContainer = NULL;
        }
    }


	DataNodeObserver * DataNodeAgentCorbaDef_Impl::findObserver ( unsigned long observerId )
	{
        FUNCTION_ENTRY("findObserver");

		// stop multi threads adding/removing observers at the same time
		TA_THREADGUARD( m_observerListLock );

		DataNodeObserver * existingObserver = 0;

		// find if the specified observerId is already in the internal list
		std::map< unsigned long, DataNodeObserver * >::iterator itObserver = m_listOfObservers.find( observerId );
		// if found one
		if ( itObserver != m_listOfObservers.end() )
		{
			existingObserver = itObserver->second;
		}
		else
		{
			throw TA_Base_Core::BadParameterException();
        }

        FUNCTION_EXIT;
		return existingObserver;
	}

	CORBA::ULong DataNodeAgentCorbaDef_Impl::regiesterDataNodeSynState()
    {
		FUNCTION_ENTRY("regiesterDataNodeSynState");

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}
        
        if (m_synClientExisted)
        {
	        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "The datanode synchronization client already existed, but received another request, may because the origional one [%u] got timeout and was deleted.", m_synClientID);
			//this maybe not an error, because the syn client may cause timeout and try to reconnect
            //throw TA_Base_Core::OperationModeException();
        }

        CORBA::ULong observerID = createObserver();
        m_synClientID = static_cast<unsigned long>(observerID);
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "The datanode synchronization client [%lu] is created.",  m_synClientID);

	    TA_ASSERT ( m_specificAgent != NULL, "The datanode agent is NULL!" );

        std::map < unsigned long, DataNode * > allsyn;
        m_specificAgent->getAllDataNodes(allsyn);

        std::map < unsigned long, DataNode * >::iterator itr;
        ScadaCorbaTypes::KeySequence keys;
        keys.length(allsyn.size());

        unsigned int sequenceIndex = 0;
        for (itr = allsyn.begin(); itr != allsyn.end(); ++itr)
        {
            keys[sequenceIndex++] = itr->first;
        }
        if ( sequenceIndex != 0)
        {
            addToObserver( observerID, keys);
        }
        //set the flag to indicate the client is already existed, since currently only one
        //client is allowed
        m_synClientExisted = true;
		FUNCTION_EXIT;
		return observerID;
    }

	//
	// DataNodeAgentCorbaDef implementation
	//


	CORBA::ULong DataNodeAgentCorbaDef_Impl::createObserver()
	{
		FUNCTION_ENTRY("createObserver");

		// stop multi threads adding/removing observers at the same time
		TA_THREADGUARD( m_observerListLock );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		// create an instance of DataNodeObserver and put into the list for further
		// references
		unsigned long observerId = generateNextObserverID();
		m_listOfObservers[ observerId ] = new DataNodeObserver( m_specificAgent, observerId );

		std::stringstream ss;
		ss << m_agentName << "-->createObserver: has created Observer ID " << observerId;
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str() );

		FUNCTION_EXIT;
		return observerId;
	}


	void DataNodeAgentCorbaDef_Impl::destroyObserver (CORBA::ULong observerId)
	{
		FUNCTION_ENTRY("destroyObserver");

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		// stop multi threads adding/removing observers at the same time
		TA_THREADGUARD( m_observerListLock );

		// find if the specified observerId is already in the internal list
		std::map< unsigned long, DataNodeObserver * >::iterator itObserver = m_listOfObservers.find ( observerId );

		if ( itObserver == m_listOfObservers.end() )
		{
			throw TA_Base_Core::BadParameterException();
		}

		// if found one
        delete itObserver->second;
        m_listOfObservers.erase( itObserver );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Removed DataNodeObserver [%lu]",
            observerId );

		FUNCTION_EXIT;
	}


	void DataNodeAgentCorbaDef_Impl::addToObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		std::stringstream ss;
		ss << m_agentName << "-->addDataNodesToObserver for observer ID " << observerId << ": number of entities = " << keys.length();
		FUNCTION_ENTRY (ss.str().c_str() );

		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		// find if the specified observerId is already in the internal list
		TA_THREADGUARD( m_observerListLock );
        DataNodeObserver * existingObserver = findObserver( observerId );
        
		// if found one
		if ( 0 != existingObserver )
		{
			// pass the list of entity keys to the existing observer and returns
        	existingObserver->addDataNodesToObserver( keys );
		}
		// if found none
		else
		{
			throw TA_Base_Core::BadParameterException();
		}

		FUNCTION_EXIT;
	}


	void DataNodeAgentCorbaDef_Impl::removeFromObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		std::stringstream ss;
		ss << m_agentName << "-->removeDataNodesFromObserver for observer ID "  << observerId << ": number of entities = " << keys.length();
		FUNCTION_ENTRY (ss.str().c_str() );

		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		// find if the specified observerId is already in the internal list
		TA_THREADGUARD( m_observerListLock );
        DataNodeObserver * existingObserver = findObserver( observerId );

		// if found one
		if ( 0 != existingObserver )
		{
        	// pass the list of entity keys to the existing observer to remove
        	existingObserver->removeDataNodesFromObserver( keys );
		}
		// if found none
		else
		{
			throw TA_Base_Core::BadParameterException();
		}

		FUNCTION_EXIT;
	}


	DataNodeCorbaTypes::SyncUpdateSequence* DataNodeAgentCorbaDef_Impl::getUpdates(CORBA::ULong observerId)
	{
		std::stringstream ss;
		ss << m_agentName << "-->getUpdates for observer ID " << observerId;
		FUNCTION_ENTRY (ss.str().c_str() );

		if ( TA_Base_Core::Control != m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}

		// find if the specified observerId is already in the internal list
        TA_THREADGUARD( m_observerListLock );
		DataNodeObserver * existingObserver = findObserver( observerId );

		// if found one
		if ( 0 != existingObserver )
		{
			// get and return the data point updates from the observer
        	FUNCTION_EXIT;
			return existingObserver->getUpdates();
		}
		// if found none
		else
		{
			throw TA_Base_Core::BadParameterException();
		}
	}


	void DataNodeAgentCorbaDef_Impl::setToControl()
	{
        FUNCTION_ENTRY("setToControl");
		m_operationMode = TA_Base_Core::Control;

		if ( !m_servantActivated )
		{
			m_servantActivated = true;
			activateServantWithName ( m_agentName + TA_Base_Bus::DN_BULK_POLLED_INTERFACE );
		}

        // stop the synchronize thread, just set a signal without waiting
		m_synHandler->stopSynchronize();

        //todo: check whether the container is constructed or not
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        
		// start the periodic timer
		TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, 
			TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS / 2, false );
        FUNCTION_EXIT;
	}


	void DataNodeAgentCorbaDef_Impl::setToMonitor()
	{
        FUNCTION_ENTRY("setToMonitor");

		if ( !m_servantActivated )
		{
			m_servantActivated = true;
			activateServantWithName ( m_agentName + TA_Base_Bus::DN_BULK_POLLED_INTERFACE );
		}

		m_operationMode = TA_Base_Core::Monitor;
		m_generatedObservedID = time ( NULL );
		m_synClientID = 0;
		m_synClientExisted = false;
        m_generatedClientID = time ( NULL );

		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
		//start the agent synhandler, just set a signal without waiting
		m_synHandler->startSynchronize();
        FUNCTION_EXIT;
	}
        
	void DataNodeAgentCorbaDef_Impl::setToStandby()
	{
		FUNCTION_ENTRY("setToStandby");

		if ( m_servantActivated )
		{
			m_servantActivated = false;
			deactivateServant();
		}

		m_operationMode = TA_Base_Core::Standby;
		// stop the synchronize thread, just set a signal without waiting
		m_synHandler->stopSynchronize();

		m_generatedObservedID = time ( NULL );
		m_synClientID = 0;
		m_synClientExisted = false;
		m_generatedClientID = time ( NULL );
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
		FUNCTION_EXIT;
	}

	void DataNodeAgentCorbaDef_Impl::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if ( m_operationMode != mode )
		{
			m_operationMode = mode;

			switch (m_operationMode)
			{
			case TA_Base_Core::Monitor:
				setToMonitor();
				break;
			case TA_Base_Core::Control:
				setToControl();
				break;
			case TA_Base_Core::Standby:
				setToStandby();
				break;
			default:
				break;
			}
		}
	}
}
