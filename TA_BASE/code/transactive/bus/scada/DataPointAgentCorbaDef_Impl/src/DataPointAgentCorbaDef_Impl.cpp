/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.cpp $
  * @author  HoaVu
  * @version $Revision: #4 $
  * Last modification : $DateTime: 2015/02/11 16:14:07 $
  * Last modified by : $Author: haijun.li $
  *
  *	DataPointAgentCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */

#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointDisplayClient.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointObserver.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentSynHandler.h"


#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/naming/src/Naming.h"

#include "time.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	DataPointAgentCorbaDef_Impl::DataPointAgentCorbaDef_Impl ( const std::string & agentName, TA_Base_Bus::IDataPointAgent * specificDataPointAgent)
	:
	m_agentName ( agentName ),
	m_operationMode ( TA_Base_Core::Monitor ),
	m_specificAgent ( specificDataPointAgent ),	
	 m_generatedObservedID ( time( NULL ) ) ,
    m_generatedClientID( time( NULL ) ),
    m_stateContainer(NULL),
    m_synClientID(0),
    m_synClientExisted(false),
    m_synHandler(NULL)
	{
        m_listOfDisplayClients.clear();
		m_listOfObservers.clear();

		// activate the ServantBase of this class
		activateServantWithName ( m_agentName + TA_Base_Bus::DP_BULK_POLLED_INTERFACE);

        m_stateContainer = new DataPointStateContainer(m_specificAgent);

        m_synHandler = new DataPointAgentSynHandler(m_agentName, m_specificAgent);
    }


	DataPointAgentCorbaDef_Impl::~DataPointAgentCorbaDef_Impl()
	{
		m_operationMode = TA_Base_Core::Monitor;		
		
		// CL-17309 ++
		deactivateServant();
		// this will cause recursion call of destructor
		//deactivateAndDeleteServant();
		// ++ CL-17309
		
		if (NULL != m_synHandler)
		{
			m_synHandler->stopSynchronize();
			delete m_synHandler;
			m_synHandler = NULL;
		}
        // ensure the timer is no longer scheduled to call back on a NULL object
        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
        
		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificAgent = NULL;
        {
			// destroy all observers
        	TA_Base_Core::ThreadGuard guard( m_observerListLock );
		    // destroy all observers
		    std::map < unsigned long, DataPointObserver * >::iterator itr;
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

	//TD15618, use timer to trigger if the observers are timeout
	void DataPointAgentCorbaDef_Impl::threadedTimerExpired(long timerId, void* userData)
	{
        // flag to indicate if all observers are gone
        bool allObserversEmpty = true;
        
        {
		    // stop multi threads adding/removing observers while we are doing this
		    TA_Base_Core::ThreadGuard guard( m_observerListLock );

		    // for each of the existing observers in the list
			std::map < unsigned long, DataPointObserver * >::iterator itObserver = m_listOfObservers.begin();
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
		    TA_Base_Core::ThreadGuard guard( m_clientListLock );
              
            if (m_stateContainer != NULL) 
			{        
				std::map < unsigned long, DataPointDisplayClient * >::iterator itDisplayClient = m_listOfDisplayClients.begin();
				while ( itDisplayClient != m_listOfDisplayClients.end() )
				{
			        if ( TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS < itDisplayClient->second->getIdleTime() )
			        {
				        // if this happens, destroy the observer
				        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
							"Clinet [%lu] timeout [%lu ms] expired and its updates will be deleted as well as itself", 
							itDisplayClient->second->getClientID(), TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS);                        

						m_stateContainer->removeDisplayClient(itDisplayClient->first);

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


	unsigned long DataPointAgentCorbaDef_Impl::generateNextObserverID()
	{
		// increment and return the ID
	  return ++m_generatedObservedID;
	}


	unsigned long DataPointAgentCorbaDef_Impl::generateNextClientID()
	{
		// increment and return the ID
		m_generatedClientID = m_generatedClientID + 1;
		return m_generatedClientID;
	}

	DataPointDisplayClient * DataPointAgentCorbaDef_Impl::findDisplayClient ( unsigned long clientId )
	{
		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		DataPointDisplayClient * existingClient = 0;

		// find if the specified observerId is already in the internal list
		std::map < unsigned long, DataPointDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( clientId );

		// if found one
		if ( itr != m_listOfDisplayClients.end() )
		{
			existingClient = itr->second;
		}

		return existingClient;
	}


	CORBA::ULong DataPointAgentCorbaDef_Impl::regiesterDisplayDataPointClient()
    {
		FUNCTION_ENTRY("regiesterDisplayDataPointClient");

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		unsigned long cleintID = generateNextClientID();
        if (m_stateContainer != NULL) 
        {				    
		    // create an instance of DataPointObserver and put into the list for further
		    // references
		    m_listOfDisplayClients [ cleintID ] = new DataPointDisplayClient ( cleintID );

            m_stateContainer->addDisplayClient(cleintID, m_listOfDisplayClients [ cleintID ]);

            std::stringstream ss;
		    ss << m_agentName << "-->register display datapoint client: has created Client ID " << cleintID;
		    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str() );
        }
		return cleintID;

		FUNCTION_EXIT;
    }


    void DataPointAgentCorbaDef_Impl::unregiesterDisplayDataPointClient(CORBA::ULong cleintId)
	{
		FUNCTION_ENTRY("unregiesterDisplayDataPointClient");

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_clientListLock );

        if (m_stateContainer != NULL) 
        {
		    // find if the specified observerId is already in the internal list
		    std::map < unsigned long, DataPointDisplayClient * >::iterator itr = m_listOfDisplayClients.find ( cleintId );

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

    
    DataPointCorbaTypes::DisplayStateSequence* DataPointAgentCorbaDef_Impl::getDisplayDataPointUpdate(CORBA::ULong clientId, CORBA::Boolean getall)
    {
		std::stringstream ss;
		ss << m_agentName << "-->getUpdates for Client ID "  << clientId;
		FUNCTION_ENTRY (ss.str().c_str() );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

        if (m_stateContainer != NULL) 
        {
            // find if the specified clientid is already in the internal list
		    DataPointDisplayClient * existingClient = 0;
		    existingClient = findDisplayClient ( clientId );

		    // if found one
		    if ( 0 != existingClient )
		    {
			    // get and return the data point updates from the observer
                if (getall)
                {
                    return m_stateContainer->getAllDataPointDisplayState();
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


    void DataPointAgentCorbaDef_Impl::cleanupDisplayContainer()
    {
        if (m_stateContainer != NULL)
        {
		    TA_Base_Core::ThreadGuard guard( m_clientListLock );
		    // destroy all observers
		    std::map < unsigned long, DataPointDisplayClient * >::iterator itrc;
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


	void DataPointAgentCorbaDef_Impl::cleanupObserversAndClients()
	{
		{
			TA_THREADGUARD( m_observerListLock );
			
		    std::map < unsigned long, DataPointObserver * >::iterator itr;
		    for ( itr = m_listOfObservers.begin();
			      itr != m_listOfObservers.end();
			      itr++ )
		    {
			    delete itr->second;
				itr->second = NULL;
		    }
		    m_listOfObservers.clear();
		}
	
		{
			TA_Base_Core::ThreadGuard guard( m_clientListLock );
			
		    std::map < unsigned long, DataPointDisplayClient * >::iterator itrc;
		    for ( itrc = m_listOfDisplayClients.begin();
			      itrc != m_listOfDisplayClients.end();
			      ++itrc )
		    {
	            m_stateContainer->removeDisplayClient(itrc->first);
			    delete itrc->second;
		    }
		    m_listOfDisplayClients.clear();
		}
	}

	DataPointObserver * DataPointAgentCorbaDef_Impl::findObserver ( unsigned long observerId )
	{
        FUNCTION_ENTRY("findObserver");

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_observerListLock );

		DataPointObserver * existingObserver = 0;

		// find if the specified observerId is already in the internal list
		std::map < unsigned long, DataPointObserver * >::iterator itr = m_listOfObservers.find ( observerId );

		// if found one
		if ( itr != m_listOfObservers.end() )
		{
			existingObserver = itr->second;
		}

        FUNCTION_EXIT;
		return existingObserver;
	}


	CORBA::ULong DataPointAgentCorbaDef_Impl::regiesterDataPointSynState()
    {
		FUNCTION_ENTRY("regiesterDataPointSynState");

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}
        
        if (m_synClientExisted)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "The datapoint synchronization client already existed, but received another request, may because the origional one [%u] got timeout and was deleted.", m_synClientID);
            //this maybe not an error, because the syn client may cause timeout and try to reconnect
            //throw TA_Base_Core::OperationModeException();
        }

        CORBA::ULong observerID = createObserver();
        m_synClientID = static_cast<unsigned long>(observerID);
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "The datapoint synchronization client [%lu] is created.",  m_synClientID);

	    TA_ASSERT ( m_specificAgent != NULL, "The datapoint agent is NULL!" );
        
        std::map < unsigned long, DataPoint * > allsyn;
        m_specificAgent->getAllDataPoints(allsyn);

        std::map < unsigned long, DataPoint * >::iterator itr;
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
		

	CORBA::ULong DataPointAgentCorbaDef_Impl::createObserver()
	{
		FUNCTION_ENTRY("createObserver");

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_observerListLock );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		// create an instance of DataPointObserver and put into the list for further
		// references
		unsigned long observerId = generateNextObserverID();
		m_listOfObservers [ observerId ] = new DataPointObserver ( m_specificAgent, observerId );

		std::stringstream ss;
		ss << m_agentName << "-->createObserver: has created Observer ID " << observerId;
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str() );

		FUNCTION_EXIT;
		return observerId;
	}


	void DataPointAgentCorbaDef_Impl::destroyObserver(CORBA::ULong observerId)
	{
		FUNCTION_ENTRY("destroyObserver");
		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		// stop multi threads adding/removing observers at the same time
		TA_Base_Core::ThreadGuard guard( m_observerListLock );

		// find if the specified observerId is already in the internal list
		std::map< unsigned long, DataPointObserver * >::iterator itObserver = m_listOfObservers.find ( observerId );

		if ( itObserver == m_listOfObservers.end() )
		{
			throw TA_Base_Core::BadParameterException();
		}

		// if found one
        delete itObserver->second;
        m_listOfObservers.erase( itObserver );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Removed DataPointObserver [%lu]",
            observerId );

		FUNCTION_EXIT;
	}


	void DataPointAgentCorbaDef_Impl::addToObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		std::stringstream ss;
		ss << m_agentName << "-->addDataPointsToObserver for observer ID "  << observerId << ": number of entities = " << keys.length();
		FUNCTION_ENTRY (ss.str().c_str() );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		// find if the specified observerId is already in the internal list
		DataPointObserver * existingObserver = findObserver ( observerId );

		// if found one
		if ( 0 != existingObserver )
		{
			// pass the list of entity keys to the existing observer and returns
			existingObserver->addDataPointsToObserver ( keys );
		}
		// if found none
		else
		{
			throw TA_Base_Core::BadParameterException();
		}

		FUNCTION_EXIT;
	}


	void DataPointAgentCorbaDef_Impl::removeFromObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& keys)
	{
		std::stringstream ss;
		ss << m_agentName << "-->removeDataPointsFromObserver for observer ID "  << observerId << ": number of entities = " << keys.length();
		FUNCTION_ENTRY (ss.str().c_str() );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		// find if the specified observerId is already in the internal list
		DataPointObserver * existingObserver = findObserver ( observerId );

		// if found one
		if ( 0 != existingObserver )
		{
			// pass the list of entity keys to the existing observer to remove
			existingObserver->removeDataPointsFromObserver ( keys );
		}
		// if found none
		else
		{
			throw TA_Base_Core::BadParameterException();
		}

		FUNCTION_EXIT;
	}


	DataPointCorbaTypes::DynamicSyncUpdateSequence* DataPointAgentCorbaDef_Impl::getUpdates(CORBA::ULong observerId)
	{
		std::stringstream ss;
		ss << m_agentName << "-->getUpdates for observer ID "  << observerId;
		FUNCTION_ENTRY (ss.str().c_str() );

		// throw TA_Base_Core::OperationModeException if the specific agent is in Monitor mode.
		if (TA_Base_Core::Control != m_operationMode)
		{
			throw TA_Base_Core::OperationModeException();
		}

		// find if the specified observerId is already in the internal list
		DataPointObserver * existingObserver = findObserver ( observerId );

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


	void DataPointAgentCorbaDef_Impl::setToControl()
	{
        FUNCTION_ENTRY("setToControl");

		m_operationMode = TA_Base_Core::Control;

        // stop the synchronize thread, just set a signal without waiting
		m_synHandler->stopSynchronize();

		TA_ASSERT(m_stateContainer != NULL, "Failed to create datapoint container in DataPointAgentCorbaDef_Impl.");
    
		// the timer may not have been stopped, it is stopped once all observers are deleted
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );

		// start the periodic timer
		TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this,
			TA_Base_Bus::ScadaCorbaTypes::BULK_POLLED_MAX_IDLE_TIME_MSECS / 2, false );
        
        FUNCTION_EXIT;
	}


	void DataPointAgentCorbaDef_Impl::setToMonitor()
	{
        FUNCTION_ENTRY("setToMonitor");

		m_operationMode = TA_Base_Core::Monitor;
        m_generatedObservedID = time ( NULL );
		m_synClientID = 0;
		m_synClientExisted = false;
        m_generatedClientID = time ( NULL );

       //start the agent synhandler, just set a signal without waiting
		m_synHandler->startSynchronize();
        
        FUNCTION_EXIT;
	}
	
	void DataPointAgentCorbaDef_Impl::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		m_operationMode = mode;
		if (TA_Base_Core::Control == m_operationMode)
		{
			setToControl();
		}
		else if (TA_Base_Core::Monitor == m_operationMode)
		{
			setToMonitor();
		}
		else
		{
			m_synHandler->stopSynchronize();
			TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
			cleanupObserversAndClients();
		}
	}
	
}
