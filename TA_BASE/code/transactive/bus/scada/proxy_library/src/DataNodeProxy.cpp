/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeProxy.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modified by:  $Author: haijun.li $
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * 
  * This class provides access to a piece of equipment and its associated data points.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <string>
#include <limits>

#include "bus/scada/proxy_library/src/DataNodeProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataNodeLocalAccess.h"
#include "bus/scada/proxy_library/src/DataNodeRemoteAccess.h"
#include "bus/scada/datanode_library/src/DataNode.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	DataNodeProxy::DataNodeProxy(TA_Base_Core::DataNodeEntityDataPtr dnEntityData,
								 DataNodeSyncManager& dataNodeManager)
			   :
                m_entityData(dnEntityData),
                m_objAccess(NULL),
                m_syncManager(dataNodeManager),
				m_isLocal(true)
	{
		FUNCTION_ENTRY ("Constructor");

        m_proxyData.isConfigValid = false;
        m_proxyData.isSynchronized = false;
		m_isLocal = m_syncManager.isLocal(getEntityKey());
        loadConfiguration();
		FUNCTION_EXIT;
	}

	DataNodeProxy::~DataNodeProxy()
	{
		FUNCTION_ENTRY("Destructor");

		TA_THREADGUARD(m_updateLock); //TD14190, to ensure other functions finished completely.
		
		try
		{
            if (m_objAccess != NULL)
            {
                delete m_objAccess;
				m_objAccess = NULL;
            }
		}
		catch(TA_Base_Core::ScadaProxyException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"caught unhandled exception in destructor of proxy %s", ex.what());
		}
		FUNCTION_EXIT;
	}

	void DataNodeProxy::updateTagState(const TA_Base_Bus::TagState& tagState)
	{	
		TA_THREADGUARD(m_updateLock);

        m_proxyData.dynamicData.tagState = tagState;
	}

	
	void DataNodeProxy::updateAlarmSummaryState(const TA_Base_Bus::BooleanTag& alarmSummary)
	{
		TA_THREADGUARD(m_updateLock);

        m_proxyData.dynamicData.alarmSummary = alarmSummary;
	}

	void DataNodeProxy::updateAlarmAckSummaryState(const TA_Base_Bus::EnumTag& alarmAckSummary)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.ackSummary = alarmAckSummary;
	}

	void DataNodeProxy::updateScanInhibitSummaryState(const TA_Base_Bus::BooleanTag& scanInhibitSummary)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.scanInhibitSummary = scanInhibitSummary;
	}

	void DataNodeProxy::updateAlarmInhibitSummaryState(const TA_Base_Bus::BooleanTag& alarmInhibitSummary)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.alarmInhibitSummary = alarmInhibitSummary;
	}

	void DataNodeProxy::updateControlInhibitSummaryState(const TA_Base_Bus::BooleanTag& controlInhibitSummary)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.controlInhibitSummary = controlInhibitSummary;
	}

	void DataNodeProxy::updateMmsAlarmInhibitSummaryState(const TA_Base_Bus::BooleanTag& mmsAlarmInhibitSummary)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.mmsAlarmInhibitSummary = mmsAlarmInhibitSummary;
	}

	void DataNodeProxy::updateOverrideSummaryState(const TA_Base_Bus::BooleanTag& overrideSummary)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.overrideSummary = overrideSummary;
	}

	void DataNodeProxy::updateQualitySummaryState(const TA_Base_Bus::EnumTag& qualitySummary)
	{
		TA_THREADGUARD(m_updateLock);
		
		m_proxyData.dynamicData.qualitySummary = qualitySummary;
	}

	void DataNodeProxy::updateControlPendingSummaryState(const TA_Base_Bus::BooleanTag& controlPendingSummary)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.controlPendingSummary = controlPendingSummary;
	}

	void DataNodeProxy::updateNotesState(const std::string& notes)
	{
		TA_THREADGUARD(m_updateLock);
		
		m_proxyData.dynamicData.notes = notes;
	}

	void DataNodeProxy::updateInhibitState(const bool inhibitState)
	{
		TA_THREADGUARD(m_updateLock);
	
		m_proxyData.dynamicData.inhibitMMSState = inhibitState;
	}

    void DataNodeProxy::updateLastOperator(const TA_Base_Bus::LastOperatorInfo& lastOperator)
    {
        TA_THREADGUARD(m_updateLock);
        
        m_proxyData.dynamicData.lastOperator = lastOperator;
    }
    
	void DataNodeProxy::updateCombinedInhibitMMSState(const TA_Base_Bus::BooleanTag& combinedMMS)
	{
		TA_THREADGUARD(m_updateLock);

		m_proxyData.dynamicData.combinedInhibitMMSState = combinedMMS;
	}

    void DataNodeProxy::loadConfiguration()
    {
        TA_THREADGUARD(m_updateLock);
        
        if (m_proxyData.isConfigValid)
        {
            // configuration info already loaded, do nothing
            return;
        }
        
        try
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugTrace, "Loading configuration for entity [%u]", getEntityKey());
            boost :: shared_ptr<DataNodeStaticConfiguration> configuration = getObjAccess()->getConfiguration();
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Finished loading configuration for entity [%u]", getEntityKey());

            m_proxyData.configData = *(configuration.get());

        //set isConfigValid indicator until get complete update or fail to connect to agent
			m_proxyData.isConfigValid = true;
        }
        // dont kill the bulk polling thread
        catch( const TA_Base_Core::TransactiveException & te)
        {
            std::ostringstream desc;
            desc << "[DataNodeProxy::loadConfiguration] " << te.what();
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", desc.str().c_str() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "[DataNodeProxy::loadConfiguration]" );
        }
    }

	void DataNodeProxy::updateState(const TA_Base_Bus::DnCompleteDynamicSyncState& state)
	{
		TA_THREADGUARD(m_updateLock);
					
		updateNotesState(state.notes);
		updateTagState(state.tagState);
		updateInhibitState(state.inhibitMMSState);
		updateAlarmSummaryState(state.alarmSummary);
		updateAlarmAckSummaryState(state.ackSummary);
		updateScanInhibitSummaryState(state.scanInhibitSummary);
		updateAlarmInhibitSummaryState(state.alarmInhibitSummary);
		updateMmsAlarmInhibitSummaryState(state.mmsAlarmInhibitSummary);
		updateControlInhibitSummaryState(state.controlInhibitSummary);
		updateOverrideSummaryState(state.overrideSummary);
		updateQualitySummaryState(state.qualitySummary);
		updateCombinedInhibitMMSState(state.combinedInhibitMMSState);
		updateControlPendingSummaryState(state.controlPendingSummary);
        updateLastOperator(state.lastOperator);

        m_proxyData.isConfigValid = true;
        m_proxyData.isSynchronized = true;
    }

	void DataNodeProxy::updateAgentNotAvailableStatus()
	{
		TA_THREADGUARD(m_updateLock);
			
		m_proxyData.dynamicData.qualitySummary.value = QUALITY_BAD;
		m_proxyData.dynamicData.qualitySummary.quality = QUALITY_BAD_LAST_KNOWN_VALUE;

        m_proxyData.isConfigValid = true;
        m_proxyData.isSynchronized = false;
	}

	bool DataNodeProxy::getPScadaFlag() const
	{
		return m_proxyData.configData.pscadaFlag;
	}
    
    IDataNodeAccess* DataNodeProxy::getObjAccess()
    {
        if (m_objAccess == NULL)
        {
            if (m_syncManager.isLocal(getEntityKey()))
            {
                TA_Base_Bus::IEntity* localEntity = m_syncManager.getLocalEntity(getEntityKey());
                TA_Base_Bus::DataNode* localDataNode = dynamic_cast<TA_Base_Bus::DataNode*>(localEntity);
                
                TA_ASSERT(localDataNode != NULL, "non-local datanode was added into local datanode synchroniser");
                
                m_objAccess = new DataNodeLocalAccess(localDataNode);
            }
            else
            {
                m_objAccess = new DataNodeRemoteAccess(m_entityData);
            }
        }
        
        return m_objAccess;
    }
}
