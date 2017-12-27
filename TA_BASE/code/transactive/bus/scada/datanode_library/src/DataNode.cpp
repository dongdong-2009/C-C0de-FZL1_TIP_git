/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datanode_library/src/DataNode.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  * DataNode is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * DataNode also realises the DataNodeCorbaDef interface,
  * exposing the DataNode's configuration to remote
  * clients.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/IDataNodeObserverNotification.h"
#include "bus/scada/datanode_library/src/DataNodeStates.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/ScadaRootEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/DataNodeStateUpdate_MessageTypes.h"
#include "core/message/types/DataNodeAudit_MessageTypes.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	unsigned long	DataNode::m_scadaRootKey = ULONG_MAX;
	unsigned long   DataNode::m_parentEntityKey = 0;
	// be careful these function pointers' sequence should the same 
	// with E_DN_CALC_TYPE_FROM_CHILD type define sequence
	const DataNode::RecalculateFunc DataNode::s_reCalculateFuncsFC[E_DN_CALC_MAX_FC] = {
		&DataNode::recalculateQualitySummary,
		&DataNode::recalculateAlarmSummary,
		&DataNode::recalculateAlarmAckSummary,
		&DataNode::recalculateAlarmInhibitSummary,
		&DataNode::recalculateMmsAlarmInhibitSummary,
		&DataNode::recalculateScanInhibitSummary,
		&DataNode::recalculateControlInhibitSummary,
		&DataNode::recalculateLastOperatorInfo,
		&DataNode::recalculateControlPendingSummary,
		&DataNode::recalculateOverrideSummary
	};

	// be careful these function pointers' sequence should the same 
	// with E_DN_CALC_TYPE_FROM_PARENT type define sequence
	const DataNode::RecalculateFunc DataNode::s_reCalculateFuncsFP[E_DN_CALC_MAX_FP] = {
		&DataNode::recalculateCombinedInhibitMMS
	};

    DataNode::DataNode( TA_Base_Core::DataNodeEntityDataPtr dataNodeEntityAccess, ScadaPersistencePtr persistence,unsigned long scadaRootkey )
    :
	m_operationMode(TA_Base_Core::Monitor),
    m_dataNodeEntityAccess(dataNodeEntityAccess),
    m_scadaProxyFactory(TA_Base_Bus::ScadaProxyFactory::getInstance()),
    m_summaryQ( SummaryProcessor::getInstance() ),
    m_auditSender( NULL ),
	m_parentProxy( NULL ),
	m_childProxiesCreated(false),
    m_persistence( persistence )
    {
        FUNCTION_ENTRY( "DataNode()" );
		m_scadaRootKey = scadaRootkey;
		initCalcFlags();
		getScadaRootKey();
        readStaticConfiguration();
        readDynamicState();
        clearSummaries();
		initCombinedState();
        //updateActivation();
		//processUpdateActivation();
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataNode %lu - %s constructed",m_staticConfig.entityKey,m_staticConfig.name.c_str() );

        FUNCTION_EXIT;
    }


    DataNode::~DataNode()
    {
        FUNCTION_ENTRY( "~DataNode()" );

        destroyProxies();

        if (m_auditSender != NULL)
        {
            delete m_auditSender;
            m_auditSender = NULL;
        }

        TA_Base_Bus::ScadaProxyFactory::removeInstance();

        FUNCTION_EXIT;
    }

    //
    //  IEntity Interface Implementation
    //

    /**
      * isValid
      *
      * This is part of the implementation of the IEntity Interface
      * This method returns true if the DataNode entity is 
      * valid. Otherwise it will return false.
      *
      * @return TRUE if the entity is valid, FALSE otherwise 
      */
    bool DataNode::isValid()
    {
        return true;
    }

    
    /**
      * start
      *
      * This is part of the implementation of the IEntity Interface
      * This method must start any object specific tasks,
      * such as creating its own thread.
      */
    void DataNode::start()
    {
    }

    /**
      * stop
      *
      * This is part of the implementation of the IEntity Interface
      * This method will stop any object specific tasks, such 
      * as stopping its own thread. 
      */
    void DataNode::stop()
    {
    }

    /**
      * update
      *
      * This is part of the implementation of the IEntity Interface
      * This method is called by the agent controlling the entity to ask 
      * the entity to update its configuration. Called after an update 
      * configuration event is received.
      */
    void DataNode::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        updateStaticConfiguration();
    }

    /**
      * remove
      *
      * This is part of the implementation of the IEntity Interface
      * When a configuration update event is received to delete an entity 
      * from an agent, the agent can call this method to tell the entity 
      * to commence any clean up opperations.
      * 
      * The agent then still has to remove the object from its vector of entities.
      */
    void DataNode::remove()
    {
    }

    /**
      * setToControlMode
      *
      * This is part of the implementation of the IEntity Interface
      * This method can be called by the agent responsible for the entity in
      * the event of its mode of operation changing to control. This enables the
      * entity to perform tasks such as connecting to any field devices it is
      * responsible for communicating with.
      */
    void DataNode::setToControlMode()
    {
       
    }


    /**
      * setToMonitorMode
      *
      * This is part of the implementation of the IEntity Interface
      * This method can be called by the agent responsible for the entity in
      * the event of its mode of operation changing to monitor. This enables the
      * entity to perform tasks such as disconnecting from any field devices it was
      * communicating with.
      */
    void DataNode::setToMonitorMode()
    {
      
    }


    TA_Base_Core::EOperationMode DataNode::getOperationMode()
    {
        return m_operationMode;
    }


    bool DataNode::changeOperationMode(TA_Base_Core::EOperationMode operationMode)
    {
        if (m_operationMode != operationMode)
        {
			m_operationMode = operationMode;

			// TD18751
			// if there are observers, need to notify them with complete dynamic infomation when change to control
			// to do this is because the local proxies will not be refreshed when change to control.
			if (TA_Base_Core::Control == m_operationMode)
			{
				boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> dnupdate(new DnSyncUpdate);
				dnupdate->updateType = TA_Base_Bus::DnCompleteSyncState;

				boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> completeState = getCompleteDynamicStates();
				dnupdate->completeUpdate = *(completeState.get());

				notifyRegisteredObservers(dnupdate);
			}

			return true;
        }
		else
		{
			return false;
		}
    }

    void DataNode::checkOperationMode()
    {
        if (m_operationMode != TA_Base_Core::Control)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s - in monitor mode", m_staticConfig.name.c_str() );

            throw TA_Base_Core::OperationModeException();
        }
    }

    void DataNode::destroyProxies()
    {
        FUNCTION_ENTRY( "destroyProxies()" );

        TA_THREADGUARD(m_childproxieslock);
        
		m_childProxies.freeChildProxy();
 
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "destroyProxies() - %s: Proxies destroyed",
            m_staticConfig.name.c_str() );

        clearSummaries();

		if (m_parentProxy != NULL)
		{
			delete m_parentProxy;
			m_parentProxy = NULL;
		}

        FUNCTION_EXIT;
    }


    void DataNode::recalculateScanInhibitSummary()
    {
       FUNCTION_ENTRY( "recalculateScanInhibitSummary()" );

        BooleanTag scanInhibit;
        scanInhibit.value = false;
        scanInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
        
        bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();
			//
			// Update the summary by reading all DataPoints
			//
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				itDataPoint != m_childProxies.getDataPoints().end() && false == scanInhibit.value; 
				++itDataPoint )
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;

				try
				{
					if ( false == (*dp)->isWriteable() && true == (*dp)->isScanInhibited())
					{
						scanInhibit.value = true;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "SPEX:SIS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());

					// set the flag to indicate data point is not available
					proxiesOk = false;
				}
			}

			//
			// Update the summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				itDataNode != m_childProxies.getDataNodes().end() && false == scanInhibit.value;
				++itDataNode )
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{

					BooleanTag dnScanInhibit = (*dn)->getScanInhibitSummary();
					if (true == dnScanInhibit.value)
					{
						scanInhibit.value = true;
					}

					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnScanInhibit.quality)
					{
						// set the flag to indicate data node data is uncertain
						proxiesOk = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e) 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:SIS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());

					// set the flag to indicate data point is not available
					proxiesOk = false;
				}
			}
		}
        
        // Update the quality
        if (false == proxiesOk && false == scanInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            scanInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }
        
        // Check to see if anything has changed
        if ( ( scanInhibit.value != m_dynamicState.scanInhibitSummary.value ) ||
             ( scanInhibit.quality != m_dynamicState.scanInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.scanInhibitSummary.value = scanInhibit.value;
            m_dynamicState.scanInhibitSummary.quality = scanInhibit.quality;

            submitSummaryUpdateEvent(ScanInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateAlarmInhibitSummary()
    {
        FUNCTION_ENTRY( "recalculateAlarmInhibitSummary()" );

        BooleanTag alarmInhibit;
        
        alarmInhibit.value = false;
        alarmInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();

			// Update the summary by reading all DataPoints
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end() && false == alarmInhibit.value;
				  ++itDataPoint)
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;

				try
				{
					if ( false == (*dp)->isWriteable() &&
						 true == (*dp)->isAlarmInhibited() )
					{
						alarmInhibit.value = true;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:AIS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());

					proxiesOk = false;
				}
			}

			//
			// Update the summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() && false == alarmInhibit.value;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					 BooleanTag dnAlarmInhibit = (*dn)->getAlarmInhibitSummary();
					 if (dnAlarmInhibit.value)
					{
						alarmInhibit.value = true;
					}
	            
					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnAlarmInhibit.quality)
					{
						// set the flag to indicate data node data is uncertain
						proxiesOk = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:AIS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());

					proxiesOk = false;
				}

			}
		}
                
        // Update the quality
        if (false == proxiesOk && false == alarmInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            alarmInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        // Check to see if anything has changed
        if ( ( alarmInhibit.value != m_dynamicState.alarmInhibitSummary.value ) ||
             ( alarmInhibit.quality != m_dynamicState.alarmInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.alarmInhibitSummary.value = alarmInhibit.value;
            m_dynamicState.alarmInhibitSummary.quality = alarmInhibit.quality;

            submitSummaryUpdateEvent(AlarmInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateMmsAlarmInhibitSummary()
    {
        FUNCTION_ENTRY( "recalculateMmsAlarmInhibitSummary()" );

        BooleanTag mmsAlarmInhibit;
        
        mmsAlarmInhibit.value = false;
        mmsAlarmInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();

			if (m_dynamicState.combinedInhibitMMSState.quality == QUALITY_GOOD_NO_SPECIFIC_REASON)
			{
				mmsAlarmInhibit.value = m_dynamicState.combinedInhibitMMSState.value;
			}
			else
			{
				proxiesOk = false;
			}
	        
			// Update the summary by reading all DataPoints
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end() && false == mmsAlarmInhibit.value;
				  ++itDataPoint)
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;

				try
				{
					if ( false == (*dp)->isWriteable() && true == (*dp)->isMmsAlarmInhibited())
					{
						mmsAlarmInhibit.value = true;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:MAIS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());
	                
					proxiesOk = false;
				}
			}

			//
			// Update the summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() && false == mmsAlarmInhibit.value;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					BooleanTag dnMmsAlarmInhibit = (*dn)->getMmsAlarmInhibitSummary();
					if (dnMmsAlarmInhibit.value)
					{
						mmsAlarmInhibit.value = true;
					}
	                    
					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnMmsAlarmInhibit.quality)
					{
						// set the flag to indicate data node data is uncertain
						proxiesOk = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:MAIS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());

					proxiesOk = false;
				}
			}
		}
        
        // Update the quality
        if (false == proxiesOk && false == mmsAlarmInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            mmsAlarmInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }
        
        // Check to see if anything has changed
        if ( ( mmsAlarmInhibit.value != m_dynamicState.mmsAlarmInhibitSummary.value ) ||
             ( mmsAlarmInhibit.quality != m_dynamicState.mmsAlarmInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.mmsAlarmInhibitSummary.value = mmsAlarmInhibit.value;
            m_dynamicState.mmsAlarmInhibitSummary.quality = mmsAlarmInhibit.quality;

            submitSummaryUpdateEvent(MmsAlarmInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateControlInhibitSummary()
    {
        FUNCTION_ENTRY( "recalculateControlInhibitSummary()" );

        BooleanTag controlInhibit;
        
        controlInhibit.value = false;
        controlInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();

			// Update the summary by reading all DataPoints         
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end() && false == controlInhibit.value;
				  ++itDataPoint )
			{
				DataPointProxySmartPtr* dp = itDataPoint->second;

				try
				{
					// only proceed if the data point is an output as you cannot
					// control inhibit an input data point
					if ( true == (*dp)->isWriteable() &&
						 true == (*dp)->isControlInhibited() )
					{
						controlInhibit.value = true;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:CIS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());

					proxiesOk = false;
				}
			}

			//
			// Update the summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() && false == controlInhibit.value;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					BooleanTag dnControlInhibit = (*dn)->getControlInhibitSummary();
					if (dnControlInhibit.value)
					{
						controlInhibit.value = true;
					}
	                    
					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnControlInhibit.quality)
					{
						// set the flag to indicate data node data is uncertain
						proxiesOk = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:CIS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());
	                
					proxiesOk = false;
				}
			}
		}
        
        // Update the quality
        if (false == proxiesOk && false == controlInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            controlInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }
        
        // Check to see if anything has changed
        if ( ( controlInhibit.value != m_dynamicState.controlInhibitSummary.value ) ||
             ( controlInhibit.quality != m_dynamicState.controlInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.controlInhibitSummary.value = controlInhibit.value;
            m_dynamicState.controlInhibitSummary.quality = controlInhibit.quality;

            submitSummaryUpdateEvent(ControlInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateAlarmSummary()
    {
        FUNCTION_ENTRY( "recalculateAlarmSummary()" );

        BooleanTag alarmSummary;

        // default value
        alarmSummary.value = false;
        alarmSummary.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();

			//
			// Update the alarm summary by reading all DataPoints
			//

			ChildDataPointMapIt itDataPoint;
			for (itDataPoint = m_childProxies.getDataPoints().begin();
				 itDataPoint != m_childProxies.getDataPoints().end() && false == alarmSummary.value && true == proxiesOk;
				 ++itDataPoint)
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;

				try
				{
					// only proceed if the data point is an input
					if ( false == (*dp)->isWriteable() && true == (*dp)->isInAlarmState() )
					{
						// update the Alarm Summary
						alarmSummary.value = true;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:AS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());
	                
					// set the flag to indicate data point is not available
					proxiesOk = false;
				}
			}

			//
			// Update the summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() && false == alarmSummary.value && true == proxiesOk;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					BooleanTag dnAlarmSummary = (*dn)->getAlarmSummary();

					// if the data node alarm summary is on
					if (true == dnAlarmSummary.value)
					{
						// update the alarm summary of this node
						alarmSummary.value = true;
					}
	            
					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnAlarmSummary.quality)
					{
						// set the flag to indicate data node data is uncertain
						proxiesOk = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:AS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());
	                
					// set the flag to indicate data point is not available
					proxiesOk = false;
				}
	            
			}
		}
        
        // Update the quality
        if (false == proxiesOk)
        {   
            // There is something wrong with the proxies and we haven't detected
            // alarm state
            alarmSummary.value = true;
            alarmSummary.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( ( alarmSummary.value != m_dynamicState.alarmSummary.value ) ||
             ( alarmSummary.quality != m_dynamicState.alarmSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.alarmSummary.value  = alarmSummary.value;
            m_dynamicState.alarmSummary.quality = alarmSummary.quality;

            submitSummaryUpdateEvent(AlarmSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateAlarmAckSummary()
    {
        FUNCTION_ENTRY( "recalculateAlarmAckSummary()" );
        EnumTag alarmAck;
        alarmAck.value = ScadaAlarmNotPresent;
        alarmAck.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
        
		bool summaryCertain = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			summaryCertain = areChildProxiesCreated();
			// Update the summary by reading all DataPoints
			ChildDataPointMapIt itDataPoint;
			for (itDataPoint = m_childProxies.getDataPoints().begin();
				 itDataPoint != m_childProxies.getDataPoints().end() && alarmAck.value != ScadaAlarmNack && true == summaryCertain;
				 ++itDataPoint)
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;

				try
				{
					if (false == (*dp)->isWriteable())
					{
						// Update the Alarm Ack Summary for input points
						switch ((*dp)->getAlarmAckState())
						{
						case ScadaAlarmNack:
							alarmAck.value = ScadaAlarmNack;
							break;

						case ScadaAlarmAck:
							if (alarmAck.value != ScadaAlarmNack)
							{
								alarmAck.value = ScadaAlarmAck;
							}
							break;

						case ScadaAlarmNotPresent:
						case ScadaAlarmUnknown:
							break;

						default:
							summaryCertain = false;

							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
								"recalculateAlarmAckSummary() - %s - Alarm ack state for DataPoint %d is not yet available",
								m_staticConfig.name.c_str(), (*dp)->getEntityKey());
							break;
						}
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:AAS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());
	                
					summaryCertain = false;
				}
			}
	    
			// Update the summary by reading all DataNodes
			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() && alarmAck.value != ScadaAlarmNack && true == summaryCertain;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					EnumTag dnAckSummary = (*dn)->getAlarmAckStateSummary();
	                
					if (dnAckSummary.value == ScadaAlarmNack)
					{
						alarmAck.value = ScadaAlarmNack;
					}
					else if (dnAckSummary.value == ScadaAlarmAck && alarmAck.value != ScadaAlarmNack)
					{
						alarmAck.value =  ScadaAlarmAck;
					}
	        
					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnAckSummary.quality)
					{
						// set the flag to indicate data node data is uncertain
						summaryCertain = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:AAS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());
	                
					summaryCertain = false;
				}
			}
		}

        if (false == summaryCertain)
        {
            // We have some uncertainty with the summary
            // We can only be sure of an NACK
            alarmAck.value = ScadaAlarmNack;
            alarmAck.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( alarmAck.value != m_dynamicState.ackSummary.value ||
             alarmAck.quality != m_dynamicState.ackSummary.quality )
        {
            m_dynamicState.ackSummary.value = alarmAck.value;
            m_dynamicState.ackSummary.quality = alarmAck.quality;

            submitSummaryUpdateEvent(AckSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateOverrideSummary()
    {
        FUNCTION_ENTRY( "recalculateOverrideSummary()" );

        BooleanTag override;

        // default values
        override.value = false;
        override.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();

			//
			// Update the summary by reading all DataPoints
			//
	        
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end() && false == override.value;
				  ++itDataPoint )
			{
				DataPointProxySmartPtr* dp = itDataPoint->second;

				try
				{
					// only proceed if the data point is an input
					if ( false == (*dp)->isWriteable()  && true == (*dp)->isOverrideEnabled() )
					{
						// update the Override summary of this node
						override.value = true;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:OS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());

					// set the flag to indicate the data point is not available
					proxiesOk = false;
				}
			}

			//
			// Update the override summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() && false == override.value;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					BooleanTag dnOverrideSummary = (*dn)->getOverrideSummary();

					// if the data node override summary is on
					if (dnOverrideSummary.value)
					{
						// update override summary of this node
						override.value = true;
					}
	                    
					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnOverrideSummary.quality)
					{
						// set the flag to indicate data node data is uncertain
						proxiesOk = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:OS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());

					// set the flag to indicate the data point is not available
					proxiesOk = false;
				}
	            
			}
		}
        
        // Update the quality
        if (false == proxiesOk && false == override.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // override state
            override.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( ( override.value != m_dynamicState.overrideSummary.value ) ||
             ( override.quality != m_dynamicState.overrideSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.overrideSummary.value = override.value;
            m_dynamicState.overrideSummary.quality = override.quality;

            submitSummaryUpdateEvent(OverrideSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateQualitySummary()
    {
        FUNCTION_ENTRY( "recalculateQualitySummary()" );

        unsigned long qualitySummary = QUALITY_GOOD;
		
		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();
			if ( !proxiesOk )
			{
				// Not all proxies created so we just set the quality to uncertain
				qualitySummary = QUALITY_UNCERTAIN;
			}
			else
			{
				//
				// Update the quality summary by reading all DataPoints
				//

				ChildDataPointMapIt itDataPoint;
				for (itDataPoint = m_childProxies.getDataPoints().begin();
					 itDataPoint != m_childProxies.getDataPoints().end() && qualitySummary != QUALITY_BAD;
					 ++itDataPoint)
				{
					DataPointProxySmartPtr* dp = (*itDataPoint).second;

					try
					{
						// only proceed if data point is an input
						if ( false == (*dp)->isWriteable() )
						{
							// if data point quality status is not good
							// (don't care what type of bad quality it is at this stage)
							if ( QUALITY_GOOD != ((*dp)->getStatus() &  QUALITY_MASK) )
							{
								// force quality summary value to bad
								qualitySummary = QUALITY_BAD;
							}
						}
					}
					catch (TA_Base_Core::ScadaProxyException &e)
					{
						// already logged by the exception throw. This is too noisy
						// make it debug rather than an exception catch
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
							"SPEX:QS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());

						// data point is not available, force quality summary value to bad
						qualitySummary = QUALITY_BAD;
					}
				}

				//
				// Update the quality summary by reading all DataNodes
				//

				ChildDataNodeMapIt itDataNode;
				for (itDataNode = m_childProxies.getDataNodes().begin();
					 itDataNode != m_childProxies.getDataNodes().end() &&  qualitySummary != QUALITY_BAD;
					 ++itDataNode)
				{
					DataNodeProxySmartPtr* dn = (*itDataNode).second;	 
					try
					{
						EnumTag dnQualitySummary = (*dn)->getQualitySummary();

						// if the data node quality summary value is not good
						// (don't care what type of bad quality it is at this stage)
						if ( QUALITY_GOOD != dnQualitySummary.value ||
							 QUALITY_GOOD_NO_SPECIFIC_REASON != dnQualitySummary.quality)
						{
							// force quality summary value to bad
							qualitySummary = QUALITY_BAD;
						}
					}
					catch (TA_Base_Core::ScadaProxyException &e)
					{
						// already logged by the exception throw. This is too noisy
						// make it debug rather than an exception catch
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
							"SPEX:QS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());

						// data point is not available, force quality summary value to bad
						qualitySummary = QUALITY_BAD;
					}
				}
			}
		}

        if ( qualitySummary != m_dynamicState.qualitySummary.value ||
			 QUALITY_BAD_NO_SPECIFIC_REASON == m_dynamicState.qualitySummary.quality)
        {
            // Update the cached summaries
            m_dynamicState.qualitySummary.value   = qualitySummary;
			m_dynamicState.qualitySummary.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
            submitSummaryUpdateEvent(QualitySummaryUpdate);
        }
	
		FUNCTION_EXIT;
    }


    void DataNode::recalculateControlPendingSummary()
    {
        FUNCTION_ENTRY( "recalculateControlPendingSummary()" );

        BooleanTag controlPending;

        // default values
        controlPending.value = false;
        controlPending.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();

			//
			// Update the control pending summary by reading all DataPoints
			//
			ChildDataPointMapIt itDataPoint;
			for (itDataPoint = m_childProxies.getDataPoints().begin();
				 itDataPoint != m_childProxies.getDataPoints().end() && false == controlPending.value;
				 ++itDataPoint)
			{
				DataPointProxySmartPtr* dp = itDataPoint->second;

				try
				{
					// only proceed if the data point is an output
					if ( true == (*dp)->isWriteable() && pending == (*dp)->getControlState() )
					{
						// update the ControlPending Summary
						controlPending.value = true;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:CPS:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());

					// set the flag to indicate data point is not available
					proxiesOk = false;
				}
			}

			//
			// Update the summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() && false == controlPending.value;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					BooleanTag dnControlPending = (*dn)->getControlPendingSummary();

					// if the data node alarm summary is on
					if ( true == dnControlPending.value )
					{
						// update the control pending summary of this node
						controlPending.value = true;
					}
	                                
					if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnControlPending.quality)
					{
						// set the flag to indicate data node data is uncertain
						proxiesOk = false;
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:CPS:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());

					// set the flag to indicate data point is not available
					proxiesOk = false;
				}
			}
		}
        
        // Update the quality
        if (false == proxiesOk && false == controlPending.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // control pending state
            controlPending.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( ( controlPending.value != m_dynamicState.controlPendingSummary.value ) ||
             ( controlPending.quality != m_dynamicState.controlPendingSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.controlPendingSummary.value = controlPending.value;
            m_dynamicState.controlPendingSummary.quality = controlPending.quality;

            submitSummaryUpdateEvent(ControlSummaryUpdate);
        }

        FUNCTION_EXIT;
    }

    void DataNode::recalculateLastOperatorInfo()
    {
        FUNCTION_ENTRY( "recalculateLastOperatorInfo()" );

        time_t b4updatetimestamp = m_dynamicState.lastOperator.timeStamp.time;
        std::string b4updatesessionid = m_dynamicState.lastOperator.sessionID;

		bool proxiesOk = false;
		{
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);
			proxiesOk = areChildProxiesCreated();
       
			//
			// Update the summary by reading all DataPoints
			//
	        
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end() ;
				  ++itDataPoint )
			{
				DataPointProxySmartPtr* dp = itDataPoint->second;

				try
				{
					long latesttimestamp = (*dp)->getLastStatusChangeTimestamp();
					if (latesttimestamp >= m_dynamicState.lastOperator.timeStamp.time)
					{
						m_dynamicState.lastOperator.timeStamp.time = latesttimestamp;
						m_dynamicState.lastOperator.sessionID = (*dp)->getLastStatusChangeSessionId();
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:LOI:%s - DP[%d]", m_staticConfig.name.c_str(), (*dp)->getEntityKey());

					// set the flag to indicate the data point is not available
					proxiesOk = false;
				}
			}

			//
			// Update the last operator summary by reading all DataNodes
			//

			ChildDataNodeMapIt itDataNode;
			for (itDataNode = m_childProxies.getDataNodes().begin();
				 itDataNode != m_childProxies.getDataNodes().end() ;
				 ++itDataNode)
			{
				DataNodeProxySmartPtr* dn = (*itDataNode).second;
				try
				{
					long latesttimestamp = (*dn)->getLastStatusChangeTimestamp();
					if (latesttimestamp >= m_dynamicState.lastOperator.timeStamp.time)
					{
						m_dynamicState.lastOperator.timeStamp.time = latesttimestamp;
						m_dynamicState.lastOperator.sessionID = (*dn)->getLastStatusChangeSessionId();
					}
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// already logged by the exception throw. This is too noisy
					// make it debug rather than an exception catch
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
						"SPEX:LOI:%s - DN[%d]", m_staticConfig.name.c_str(), (*dn)->getEntityKey());

					// set the flag to indicate the data point is not available
					proxiesOk = false;
				}
			}
		}

        if ( ( b4updatetimestamp != m_dynamicState.lastOperator.timeStamp.time ) ||
             ( b4updatesessionid.compare(m_dynamicState.lastOperator.sessionID) != 0 ) )
        {
            submitSummaryUpdateEvent(ConfigStateLastOperatorUpdate);
        }

        
       FUNCTION_EXIT;
    }

    void DataNode::readStaticConfiguration()
    {
        FUNCTION_ENTRY( "readStaticConfiguration()" );
		//
		// Initialize DataNode Configuration first
		//
		/*
		m_staticConfig.name = "";
		m_staticConfig.description = "";
		m_staticConfig.entityKey = 0;
		m_staticConfig.subsystemKey = 0;
		m_staticConfig.locationKey = 0;
		m_staticConfig.agentName = "";
		m_staticConfig.label = "";
		m_staticConfig.shortLabel = "";
		m_staticConfig.assetName = "";
		m_staticConfig.displayOrder = 0 ;
		*/
		
        // Retrieve configuration
        m_staticConfig.name = m_dataNodeEntityAccess->getName();
        m_staticConfig.description = m_dataNodeEntityAccess->getDescription();
        m_staticConfig.entityKey = m_dataNodeEntityAccess->getKey();
        m_staticConfig.subsystemKey = m_dataNodeEntityAccess->getSubsystem();
        m_staticConfig.locationKey = m_dataNodeEntityAccess->getLocation();
        //m_staticConfig.agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		m_staticConfig.agentName = m_dataNodeEntityAccess->getAgentName();
		m_staticConfig.assetName = m_dataNodeEntityAccess->getAssetName();
        m_staticConfig.label = m_dataNodeEntityAccess->getLabel();
        m_staticConfig.shortLabel = m_dataNodeEntityAccess->getShortLabel();
        m_staticConfig.displayOrder = m_dataNodeEntityAccess->getDisplayOrder();

        //
        // retrieve the corba names of the children, based on the configured
        // synthesis type
        //
        m_staticConfig.childDataNodeEntities.clear();
        m_staticConfig.childDataPointEntities.clear();

		//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Start Set DataNode [%lu] %s Child",m_staticConfig.entityKey, m_staticConfig.name.c_str());
		m_staticConfig.childDataNodeEntities = m_dataNodeEntityAccess->getChildDataNodes();
		m_staticConfig.childDataPointEntities = m_dataNodeEntityAccess->getChildDataPoints();
		//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"End   Set DataNode [%lu] %s Child",m_staticConfig.entityKey, m_staticConfig.name.c_str());

        FUNCTION_EXIT;
    }

    void DataNode::updateStaticConfiguration()
    {
        FUNCTION_ENTRY( "updateStaticConfiguration()" );

        // invalidate the DataNodeEntityData access so that latest values are
        // retrieved from the database
        m_dataNodeEntityAccess->invalidate();

        // Retrieve online updatable configuration
        m_staticConfig.assetName = m_dataNodeEntityAccess->getAssetName();
        m_staticConfig.label = m_dataNodeEntityAccess->getLabel();
        m_staticConfig.shortLabel = m_dataNodeEntityAccess->getShortLabel();
        m_staticConfig.displayOrder = m_dataNodeEntityAccess->getDisplayOrder();

        FUNCTION_EXIT;
    }

    void DataNode::readDynamicState()
    {
        FUNCTION_ENTRY( "readDynamicState()" );
		
		m_dynamicState.tagState.tagState = TA_Base_Bus::NotTagged;
		m_dynamicState.tagState.tagInhibitState.inputInhibit = TA_Base_Bus::NoInputInhibit;
		m_dynamicState.tagState.tagInhibitState.outputInhibit = TA_Base_Bus::NoOutputInhibit;
		m_dynamicState.tagState.ptwId = "";
		m_dynamicState.tagState.comment = "";
		m_dynamicState.notes = "";
		m_dynamicState.inhibitMMSState = false;
		m_dynamicState.lastOperator.sessionID = "";
		m_dynamicState.lastOperator.timeStamp.time = 0;

        //
        // Retrieve current state for items stored in the database while we're at it
        //
        TA_ASSERT( m_persistence.get() != NULL, "ScadaPersistence has not been intialised" );
        m_persistence->readDataNodeState( m_staticConfig.entityKey, m_dynamicState );

        FUNCTION_EXIT;
    }

    void DataNode::processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType)
    {
        FUNCTION_ENTRY ("processEntityUpdateEvent()");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "processEntityUpdateEvent: entityKey [%lu] - UpdateType[%u]", entityKey, updateType);		
		if (entityKey == m_parentKey)
		{
			bool tmpCalcFlags[E_DN_CALC_MAX_FP] = {false};

			switch ( updateType )
			{
			case CombinedInhibitMMSUpdate:
			case ConfigAvailable:
				tmpCalcFlags[E_DN_CALC_COMBINE_MMS_I_FP] = true;
				break;

			default:
				break;
			}

			bool needReCalculate = false;
			TA_Base_Core::ThreadGuard lock(m_calcFlagsFromParentLock);
			for (int i = E_DN_CALC_MIN_FP; i < E_DN_CALC_MAX_FP; ++i)
			{
				if ( tmpCalcFlags[i] && !m_calcFlagsFromParent[i])
				{
					m_calcFlagsFromParent[i] = true;
					needReCalculate			 = true;
				}
			}

			if ( needReCalculate )
			{
				SummaryRequest summaryRequest(m_staticConfig.entityKey, SummaryRequest::ReCalculateRequestFromParent);
				// we need to recalculate immediately
				m_summaryQ->queueItemUnique( summaryRequest, true);
			}
		}
		else
		{
			bool tmpCalcFlags[E_DN_CALC_MAX_FC] = {false};

			switch ( updateType )
			{
			case ValueStateUpdate:
				tmpCalcFlags[E_DN_CALC_QUALITY_FC] = true;
				tmpCalcFlags[E_DN_CALC_ALARM_FC]   = true;
				tmpCalcFlags[E_DN_CALC_OVERRIDE_FC]= true;
				break;

			case ControlStateUpdate:
				tmpCalcFlags[E_DN_CALC_CRTL_PENDING_FC] = true;
				break;

			case ConfigStateLastOperatorUpdate:
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;
				break;

			case InhibitStateUpdate:
				tmpCalcFlags[E_DN_CALC_SCAN_I_FC]   =  true;
				tmpCalcFlags[E_DN_CALC_ALARM_I_FC]  =  true;
				tmpCalcFlags[E_DN_CALC_CTRL_I_FC]	= true;
				tmpCalcFlags[E_DN_CALC_MMS_I_FC]	= true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;

			case ConfigStateUpdate:
				tmpCalcFlags[E_DN_CALC_ACKSTATE_FC] = true;
				break;

			case ScanInhibitSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_SCAN_I_FC] = true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;

			case AlarmInhibitSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_ALARM_I_FC]  =  true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;

			case MmsAlarmInhibitSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_MMS_I_FC]	= true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;

			case ControlInhibitSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_CTRL_I_FC]	= true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;

			case ControlSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_CRTL_PENDING_FC] = true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;

			case AlarmSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_ALARM_FC] = true;
				break;

			case AckSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_ACKSTATE_FC] = true;
				break;

			case OverrideSummaryUpdate:
				tmpCalcFlags[E_DN_CALC_OVERRIDE_FC] = true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;

			case QualitySummaryUpdate:
				tmpCalcFlags[E_DN_CALC_QUALITY_FC] = true;
				break;

			case ConfigAvailable:
				// recalculate everything on config available
				// used to be done on creating the proxy
				tmpCalcFlags[E_DN_CALC_CRTL_PENDING_FC] = true;
				tmpCalcFlags[E_DN_CALC_ALARM_FC] = true;
				tmpCalcFlags[E_DN_CALC_ACKSTATE_FC] = true;
				tmpCalcFlags[E_DN_CALC_OVERRIDE_FC] = true;
				tmpCalcFlags[E_DN_CALC_QUALITY_FC] = true;
				tmpCalcFlags[E_DN_CALC_SCAN_I_FC]   =  true;
				tmpCalcFlags[E_DN_CALC_ALARM_I_FC]  =  true;
				tmpCalcFlags[E_DN_CALC_CTRL_I_FC]	= true;
				tmpCalcFlags[E_DN_CALC_MMS_I_FC]	= true;
				tmpCalcFlags[E_DN_CALC_LAST_OP_INFO_FC] = true;	
				break;
			}

			bool needReCalculate = false;
			TA_Base_Core::ThreadGuard lock(m_calcFlagsFromChildLock);
			for (int i = E_DN_CALC_MIN_FC; i < E_DN_CALC_MAX_FC; ++i)
			{
				if ( tmpCalcFlags[i] && !m_calcFlagsFromChild[i])
				{
					m_calcFlagsFromChild[i] = true;
					needReCalculate			= true;
				}
			}

			if ( needReCalculate && m_childProxiesCreated)
			{
				SummaryRequest summaryRequest(m_staticConfig.entityKey, SummaryRequest::ReCalculateRequestFromChildren);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "queueItemUnique: entityKey [%lu] ReCalculateRequestFromChildren", m_staticConfig.entityKey);		
				m_summaryQ->queueItemUnique( summaryRequest, false);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "queueItemUnique: entityKey [%lu] ReCalculateRequestFromChildren", m_staticConfig.entityKey);		
			}
		}

        FUNCTION_EXIT;
    }


    void DataNode::submitUpdateEvent( ScadaEntityUpdateType type )
    {
        FUNCTION_ENTRY( "submitUpdateEvent()" );

        // only proceed if operates in Control Mode
        if (m_operationMode != TA_Base_Core::Control)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s - in monitor mode", m_staticConfig.name.c_str() );
            
            return;
        }
        
        boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> toupdate(new TA_Base_Bus::DnSyncUpdate);

        switch (type)
        {
        case TagStateUpdate:
            {
                toupdate->updateType = DnTagStateUpdate;
                toupdate->tagState.comment = m_dynamicState.tagState.comment;
                toupdate->tagState.ptwId = m_dynamicState.tagState.ptwId;
                toupdate->tagState.tagInhibitState = m_dynamicState.tagState.tagInhibitState;
                toupdate->tagState.tagState = m_dynamicState.tagState.tagState;
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "submitUpdateEvent() - %s - Tag state updated",
                     m_staticConfig.name.c_str() );
            }
            break;
        
        case NotesStateUpdate:
            {
                toupdate->updateType = DnNotesUpdate;
                toupdate->notes = m_dynamicState.notes;
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "submitUpdateEvent() - %s - Notes updated",
                     m_staticConfig.name.c_str() );
            }
            break;

        case InhibitStateUpdate:
            {

                toupdate->updateType = DnInhibitMMSUpdate;
                toupdate->inhibitMMSState = m_dynamicState.inhibitMMSState;
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "submitUpdateEvent() - %s - MMS inhibit state updated",
                     m_staticConfig.name.c_str() );
            }
            break;

        default:
            TA_ASSERT(true, "Bad Update Type");
            break;
        }

        // notify all registered observers of the latest changes
        notifyRegisteredObservers ( toupdate );

        if (type != NotesStateUpdate)
        {
            //submit the last operator info for mms inhibit and tag update
            submitSummaryUpdateEvent(ConfigStateLastOperatorUpdate);
        }

        FUNCTION_EXIT;
    }

    void DataNode::submitSummaryUpdateEvent(ScadaEntityUpdateType updateType)
    {
        FUNCTION_ENTRY( "submitSummaryUpdateEvent()" );

        if (m_operationMode != TA_Base_Core::Control)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s - in monitor mode", m_staticConfig.name.c_str() );
            
            return;
        }

        boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> toupdate(new TA_Base_Bus::DnSyncUpdate);
      
        switch (updateType)
        {
        case AlarmSummaryUpdate:
            {
                toupdate->updateType = DnAlarmSummaryUpdate;
                toupdate->alarmSummary.value = m_dynamicState.alarmSummary.value;
                toupdate->alarmSummary.quality = m_dynamicState.alarmSummary.quality;
            }
            break;

        case AckSummaryUpdate:
            {
                toupdate->updateType = DnAlarmAckSummaryUpdate;
                toupdate->ackSummary.value = m_dynamicState.ackSummary.value;
                toupdate->ackSummary.quality = m_dynamicState.ackSummary.quality;             
			}
            break;
            
        case ScanInhibitSummaryUpdate:
            {
                toupdate->updateType = DnScanInhibitSummaryUpdate;
                toupdate->scanInhibitSummary.value = m_dynamicState.scanInhibitSummary.value;
                toupdate->scanInhibitSummary.quality = m_dynamicState.scanInhibitSummary.quality;             
			}
            break;

        case AlarmInhibitSummaryUpdate:
            {
                toupdate->updateType = DnAlarmInhibitSummaryUpdate;
                toupdate->alarmInhibitSummary.value = m_dynamicState.alarmInhibitSummary.value;
                toupdate->alarmInhibitSummary.quality = m_dynamicState.alarmInhibitSummary.quality;
            }
            break;

        case MmsAlarmInhibitSummaryUpdate:
            {
                toupdate->updateType = DnMmsAlarmInhibitSummaryUpdate;
                toupdate->mmsAlarmInhibitSummary.value = m_dynamicState.mmsAlarmInhibitSummary.value;
                toupdate->mmsAlarmInhibitSummary.quality = m_dynamicState.mmsAlarmInhibitSummary.quality;
			}
            break;

        case ControlInhibitSummaryUpdate:            
            {
                toupdate->updateType = DnControlInhibitSummaryUpdate;
                toupdate->controlInhibitSummary.value = m_dynamicState.controlInhibitSummary.value;
                toupdate->controlInhibitSummary.quality = m_dynamicState.controlInhibitSummary.quality;
            }
            break;

        case OverrideSummaryUpdate:
            {
                toupdate->updateType = DnOverrideSummaryUpdate;
                toupdate->overrideSummary.value = m_dynamicState.overrideSummary.value;
                toupdate->overrideSummary.quality = m_dynamicState.overrideSummary.quality;
            }
            break;
        
        case QualitySummaryUpdate:
            {
                toupdate->updateType = DnQualitySummaryUpdate;
                toupdate->qualitySummary.value = m_dynamicState.qualitySummary.value;
                toupdate->qualitySummary.quality = m_dynamicState.qualitySummary.quality;
            }
            break;
            
        case ControlSummaryUpdate:
            {
                toupdate->updateType = DnControlPendingSummaryUpdate;
                toupdate->controlPendingSummary.value = m_dynamicState.controlPendingSummary.value;
                toupdate->controlPendingSummary.quality = m_dynamicState.controlPendingSummary.quality;
            }
            break;

        case ConfigStateLastOperatorUpdate:
            {
                m_persistence->writePersistedValue( DN_LAST_OPERATOR_SESSION_ID, m_staticConfig.entityKey, m_dynamicState.lastOperator.sessionID );
                m_persistence->writePersistedValue( DN_LAST_OPERATOR_TIMESTAMP, m_staticConfig.entityKey, m_dynamicState.lastOperator.timeStamp.time ); 
                toupdate->updateType = DnLastOperatorUpdate;
                toupdate->lastOperator.sessionID = m_dynamicState.lastOperator.sessionID;
                toupdate->lastOperator.timeStamp.time = m_dynamicState.lastOperator.timeStamp.time;
            }
            break;		

        default:
            break;
        }

        notifyRegisteredObservers ( toupdate );

        FUNCTION_EXIT;
    }

    void DataNode::submitAuditEvent( const std::string & sessionID,
                                     const TA_Base_Core::DescriptionParameters & description,
                                     const TA_Base_Core::MessageType & messageType )
    {
		FUNCTION_ENTRY( "submitAuditEvent()" );

        // get handle to AuditMessageSender for the audit message type DataNodeAudit,
        // if not already done so
        if ( 0 == m_auditSender )
        {
            m_auditSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender ( TA_Base_Core::DataNodeAudit::Context );
        }

        // if there is handle to AuditMessageSender
        if ( 0 != m_auditSender )
        {
            try
            {
                // send the new audit message type
                m_auditSender->sendAuditMessageWithAsset ( messageType,                     // message type
                                                          m_staticConfig.entityKey,         // entity key
                                                          description,                      // description
                                                          "",                               // details
                                                          sessionID,                        // session ID
                                                          "",                               // alarm ID
                                                          "",                               // incidentkey
                                                          "",                               // parent event ID
                                                          m_staticConfig.assetName );   // asset name
            }
            catch ( ... )
            {
            }
        }
        // else do nothing

		FUNCTION_EXIT;
    }

    bool DataNode::isActionPermitted ( const std::string & sessionID, unsigned long actionKey )
    {
        bool response = false;
        
        // action is only permitted when data point is running in control mode
        if ( TA_Base_Core::Control == m_operationMode )
        {
            if ( ( DPT_INTERNAL_SESSION_ID == sessionID ) || ( true == sessionID.empty() ) )
            {
                response = true;
            }
            else
            {
                // get the response from the ScadaUtilities
                response = TA_Base_Bus::ScadaUtilities::getInstance().isActionAllowedWithLocSub (
                    actionKey, m_staticConfig.entityKey, m_staticConfig.locationKey, m_staticConfig.subsystemKey, sessionID );
                
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "isActionAllowedWithLocSub() returned: %s", ( true == response ) ? "TRUE" : "FALSE" );
			}
		}
		return response;
    }

    unsigned short DataNode::calculateFlexibleTimeout()
    {
        FUNCTION_ENTRY( "calculateFlexibleTimeout()" );

        unsigned short timeout = 20*3*m_staticConfig.childDataPointEntities.size();

        FUNCTION_EXIT;
        return timeout; 
    }

    void DataNode::inhibitDataPoints(TA_Base_Bus::EOutputInhibitState tagOutputInhibitState, 
                                     TA_Base_Bus::EInputInhibitState tagInputInhibitState,
                                     const std::string& sessionIdStr)
    {
        FUNCTION_ENTRY( "inhibitDataPoints()" );

		if(m_childProxies.isChildDataPointProxyCreated())
		{
		
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);

			// Perform inhibits on DataPoints
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end(); 
				  ++itDataPoint )
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;  
				try
				{
					if ((*dp)->isWriteable())
					{
						(*dp)->setOutputInhibitStatus(tagOutputInhibitState, sessionIdStr);
					}
					else
					{
						(*dp)->setInputInhibitStatus(tagInputInhibitState, sessionIdStr);
					}
				}
				catch (...)
				{
					// Any error - need to roll back all the other points...
					ChildDataPointMapIt rollbackIt;
					// Increment to make the next loop inclusive
					itDataPoint++;

					for (rollbackIt = m_childProxies.getDataPoints().begin(); 
						 rollbackIt != itDataPoint; 
						 rollbackIt++)
					{
						DataPointProxySmartPtr* rollbackDp = (*rollbackIt).second;   
						try
						{
							if ((*dp)->isWriteable())
							{
								(*rollbackDp)->setOutputInhibitStatus(tagOutputInhibitState, sessionIdStr);
							}
							else
							{
								(*rollbackDp)->setInputInhibitStatus(tagInputInhibitState, sessionIdStr);
							}
						}
						catch (...)
						{
							// Not much we can do here
						}
					}   
                
					TA_THROW(TA_Base_Core::ScadaCommonException("Can not inhibit datapoints - rolled back"));
				} 
			}
        }
		else
		{
			TA_THROW(TA_Base_Core::ScadaCommonException("the child datapoint proxies are not config valid."));
		}

        FUNCTION_EXIT;
    }

    void DataNode::syncInhibitDataPoints(TA_Base_Bus::EOutputInhibitState tagOutputInhibitState, 
                                     TA_Base_Bus::EInputInhibitState tagInputInhibitState,
                                     const std::string& sessionIdStr)
    {
        FUNCTION_ENTRY( "syncInhibitDataPoints()" );

		if(m_childProxies.isChildDataPointProxyCreated())
		{
		
			TA_Base_Core::ThreadGuard lock(m_childproxieslock);

			// Perform inhibits on DataPoints
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end(); 
				  ++itDataPoint )
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;  
				try
				{
					if ((*dp)->isWriteable())
					{
						(*dp)->setOutputInhibitStatus(tagOutputInhibitState, sessionIdStr);
					}
					else
					{
						(*dp)->setInputInhibitStatus(tagInputInhibitState, sessionIdStr);
					}
				}
				catch (...)
				{
					// Any error - need to roll back all the other points...
					ChildDataPointMapIt rollbackIt;
					// Increment to make the next loop inclusive
					itDataPoint++;

					for (rollbackIt = m_childProxies.getDataPoints().begin(); 
						 rollbackIt != itDataPoint; 
						 rollbackIt++)
					{
						DataPointProxySmartPtr* rollbackDp = (*rollbackIt).second;   
						try
						{
							if ((*dp)->isWriteable())
							{
								(*rollbackDp)->setOutputInhibitStatus(tagOutputInhibitState, sessionIdStr);
							}
							else
							{
								(*rollbackDp)->setInputInhibitStatus(tagInputInhibitState, sessionIdStr);
							}
						}
						catch (...)
						{
							// Not much we can do here
						}
					}   
                
					TA_THROW(TA_Base_Core::ScadaCommonException("Can not inhibit monitor datapoints - rolled back"));
				} 
			}
        }
		else
		{
			TA_THROW(TA_Base_Core::ScadaCommonException("the child monitor datapoint proxies are not config valid."));
		}

        FUNCTION_EXIT;
    }

    void DataNode::setOutputInhibitStatus(TA_Base_Bus::DataPointAgentInterface& dataPointNamedObject, 
                                          unsigned long entitykey,
                                          TA_Base_Bus::EOutputInhibitState currentInhibit,
                                          TA_Base_Bus::EOutputInhibitState newInhibit,
                                          std::string sessionId)
    {
		FUNCTION_ENTRY( "setOutputInhibitStatus()" );

        TA_Base_Bus::EOutputInhibitState fromdp;
		TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState ret;

		CORBA_CALL_RETURN( ret,
		                   dataPointNamedObject,
		                   getOutputInhibitState,
		                   ( entitykey ) );
		convertOutputInhibitState( ret, fromdp );

        if ( ( currentInhibit != newInhibit) || (currentInhibit != fromdp) )
        {
            try
            {
                TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState toset;
                convertOutputInhibitState(newInhibit, toset);
                CORBA_CALL( dataPointNamedObject, setOutputInhibitState, ( entitykey, sessionId.c_str(), toset ) );//limin
            }   
            catch (...)
            {
                std::string text = "Error: " + m_staticConfig.name + " Internal Error setting DataPoint input inhibit status";
                TA_THROW(TA_Base_Core::ScadaCommonException(text));
            }
        }
		FUNCTION_EXIT;
    }

    void DataNode::setInputInhibitStatus(TA_Base_Bus::DataPointAgentInterface& dataPointNamedObject,
                                         unsigned long entitykey,
                                         TA_Base_Bus::EInputInhibitState currentInhibit,
                                         TA_Base_Bus::EInputInhibitState newInhibit, 
                                         std::string sessionId)
    {
		FUNCTION_ENTRY( "setInputInhibitStatus()" );

        TA_Base_Bus::EInputInhibitState fromdp;

		TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState ret;
		CORBA_CALL_RETURN( ret,
		                   dataPointNamedObject,
		                   getInputInhibitState,
		                   ( entitykey ) );
		convertInputInhibitState(ret, fromdp);

        if ( (currentInhibit != newInhibit) || (currentInhibit != fromdp ) )
        {
            try
            {
                TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState toset;
                convertInputInhibitState(newInhibit, toset);
                CORBA_CALL( dataPointNamedObject, setInputInhibitState, ( entitykey, sessionId.c_str(),toset ) );//limin
            }   
            catch (...)
            {
                std::string text = "Error: " + m_staticConfig.name + " Internal Error setting DataPoint input inhibit status";
                TA_THROW(TA_Base_Core::ScadaCommonException(text));
            }
        }

		FUNCTION_EXIT;
    }


    bool DataNode::isActivated()
    {
		return (m_operationMode == TA_Base_Core::Control);
    }

    
    bool DataNode::areChildProxiesCreated()
    {
        return ( m_childProxies.getDataNodes().size() == m_staticConfig.childDataNodeEntities.size() &&
                 m_childProxies.getDataPoints().size() == m_staticConfig.childDataPointEntities.size() );
    }


    void DataNode::updateActivation()
    {
		SummaryRequest request(m_staticConfig.entityKey, SummaryRequest::UpdateActivation);
        m_summaryQ->queueItemUnique(request, false);
    }

    void DataNode::processUpdateActivation()
    {
        FUNCTION_ENTRY ("processUpdateActivation()");

		TA_THREADGUARD(m_childproxieslock);

		//first create parent proxy
		if (NULL == m_parentProxy && m_parentKey != m_scadaRootKey && m_parentKey != m_parentEntityKey)
		{
			m_parentProxy = new TA_Base_Bus::DataNodeProxySmartPtr();

			//get the parentEntity from the DataNodeAccessFactory to avoid the db access.
			TA_Base_Core::IEntityDataPtr parentEntityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(m_parentKey);

			if (parentEntityData.get() != NULL)
			{
				m_scadaProxyFactory.registerEntityData(parentEntityData,TA_Base_Core::DataNodeEntityData::getStaticType());
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Data node %lu can not find the parent key %lu in the DataNodeAccessFactory",m_staticConfig.entityKey,m_parentKey);
			}
			
			m_scadaProxyFactory.createDataNodeProxy(m_parentKey, *this, *m_parentProxy);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataNode [%lu] %s add proxy observer %u for parent DataNode [%lu] %s",
				m_staticConfig.entityKey, 
				m_staticConfig.name.c_str(),
				this,
				m_parentKey,
				(*m_parentProxy)->getEntityName().c_str() );			
		}

		//second create child proxy
		m_childProxies.checkChilidProxiesCreated(m_staticConfig, m_scadaProxyFactory, *this);
	
		if ( !m_childProxiesCreated )
		{
			// whether areChildProxiesCreated is successful or not, we set this flag true to trigger summary calculate
			m_childProxiesCreated = true;
			// here we must queue once to trigger recalculate, this is important
			// don't omit this summaryRequest when m_childProxiesCreated changed from false to true
			//SummaryRequest summaryRequest(m_staticConfig.entityKey, SummaryRequest::ReCalculateRequestFromChildren);
			//m_summaryQ->queueItemUnique( summaryRequest, false);
			//First time we only care about make a link for the data node and its children
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "processUpdateActivation, DataNode: [%lu] %s",m_staticConfig.entityKey,m_staticConfig.name.c_str());
			processRecalculateRequestFromChildren();
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "processRecalculateRequestFromChildren, DataNode: [%lu] %s",m_staticConfig.entityKey,m_staticConfig.name.c_str());
		}	
		
		if ( !areChildProxiesCreated() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "createProxies() - %s: Not all child proxies created, will retry later.",
				m_staticConfig.name.c_str() );

			updateActivation();
		}

        FUNCTION_EXIT;
    }

	void DataNode::processRecalculateRequestFromParent()
	{
		FUNCTION_ENTRY ("processRecalculateRequestFromParent()");

		bool calcFlags[E_DN_CALC_MAX_FP] = {false};
		int total = 0;
		{
			TA_Base_Core::ThreadGuard lock(m_calcFlagsFromParentLock);
			for (int i = E_DN_CALC_MIN_FP; i < E_DN_CALC_MAX_FP; ++i)
			{
				if ( m_calcFlagsFromParent[i] )
				{
					++total;
					calcFlags[i] = true;
					m_calcFlagsFromParent[i] =  false;
				}
				else
				{
					calcFlags[i] = false;
				}			
			}
		}

		if ( total > 0 )
		{
			for (int i = E_DN_CALC_MIN_FP; i < E_DN_CALC_MAX_FP; ++i)
			{
				if ( calcFlags[i] )
				{
					(this->*(s_reCalculateFuncsFP[i]))();
				}
			}

			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "processRecalculateRequestFromParent, key:%lu, total:%d", 
				m_dataNodeEntityAccess->getKey(), total);
		}
			
		FUNCTION_EXIT;
	}

	void DataNode::processRecalculateRequestFromChildren()
	{
		FUNCTION_ENTRY ("processRecalculateRequestFromChildren()");
		
		bool calcFlags[E_DN_CALC_MAX_FC] = {false};
		int total = 0;
		{
			TA_Base_Core::ThreadGuard lock(m_calcFlagsFromChildLock);
			for (int i = E_DN_CALC_MIN_FC; i < E_DN_CALC_MAX_FC; ++i)
			{
				if ( m_calcFlagsFromChild[i] )
				{
					++total;
					calcFlags[i]  =  true;
					m_calcFlagsFromChild[i] =  false;
				}
				else
				{
					calcFlags[i]  =  false;
				}							
			}
		}
		
		if ( total > 0 )
		{
			for (int i = E_DN_CALC_MIN_FC; i < E_DN_CALC_MAX_FC; ++i)
			{
				if ( calcFlags[i] )
				{
					(this->*(s_reCalculateFuncsFC[i]))();
				}
			}

			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "processRecalculateRequestFromChildren, key:%lu, total:%d", 
				m_dataNodeEntityAccess->getKey(), total);
		}

		FUNCTION_EXIT;
	}

    bool DataNode::registerForUpdateDetails ( TA_Base_Bus::IDataNodeObserverNotification * updateDetailsObserver )
    {
		FUNCTION_ENTRY ("registerForUpdateDetails()");

        TA_THREADGUARD( m_updateDetailsObserverlock );

        bool result = false;

        if ( 0 != updateDetailsObserver )
        {
            bool existed = false;

            std::vector < TA_Base_Bus::IDataNodeObserverNotification * >::iterator itr;
            for ( itr = m_listOfUpdateDetailsObservers.begin();
                  itr != m_listOfUpdateDetailsObservers.end();
                  itr++ )
            {
                if ( updateDetailsObserver == (*itr) )
                {
                    existed = true;
                    break;
                }
            }

            if ( false == existed )
            {
                m_listOfUpdateDetailsObservers.push_back ( updateDetailsObserver );
            }

            result = true;
        }

		FUNCTION_EXIT;
        return result;
    }

    bool DataNode::deregisterForUpdateDetails ( TA_Base_Bus::IDataNodeObserverNotification * updateDetailsObserver )
    {
		FUNCTION_ENTRY ("deregisterForUpdateDetails()");
  
        TA_THREADGUARD( m_updateDetailsObserverlock );

        bool result = false;

        if ( 0 != updateDetailsObserver )
        {
            bool existed = false;

            std::vector < TA_Base_Bus::IDataNodeObserverNotification * >::iterator itr;
            for ( itr = m_listOfUpdateDetailsObservers.begin();
                  itr != m_listOfUpdateDetailsObservers.end();
                  itr++ )
            {
                if ( updateDetailsObserver == (*itr) )
                {
                    existed = true;
                    break;
                }
            }

            if ( true == existed )
            {
                m_listOfUpdateDetailsObservers.erase ( itr );

                result = true;
            }
        }

		FUNCTION_EXIT;
        return result;
    }

    void DataNode::notifyRegisteredObservers ( boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails )
    {
		FUNCTION_ENTRY ("notifyRegisteredObservers()");

        TA_THREADGUARD( m_updateDetailsObserverlock );

        std::vector < TA_Base_Bus::IDataNodeObserverNotification * >::iterator itr;
        for ( itr = m_listOfUpdateDetailsObservers.begin();
              itr != m_listOfUpdateDetailsObservers.end();
              itr++ )
        {
            try
            {
                (*itr)->processDataNodeUpdate ( m_staticConfig.entityKey, updateDetails );
            }
            catch ( ... )
            {
            }
        }

		FUNCTION_EXIT;
    }


    unsigned long DataNode::getEntityKey() const
    {
        return m_staticConfig.entityKey;
    }

    const std::string& DataNode::getName() const
    {
        return m_staticConfig.name;
    }

    boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> DataNode::getCompleteStaticConfigurations()
    {
        boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> toReturn(new TA_Base_Bus::DataNodeStaticConfiguration);
        *toReturn = m_staticConfig;
        return toReturn;
    }

    boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> DataNode::getCompleteDynamicStates()
    {
        boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> toReturn(new TA_Base_Bus::DnCompleteDynamicSyncState);
        *toReturn = m_dynamicState;
        return toReturn;
    }

    bool DataNode::getInhibitMMS()
    {
        return m_dynamicState.inhibitMMSState;
    }
        
	BooleanTag DataNode::getCombinedInhibitMMS()
	{
		return m_dynamicState.combinedInhibitMMSState;
	}

    boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> DataNode::getDisplayState()
    {
		FUNCTION_ENTRY("getDisplayState");

        boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> dnstate(new TA_Base_Bus::DataNodeDisplayState);

        //entity key
        dnstate->entityKey = getEntityKey();
        //ack state
        dnstate->alarmAckState.quality  = m_dynamicState.ackSummary.quality;
        dnstate->alarmAckState.value  = m_dynamicState.ackSummary.value;
        //alarm state
        dnstate->alarmState.quality = m_dynamicState.alarmSummary.quality;
        dnstate->alarmState.value = m_dynamicState.alarmSummary.value;
        //quality
        dnstate->quality.quality = m_dynamicState.qualitySummary.quality;
        dnstate->quality.value = m_dynamicState.qualitySummary.value;
        
        std::ostringstream onitp;
		//
		// Override
		//
		{
			if (true == m_dynamicState.overrideSummary.value)
			{
				onitp << 'O';
			}
		}
		//
		// Notes
		//
		{
			bool notes = (0 < m_dynamicState.notes.size());
			
			if ( true == notes )
			{
				onitp << 'N';
			}
		}

		//
		// Inhibit
		//
		{
				
            bool inhibittag = false;
			if ( m_dynamicState.scanInhibitSummary.value )
			{
                inhibittag = true;
			}
            else if (m_dynamicState.alarmInhibitSummary.value) 
            {
                inhibittag = true;
            }
            else if (m_dynamicState.mmsAlarmInhibitSummary.value)
            {
                inhibittag = true;
            }
            else if (m_dynamicState.controlInhibitSummary.value)
            {
                inhibittag = true;
            }
            
			if ( true == inhibittag )
			{
				onitp << 'I';
			}

		}

		//
		// Tag
		//
		{
			if ( TA_Base_Bus::NotTagged != m_dynamicState.tagState.tagState )
			{
				onitp << 'T';
			
				if ( TA_Base_Bus::TaggedPtw == m_dynamicState.tagState.tagState )
				{
					onitp << 'P';
				}
			}
		}
        //ONITP
        dnstate->ONITP = onitp.str();

		FUNCTION_EXIT;
        return dnstate;
    }

    void DataNode::setOperationMode( bool inControlMode )
    {
        std::ostringstream fnName;
        fnName << "setOperationMode() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        changeOperationMode( inControlMode ? TA_Base_Core::Control : TA_Base_Core::Monitor );

        FUNCTION_EXIT;
    }

    void DataNode::changeDataNodeTag( const std::string& sessionId, 
                            TA_Base_Bus::ETagState tagState,
                            TA_Base_Bus::EOutputInhibitState tagOutputInhibitState,
                            TA_Base_Bus::EInputInhibitState tagInputInhibitState,
                            const std::string& ptwId, const std::string& comment)
    {
		FUNCTION_ENTRY( "changeDataNodeTag()" );

		checkOperationMode();

		if (tagState != TA_Base_Bus::TaggedPtw)
		{
			if ( !ptwId.empty())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected PTW ID", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Unexpected PTW ID");
			}
		}
		else
		{
			if( ptwId.empty() || comment.empty() )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Missing PTW ID and Comment for Tagged with PTW", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Missing PTW ID and Comment");
			}
		}
		
		if (tagState == TA_Base_Bus::NotTagged)
		{
			if (tagOutputInhibitState != TA_Base_Bus::NoOutputInhibit ||
				tagInputInhibitState != TA_Base_Bus::NoInputInhibit)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected Inhibit State", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Unexpected Inhibit State");
			}

			if (!comment.empty())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected PTW ID", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Unexpected Comment");
			}
		}

		// if the requested action is not authorised for the sessionID
		if ( (true != isActionPermitted ( sessionId, TA_Base_Bus::aca_DATA_NODE_TAG )) && 
			 (true != isActionPermitted( sessionId, TA_Base_Bus::aca_DATA_NODE_TAG_PTW)) )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Access denied",
                m_staticConfig.name.c_str() );
			FUNCTION_EXIT;

			throw TA_Base_Core::AccessDeniedException();
		}

		// Inhibit DataPoints first
		try
		{
			inhibitDataPoints(tagOutputInhibitState, tagInputInhibitState, sessionId);
		}
		catch (TA_Base_Core::ScadaCommonException& e)
		{
			throw TA_Base_Core::ScadaCommonException(e.what());
		}

		TA_Base_Bus::ETagState originalTagState = m_dynamicState.tagState.tagState;
		std::string originalPtwId = m_dynamicState.tagState.ptwId;
		std::string originalComment = m_dynamicState.tagState.comment;
		TA_Base_Bus::InhibitState originalInhibitState = m_dynamicState.tagState.tagInhibitState;
      
        //
		// Persist to database first
        //
        TA_ASSERT( m_persistence.get() != NULL, "ScadaPersistence has not been initialised" );
        if ( ! m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString(tagState) ) )
		{
			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
        }

        if ( ! m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                  tagOutputInhibitStateToString(tagOutputInhibitState) ) )
        {
			if ( ! m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) ) )
			{
				// Rollback Inhibits
				try
				{
					inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
				}
				catch (TA_Base_Core::ScadaCommonException&)
				{
				}
			}
			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}
			
		if ( ! m_persistence->writePersistedValue( DN_TAG_INPUT_INHIBIT, m_staticConfig.entityKey, 
                                                   tagInputInhibitStateToString(tagInputInhibitState) ) )
		{
            // Rollback Inhibits
            try
            {
                inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
            }
            catch (TA_Base_Core::ScadaCommonException&)
            {
            }

            m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) );
            m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagOutputInhibitStateToString(originalInhibitState.outputInhibit) );

			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}

		if ( ! m_persistence->writePersistedValue( DN_TAG_PTW, m_staticConfig.entityKey, ptwId ) )
		{
            // Rollback Inhibits
            try
            {
                inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
            }
            catch (TA_Base_Core::ScadaCommonException&)
            {
            }

            m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) );
            m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagOutputInhibitStateToString(originalInhibitState.outputInhibit) );
            m_persistence->writePersistedValue( DN_TAG_INPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagInputInhibitStateToString(originalInhibitState.inputInhibit) );

			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}
			
        if ( ! m_persistence->writePersistedValue( DN_PTW_COMMENTS, m_staticConfig.entityKey, comment ) )
		{
            // Rollback Inhibits
            try
            {
                inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
            }
            catch (TA_Base_Core::ScadaCommonException&)
            {
            }

            m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) );
            m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagOutputInhibitStateToString(originalInhibitState.outputInhibit) );
            m_persistence->writePersistedValue( DN_TAG_INPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagInputInhibitStateToString(originalInhibitState.inputInhibit) );
            m_persistence->writePersistedValue( DN_TAG_PTW, m_staticConfig.entityKey, originalPtwId );

			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}

		// Update local store
		m_dynamicState.tagState.tagState = tagState;
		m_dynamicState.tagState.tagInhibitState.outputInhibit = tagOutputInhibitState;
		m_dynamicState.tagState.tagInhibitState.inputInhibit = tagInputInhibitState;
		m_dynamicState.tagState.ptwId = ptwId;
		m_dynamicState.tagState.comment = comment;
        saveLastOperatorInfo(sessionId);	
		// Send update event
		submitUpdateEvent (TagStateUpdate);

		//
		// submit audit event
		//

		std::stringstream inhibitStateStr;
		inhibitStateStr << "With";
		switch ( m_dynamicState.tagState.tagInhibitState.inputInhibit )
		{
			case NoInputInhibit:
			{
				if ( TA_Base_Bus::NoOutputInhibit == m_dynamicState.tagState.tagInhibitState.outputInhibit )
				{
					inhibitStateStr << " No Inhibit";
				}
				
				break;
			}
			case InhibitScan:
			{
				inhibitStateStr << " Scan Inhibit";
				break;
			}
			case InhibitAlarm:
			{
				inhibitStateStr << " Alarm Inhibit";
				break;
			}
			case InhibitMmsAlarm:
			{
				inhibitStateStr << " MMS Inhibit";
				break;
			}
			default:
			{
				break;
			}
		}

        if ( TA_Base_Bus::NoOutputInhibit == m_dynamicState.tagState.tagInhibitState.outputInhibit )
		{
			inhibitStateStr << " Control Inhibit";
		}

		// construct the DescriptionParameters
		TA_Base_Core::DescriptionParameters descriptionParameters;
		descriptionParameters.clear();
		TA_Base_Core::NameValuePair pair0("Value", " ");
		descriptionParameters.push_back (&pair0);
		TA_Base_Core::NameValuePair pair1 ( "EquipmentDescription", m_staticConfig.description );
		descriptionParameters.push_back ( &pair1 );

		// Tag Added
		if ( ( TA_Base_Bus::Tagged != originalTagState ) &&
			 ( TA_Base_Bus::Tagged == m_dynamicState.tagState.tagState ) )
		{				
			TA_Base_Core::NameValuePair pair2 ( "InhibitState", inhibitStateStr.str() );				
			descriptionParameters.push_back ( &pair2 );

			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodeTagAdd );
		}

		// Tag With PTW Added
		else if ( ( TA_Base_Bus::TaggedPtw != originalTagState ) &&
				  ( TA_Base_Bus::TaggedPtw == m_dynamicState.tagState.tagState ) )
		{				
			TA_Base_Core::NameValuePair pair2 ( "InhibitState", inhibitStateStr.str() );
			descriptionParameters.push_back ( &pair2 );

			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodePTWAdd );
		}

		// Tag Removed
		else if ( ( TA_Base_Bus::Tagged == originalTagState ) &&
				  ( TA_Base_Bus::Tagged != m_dynamicState.tagState.tagState ) )
		{
			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodeTagRemove );
		}

		// Tag With PTW Removed
		else if ( ( TA_Base_Bus::TaggedPtw == originalTagState ) &&
				  ( TA_Base_Bus::TaggedPtw != m_dynamicState.tagState.tagState ) )
		{				
			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodePTWRemove );
		}

		FUNCTION_EXIT;
    }


	void DataNode::synTagState( TagState inTagState, const std::string& sessionId)
    {
		FUNCTION_ENTRY( "synTagState()" );

		if (inTagState.tagState != TA_Base_Bus::TaggedPtw)
		{
			if ( !inTagState.ptwId.empty())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected PTW ID", m_staticConfig.name.c_str());
				return;
			}
		}
		else
		{
			if( inTagState.ptwId.empty() || inTagState.comment.empty() )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Missing PTW ID and Comment for Tagged with PTW", m_staticConfig.name.c_str());
				return;
			}
		}
		
		if (inTagState.tagState == TA_Base_Bus::NotTagged)
		{
			if (inTagState.tagInhibitState.outputInhibit != TA_Base_Bus::NoOutputInhibit ||
				inTagState.tagInhibitState.inputInhibit != TA_Base_Bus::NoInputInhibit)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected Inhibit State", m_staticConfig.name.c_str());
				return;
			}

			if (!inTagState.comment.empty())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected PTW ID", m_staticConfig.name.c_str());
				return;
			}
		}

		// Update local store
		try
		{
	        syncInhibitDataPoints(inTagState.tagInhibitState.outputInhibit, inTagState.tagInhibitState.inputInhibit, sessionId);
			m_dynamicState.tagState = inTagState;
			saveLastOperatorInfo(sessionId);			
		}
		catch (TA_Base_Core::ScadaCommonException& e)
		{
			throw TA_Base_Core::ScadaCommonException(e.what());
		}
		
		FUNCTION_EXIT;
    }

    void DataNode::setNotes(const std::string& sessionId, const std::string& notes)
    {
		FUNCTION_ENTRY( "setNotes()" );

		checkOperationMode();

		// if the requested action is authorised for the sessionID
		if ( false == isActionPermitted ( sessionId, TA_Base_Bus::aca_DATA_NODE_NOTES ) )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setDataNodeNotes() - %s - Access denied", m_staticConfig.name.c_str());
			FUNCTION_EXIT;

			throw TA_Base_Core::AccessDeniedException();
		}

		{
			try
			{
				// persist to entity first
                m_persistence->writePersistedValue( DN_NOTES, m_staticConfig.entityKey, notes );

				// Update local store for notes
				m_dynamicState.notes = notes;
			}
			catch (...)
			{
				throw TA_Base_Core::DatabaseErrorException("setDataNodeNotes: Can not persist to Database");
			}
		
			// Send update event
			submitUpdateEvent (NotesStateUpdate);

			//
			// submit audit message
			//

			// construct the DescriptionParameters
			TA_Base_Core::DescriptionParameters descriptionParameters;
			descriptionParameters.clear();
			TA_Base_Core::NameValuePair pair0("Value", " ");
			descriptionParameters.push_back (&pair0);
			TA_Base_Core::NameValuePair pair1 ( "EquipmentDescription", m_staticConfig.description );
			descriptionParameters.push_back ( &pair1 );

			submitAuditEvent ( sessionId, descriptionParameters, ( false == notes.empty() ? TA_Base_Core::DataNodeAudit::DataNodeNotesAdd : TA_Base_Core::DataNodeAudit::DataNodeNotesRemove ) );

		}

		FUNCTION_EXIT;
    }

	void DataNode::synNotesUpdate( const std::string	& notes)
	{
		m_dynamicState.notes = notes;
	}

    void DataNode::setInhibitMMS( const std::string& sessionID, bool inhibitMms )
    {
		FUNCTION_ENTRY( "DataNode::setInhibitMMS" );

		checkOperationMode();

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, 
			"setInhibitMMS()-entitykey:%d-setInhibitMMS:%d-currentInhibitMMS:%d-combinedMMSvalue:%d-combinedMMSQuality:%d", 
			m_staticConfig.entityKey, inhibitMms,
			m_dynamicState.inhibitMMSState,
			m_dynamicState.combinedInhibitMMSState.value,
			m_dynamicState.combinedInhibitMMSState.quality);

		if ( m_dynamicState.inhibitMMSState != inhibitMms &&
			!(inhibitMms && m_dynamicState.combinedInhibitMMSState.value &&
			  (m_dynamicState.combinedInhibitMMSState.quality == QUALITY_GOOD_NO_SPECIFIC_REASON)))
		{
			if ( true == isActionPermitted ( sessionID, ( true == inhibitMms ? TA_Base_Bus::aca_DATA_NODE_MMS_INHIBIT : TA_Base_Bus::aca_DATA_NODE_MMS_UNINHIBIT ) ) )
			{
				// persist new state
				TA_ASSERT( m_persistence.get() != NULL, "ScadaPersistence is not initialised" );
				m_persistence->writePersistedValue( DN_MMS_INHIBIT_STATE, m_staticConfig.entityKey, inhibitMms );

				// update local MMS inhibit state
				m_dynamicState.inhibitMMSState = inhibitMms;
				saveLastOperatorInfo(sessionID);

				//notify this state to its observers
				submitUpdateEvent ( InhibitStateUpdate );

				recalculateCombinedInhibitMMS();

				// construct the DescriptionParameters
				TA_Base_Core::DescriptionParameters descriptionParameters;
				descriptionParameters.clear();
				TA_Base_Core::NameValuePair pair0("Value", " ");
				descriptionParameters.push_back (&pair0);
				TA_Base_Core::NameValuePair pair1 ( "EquipmentDescription", m_staticConfig.description );
				descriptionParameters.push_back ( &pair1 );

				submitAuditEvent ( sessionID, descriptionParameters, ( inhibitMms ? TA_Base_Core::DataNodeAudit::DataNodeInhibitMMS : TA_Base_Core::DataNodeAudit::DataNodeUnInhibitMMS ) );
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setInhibitMMS() - %s - Access denied", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::AccessDeniedException();
			}	
		}

		FUNCTION_EXIT;
    }

	void DataNode::synInhibitMMSUpdate(bool inputInhibitMMSState, const std::string & sessionId)
	{
		// update local MMS inhibit state
		if ( m_dynamicState.inhibitMMSState != inputInhibitMMSState &&
			!(inputInhibitMMSState && m_dynamicState.inhibitMMSState &&
			  (m_dynamicState.combinedInhibitMMSState.quality == QUALITY_GOOD_NO_SPECIFIC_REASON)))
		{
			m_dynamicState.inhibitMMSState = inputInhibitMMSState;
			saveLastOperatorInfo(sessionId);
		}
	}

	void DataNode::saveLastOperatorInfo ( const std::string & sessionID )
	{
        FUNCTION_ENTRY("saveLastOperatorInfo");

		struct timeb currentSystemTime;
		ftime ( &currentSystemTime );
		m_dynamicState.lastOperator.sessionID = sessionID;
		m_dynamicState.lastOperator.timeStamp.time = currentSystemTime.time;
		m_dynamicState.lastOperator.timeStamp.millitm = currentSystemTime.millitm;

        FUNCTION_EXIT;
	}

    void DataNode::clearSummaries()
    {
		FUNCTION_ENTRY("clearSummaries()");

        m_dynamicState.qualitySummary.value = QUALITY_BAD;
        m_dynamicState.qualitySummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.alarmSummary.value = QUALITY_BAD;
        m_dynamicState.alarmSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.ackSummary.value = QUALITY_BAD;
        m_dynamicState.ackSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.scanInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.scanInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.alarmInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.alarmInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.mmsAlarmInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.mmsAlarmInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.controlInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.controlInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.overrideSummary.value = QUALITY_BAD;
        m_dynamicState.overrideSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.controlPendingSummary.value = QUALITY_BAD;
        m_dynamicState.controlPendingSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

		FUNCTION_EXIT;
    }

    
    void DataNode::updateSynState(const TA_Base_Bus::DnSyncUpdate& toupdate)
    {
        FUNCTION_ENTRY("updateSynState()");
        //only the monitor mode datanode required to syn the control mode
        if ( TA_Base_Core::Control == m_operationMode )
        {
            throw( TA_Base_Core::OperationModeException() );
        }
        boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> dnupdate(new DnSyncUpdate);
		dnupdate->updateType = toupdate.updateType;
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "update [%d] for entity %u", toupdate.updateType, m_staticConfig.entityKey);
        switch(toupdate.updateType) 
        {		
        case DnCompleteSyncState:
            m_dynamicState = toupdate.completeUpdate;
			dnupdate->completeUpdate = toupdate.completeUpdate;
			//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "update [%d] for entity %d", toupdate.updateType, m_staticConfig.entityKey);
            break;
        case DnTagStateUpdate:
            synTagState(toupdate.tagState, toupdate.lastOperator.sessionID);
            dnupdate->tagState.comment = m_dynamicState.tagState.comment;
            dnupdate->tagState.ptwId = m_dynamicState.tagState.ptwId;
            dnupdate->tagState.tagInhibitState = m_dynamicState.tagState.tagInhibitState;
            dnupdate->tagState.tagState = m_dynamicState.tagState.tagState;
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "update tag as [%d] for entity %u", m_dynamicState.tagState.tagState, m_staticConfig.entityKey);
			break;
        case DnNotesUpdate:
            synNotesUpdate(toupdate.notes);
            dnupdate->notes = m_dynamicState.notes;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "update notes as [%s] for entity %u", m_dynamicState.notes.c_str(), m_staticConfig.entityKey);
			break;
        case DnInhibitMMSUpdate:
            synInhibitMMSUpdate(toupdate.inhibitMMSState, toupdate.lastOperator.sessionID);
			dnupdate->inhibitMMSState = m_dynamicState.inhibitMMSState;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "update inhibitMMSState as [%d] for entity %u", m_dynamicState.inhibitMMSState, m_staticConfig.entityKey);
            break;
        case DnLastOperatorUpdate:
			{
				time_t b4updatetimestamp = m_dynamicState.lastOperator.timeStamp.time;
				std::string b4updatesessionid = m_dynamicState.lastOperator.sessionID;

				if ( ( b4updatetimestamp != toupdate.lastOperator.timeStamp.time ) ||
					 ( b4updatesessionid.compare(toupdate.lastOperator.sessionID) != 0 ) )
				{
					m_dynamicState.lastOperator.sessionID = toupdate.lastOperator.sessionID;
					dnupdate->lastOperator.sessionID = m_dynamicState.lastOperator.sessionID;
					m_dynamicState.lastOperator.timeStamp.time = toupdate.lastOperator.timeStamp.time;
					dnupdate->lastOperator.timeStamp.time = toupdate.lastOperator.timeStamp.time;
					m_dynamicState.lastOperator.timeStamp.millitm = toupdate.lastOperator.timeStamp.millitm;
					dnupdate->lastOperator.timeStamp.millitm = toupdate.lastOperator.timeStamp.millitm;
				}
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New m_lastOperatorInfo (%s,%u) for entity %u", m_dynamicState.lastOperator.sessionID.c_str()
											, m_dynamicState.lastOperator.timeStamp.time, m_staticConfig.entityKey);
					
			}
            break;
        default:
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Not handling updatetype[%d] in Monitor mode datanode [%u]", toupdate.updateType, getEntityKey() );
			break;
			FUNCTION_EXIT;
			return;
            //break;
        }
		// notify all registered observers of the latest changes
        notifyRegisteredObservers ( dnupdate );

        FUNCTION_EXIT;
    }
   

	unsigned long DataNode::getScadaRootKey()
	{
		FUNCTION_ENTRY( "getScadaRootKey()" );

		if (ULONG_MAX == m_scadaRootKey)
		{
			TA_THREADGUARD(m_scadaRootKeyLock);

			if (ULONG_MAX == m_scadaRootKey)
			{
				std::string scadaRootType = TA_Base_Core::ScadaRootEntityData::getStaticType();
				TA_Base_Core::IEntityDataList dataList;
				try
				{
					dataList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(scadaRootType, false, false, false);
					if (0 < dataList.size())
					{
						TA_Base_Core::ScadaRootEntityData* pData = dynamic_cast<TA_Base_Core::ScadaRootEntityData*>(dataList[0]);
						if (pData != NULL)
						{
							m_scadaRootKey = pData->getKey();
						}
					}
				}
				catch(...)
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "get scada root entity failed!");
				}

				TA_Base_Core::IEntityDataList::iterator iter = dataList.begin();
				for (; iter != dataList.end(); ++iter)
				{
					delete *iter;
				}

				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "get scada root key:%u", m_scadaRootKey);
			}
		}

		FUNCTION_EXIT;
		return m_scadaRootKey;
	}

	void DataNode::initCombinedState()
	{
		FUNCTION_ENTRY( "initCombinedState()" );

		m_parentKey = m_dataNodeEntityAccess->getParent();

		if (m_parentKey == DataNode::m_scadaRootKey ||
			m_parentKey == DataNode::m_parentEntityKey)
		{
			m_dynamicState.combinedInhibitMMSState.value = m_dynamicState.inhibitMMSState;
			m_dynamicState.combinedInhibitMMSState.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
		}
		else if (m_dynamicState.inhibitMMSState)
		{
			m_dynamicState.combinedInhibitMMSState.value = true;
			m_dynamicState.combinedInhibitMMSState.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
		}
		else
		{
			//this situation need know parent's inhibitMMSState, proxy will notify later
			m_dynamicState.combinedInhibitMMSState.value = false;
			m_dynamicState.combinedInhibitMMSState.quality = QUALITY_BAD_NO_SPECIFIC_REASON;
		}

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, 
			"datanode:%d initCombinedState,inhibitMMSState:%d,CombineMMSValue:%d,CombineMMSQuality:%d", 
			m_dataNodeEntityAccess->getKey(),
			m_dynamicState.inhibitMMSState,
			m_dynamicState.combinedInhibitMMSState.value, 
			m_dynamicState.combinedInhibitMMSState.quality);

		FUNCTION_EXIT;
	}

	void DataNode::submitCombinedUpdateEvent(ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY( "submitCombinedUpdateEvent()" );

		if (m_operationMode != TA_Base_Core::Control)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s - in monitor mode", m_staticConfig.name.c_str() );

			return;
		}

		boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> toupdate(new TA_Base_Bus::DnSyncUpdate);
		std::stringstream ss;

		switch (updateType)
		{
		case CombinedInhibitMMSUpdate:
			{
				toupdate->updateType = DnCombinedInhibitMMSUpdate;
				toupdate->combinedInhibitMMSState = m_dynamicState.combinedInhibitMMSState;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
					"submitCombinedUpdateEvent() - %s -combined MMS inhibit state updated",
					m_staticConfig.name.c_str() );
			}
			break;
		default:
			TA_ASSERT(true, "Bad Update Type");
			break;
		}

		notifyRegisteredObservers ( toupdate );
		
		FUNCTION_EXIT;
	}

	void DataNode::recalculateCombinedInhibitMMS()
	{
		FUNCTION_ENTRY( "recalculateCombinedInhibitMMS" );

		bool isException = false;
		bool isNotify = false;
		BooleanTag parentCombinedMMS;
		BooleanTag preCombineMMS = m_dynamicState.combinedInhibitMMSState;
		if (m_dynamicState.inhibitMMSState)
		{
			m_dynamicState.combinedInhibitMMSState.value = true;
			m_dynamicState.combinedInhibitMMSState.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
		}
		else if(m_parentKey == m_scadaRootKey || m_parentKey == m_parentEntityKey)
		{
			m_dynamicState.combinedInhibitMMSState.value = false;
			m_dynamicState.combinedInhibitMMSState.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
		}
		else
		{
			try
			{
				parentCombinedMMS = (*m_parentProxy)->getCombinedInhibitMms();

				if (parentCombinedMMS.quality == QUALITY_GOOD_NO_SPECIFIC_REASON)
				{
					m_dynamicState.combinedInhibitMMSState = parentCombinedMMS;
				}
			}
			catch(...)
			{
				isException = true;

				m_dynamicState.combinedInhibitMMSState.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
			}
		}

		if (preCombineMMS.quality != m_dynamicState.combinedInhibitMMSState.quality ||
			preCombineMMS.value != m_dynamicState.combinedInhibitMMSState.value)
		{
			isNotify = true;
			submitCombinedUpdateEvent(CombinedInhibitMMSUpdate);
	
			// we need recalculate MmsAlarmInhibitSummary now that CombinedInhibitMMSUpdate
			bool shouldRequest = false;

			TA_Base_Core::ThreadGuard lock(m_calcFlagsFromChildLock);
			if ( !m_calcFlagsFromChild[E_DN_CALC_MMS_I_FC] )
			{
				m_calcFlagsFromChild[E_DN_CALC_MMS_I_FC] = true;
				shouldRequest = true;
			}
			
			if ( shouldRequest )
			{
				SummaryRequest request(m_staticConfig.entityKey, SummaryRequest::ReCalculateRequestFromChildren);			
				m_summaryQ->queueItemUnique( request, false);
			}
		}

		if (isException)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
				"%d process parent update,noitify:%d-MMSState:%d-Combinedvalue:%d-CombineQuality:%d", 
				m_staticConfig.entityKey, isNotify, 
				m_dynamicState.inhibitMMSState,
				m_dynamicState.combinedInhibitMMSState.value, 
				m_dynamicState.combinedInhibitMMSState.quality);
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
				"%d process parent update,noitify:%d-parentCombineValue:%d-ParentCombineQuality:%d-MMSState:%d-Combinedvalue:%d-CombineQuality:%d", 
				m_staticConfig.entityKey, isNotify, 
				parentCombinedMMS.value,parentCombinedMMS.quality,
				m_dynamicState.inhibitMMSState,
				m_dynamicState.combinedInhibitMMSState.value, 
				m_dynamicState.combinedInhibitMMSState.quality);
		}

		FUNCTION_EXIT;
	}

	void DataNode::initCalcFlags()
	{
		FUNCTION_ENTRY( "initCalcFlags()" );

		// initialize calculate flags request from children
		{
			TA_Base_Core::ThreadGuard lock(m_calcFlagsFromChildLock);
			for (int i = E_DN_CALC_MIN_FC; i < E_DN_CALC_MAX_FC; ++i)
			{
				m_calcFlagsFromChild[i] = false;
			}
		}
		
		// initialize calculate flags request from parent
		{
			TA_Base_Core::ThreadGuard lock(m_calcFlagsFromParentLock);
			for (int i = E_DN_CALC_MIN_FP; i < E_DN_CALC_MAX_FP; ++i)
			{
				m_calcFlagsFromParent[i] = false;
			}
		}
		
		FUNCTION_EXIT;
	}

	unsigned long DataNode::getParentKey()
	{
		return m_dataNodeEntityAccess->getParent();
	}

	TA_Base_Core::DataNodeEntityData::ESynthesisType DataNode::getSynthesisTypeEnum()
	{
		return m_dataNodeEntityAccess->getSynthesisTypeEnum();
	}
}
