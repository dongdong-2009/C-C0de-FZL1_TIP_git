/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeProxy_Read.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
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

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{

	bool DataNodeProxy::isConfigValid() const
	{
		return m_proxyData.isConfigValid;
	}


	bool DataNodeProxy::isTagged() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isTagged() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return (m_proxyData.dynamicData.tagState.tagState != NotTagged);
	}

			
	ETagState DataNodeProxy::getTaggingState() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getTaggingState() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.tagState.tagState;
	}

	InhibitState DataNodeProxy::getTagInhibitState() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getTagInhibitState() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.tagState.tagInhibitState;
	}

	std::string	DataNodeProxy::getPtwId() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getPtwId() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);

        return m_proxyData.dynamicData.tagState.ptwId;
	}

	std::string	DataNodeProxy::getTagComment() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getTagComment() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);

        return m_proxyData.dynamicData.tagState.comment;
	}

	TA_Base_Bus::BooleanTag DataNodeProxy::getAlarmSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getAlarmSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.alarmSummary;
	}

	

	TA_Base_Bus::EnumTag DataNodeProxy::getAlarmAckStateSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getAlarmAckStateSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.ackSummary;
	}

	TA_Base_Bus::BooleanTag DataNodeProxy::getControlInhibitSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getControlInhibitSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.controlInhibitSummary;
	}

	unsigned long DataNodeProxy::getLastStatusChangeTimestamp() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getLastStatusChangeTimestamp() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.lastOperator.timeStamp.time;
	}


	std::string DataNodeProxy::getLastStatusChangeSessionId() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getLastStatusChangeSessionId() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.lastOperator.sessionID;
	}


	TA_Base_Bus::BooleanTag DataNodeProxy::getAlarmInhibitSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getAlarmInhibitSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.alarmInhibitSummary;
	}

	TA_Base_Bus::BooleanTag DataNodeProxy::getScanInhibitSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getScanInhibitSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.scanInhibitSummary;
	}

	TA_Base_Bus::BooleanTag DataNodeProxy::getMmsAlarmInhibitSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getMmsAlarmInhibitSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.mmsAlarmInhibitSummary;
	}

	bool DataNodeProxy::getInhibitMms() const
	{
		if (!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Summary not available for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getInhibitMms() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return m_proxyData.dynamicData.inhibitMMSState;
	}

	TA_Base_Bus::BooleanTag DataNodeProxy::getCombinedInhibitMms() const
	{
		if (!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Summary not available for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getCombinedInhibitMms() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.combinedInhibitMMSState;
	}

	// Accessor for Override Summary
	TA_Base_Bus::BooleanTag DataNodeProxy::getOverrideSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getOverrideSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.overrideSummary;
	}

	// Accessor for Quality Summary
	TA_Base_Bus::EnumTag DataNodeProxy::getQualitySummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getQualitySummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.qualitySummary;
	}
			
	// Accessor for Control Pending Summary
	TA_Base_Bus::BooleanTag DataNodeProxy::getControlPendingSummary() const
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getControlPendingSummary() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.controlPendingSummary;
	}

	std::string DataNodeProxy::getNotes() const
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Notes not available for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getNotes() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return m_proxyData.dynamicData.notes;
	}

	unsigned long DataNodeProxy::getEntityKey() const
	{
		return m_entityData->getKey();
	}

    std::string DataNodeProxy::getAgentName() const
	{
		return m_entityData->getAgentName();
	}

    std::string DataNodeProxy::getEntityName() const
    {
        return m_entityData->getName();
    }
    
	const std::string& DataNodeProxy::getDescription() const
	{
        //because entitydata->getDescription return a object, so can't return it as a reference directly.
        if(!m_proxyData.isConfigValid)
        {
            TA_THREADGUARD(m_updateLock);
            const_cast<DataNodeProxy*>(this)->m_proxyData.configData.description = m_entityData->getDescription();
        }
        
        return m_proxyData.configData.description;
	}
    
    unsigned long DataNodeProxy::getSubSystemKey() const
    {
        return m_entityData->getSubsystem();
    }
    
    unsigned long DataNodeProxy::getPhysicalSubSystemKey() const
    {
        return m_entityData->getPhysicalSubsystem();
    }
    
    unsigned long DataNodeProxy::getParentKey() const
    {
        return m_entityData->getParent();
    }
    
    unsigned long DataNodeProxy::getLocationKey() const
    {
        return m_entityData->getLocation();
    }
    
	const std::string& DataNodeProxy::getLabel() const
	{
		return m_entityData->getLabel();
	}
	
	const std::string& DataNodeProxy::getShortLabel() const
	{
		return m_entityData->getShortLabel();
	}

	const std::string& DataNodeProxy::getAssetName() const
	{
        return m_entityData->getAssetName();
	}

    TA_Base_Core::SharedIEntityDataList DataNodeProxy::getChildDataPointEntities() const
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getChildDataPointEntities() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		if (m_isLocal)
		{
			return m_proxyData.configData.childDataPointEntities;
		}
		else
		{
			return m_entityData->getChildDataPoints();
		}
	}

    TA_Base_Core::SharedIEntityDataList DataNodeProxy::getChildDataNodeEntities() const
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getChildDataNodeEntities() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		if ( m_isLocal )
		{
			return m_proxyData.configData.childDataNodeEntities;
		}
		else
		{
			return m_entityData->getChildDataNodes();
		}
	}

	bool DataNodeProxy::isEquipmentNode() const
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataNode " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isEquipmentNode() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        bool isEquipment;
        
		if (m_proxyData.configData.childDataPointEntities.size() > 0 &&
            "HIERARCHY" == m_entityData->getSynthesisType())
        {
            isEquipment = true;
        }
		else
        {
            isEquipment = false;
        }

		return isEquipment;
	}

}
