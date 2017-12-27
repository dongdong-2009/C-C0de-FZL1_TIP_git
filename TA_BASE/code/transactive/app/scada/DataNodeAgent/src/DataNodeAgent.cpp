/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeAgent/src/DataNodeAgent.cpp $
  * @author:	Darren Sampson
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2012/06/12 13:35:44 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	DataNodeAgent implementes the GenericAgent interface.  DataNodeAgent monitors and controls the 
  * the entities associated with its process, such as PMS RTU and the DataNode entities.
  *
  */

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/DataNodeAgentStateUpdate_MessageTypes.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_agent/src/GenericAgent.h"

#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"

#include "app/scada/DataNodeAgent/src/DataNodeAgent.h"


//limin++, failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
    public:
        
        typedef void ( T::*MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
        static ThreadedMemberFunction& alloc( T& o )
        {
            instance()->clear();
            instance() = new ThreadedMemberFunction( o );
            
            return *instance();
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member functions to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o )
            : m_object( o )
        {
        }
        
        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        static ThreadedMemberFunction*& instance()
        {
            static ThreadedMemberFunction* m_instance = NULL;
            return m_instance;
        }
        
        static void clear()
        {
            delete instance();
            instance() = NULL;
        }
        
        T& m_object;
        MemberFunctionList m_members;
    };
    
    typedef ThreadedMemberFunction< TA_Base_App::DataNodeAgent > DataNodeAgentThreadedMemberFunction;
}
//++limin, failover


using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

namespace TA_Base_App
{
	DataNodeAgent::DataNodeAgent ( int argc, char* argv[] )
	:
    m_DataNodeAgentPrimaryKey(0),
    m_agentSetControl( false ),
    m_genericAgent( NULL ),
    m_scadaProxyFactory( NULL ),
    m_summaryProcessor( NULL ),
    m_bulkPolledInterface( NULL ),
	m_datanodeAccessInterface( NULL )
	{
		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		// create handle to Generic Agent
		m_genericAgent = new TA_Base_Bus::GenericAgent ( argc, argv, this );

		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());

		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());
	}


	DataNodeAgent::~DataNodeAgent()
	{
		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish remove SummaryProcessor" );
		}

		// remove handle to ScadaProxyFactory
		if (NULL != m_scadaProxyFactory)
		{
			TA_Base_Bus::ScadaProxyFactory::removeInstance();
			m_scadaProxyFactory = NULL;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish remove ScadaProxyFactory" );
		}

		// if bulk interface is created
		if ( NULL != m_bulkPolledInterface )
		{
			delete m_bulkPolledInterface;
			m_bulkPolledInterface = NULL;
		}

		if ( NULL != m_datanodeAccessInterface )
		{
			delete m_datanodeAccessInterface;
			m_datanodeAccessInterface = NULL;
		}

		// stop and remove SingletonTimerUtil singleton
		TA_Base_Core::SingletonTimerUtil::getInstance().invalidate();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish invalidate SingletonTimerUtil" );
		TA_Base_Core::SingletonTimerUtil::removeInstance();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish remove SingletonTimerUtil" );
		
		// clean up the GenericAgent
		if ( NULL != m_genericAgent )
		{
			try
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = NULL;
			}
			catch ( ... )
			{
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Failed to deactivate and delete m_genericAgent servant.");
			}
		}

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish call GenericAgent::deactivateAndDeleteServant" );
		
	}

	/**
      * agentTerminate
      *
      * This method is called from the onTerminate
      * method in GenericAgent. This allows the agent to do any
      * of its own cleaning up before exiting.
      *
      * If the agent's entities implement CORBA interfaces,
      * this method should take care of deactivating them. It is not
      * possible for GenericAgent to do this because the IEntity 
      * interface is not a ServantBase.
      *
      * If the class implementing this interface holds the GenericAgent
      * as a member variable, DO NOT delete it inside this method.
      * GenericAgent waits for this call to return before shutting down
      * the ORB. Deleting the GenericAgent from within this method will
      * cause obvious problems.
      */
    void DataNodeAgent::agentTerminate()
	{
		if ( NULL != m_summaryProcessor )
		{
			m_summaryProcessor->stopProcessor();
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish stop SummaryProcessor" );
		}

		if (NULL != m_scadaProxyFactory)
		{
			m_scadaProxyFactory->stopProcessing();
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish stop ScadaProxyFactory" );
		}
	}


    bool DataNodeAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                           const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                           TA_Base_Bus::IEntityList& createdEntities)
    {
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator it = entityDataList.begin();
        it != entityDataList.end(); ++it )
        {
            std::string entityDataType = (*it)->getType();
            if ((entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType()) ||
                (entityDataType == TA_Base_Core::DataPointEntityData::getStaticType()) )
            {
                m_scadaProxyFactory->registerEntityData(*it, entityDataType);
            }
        }
        // create an instance of DataNodeAccessFactory if not done already
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Start of create DataNode Hierarchy Map" );
		TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataNode Hierarchy Map created, creating scada persistence for DataNodes" );

        // initialise ScadaPersistence if not done so already
        if ( m_persistence.get() == NULL )
        {
            m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntityData->getKey()) );
        }

        for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
        iter != entityDataList.end(); ++iter )
        {
            createdEntities.push_back( 
                TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
        }

        // return true to indicate not to call createEntity
        return true;
    }


	TA_Base_Bus::IEntity* DataNodeAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		TA_Base_Bus::DataNode* dn = NULL;

		if (entityData->getType() == TA_Base_Core::DataNodeEntityData::getStaticType())
		{
            TA_Base_Core::DataNodeEntityDataPtr dataNodeEntityData =
                boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(entityData);
            TA_ASSERT(dataNodeEntityData.get() != NULL, "Unable to cast when clearly it should work");

			try
			{
				dn = new TA_Base_Bus::DataNode (dataNodeEntityData, m_persistence);
                
                m_dataNodes.push_back( dn );
			}
			catch(TA_Base_Core::DatabaseException& dbe)
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    entityData->getName().c_str(), entityData->getKey() );

				TA_THROW(GenericAgentException(dbe.what()));
			}
			catch(TA_Base_Core::DataException& de)
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    entityData->getName().c_str(), entityData->getKey() );

				TA_THROW(GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, de.what()));  
			}
			catch(TA_Base_Core::ObjectResolutionException& oe)
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    entityData->getName().c_str(), entityData->getKey() );

				TA_THROW(GenericAgentException(oe.what()));
			}
			catch( ... )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    entityData->getName().c_str(), entityData->getKey() );

				TA_THROW(GenericAgentException("Unknown exception raised while creating DataNode"));
			}
		}
		return dn;
	}


    void DataNodeAgent::entityCreationComplete()
    {
        // loading complete, free some memory
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
    }


    /**
      * agentSetMonitor
      *
      * This method is called by GenericAgent whenever it receives
      * a request from the Process Monitor to change its operating state to monitor.
      * The derived agent should respond by subscribing/unsubscribing to/from the 
      * appropriate message channels and performing any other internal house keeping.
      */
    void DataNodeAgent::agentSetMonitor()
	{
		// if the agent was in Control mode before
		if ( true == m_agentSetControl )
		{
			// set the flag to indicate the agent is now in Standby mode
			m_agentSetControl = false;

            // set all datanodes to monitor mode
            for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
            {
                (*itr)->setOperationMode( false );
            }

			// set the proxies to monitor mode
			m_scadaProxyFactory->setProxiesToMonitorMode();

			// if bulk interface is set up
			if ( NULL != m_bulkPolledInterface )
			{
				// stop receiving remote requests
				m_bulkPolledInterface->setToMonitor();
			}

			if (NULL != m_datanodeAccessInterface) 
			{
				m_datanodeAccessInterface->setToMonitor();
			}
		}
	}

    /**
      * agentSetControl
      *
      * This method is called by GenericAgent whenever it receives
      * a request from the Process Monitor to change its operating state to control.
      * The derived agent should respond by subscribing/unsubscribing to/from the 
      * appropriate message channels and performing any other internal house keeping.
      */
	void DataNodeAgent::agentSetControl()
	{
		// if the agent was in Monitor mode before
		if ( false == m_agentSetControl )
		{
            //limin++--, for failover
            DataNodeAgentThreadedMemberFunction& thrd = DataNodeAgentThreadedMemberFunction::alloc( *this );
            thrd.add( &DataNodeAgent::agentSetControlInThread );
            thrd.start();
            // set all datanodes to control mode
            //for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
            //{
            //    (*itr)->setOperationMode( true );
            //}
            //limin++--, for failover

			// if bulk interface is set up
			////if ( NULL != m_bulkPolledInterface )
			//{
			//	// start receiving remote requests
			//	m_bulkPolledInterface->setToControl();
			//}

			//if (NULL != m_datanodeAccessInterface)
			//{
			//	m_datanodeAccessInterface->setToControl();
			//}

			//// set the proxies to control mode
			//m_scadaProxyFactory->setProxiesToControlMode();
            //limin++--, for failover
		}
	}


    /**
     * limin, for failover
     */
    void DataNodeAgent::agentSetControlInThread()
    {
        FUNCTION_ENTRY( "agentSetControlInThread" );

			// set the flag to indicate the agent is now in Control mode
			m_agentSetControl = true;
            
            // set all datanodes to control mode
            for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
            {
                (*itr)->setOperationMode( true );
            }

			// if bulk interface is set up
			if ( NULL != m_bulkPolledInterface )
			{
				// start receiving remote requests
				m_bulkPolledInterface->setToControl();
			}

			if (NULL != m_datanodeAccessInterface)
			{
				m_datanodeAccessInterface->setToControl();
			}

			// set the proxies to control mode
			m_scadaProxyFactory->setProxiesToControlMode();

        FUNCTION_EXIT;
	}


	/** 
	  * notifyGroupOffline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOffline from the Process Monitor.
      *
      * @param The group name
	  */
	void DataNodeAgent::notifyGroupOffline( const std::string& group )
	{
		// nothing to do at this stage
	}

	/** 
	  * notifyGroupOnline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOnline from the Process Monitor.
      *
      * @param The group name
	  */
	void DataNodeAgent::notifyGroupOnline( const std::string& group )
	{
		// nothing to do at this stage
	}

    /**
      * registerForStateUpdates
      *
      * This method forces agents to subscribe for either
      * state updates or state update requests, depending on whether or not they
      * are operating in monitor or control mode.
      */
	void DataNodeAgent::registerForStateUpdates()
	{
	}

    /**
      * receiveSpecialisedMessage
      *
      * This method is called whenever the Agent receives a state update or
      * state update request (depending on their mode of operation)
      */
    void DataNodeAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
	}

    /**
      * processOnlineUpdate
      *
      * Generic Agent will handle the receiving of online updates. However, in some
      * cases, it will be necessary to pass the updates on to the registered agent.
      * For example, if a child entity needs to be updated.
      *
      * @param The update event
      */
    void DataNodeAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// for each of the DataNodes that belong to this agent
        const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
        std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find(updateEventPKey);
        DataNodeList::iterator itrDN;

		if ( itr != dataNodes->end() )
		{
			// Have a match - update appropriately
			TA_Base_Bus::DataNode * dn = dynamic_cast< TA_Base_Bus::DataNode * >(itr->second);

			// get the type of updating event
			switch ( updateEvent.getModifications() )
			{
            case TA_Base_Core::Update:
                // pass the config update to the data point for updating
                dn->update( updateEvent );
                break;
            
            case TA_Base_Core::Delete:
                for ( itrDN = m_dataNodes.begin(); itrDN != m_dataNodes.end(); /*++itrDN*/ )
                {
                    if ( (*itrDN)->getEntityKey() == updateEventPKey )
                    {
                        itrDN = m_dataNodes.erase( itrDN );	// must get return iterator
                    }
					else
					{
						++itrDN;
					}
                }
                m_genericAgent->deleteEntity( updateEventPKey );
				break;
            
            case TA_Base_Core::Create:
                // This doesn't actually make sense - can't create something
                // that already exists
                break;
            
            default:
                break;
			}

			dn = NULL;
		}
		else
		{
			// Not found - next we need to see if this is a create event
			if ( updateEvent.getModifications() == TA_Base_Core::Create )
			{
                TA_Base_Core::IEntityDataPtr entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(updateEventPKey));

                if (entityData->getType() == TA_Base_Core::DataNodeEntityData::getStaticType())
                {
                    TA_Base_Core::DataNodeEntityDataPtr dataNodeEntityData =
                        boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(entityData);
                    TA_ASSERT(dataNodeEntityData.get() != NULL, "Unable to cast when clearly it should work");

                    try
                    {
                        TA_Base_Bus::DataNode * dn = new TA_Base_Bus::DataNode ( dataNodeEntityData, m_persistence );
                        
                        // Insert new Datanode after its parent
                        for ( itrDN = m_dataNodes.begin(); itrDN != m_dataNodes.end(); ++itrDN )
                        {
                            if ( (*itrDN)->getEntityKey() == entityData->getParent() )
                            {
                                ++itrDN;
                                break;
                            }
                        }

                        m_dataNodes.insert( itrDN, dn );
                        m_genericAgent->addEntity( updateEventPKey, dn );

                        dn->setOperationMode( m_agentSetControl );
                    }
					catch(TA_Base_Core::DatabaseException& dbe)
					{
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                            entityData->getName().c_str(), entityData->getKey() );

						TA_THROW(GenericAgentException(dbe.what()));
					}
					catch(TA_Base_Core::DataException& de)
					{
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                            entityData->getName().c_str(), entityData->getKey() );

						TA_THROW(GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, de.what()));  
					}
					catch(TA_Base_Core::ObjectResolutionException& oe)
					{
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                            entityData->getName().c_str(), entityData->getKey() );

						TA_THROW(GenericAgentException(oe.what()));
					}
					catch( ... )
					{
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                            entityData->getName().c_str(), entityData->getKey() );

                        TA_THROW(GenericAgentException("Unknown exception raised while creating DataNode"));
					}
                }
			}
		}
	}


	void DataNodeAgent::startDataNodeAgent()
	{
		// check the runtime parameter OperationMode
		std::string operationMode = TA_Base_Core::RunParams::getInstance().get( RPARAM_OPERATIONMODE );

		// if defined as "Control"
		if ( RPARAM_CONTROL == operationMode )
		{
			m_agentSetControl = false;
			agentSetControl();
		}
		else if ( RPARAM_MONITOR == operationMode )
		{
			m_agentSetControl = true;
			agentSetMonitor();
		}

		// create an instance of the class DataPointAgentCorbaDef_Impl
		m_bulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl( m_agentName, this );

		if ( true == m_agentSetControl )
		{
			// start receiving remote requests
			m_bulkPolledInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_bulkPolledInterface->setToMonitor();
		}

		m_datanodeAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl( m_agentName, this );

		if ( true == m_agentSetControl )
		{
			// start receiving remote requests
			m_datanodeAccessInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_datanodeAccessInterface->setToMonitor();
		}


		// start the Generic Agent loop
		if ( NULL != m_genericAgent )
		{
			m_genericAgent->run();
		}
	}


	TA_Base_Bus::DataNode * DataNodeAgent::getDataNode( unsigned long entityKey )
	{
		// stop multi threads accessing the list of DataNode objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );

		TA_Base_Bus::DataNode * dn = NULL;
        
        const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find( entityKey );

		if ( itr != dataNodes->end() )
		{
			dn = dynamic_cast<TA_Base_Bus::DataNode *>(itr->second);

            if ( NULL == dn )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Could not convert entity %ul to a DataNode", entityKey );
			}
        }

		return dn;
	}
    
    void DataNodeAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );

        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }

}
