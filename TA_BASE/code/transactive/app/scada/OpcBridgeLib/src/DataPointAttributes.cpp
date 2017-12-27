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
  * The DataPointAttributes class maintains a list of attributes currently requested by the OPC Client
  * for a particular datapoint
  *
  */

#include <sstream>
#include "ace/OS.h"
#include "app/scada/OpcBridgeLib/src/DataPointAttributes.h"
#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"
#include "app/scada/OpcBridgeLib/attributes/DpAckState.h"
#include "app/scada/OpcBridgeLib/attributes/DpAlarmState.h"
#include "app/scada/OpcBridgeLib/attributes/DpQuality.h"
#include "app/scada/OpcBridgeLib/attributes/DpValue.h"
#include "app/scada/OpcBridgeLib/attributes/DpAttributeNames.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    DataPointAttributes::DataPointAttributes(TA_Base_Core::IEntityData* entityData)
    :
    AbstractDataObjectAttributes(entityData),
	m_configAvaliable(false)
    {
		initDisplayState();
    }
    
	DataPointAttributes::DataPointAttributes(DataPointCacheData& opcDpData)
	:
	AbstractDataObjectAttributes(opcDpData.entityKey, opcDpData.agentKey, opcDpData.locationKey, 
		opcDpData.preCreateAttrs, opcDpData.entityName),
	m_configAvaliable(false)
	{
		initDisplayState();
	}

    DataPointAttributes::~DataPointAttributes()
    {
    }

    bool DataPointAttributes::isAttributeNameValid(const std::string & attributeName, EOPCATTRIBUTETYPE& attrType)
    {
		bool isVaild = true;
        if ( attributeName == DPT_ATTR_NAME_VALUE )
		{
			attrType = E_OPC_DP_VALUE;
		}
		else if( attributeName == DPT_ATTR_NAME_ALARM_STATE )
		{
			attrType = E_OPC_DP_ALARMSTATE;
		}
		else if( attributeName == DPT_ATTR_NAME_QUALITY )
		{
			attrType = E_OPC_DP_QUALITY;
		}
		else if( attributeName == DPT_ATTR_NAME_ACK_STATE )
        {
            attrType = E_OPC_DP_ACKSTATE;
        }
		else
		{
			attrType = E_OPC_ATTR_UNKNOWN;
			isVaild = false;
		}

        return isVaild;
    }

	void DataPointAttributes::getPrecreateAttributes(std::vector<AttrData>& attrs)
	{
		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DP_ACKSTATE)
		{
			attrs.push_back(AttrData(E_OPC_DP_ACKSTATE, DPT_ATTR_NAME_ACK_STATE));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DP_ALARMSTATE)
		{
			attrs.push_back(AttrData(E_OPC_DP_ALARMSTATE, DPT_ATTR_NAME_ALARM_STATE));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DP_QUALITY)
		{
			attrs.push_back(AttrData(E_OPC_DP_QUALITY, DPT_ATTR_NAME_QUALITY));
		}

		if (m_preCreateAttrFlags & E_OPC_DATA_ATTR_DP_VALUE)
		{
			attrs.push_back(AttrData(E_OPC_DP_VALUE, DPT_ATTR_NAME_VALUE));
		}
	}
    
	Attribute* DataPointAttributes::newAttribute(const std::string& tagName, EOPCATTRIBUTETYPE attrType)
	{
		Attribute* attribute = NULL;

		switch( attrType )
		{
		case E_OPC_DP_VALUE:
			attribute = new DpValue(this, tagName);
			break;
		case E_OPC_DP_QUALITY:
			attribute = new DpQuality(this, tagName);
			break;
		case E_OPC_DP_ALARMSTATE:
			attribute = new DpAlarmState(this, tagName);
			break;
		case E_OPC_DP_ACKSTATE:
			attribute = new DpAckState(this, tagName);
			break;
		default:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid DP attribute:%d for tag:%s", attrType, tagName.c_str());
		}

		return attribute;
	}

	EOPCATTRIBUTETYPE DataPointAttributes::getAttributeType( std::string& attrName )
	{
		EOPCATTRIBUTETYPE attType = E_OPC_ATTR_UNKNOWN;

		isAttributeNameValid(attrName, attType);

		return attType;
	}

	OPC_ENTITY_TYPE DataPointAttributes::getObjectTypeNo()
	{
		return E_OPC_DP;
	}

    bool DataPointAttributes::isConfigAvailable()
    {
		TA_Base_Core::ThreadGuard guard(m_stateLock);
        return m_configAvaliable;
    }

	void DataPointAttributes::initDisplayState()
	{
		m_state.entityKey = m_entityKey;

		//default value in accordance with tag default value
		m_state.dataPointStatus = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;
		m_state.alarmState = false;
		m_state.alarmAckState = TA_Base_Bus::ScadaAlarmNotPresent;
		m_state.value.dpType = TA_Base_Bus::DPT_INT16_TYPE;
		m_state.value.floatValue = 0;
	}

	bool DataPointAttributes::updateState(TA_Base_Bus::DataPointDisplayState& newState)
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		m_state.value = newState.value;
		m_state.alarmState = newState.alarmState;
		m_state.alarmAckState = newState.alarmAckState;
		m_state.dataPointStatus = newState.dataPointStatus;
		m_configAvaliable = true;
		return true;
	}

	TA_Base_Bus::DisplayDataPointValue DataPointAttributes::getValue()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.value;
	}

	_variant_t DataPointAttributes::getVariantValue()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);

		switch(m_state.value.dpType)
		{
		case TA_Base_Bus::DPT_INT16_TYPE:
		case TA_Base_Bus::DPT_INT32_TYPE:
		case TA_Base_Bus::DPT_INT16S_TYPE:
		case TA_Base_Bus::DPT_INT32S_TYPE:
		case TA_Base_Bus::DPT_UINT16_TYPE:
		case TA_Base_Bus::DPT_UINT32_TYPE:
		case TA_Base_Bus::DPT_IEEE32_TYPE:
			return m_state.value.floatValue;
			//break;

		case TA_Base_Bus::DPT_ENUM_TYPE:
			return (long)m_state.value.enumValue;
			//break;

		case TA_Base_Bus::DPT_BOOL_TYPE:
			return m_state.value.booleanValue;
			//break;

		case TA_Base_Bus::DPT_TEXT_TYPE:
			std::ostringstream valueStr;
			for ( unsigned int i = 0; i < m_state.value.stringValue.size(); ++i )
			{
				valueStr << (char) m_state.value.stringValue[i];
			}

			return valueStr.str().c_str();
			//break;
		}

		_variant_t value;
		return value;
	}

	bool DataPointAttributes::isInAlarmState()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.alarmState;
	}

	TA_Base_Bus::EScadaAlarmAckState DataPointAttributes::getAlarmAckState()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return m_state.alarmAckState;
	}

	WORD DataPointAttributes::getStatus()
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		return (WORD) m_state.dataPointStatus;
	}

	void DataPointAttributes::setStatus(unsigned long newStatus)
	{
		TA_Base_Core::ThreadGuard guard(m_stateLock);
		m_state.dataPointStatus = newStatus;
	}
}
