 /**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// DataPointAgentProxy.cpp: implementation of the DataPointAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>
#include "time.h"
#include "boost/cast.hpp"
#include "ace/OS.h"
#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"
#include "app/scada/OpcBridgeLib/src/DataPointAgentProxy.h"
#include "app/scada/OpcBridgeLib/src/DisplayDataPointProxy.h"

#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const unsigned short BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS = 15;
    const unsigned short SET_BAD_QUALITY_RETRY_PERIOD_SECS = 10;

    DataPointAgentProxy::DataPointAgentProxy(unsigned long agentKey, unsigned long locationKey, DisplayProxyManager& proxyManager)
        :
        m_toBeTerminated(false),
        m_isSynchronising(false),
		m_isPrevSynchronising(true),
        m_observerId(0),
        m_agentInterface(NULL),
		m_locationKey(locationKey),
		m_agentKey(agentKey),
        m_agentName(""),
        m_proxyManager(proxyManager),
        m_pollPeriod(500),
        m_retryPeriod(10),
        m_connected(true),
        m_timeLastConnected(time(NULL)),
		m_ObserverResetAndResume(false)
    {
        // set poll period if parameter is set between 100 to 10000 ms
        unsigned long pollPeriod = atol(TA_Base_Core::RunParams::getInstance().get("PollPeriodMs").c_str());

        if ( (pollPeriod >= 100) && (pollPeriod <= 10000) )
        {
            m_pollPeriod = pollPeriod;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Bulk poll period is set to [%u] ms for agent [%s]", m_pollPeriod, m_agentName.c_str());
        }

        // set retry period if parameter is set between m_pollPeriod to 600 seconds
        unsigned long retryPeriod = atol(TA_Base_Core::RunParams::getInstance().get("RetryPeriodSecond").c_str());

        if ( (retryPeriod * 1000 >= m_pollPeriod) && (retryPeriod <= 1800) )
        {
            m_retryPeriod = retryPeriod;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retry period is set to [%u] seconds for agent [%s]", m_retryPeriod, m_agentName.c_str());
        }

		m_attributeMap.clear();

		start();
    };
    
    DataPointAgentProxy::~DataPointAgentProxy()
    {
        terminateAndWait();

        if (m_agentInterface != NULL)
        {
            delete m_agentInterface;
			m_agentInterface = NULL;
        }
        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DataPointAgentProxy [%s] distroyed", m_agentName.c_str());
    }
    
    void DataPointAgentProxy::terminate()
    {
        m_toBeTerminated = true;
		m_conditionLoop.signal();
    }

    void DataPointAgentProxy::startSynchronise()
    {
        if ( !m_isSynchronising )
        {
            m_isSynchronising = true;
			m_isPrevSynchronising = false;
        }
    }
    
    void DataPointAgentProxy::stopSynchronise()
    {
        if ( m_isSynchronising )
        {
	        m_isSynchronising = false;
			m_isPrevSynchronising= true;
        }
    }
    
    bool DataPointAgentProxy::isSynchronised()
    {
        return m_isSynchronising;
    }
    
	void DataPointAgentProxy::run()
	{
		try
		{
			TA_Base_Core::CorbaName corbaName = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(m_agentKey, true);
			m_agentName = corbaName.getObjectName();
			m_agentInterface = new DataPointAgentInterface();
			m_agentInterface->setCorbaName(m_agentName, m_agentName + TA_Base_Bus::DP_BULK_POLLED_INTERFACE);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--DataPointAgentProxy get corba name for Agent:%u failed ", m_agentKey);
		}

		if (NULL == m_agentInterface) return;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--DPAgent:[%s] run, attribute:%d", 
			m_agentName.c_str(), m_attributeMap.size());

		bool logskipstep = true;
		unsigned long maxSkipSteps = BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS * 1000 / m_pollPeriod;
		unsigned long skipSteps = 0; //to skip some loops when can't connect to agent
		unsigned long lossCount = 0;

		while( !m_toBeTerminated )
		{
			try
			{
				if (m_isSynchronising) 
				{
					if ((skipSteps % maxSkipSteps) == 0)
					{                                    
						TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dataPointUpdates = NULL;

						if (0 == m_observerId)
						{
							CORBA_CALL_RETURN( m_observerId, (*m_agentInterface), regiesterDisplayDataPointClient, () );

							if (m_observerId > 0 && !m_toBeTerminated)
							{
								//first time, get all update from agent              
								CORBA_CALL_RETURN( dataPointUpdates, (*m_agentInterface), getDisplayDataPointUpdate, ( m_observerId, true ) );
								m_ObserverResetAndResume = true;
							}
						}
						else
						{
							CORBA_CALL_RETURN( dataPointUpdates, (*m_agentInterface), getDisplayDataPointUpdate, ( m_observerId, false ) );
						}

						skipSteps = 0; //reset skipSteps when connected to agent
						m_timeLastConnected = time(NULL);
						m_connected = true;
						lossCount = 0;
						processUpdates(dataPointUpdates);
					}
					else
					{
						if (logskipstep)
						{
							logskipstep = false;
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dp skip [%u] agent [%s], synch:1, loss:%lu", 
								skipSteps, m_agentName.c_str(), lossCount);
						}
						++skipSteps;
					}
				}
				else
				{
					if (m_isPrevSynchronising)
					{
						m_isPrevSynchronising = false;

						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dp skip [%u] agent [%s], synch:0, loss:%lu", 
							skipSteps, m_agentName.c_str(), lossCount);
						skipSteps = 0;
					}
				}
			}
			catch ( const TA_Base_Core::ObjectResolutionException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
				setToBadNotConnectedQuality ( OBJECT_RESOLUTION_EXCEPTION );
				// only skip poll loop when the agent can't be resolved
				if(lossCount++ < 20)
				{
					skipSteps = 0;
				}
				else
				{
					logskipstep = true;
					++skipSteps;
				}
			}
			catch ( const TA_Base_Core::ScadaProxyException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );
				setToBadNotConnectedQuality ( SCADA_PROXY_EXCEPTION );
			}
			catch ( const TA_Base_Core::BadParameterException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
				// set all data point to bad quality status and restart the polling cycle
				// by requesting the agent for new observer ID
				setToBadNotConnectedQuality ( BAD_PARAMETER_EXCEPTION );
			}
			catch ( const TA_Base_Core::OperationModeException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				// force to rebind named object next time.
				(*m_agentInterface).clear();
				// set all data point to bad quality status and restart the polling cycle
				// by requesting the agent for new observer ID
				setToBadNotConnectedQuality ( OPERATION_MODE_EXCEPTION );
			}
			catch ( const CORBA::Exception & ex )
			{
				LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
				// force to rebind named object next time.
				(*m_agentInterface).clear();
				setToBadNotConnectedQuality ( CORBA_EXCEPTION );
			}
			catch ( ... )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");
				setToBadNotConnectedQuality ( UNKNOWN_EXCEPTION );
			}

			m_conditionLoop.timedWait(m_pollPeriod);
		}

		prepareAgentThreadEnd();
	}

    void DataPointAgentProxy::setToBadNotConnectedQuality (ExceptionType exception)
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "MarkB--Begin to reset observer of Agent [%s], observer_id [%u]", m_agentName.c_str(), m_observerId);

        try
        {
			switch ( exception )
			{
			// only proceed if one of the following exceptions has occurred
			case OBJECT_RESOLUTION_EXCEPTION:     
			case CORBA_EXCEPTION:
			case BAD_PARAMETER_EXCEPTION:
			case UNKNOWN_EXCEPTION:
			case OPERATION_MODE_EXCEPTION:
			case SCADA_PROXY_EXCEPTION:
			default:
				// if met other exceptions, set to bad quality after has retried specified times.
				if (m_connected && time(NULL) > m_timeLastConnected + m_retryPeriod)
				{
					m_connected = false;

					//set observer to bad quality
					TA_Base_Core::ThreadGuard lock1(m_attributeMapLock);
					DpAttributeMap::iterator itDpAttr;
					for(itDpAttr = m_attributeMap.begin(); itDpAttr != m_attributeMap.end(); ++itDpAttr)
					{
						WORD status = itDpAttr->second->getStatus();
						if (status != TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED && 
							status != TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE)
						{
							itDpAttr->second->setStatus(TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE);
							if (itDpAttr->second->m_attrSize > 0)
							{
								m_proxyManager.queueItemUnique(itDpAttr->first, itDpAttr->second);
							}
						}                                
					}
	                
					TA_Base_Core::ThreadGuard lock2(m_displayCacheLock);
					DpDisplayStateMap::iterator itDpSate;
					for (itDpSate = m_displayCache.begin(); itDpSate != m_displayCache.end(); ++itDpSate)
					{
						unsigned long status = itDpSate->second.dataPointStatus;
						if (status != TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED && 
							status != TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE)
						{
							itDpSate->second.dataPointStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;							
						}    
					}

					LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
						"Have notified DataPointProxyManager that BulkInterface of data point agent %s is not available.",
						m_agentName.c_str() );
				}
	            
				break;
			}
        }
        catch (...) 
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during setToBadNotConnectedQuality");
        }

        m_observerId = 0;
		m_ObserverResetAndResume = false;        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "MarkB--Finished to reset observer of Agent [%s], observer_id [%u]", m_agentName.c_str(), m_observerId);
    }

	void DataPointAgentProxy::dynamicSetDpAttribute(unsigned long key, DataPointAttributes* dpAttribute)
	{
		if (dpAttribute != NULL)
		{
			//lock 1 block
			{
				TA_Base_Core::ThreadGuard lock1(m_attributeMapLock);
				m_attributeMap[key] = dpAttribute;
			}
			//lock 2 block
			{
				TA_Base_Core::ThreadGuard lock2(m_displayCacheLock);
				DpDisplayStateMap::iterator itDisplay = m_displayCache.find(key);
				if (itDisplay != m_displayCache.end())
				{
					dpAttribute->updateState(itDisplay->second);
					m_displayCache.erase(itDisplay);
				}
			}
		}
	}

	void DataPointAgentProxy::preSetDpAttribute(std::vector<DataPointAttributes*>& dpAttributes)
	{
		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
		std::vector<DataPointAttributes*>::iterator itDp;
		for (itDp = dpAttributes.begin(); itDp != dpAttributes.end(); ++itDp)
		{
			if ( *itDp != NULL)
			{
				m_attributeMap.insert(std::make_pair((*itDp)->getEntityKey(), *itDp));
			}
		}
	}

	void DataPointAgentProxy::processUpdates(TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dpUpdates)
	{
		unsigned int updateSize = dpUpdates->length();
		if (NULL != dpUpdates && !m_toBeTerminated && updateSize > 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Observer ID %u received %u data point updates from agent %s",
					m_observerId, updateSize, m_agentName.c_str());
			try
			{
				std::vector<TA_Base_Bus::DataPointDisplayState> tmpDpState;
				//lock1 block
				{
					TA_Base_Core::ThreadGuard lock1(m_attributeMapLock);
					for (unsigned int i = 0; i < updateSize && !m_toBeTerminated; ++i)
					{
						TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState&  dataPointUpdate = (*dpUpdates)[i];
						TA_Base_Bus::DataPointDisplayState toupdate;
						TA_Base_Bus::convertDpDisplayUpdate( dataPointUpdate , toupdate);

						if(m_ObserverResetAndResume == true)
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
								"MarkA--DataPointAgentProxy %s - received an update for entity %d with 1st quality as %d ",
								m_agentName.c_str(), dataPointUpdate.entityKey, toupdate.dataPointStatus);				
						}
						else
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
								"MarkA--DataPointAgentProxy %s - received an update for entity %d with quality as %d",
								m_agentName.c_str(), dataPointUpdate.entityKey, toupdate.dataPointStatus);
						}

						DpAttributeMap::iterator itDpAttr = m_attributeMap.find(dataPointUpdate.entityKey);
						if (itDpAttr != m_attributeMap.end())
						{
							bool isUpdate = itDpAttr->second->updateState(toupdate);
							if (isUpdate && (itDpAttr->second->m_attrSize > 0) )
							{
								m_proxyManager.queueItemUnique(dataPointUpdate.entityKey, itDpAttr->second);
							}                                
						}
						else
						{
							tmpDpState.push_back(toupdate);										
						}
					}
				}
				//lock2 block
				{
					TA_Base_Core::ThreadGuard lock2(m_displayCacheLock);
					std::vector<TA_Base_Bus::DataPointDisplayState>::iterator itDpState;
					for (itDpState = tmpDpState.begin(); itDpState != tmpDpState.end() && !m_toBeTerminated; ++itDpState)
					{
						TA_Base_Bus::DataPointDisplayState& dpState = m_displayCache[itDpState->entityKey];
						dpState.entityKey = itDpState->entityKey;
						dpState.alarmAckState = itDpState->alarmAckState;
						dpState.alarmState = itDpState->alarmState;
						dpState.value = itDpState->value;
						dpState.dataPointStatus = itDpState->dataPointStatus;
					}
				}
				if(m_ObserverResetAndResume == true)
				{	
					m_ObserverResetAndResume = false;
				}
			}
			catch (...) 
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
			}							
		}
		//release the points of DisplayStateSequence
		if ( NULL != dpUpdates )
		{
			delete dpUpdates;
			dpUpdates = NULL;
		}
	}

	void DataPointAgentProxy::prepareAgentThreadEnd()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--stopping DataPointAgentProxy thread for Agent [%s]", m_agentName.c_str());
		if (m_observerId > 0)
		{
			try
			{
				CORBA_CALL( (*m_agentInterface), unregiesterDisplayDataPointClient, ( m_observerId ) );
			}
			catch (...) 
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DataPointAgentProxy::run()","Unknown Exception caught during call unregiesterDisplayDataPointClient()");
			}

			//set observer to bad quality
			TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
			DpAttributeMap::iterator itDpAttr;
			for(itDpAttr = m_attributeMap.begin(); itDpAttr != m_attributeMap.end(); ++itDpAttr)
			{
				itDpAttr->second->setStatus(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
				if (itDpAttr->second->m_attrSize > 0)
				{
					itDpAttr->second->processProxyUpdate();    
				}
			}
		}	
	}
}
