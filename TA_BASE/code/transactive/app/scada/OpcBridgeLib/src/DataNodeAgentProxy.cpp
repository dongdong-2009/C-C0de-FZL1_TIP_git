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
// DataNodeAgentProxy.cpp: implementation of the DataNodeAgentProxy class.
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
#include "app/scada/OpcBridgeLib/src/DataNodeAgentProxy.h"
#include "app/scada/OpcBridgeLib/src/DisplayDataNodeProxy.h"

#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
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

    DataNodeAgentProxy::DataNodeAgentProxy(unsigned long agentKey, unsigned long locationKey, DisplayProxyManager& proxyManager)
        :
        m_toBeTerminated(false),
        m_isSynchronising(false),
		m_isPrevSynchronising(true),
        m_observerId(0),
        m_agentInterface(NULL),
		m_locationkey(locationKey),
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
        
        if ( pollPeriod >= 100 && pollPeriod <= 10000 )
        {
            m_pollPeriod = pollPeriod;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Bulk poll period is set to [%u] ms for agent [%s]", m_pollPeriod, m_agentName.c_str());
        }
        
        // set retry period if parameter is set between m_pollPeriod to 1800 seconds
        unsigned long retryPeriod = atol(TA_Base_Core::RunParams::getInstance().get("RetryPeriodSecond").c_str());
        
        if ( (retryPeriod * 1000) >= m_pollPeriod &&  retryPeriod <= 1800 )
        {
            m_retryPeriod = retryPeriod;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retry period is set to [%u] seconds for agent [%s]", m_retryPeriod, m_agentName.c_str());
        }
        m_attributeMap.clear();
		
		start();
    }

    DataNodeAgentProxy::~DataNodeAgentProxy()
    {
        terminateAndWait();
        
        if (m_agentInterface != NULL)
        {
            delete m_agentInterface;
			m_agentInterface = NULL;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DataNodeAgentProxy [%s] destroyed", m_agentName.c_str());
    }
    
    void DataNodeAgentProxy::terminate()
    {
        m_toBeTerminated = true;
		m_conditionLoop.signal();
    }
    
    void DataNodeAgentProxy::startSynchronise()
    {
        if ( !m_isSynchronising )
        {
            m_isSynchronising = true;
			m_isPrevSynchronising = false;
        }
    }
    
    void DataNodeAgentProxy::stopSynchronise()
    {
        if ( m_isSynchronising )
        {
            m_isSynchronising = false;
			m_isPrevSynchronising = true;
        }
    }
    
    bool DataNodeAgentProxy::isSynchronised()
    {
        return m_isSynchronising;
    }
    
	void DataNodeAgentProxy::run()
	{
		try
		{
			TA_Base_Core::CorbaName corbaName = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(m_agentKey, true);
			m_agentName = corbaName.getObjectName();
			m_agentInterface = new DataNodeAgentInterface();
			m_agentInterface->setCorbaName(m_agentName, m_agentName + TA_Base_Bus::DN_BULK_POLLED_INTERFACE);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--DataNodeAgentProxy get corba name for Agent:%u failed ", m_agentKey);
		}

		if (NULL == m_agentInterface) return;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--DNAgent:[%s] run, attribute:%d", 
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
	                    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dataNodeUpdates = NULL;

						if (0 == m_observerId)
	                    {
							CORBA_CALL_RETURN( m_observerId, (*m_agentInterface), regiesterDisplayDataNodeClient, () );

	                        if (m_observerId > 0 && !m_toBeTerminated)
	                        {
	                            //first time, get all update from agent
	                            CORBA_CALL_RETURN( dataNodeUpdates, (*m_agentInterface), getDisplayDataNodeUpdate, ( m_observerId, true ) );
								m_ObserverResetAndResume = true;
	                        }
	                    }
	                    else
	                    {
							CORBA_CALL_RETURN( dataNodeUpdates, (*m_agentInterface), getDisplayDataNodeUpdate, ( m_observerId, false ) );			
	                    }

	                    skipSteps = 0; //reset skipSteps when connected to agent
	                    m_timeLastConnected = time(NULL);
	                    m_connected = true;
						lossCount = 0;
	                    processUpdates(dataNodeUpdates);
	                }
	                else
	                {
						if (logskipstep)
						{
							logskipstep = false;
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dn skip [%u] agent [%s], synch:1, loss:%lu", 
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

	                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dn skip [%u] agent [%s], synch:0, loss:%lu", 
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

    void DataNodeAgentProxy::setToBadNotConnectedQuality (ExceptionType exception)
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
					DnAttributeMap::iterator itDn;
					for (itDn = m_attributeMap.begin(); itDn != m_attributeMap.end(); ++itDn)
					{
						unsigned long status = itDn->second->getQualityValue();
						if (status != TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED && 
							status != TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE)
						{
							itDn->second->setStatus(TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE);
							if (itDn->second->m_attrSize > 0)
							{
								m_proxyManager.queueItemUnique(itDn->first, itDn->second);
							}
						} 
					}
	            
					TA_Base_Core::ThreadGuard lock2(m_displayCacheLock);
					DnDisplayStateMap::iterator itDnState;
					for (itDnState = m_displayCache.begin(); itDnState != m_displayCache.end(); ++itDnState)
					{
						unsigned long status = itDnState->second.quality.value;
						if (status != TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED && 
							status != TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE)
						{
							itDnState->second.quality.value = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
						} 
					}

					LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
						"Have notified DataNodeProxyManager that BulkInterface of data point agent %s is not available.",
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

	void DataNodeAgentProxy::dynamicSetDnAttribute(unsigned long key, DataNodeAttributes* dnAttribute)
	{
		if (dnAttribute != NULL)
		{
			// lock1 block
			{
				TA_Base_Core::ThreadGuard lock1(m_attributeMapLock);
				m_attributeMap[key] = dnAttribute;
			}
			// lock2 block
			{
				TA_Base_Core::ThreadGuard lock2(m_displayCacheLock);
				DnDisplayStateMap::iterator itDisplay = m_displayCache.find(key);
				if (itDisplay != m_displayCache.end())
				{
					dnAttribute->updateState(itDisplay->second);
					m_displayCache.erase(itDisplay);
				}
			}
		}
	}

	void DataNodeAgentProxy::preSetDnAttribute(std::vector<DataNodeAttributes*>& dnAttributes)
	{
		TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
		std::vector<DataNodeAttributes*>::iterator itDn;
		for (itDn = dnAttributes.begin(); itDn != dnAttributes.end(); ++itDn)
		{
			if (*itDn != NULL)
			{
				m_attributeMap.insert(std::make_pair((*itDn)->getEntityKey(), *itDn));
			}
		}
	}

	void DataNodeAgentProxy::processUpdates(TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dnUpdates)
	{
		unsigned int  updateSize = dnUpdates->length();
		if (NULL != dnUpdates && !m_toBeTerminated && updateSize > 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Observer ID %u received %u data node updates from  agent %s",
					m_observerId, updateSize, m_agentName.c_str());
			try
			{
				std::vector<TA_Base_Bus::DataNodeDisplayState> tmpDnState;

				//lock1 block
				{
					TA_Base_Core::ThreadGuard lock1(m_attributeMapLock);
					for (unsigned int i = 0; i < updateSize && !m_toBeTerminated; ++i)
					{
						TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState&  dataNodeUpdate = (*dnUpdates)[i];
						TA_Base_Bus::DataNodeDisplayState toupdate;
						TA_Base_Bus::convertDnDisplayUpdate(dataNodeUpdate, toupdate);

						if(m_ObserverResetAndResume == true)
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
								"MarkA--DataNodeAgentProxy %s - received an update for entity %d with 1st QualityValue %d: EQualityStatus %d ",
								m_agentName.c_str(), dataNodeUpdate.entityKey, toupdate.quality.value, toupdate.quality.quality);				
						}
						else
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
								"MarkA--DataNodeAgentProxy %s - received an update for entity %d with QualityValue %d: EQualityStatus %d ",
								m_agentName.c_str(), dataNodeUpdate.entityKey, toupdate.quality.value, toupdate.quality.quality);
						}

						DnAttributeMap::iterator itDnAttr = m_attributeMap.find(dataNodeUpdate.entityKey);
						if (itDnAttr != m_attributeMap.end())
						{									
							bool isUpdate = itDnAttr->second->updateState(toupdate);
							if (isUpdate && (itDnAttr->second->m_attrSize > 0))
							{
								m_proxyManager.queueItemUnique(dataNodeUpdate.entityKey, itDnAttr->second);
							}
						}
						else
						{
							tmpDnState.push_back(toupdate);
						}
					}
				}

				//lock2 block
				{
					TA_Base_Core::ThreadGuard lock2(m_displayCacheLock);
					std::vector<TA_Base_Bus::DataNodeDisplayState>::iterator itDnState;
					for (itDnState = tmpDnState.begin(); itDnState != tmpDnState.end() && !m_toBeTerminated; ++itDnState)
					{
						TA_Base_Bus::DataNodeDisplayState& dnState = m_displayCache[itDnState->entityKey];
						dnState.entityKey = itDnState->entityKey;
						dnState.alarmState = itDnState->alarmState;
						dnState.ONITP = itDnState->ONITP;
						dnState.quality = itDnState->quality;
						dnState.alarmAckState = itDnState->alarmAckState;
					}
				}
			
				if(m_ObserverResetAndResume == true)
				{	
					m_ObserverResetAndResume = false;
				}

			}
			catch (...) 
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "...","MarkB--Unknown Exception caught during processing updates");
			}							
		}

		//release DataNodeDisplayStateSequence
		if ( NULL != dnUpdates )
		{
			delete dnUpdates;
			dnUpdates = NULL;
		}
	}

	void DataNodeAgentProxy::prepareAgentThreadEnd()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--stopping DataNodeAgentProxy thread for Agent [%s]", m_agentName.c_str());

		if (m_observerId > 0) 
		{
			try
			{
				CORBA_CALL( (*m_agentInterface), unregiesterDisplayDataNodeClient, ( m_observerId ) );
			}
			catch (...) 
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--stopping DataNodeAgentProxy thread for Agent [%s] unknow exception", m_agentName.c_str());
			}

			//set observer to bad quality
			TA_Base_Core::ThreadGuard lock(m_attributeMapLock);
			DnAttributeMap::iterator itDn;
			for (itDn = m_attributeMap.begin(); itDn != m_attributeMap.end(); ++itDn)
			{
				itDn->second->setStatus(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
				if (itDn->second->m_attrSize > 0)
				{
					itDn->second->processProxyUpdate();
				}
			}
		}	
	}
}
