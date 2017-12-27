/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  DarrenS
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * The DataNodeAttributes class maintains a list of attributes currently requested by the OPC Client
  * for a particular DataNode
  *
  */
#include "ace/OS.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include "app/scada/OpcBridgeLib/src/DataNodeAttributes.h"
#include "app/scada/OpcBridgeLib/attributes/DnAttributeNames.h"
#include "app/scada/OpcBridgeLib/attributes/DnShortLabel.h"
#include "app/scada/OpcBridgeLib/attributes/DnLabel.h"
#include "app/scada/OpcBridgeLib/attributes/DnQualitySummary.h"
#include "app/scada/OpcBridgeLib/attributes/DnAlarmStateSummary.h"
#include "app/scada/OpcBridgeLib/attributes/DnAckStateSummary.h"
#include "app/scada/OpcBridgeLib/attributes/DnAssetName.h"
#include "app/scada/OpcBridgeLib/attributes/DnOnitp.h"
#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    DataNodeAttributes::DataNodeAttributes(TA_Base_Core::IEntityData* entityData)
    :
    AbstractDataObjectAttributes(entityData),
	m_configAvaliable(false)
    {
        TA_Base_Core::DataNodeEntityData* dnEntity = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData);

        TA_ASSERT(dnEntity != NULL, "Can't convert IEntityData to DataNodeEntityData");
        
        m_label = dnEntity->getLabel();
        m_shortLabel = dnEntity->getShortLabel();
        m_assetName = dnEntity->getAssetName();

		initDisplayState();
    }

	DataNodeAttributes::DataNodeAttributes(DataNodeCacheData& opcDnData)
	:
	AbstractDataObjectAttributes(opcDnData.entityKey, opcDnData.agentKey, opcDnData.locationKey, 
		opcDnData.preCreateAttrs, opcDnData.entityName),
	m_configAvaliable(false),
	m_assetName(opcDnData.assetName),
	m_label(opcDnData.label),
	m_shortLabel(opcDnData.shortLabel)
	{
		initDisplayState();
	}

    DataNodeAttributes::~DataNodeAttributes()
	{
    }

    bool DataNodeAttributes::isAttributeNameValid(const std::string & attributeName, EOPCATTRIBUTETYPE& attrType)
    {
		bool isValid = true;
        if (attributeName == DN_ATTR_NAME_SHORT_LABEL )
		{
			attrType = E_OPC_DN_SHORTLABEL;
		}
		else if(attributeName == DN_ATTR_NAME_LABEL)
		{
			attrType = E_OPC_DN_LABEL;
		}
		else if(attributeName == DN_ATTR_NAME_ASSET_NAME)
		{
			attrType = E_OPC_DN_ASSETNAME;
		}
		else if(attributeName == DN_ATTR_NAME_QUALITY)
		{
			attrType = E_OPC_DN_QUALITY;
		}
		else if(attributeName == DN_ATTR_NAME_ALARM_STATE)
		{
			attrType = E_OPC_DN_ALARMSTATE;
		}
		else if(attributeName == DN_ATTR_NAME_ACK_STATE)
		{
			attrType = E_OPC_DN_ACKSTATE;
		}
		else if(attributeName == DN_ATTR_NAME_ONITP)
        {
            attrType = E_OPC_DN_ONTIP;
        }
		else 
		{
			attrType = E_OPC_ATTR_UNKNOWN;
			isValid = false;
		}
		
        return isValid;
    }

	void DataNodeAttributes::getPrecreateAttributes(std::vector<AttrData>& attrs)
	{	
		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DN_LABEL)
		{
			attrs.push_back(AttrData(E_OPC_DN_LABEL, DN_ATTR_NAME_LABEL));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DN_SHORTLABEL)
		{
			attrs.push_back(AttrData(E_OPC_DN_SHORTLABEL, DN_ATTR_NAME_SHORT_LABEL));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DN_ASSETNAME)
		{
			attrs.push_back(AttrData(E_OPC_DN_ASSETNAME, DN_ATTR_NAME_ASSET_NAME));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DN_ONTIP)
		{
			attrs.push_back(AttrData(E_OPC_DN_ONTIP, DN_ATTR_NAME_ONITP));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DN_ACKSTATE)
		{
			attrs.push_back(AttrData(E_OPC_DN_ACKSTATE, DN_ATTR_NAME_ACK_STATE));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DN_ALARMSTATE)
		{
			attrs.push_back(AttrData(E_OPC_DN_ALARMSTATE, DN_ATTR_NAME_ALARM_STATE));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DN_QUALITY)
		{
			attrs.push_back(AttrData(E_OPC_DN_QUALITY, DN_ATTR_NAME_QUALITY));
		}
	}
    
	Attribute* DataNodeAttributes::newAttribute(const std::string& tagName, EOPCATTRIBUTETYPE attrType)
	{
		Attribute* attribute = NULL;
		switch( attrType )
		{
		case E_OPC_DN_SHORTLABEL:
			attribute = new DnShortLabel(this, tagName, m_shortLabel);
			break;
		case E_OPC_DN_ASSETNAME:
			attribute = new DnAssetName(this, tagName, m_assetName);
			break;
		case E_OPC_DN_ONTIP:
			attribute = new DnOnitp(this, tagName);
			break;
		case E_OPC_DN_LABEL:
			attribute = new DnLabel(this, tagName, m_label);
			break;
		case E_OPC_DN_QUALITY:
			attribute = new DnQualitySummary(this, tagName);
			break;
		case E_OPC_DN_ALARMSTATE:
			attribute = new DnAlarmStateSummary(this, tagName);
			break;
		case E_OPC_DN_ACKSTATE:
			attribute = new DnAckStateSummary(this, tagName);
			break;
		default:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid attribute:%d for tag:%s", attrType, tagName.c_str());
		}
		
		return attribute;
	}

	EOPCATTRIBUTETYPE DataNodeAttributes::getAttributeType( std::string& attrName )
	{
		EOPCATTRIBUTETYPE attrType = E_OPC_ATTR_UNKNOWN;

		isAttributeNameValid(attrName, attrType);
		
		return attrType;
	}

	OPC_ENTITY_TYPE DataNodeAttributes::getObjectTypeNo()
	{
		return E_OPC_DN;
	}

    bool DataNodeAttributes::isConfigAvailable()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_configAvaliable;
    }

	bool DataNodeAttributes::updateState(TA_Base_Bus::DataNodeDisplayState& newState)
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);

		m_state.alarmAckState = newState.alarmAckState;
		m_state.alarmState = newState.alarmState;
		m_state.ONITP = newState.ONITP;
		m_state.quality = newState.quality;
		m_configAvaliable = true;
		return true;
	}

	std::string DataNodeAttributes::getONITP()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.ONITP;
	}

	_variant_t DataNodeAttributes::getVariantONITP()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.ONITP.c_str();
	}

	TA_Base_Bus::BooleanTag DataNodeAttributes::getAlarmSummary()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.alarmState;
	}

	TA_Base_Bus::EnumTag DataNodeAttributes::getAlarmAckStateSummary()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.alarmAckState;
	}

	TA_Base_Bus::EnumTag DataNodeAttributes::getQualitySummary()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.quality;
	}

	unsigned long DataNodeAttributes::getQualityValue()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.quality.value;
	}

	void DataNodeAttributes::setStatus(unsigned long newStatus)
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		m_state.quality.value = newStatus;
		m_state.quality.quality = (TA_Base_Bus::EQualityStatus)newStatus;
	}

	void DataNodeAttributes::initDisplayState()
	{
		m_state.entityKey = m_entityKey;

		//default value in accordance with tag default value
		m_state.alarmAckState.value = (unsigned long)TA_Base_Bus::ScadaAlarmNotPresent;
		m_state.alarmAckState.quality = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;

		m_state.alarmState.value = false;
		m_state.alarmState.quality = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;

		m_state.quality.value = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;
		m_state.quality.quality = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;

		m_state.ONITP = "";
	}
}
