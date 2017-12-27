/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/VirtualDataPointAgent/src/VirtualDataPointAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2012/06/12 13:35:44 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	VirtualDataPointAgent implements the GenericAgent interface.  VirtualDataPointAgent monitors and controls
  * the virtual data point entities associated with its process.
  *
  */
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/VirtualDataPointAgentEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/PMSAgentStateUpdate_MessageTypes.h"
#include "core/message/types/PMSAgentAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

#include "app/scada/VirtualDataPointAgent/src/VirtualDataPointAgent.h"


using namespace TA_Base_Core;


VirtualDataPointAgent::VirtualDataPointAgent ( int argc, char* argv[] )
:
m_agentName ( "" ),
m_genericAgent ( 0 ),
m_stateSender ( 0 ),
m_virtualDataPointAgentPrimaryKey ( 0 ),
m_stationName ( "" ),
m_threadTerminated ( false ),
m_agentSetControl ( false ),
m_auditMessageSender ( 0 ),
m_scadaProxyFactory ( 0 ),
m_proxyDpMonitoringPeriodSecs ( 10 ),
m_bulkPolledInterface ( 0 ),
m_dataPointFactory(NULL),
m_DpAccessInterface( 0 )
{
	m_virtualDataPointList.clear();

	// get a handle to the ScadaProxyFactory
	m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
    TA_ASSERT ( ( 0 != m_scadaProxyFactory ), "SCADA Proxy Factory not created");

    // create handle to DataPointFactory
    m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);

	// create handle to Generic Agent
	m_genericAgent = new GenericAgent ( argc, argv, this );

	// get the agent name
	m_agentName = RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();

	// register the local entities into ScadaProxyFactory
	m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());

	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();

	loadAgentParamValues();
}


VirtualDataPointAgent::~VirtualDataPointAgent()
{
	// if bulk interface is created
	if ( 0 != m_bulkPolledInterface )
	{
		delete m_bulkPolledInterface;
		m_bulkPolledInterface = 0;
	}
	
	if ( 0 != m_DpAccessInterface )
	{
		delete m_DpAccessInterface;
		m_DpAccessInterface = 0;
	}

	// else do nothing

	// clean up the list containing DataPoints instances created by
	// this Agent, without deleting the DataPoint instances themselves
	// since they are owned by the GenericAgent
	m_virtualDataPointList.clear();

	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();

	// clean up handle to StateUpdateMessageSender
    if( 0 != m_stateSender )
    {
        delete m_stateSender;
        m_stateSender = 0;
    }

	// remove instance of audit message sender
	if ( 0 != m_auditMessageSender )
	{
		delete m_auditMessageSender;
		m_auditMessageSender = 0;
	}

	// clean up the GenericAgent
	if ( 0 != m_genericAgent )
	{
		try
		{
			m_genericAgent->deactivateAndDeleteServant();
			m_genericAgent = 0;
		}
		catch ( ... )
		{
			// do nothing
		}
	}

	// remove handle to ScadaProxyFactory
	TA_Base_Bus::ScadaProxyFactory::removeInstance();
	m_scadaProxyFactory = 0;
}

void VirtualDataPointAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
{
	// stop multi threads accessing the list of DataPoint objects at the same time
	TA_Base_Core::ThreadGuard guard ( m_dataPointListtLock );

	dataPointList.clear();

	dataPointList = m_virtualDataPointList;
}


void VirtualDataPointAgent::loadAgentParamValues()
{
	// get configuration data for this PMS Agent
	//IEntityData* entityData = 0;
	IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

	// dynamic cast the entity data to virtualDataPointAgent EntityData
	//VirtualDataPointAgentEntityData * virtualDataPointAgentEntityData = 0;
	TA_Base_Core::VirtualDataPointAgentEntityDataPtr virtualDataPointAgentEntityData = 
		boost::dynamic_pointer_cast<TA_Base_Core::VirtualDataPointAgentEntityData>(entityData);

	// get the primary key of this VirtualDataPointAgent
	m_virtualDataPointAgentPrimaryKey = virtualDataPointAgentEntityData->getKey();

	// get the configured station name
	m_stationName = virtualDataPointAgentEntityData->getStationName();

	// get the configured health monitoring period for the proxy data points
	// defaulted to 10 secs if not configured in database
	m_proxyDpMonitoringPeriodSecs = virtualDataPointAgentEntityData->getProxyDpHealthMonitoringPeriodSecs();
	if ( 0 == m_proxyDpMonitoringPeriodSecs )
	{
		m_proxyDpMonitoringPeriodSecs = 10;
	}
}


IEntity* VirtualDataPointAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
{
	//
	// Note: the objects of interface IEntityData and IEntity are not owned
	// by this specific agent, but by the GenericAgent.  As such no object
	// clean up shall be done with regards to these objects.
	//

	// if the specified entity is of the type DataPoint
	if ( TA_Base_Core::DataPointEntityData::getStaticType() == entityData->getType() )
	{
		// create and return the DataPoint entity
		return createDataPointEntity ( entityData );
	}
	// otherwise do nothing

	return NULL;
}


void VirtualDataPointAgent::agentTerminate()
{
	// if bulk interface is created
	if ( 0 != m_bulkPolledInterface )
	{
		delete m_bulkPolledInterface;
		m_bulkPolledInterface = 0;
	}
	
	if ( 0 != m_DpAccessInterface )
	{
		delete m_DpAccessInterface;
		m_DpAccessInterface = 0;
	}
	// else do nothing

	// unsubscribe this agent StateUpdate message
	MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();
}


void VirtualDataPointAgent::agentSetMonitor()
{
LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "*** Start of agentSetMonitor()" );

	// set the flag to indicate the agent is now in Standby mode
	m_agentSetControl = false;

	// for each of the data point associated with this agent
	std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter;
	for ( dpIter = m_virtualDataPointList.begin();
		  dpIter != m_virtualDataPointList.end();
		  dpIter++ )
	{
		// make data point operated in Monitor mode
		dpIter->second->setToMonitorMode();
	}

	// if bulk interface is set up
	if ( 0 != m_bulkPolledInterface )
	{
		// stop receiving remote requests
		m_bulkPolledInterface->setToMonitor();
	}
	if ( 0 != m_DpAccessInterface )
	{
		// stop receiving remote requests
		m_DpAccessInterface->setToMonitor();
	}

	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();

	// clean up handle to StateUpdateMessageSender
	if( 0 != m_stateSender )
	{
		delete m_stateSender;
		m_stateSender = 0;
	}
	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "*** End of agentSetMonitor()" );
}


void VirtualDataPointAgent::agentSetControl()
{
LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "*** Start of agentSetControl()" );

	// set the flag to indicate the agent is now in Control mode
	m_agentSetControl = true;

	// for each of the data point associated with this agent
	std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter;
	for ( dpIter = m_virtualDataPointList.begin();
		  dpIter != m_virtualDataPointList.end();
		  dpIter++ )
	{
		// make data point operated in Control mode
		dpIter->second->setToControlMode();
	}

	// if bulk interface is set up
	if ( 0 != m_bulkPolledInterface )
	{
		// start receiving remote requests
		m_bulkPolledInterface->setToControl();
	}
	if ( 0 != m_DpAccessInterface )
	{
		// start receiving remote requests
		m_DpAccessInterface->setToControl();
	}

	// set the proxies to control mode
	m_scadaProxyFactory->setProxiesToControlMode();

	// create handle to StateUpdateMessageSender to send update state messages to
	// the Monitor agent
	if ( 0 == m_stateSender )
	{
		m_stateSender = MessagePublicationManager::getInstance().getStateUpdateMessageSender(
						PMSAgentStateUpdate::Context);
	}
	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "*** End of agentSetControl()" );
}


void VirtualDataPointAgent::registerForStateUpdates()
{
}


void VirtualDataPointAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
}


void VirtualDataPointAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
	unsigned long updateEventPKey = updateEvent.getKey();

	// if the update event is for VirtualDataPointAgent
	if ( m_virtualDataPointAgentPrimaryKey == updateEventPKey )
	{
		// update VirtualDataPointAgent configuration
		updateVirtualDataPointAgentConfiguration ( updateEvent );
		return;
	}

	// else update data point configuration
	updateDataPointConfiguration ( updateEvent );
}


void VirtualDataPointAgent::startVirtualDataPointAgent()
{
	// check the runtime parameter OperationMode
	std::string operationMode ( "" );
	operationMode = RunParams::getInstance().get ( RPARAM_OPERATIONMODE );

	// if defined as "Control"
	if ( RPARAM_CONTROL == operationMode )
	{
		agentSetControl();
	}
	else if ( RPARAM_MONITOR == operationMode )
	{
		agentSetMonitor();
	}

	// create an instance of the class DataPointAgentCorbaDef_Impl
	m_bulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
	m_DpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl(m_agentName, this);

	if ( true == m_agentSetControl )
	{
		// start receiving remote requests
		m_bulkPolledInterface->setToControl();
		m_DpAccessInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_bulkPolledInterface->setToMonitor();
		m_DpAccessInterface->setToMonitor();
	}

	// else do nothing

	// start the Generic Agent loop
	if ( 0 != m_genericAgent )
	{
		m_genericAgent->run();
	}
}


void VirtualDataPointAgent::submitAuditMessage( const std::string & eventMsg )
{
	// to be implemented if required
}



void VirtualDataPointAgent::updateVirtualDataPointAgentConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent)
{
	std::string msg ( "" );

	switch ( updateEvent.getModifications() )
	{
		// only if the VirtualDataPointAgent has been updated in the database
		case TA_Base_Core::Update:
		{
			// get the list of changed parameter from the config update
			const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

			if ( ( 0 != changes ) && ( false == changes->empty() ) )
			{
				// get configuration data for this VirtualDataPointAgent
				//IEntityData* entityData = 0;
				IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

				// dynamic cast the entity data to VirtualDataPointAgentEntityData
				//VirtualDataPointAgentEntityData * virtualDataPointAgentEntityData = 0;

				VirtualDataPointAgentEntityDataPtr virtualDataPointAgentEntityData = 
					boost::dynamic_pointer_cast<VirtualDataPointAgentEntityData>(entityData);

				// invalidate existing data so that new data is loaded from database
				virtualDataPointAgentEntityData->invalidate();

				for ( unsigned int i = 0; i < changes->size(); i++ )
				{
					// Proxy DataPoint Health Monitoring Period
					if ( TA_Base_Core::VirtualDataPointAgentEntityData::PROXY_DP_HEALTH_MONITORING_PERIOD_SECS == (*changes)[i] )
					{
						// proxy health monitoring is no longer available in
						// the latest ScadaProxyFactory --> therefore this will do nothing
					}
					// else do nothing (or log audit message?)
				}
			}

			break;
		}

		// don't care in other case
		default:
		{
			// log audit message?

			break;
		}
	}
}


void VirtualDataPointAgent::updateDataPointConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent)
{
	DataPoint * theDataPoint = 0;
	unsigned long updateEventPKey = updateEvent.getKey();

	// find if there is a data point whose pkey matches with the specified entity key
	std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itr = m_virtualDataPointList.find ( updateEventPKey );

	// if found one
	if ( itr != m_virtualDataPointList.end() )
	{
		// get the reference to the data point
		theDataPoint = itr->second;

		// get the type of updating event
		switch ( updateEvent.getModifications() )
		{
			case TA_Base_Core::Update:
			{
				// pass the config update to the data point for updating
				theDataPoint->update ( updateEvent );

				// exit switch statement
				break;
			}
			case TA_Base_Core::Delete:
			{
				// erase the iter without delete the data point entity, since
				// the entity is owned by the GenericAgent
				m_virtualDataPointList.erase ( itr );

				break;
			}
			case TA_Base_Core::Create:
			{
				// do nothing.  Creation of new DataPoint entity shall
				// be handle by the GenericAgent createEntity operation
				break;
			}
			default:
			{
				// do nothing, exit switch statement
				break;
			}
		}
	}
	// else do nothing

	theDataPoint = 0;
}


void VirtualDataPointAgent::notifyGroupOffline( const std::string& group )
{
	// nothing to do at this stage
}


void VirtualDataPointAgent::notifyGroupOnline( const std::string& group )
{
	// nothing to do at this stage
}


IEntity* VirtualDataPointAgent::createDataPointEntity ( TA_Base_Core::IEntityDataPtr entityData )
{
	DataPoint * theEntity = 0;

	// cast the EntityData to DataPointEntityData
    TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
        boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(entityData);
    TA_ASSERT(dataPointEntityData.get() != NULL, "Unable to cast when clearly it should work");

	if ( DPT_CFG_VIRTUAL == dataPointEntityData->getAddress() )
	{
		theEntity = m_dataPointFactory->createDataPoint ( dataPointEntityData );
	}
	else
	{
		// do nothing.  Only want virtual data points
	}

	if ( 0 != theEntity )
	{
		// double check again, Only look for virtual data points
		if ( true == theEntity->isVirtual() )
		{
			// insert the newly created data point to internal list for further
			// referencing
			m_virtualDataPointList[ theEntity->getEntityKey() ] = theEntity;
		}
		else
		{
			std::stringstream errorMsg;
			errorMsg << "config error, the datapoint %d is not a virtual datapoint." << theEntity->getEntityKey();
			TA_ASSERT(false, errorMsg.str().c_str());
		}
	}

	return theEntity;
}


TA_Base_Bus::DataPoint * VirtualDataPointAgent::getDataPoint ( unsigned long entityKey )
{
	// stop multi threads accessing the list of DataPoint objects at the same time
	TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

	TA_Base_Bus::DataPoint * theDataPoint = 0;

	// find if there is a data point whose pkey matches with the specified entity key
	std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itr = m_virtualDataPointList.find ( entityKey );

	// if found one
	if ( itr != m_virtualDataPointList.end() )
	{
		// get the reference to the data point
		theDataPoint = itr->second;
	}
	// else do nothing

	return theDataPoint;
}


TA_Base_Bus::DataPointFactory* VirtualDataPointAgent::getDataPointFactory()
{
    TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");

	return m_dataPointFactory;
}
