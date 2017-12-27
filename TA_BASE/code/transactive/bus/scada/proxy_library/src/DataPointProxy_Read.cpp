/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointProxy_Read.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modified by:  $Author: haijun.li $
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * 
  * This class provides access to a piece of equipment and its associated DataPoints.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include <string> 
#include <limits>
#include <time.h>

#include "bus/scada/proxy_library/src/DataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointSyncManager.h"
#include "bus/scada/common_library/src/DpValue.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	bool DataPointProxy::isConfigValid() const
	{
		return m_proxyData.isConfigValid;
	}

	bool DataPointProxy::isAlarmPropertiesValid() const
	{
		return m_proxyData.isConfigValid;
	}

	unsigned long DataPointProxy::getStatus()
	{
        if(!m_proxyData.isConfigValid)
        {
            std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getStatus() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.value.quality;
	}

	const char* const DataPointProxy::getQualityAsString() const
	{
		TA_THREADGUARD(m_updateLock);

		switch (m_proxyData.dynamicData.value.quality)
		{
		case QUALITY_BAD_CONFIGURATION_ERROR:
			return QUALITY_BAD_CONFIGURATION_ERROR_STRING;

		case QUALITY_BAD_NOT_CONNECTED:
			return QUALITY_BAD_NOT_CONNECTED_STRING;

		case QUALITY_BAD_DEVICE_FAILURE:
			return QUALITY_BAD_DEVICE_FAILURE_STRING;

		case QUALITY_BAD_SENSOR_FAILURE:
			return QUALITY_BAD_SENSOR_FAILURE_STRING;
			
		case QUALITY_BAD_LAST_KNOWN_VALUE:
			return QUALITY_BAD_LAST_KNOWN_VALUE_STRING;

		case QUALITY_BAD_COMM_FAILURE:
			return QUALITY_BAD_COMM_FAILURE_STRING;

		case QUALITY_BAD_OUT_OF_SERVICE:
			return QUALITY_BAD_OUT_OF_SERVICE_STRING;

		case QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON:
			return QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON_STRING;

		case QUALITY_UNCERTAINTY_LAST_USABLE_VALUE:
			return QUALITY_UNCERTAINTY_LAST_USABLE_VALUE_STRING;

		case QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE:
			return QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE_STRING;

		case QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED:
			return QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED_STRING;

		case QUALITY_UNCERTAINTY_SUB_NORMAL:
			return QUALITY_UNCERTAINTY_SUB_NORMAL_STRING;

		case QUALITY_GOOD_NO_SPECIFIC_REASON:
			return QUALITY_GOOD_NO_SPECIFIC_REASON_STRING;

		case QUALITY_GOOD_LOCAL_OVERRIDE:
			return QUALITY_GOOD_LOCAL_OVERRIDE_STRING;

		case QUALITY_BAD_NO_SPECIFIC_REASON:
		default:
			return QUALITY_BAD_NO_SPECIFIC_REASON_STRING;
		}
	};


	bool DataPointProxy::isBoolean()
	{
		return (m_entityData->getDataType() == DPT_CFG_BOOL);
	}


	bool DataPointProxy::isNumber()
	{
		// return true if not a boolean datapoint and also not a text datapoint
		return ( ( false == isBoolean() ) && ( false == isText() ) );
	}


	bool DataPointProxy::isText()
	{
		return (m_entityData->getDataType() == DPT_CFG_TEXT);
	}


	bool DataPointProxy::isDerived()
	{
        std::string dpType = m_entityData->getDataPointType();

        return (dpType == DPT_DERIVED_TYPE_STRING ||
                dpType == DPT_ENUM_TYPE_STRING ||
                dpType == DPT_COMBINED_OUTPUT_TYPE_STRING ||
                dpType == DPT_SAFETY_OUTPUT_TYPE_STRING);
	}
	
	DpValue DataPointProxy::getValue()
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getValue() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
        
		TA_THREADGUARD(m_updateLock);

        DpValue dpValue(m_proxyData.dynamicData.value, 
            m_proxyData.configData.dataType, 
            m_proxyData.configData.enumLabelsMap,
            m_proxyData.configData.booleanLabels,
            m_proxyData.configData.precision);

		return dpValue;
	}

	std::string DataPointProxy::getValueAsString()
	{
        if(!m_proxyData.isConfigValid)
        {
            std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getValueAsString() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);
        
		// if quality is BAD_NOT_CONNECTED, we do not know the current value
		// of the DataPoint yet
		if ( QUALITY_BAD_NOT_CONNECTED == m_proxyData.dynamicData.value.quality )
		{
			// default to this string
			return QUALITY_BAD_VALUE_UNKNOWN_STRING;
		}
		else
		{
            DpValue dpValue(m_proxyData.dynamicData.value, 
                m_proxyData.configData.dataType, 
                m_proxyData.configData.enumLabelsMap,
                m_proxyData.configData.booleanLabels,
                m_proxyData.configData.precision);
            
			return dpValue.getValueAsString();
		}
	}

	
	bool DataPointProxy::getValueAsBool()
	{
        if(!m_proxyData.isConfigValid)
        {
            std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getValueAsBool() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
		TA_THREADGUARD(m_updateLock);

		// if not a Boolean DataPoint
		if (m_proxyData.configData.dataType != DPT_BOOL_TYPE)
		{
			std::string text = "Error: \"" + m_proxyData.configData.description + "\" is not a Boolean DataPoint.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getValueAsBool() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		// if quality is BAD_NOT_CONNECTED, we do not know the value of the 
		// DataPoint yet
		if ( QUALITY_BAD_NOT_CONNECTED == m_proxyData.dynamicData.value.quality )
		{
			std::string text = "Error: \"" + m_proxyData.configData.description + "\" - quality is bad and not connected.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getValueAsBool() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		return m_proxyData.dynamicData.value.booleanValue;
	}


	double DataPointProxy::getValueAsDouble()
	{
        if(!m_proxyData.isConfigValid)
        {
            std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getValueAsDouble() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
		TA_THREADGUARD(m_updateLock);

		// if not a Numeric DataPoint, e.g Analogue/Derived/Enum etc...
		if ( m_proxyData.configData.dataType==DPT_ENUM_TYPE || 
			 m_proxyData.configData.dataType==DPT_BOOL_TYPE ||
		     m_proxyData.configData.dataType==DPT_TEXT_TYPE )
		{
			std::string text = "Error: \"" + m_proxyData.configData.description + "\" is not a Numeric DataPoint.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getValueAsDouble() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		// if quality is BAD_NOT_CONNECTED, we do not know the value of the 
		// DataPoint yet
		if ( QUALITY_BAD_NOT_CONNECTED == m_proxyData.dynamicData.value.quality )
		{
			std::string text = "Error: \"" + m_proxyData.configData.description + "\" -  quality is bad and not connected..";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getValueAsBool() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		return m_proxyData.dynamicData.value.floatValue;
	}

	long DataPointProxy::getTimestamp()
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected for DataPoint " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getTimestamp() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
		TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.value.timeStamp.time;
	}


	const timeb & DataPointProxy::getTimestampStructure()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getTimestampStructure() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		TA_THREADGUARD(m_updateLock);

		return m_proxyData.dynamicData.value.timeStamp;
	}


	std::string DataPointProxy::getLaunchingConditionResultString()
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected for DataPoint " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "getLaunchingConditionResultString() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
        TA_THREADGUARD(m_updateLock);
        
        return m_proxyData.dynamicData.controlState.lccResult;
    }


	std::string DataPointProxy::getFieldValue()
	{
		std::string valueString = QUALITY_BAD_VALUE_UNKNOWN_STRING;

		//
		// get field value from the agent
		//

		boost::shared_ptr<TA_Base_Bus::DataPointValue> fieldState = getObjAccess()->getFieldValue();

		if ( fieldState.get() != NULL )
		{
			// if quality of the field value is not BAD_NOT_CONNECTED
			if ( QUALITY_BAD_NOT_CONNECTED != fieldState->quality  )
			{
				DpValue dpFieldValue (*fieldState,
                    m_proxyData.configData.dataType, 
                    m_proxyData.configData.enumLabelsMap,
                    m_proxyData.configData.booleanLabels,
                    m_proxyData.configData.precision);
                
				// convert value to string
				valueString = dpFieldValue.getValueAsString();
			}
			// else do nothing
		}

		return valueString;
	}

	bool DataPointProxy::isOverrideEnabled()
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected for DataPoint " + m_entityData->getName() + ".";
            //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            //    "isOverrideEnabled() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            throw(TA_Base_Core::ScadaProxyException (text));
        }
        
		TA_THREADGUARD(m_updateLock);

        return (m_proxyData.dynamicData.value.quality == QUALITY_GOOD_LOCAL_OVERRIDE);
	}

	//
	// Inhibit State
	//

	bool DataPointProxy::isControlInhibited()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Inhibit Status not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isControlInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		if (!m_proxyData.configData.writable)
		{
			std::string text = "Error:DataPoint \"" + m_proxyData.configData.description + "\" is an input DataPoint - control inhibit not possible.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"isControlInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return (m_proxyData.dynamicData.inhibitState.outputInhibit == InhibitControl); 
	}



	bool DataPointProxy::isScanInhibited()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Inhibit Status not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isScanInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		if (m_proxyData.configData.writable)
		{
			std::string text = "Error:DataPoint \"" + m_proxyData.configData.description + "\" is an output DataPoint - scan inhibit not possible.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"isScanInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return (m_proxyData.dynamicData.inhibitState.inputInhibit == InhibitScan);
	}


	bool DataPointProxy::isAlarmInhibited()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Inhibit Status not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isAlarmInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		if (m_proxyData.configData.writable)
		{
			std::string text = "Error:DataPoint \"" + m_proxyData.configData.description + "\" is an output DataPoint - alarm inhibit not possible.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"isAlarmInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return (m_proxyData.dynamicData.inhibitState.inputInhibit == InhibitAlarm ||
                m_proxyData.dynamicData.inhibitState.inputInhibit == InhibitScan); 
	}


	bool DataPointProxy::isMmsAlarmInhibited()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Inhibit Status not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isMmsAlarmInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

		if (m_proxyData.configData.writable)
		{
			std::string text = "Error:DataPoint \"" + m_proxyData.configData.description + "\" is an output DataPoint - MMS inhibit not possible.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"isMmsInhibited() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return (m_proxyData.dynamicData.inhibitState.inputInhibit == InhibitMmsAlarm ||
                m_proxyData.dynamicData.inhibitState.inputInhibit == InhibitAlarm ||
                m_proxyData.dynamicData.inhibitState.inputInhibit == InhibitScan); 
	}

	

	//
	// Control State
	//

	EDataPointControlState DataPointProxy::getControlState()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Control Status not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getControlState() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		if (m_proxyData.configData.writable)
		{
			return m_proxyData.dynamicData.controlState.controlState;
		}
		else
		{
			return noControl;
		}
	}

	//
	// Control Triggering Entity
	//

	unsigned long DataPointProxy::getControlTriggeringEntity()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Control Triggering Entity not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getControlTriggeringEntity() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		if (m_proxyData.configData.writable)
		{
			return m_proxyData.dynamicData.controlState.triggerringDpKey; //TD12418
		}
		else
		{
			return 0;
		}
	}

	//TD15649 ++
	//
	// Control Triggering Entity Value
	//
	
	std::string DataPointProxy::getControlTriggeringValue()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Control Triggering Entity not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getControlTriggeringValue() - %s - %s", m_entityData->getName().c_str(), text.c_str());
			
			throw(TA_Base_Core::ScadaProxyException (text));
		}
		
        TA_THREADGUARD(m_updateLock);
        
		if (m_proxyData.configData.writable)
		{
			return m_proxyData.dynamicData.controlState.triggerringDpValue; //TD12418
		}
		else
		{
			return "";
		}
	}
	
	
	//
	// Configuration state
	// 

	unsigned long DataPointProxy::getEntityKey() const
	{
		return m_entityData->getKey();
	}

    std::string DataPointProxy::getEntityName() const
    {
        return m_entityData->getName();
    }
    
	std::string DataPointProxy::getLastStatusChangeSessionId()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Last Operator Status not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getLastStatusChangeSessionId() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		return m_proxyData.dynamicData.lastOperatorInfo.sessionID;
	}

	unsigned long DataPointProxy::getLastStatusChangeTimestamp()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Last Operator Status not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getLastStatusChangeTimestamp() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
		return m_proxyData.dynamicData.lastOperatorInfo.timeStamp.time;
	}

	unsigned long DataPointProxy::getParentKey() const
	{
		return m_entityData->getParent();
	}

	const std::string& DataPointProxy::getDescription() const
	{
        //because entitydata->getDescription return a object, so can't return it as a reference directly.
        if(!m_proxyData.isConfigValid)
        {
            TA_THREADGUARD(m_updateLock);
            const_cast<DataPointProxy*>(this)->m_proxyData.configData.description = m_entityData->getDescription();
        }
        
        return m_proxyData.configData.description;
    }

    
    unsigned long DataPointProxy::getSubSystemKey() const
    {
        return m_entityData->getSubsystem();
    }
    

    unsigned long DataPointProxy::getPhysicalSubSystemKey() const
    {
        return m_entityData->getPhysicalSubsystem();
    }
    
    
    unsigned long DataPointProxy::getLocationKey() const
    {
        return m_entityData->getLocation();
    }
    
    
	bool DataPointProxy::isWriteable()
	{
		return m_entityData->getAllowWrite();
	}

	EDataPointType DataPointProxy::getType()
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getType() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
		return m_proxyData.configData.type;
	}

	std::string DataPointProxy::getTypeAsString()
	{
		return m_entityData->getDataPointType();
	}

	std::string DataPointProxy::getShortNameOfType()
	{
        return m_entityData->getDataPointTypeShortName();
	}

	int DataPointProxy::getDisplayOrder()
	{
		return m_entityData->getDisplayOrder();
	}

	std::string DataPointProxy::getUnit()
	{
        return m_entityData->getEngineeringUnits();
	}

	double DataPointProxy::getLimit(bool lowLimit)
	{
		if (lowLimit)
		{
            return m_entityData->getEngineeringLimitLow();
		}
		else
		{
            return m_entityData->getEngineeringLimitHigh();
		}				
	}


	std::string DataPointProxy::getBooleanLabel(bool bTrue)
	{
		if (m_entityData->getDataType() != DPT_CFG_BOOL)
		{
			std::string text = "Error: \"" + m_proxyData.configData.description + "\" is not a boolean type.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getBooleanLabel() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

		if (bTrue)
		{
			return m_entityData->getTrueLabel();
		}
		else
		{
			return m_entityData->getFalseLabel();				
		}
	}

	DpvEnumLabelsMap DataPointProxy::getDerivedStatesAsMap()
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getDerivedStatesAsMap() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}

        TA_THREADGUARD(m_updateLock);
        
		if ( false == isDerived() )
		{
			std::string text = "Error: \"" + m_proxyData.configData.description + "\" is not a derived type.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getDerivedStateAsString() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

		return m_proxyData.configData.enumLabelsMap;		
	}	

	std::string DataPointProxy::getDerivedStateAsString(unsigned long stateValue)
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getDerivedStateAsString() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
		if ( false == isDerived() )
		{
			std::string text = "Error: \"" + m_proxyData.configData.description + "\" is not a derived type.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getDerivedStateAsString() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}
		
		DpvEnumLabelsMap::iterator it = m_proxyData.configData.enumLabelsMap.find(stateValue);

		if (it != m_proxyData.configData.enumLabelsMap.end())
		{
			return it->second;
		}
		else
		{
			std::string text = "Error: Invalid derived state for DataPoint \"" + m_proxyData.configData.description + "\".";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getDerivedStateAsString() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

		return "";
	}	

	unsigned long DataPointProxy::getDerivedStateAsUnsignedLong(std::string stateValue)
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Configuration not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getDerivedStateAsUnsignedLong() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
		if (stateValue.empty())
		{
			std::string text = "Error: passed empty string as state value for DataPoint \"" + m_proxyData.configData.description + "\" ";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getDerivedStateAsUnsignedLong() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
		if ( false == isDerived() )
		{
			std::string text = "Error: DataPoint \"" + m_proxyData.configData.description + "\" is not a derived type.";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getDerivedStateAsUnsignedLong() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

		DpValue dpValue(m_proxyData.configData.dataType,
					    m_proxyData.configData.enumLabelsMap, 
                        m_proxyData.configData.booleanLabels);
		dpValue.setEnum(stateValue);

		if (stateValue != dpValue.getValueAsString())
		{
			std::string text = "Error: DataPoint \"" + m_proxyData.configData.description + "\" does not have a label " + stateValue;
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getDerivedStateAsUnsignedLong() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

		return dpValue.getEnum();
	}	

	AlarmPropertyVect DataPointProxy::getAlarmProperties()
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Alarm Properties not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getAlarmProperties() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
		return m_proxyData.configData.alarmPropertyVect;
	}

	bool DataPointProxy::isAlarmEnabled(std::string alarmType)
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Alarm Properties not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isAlarmEnabled() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
        EDataPointAlarms type = stringToDpAlarmType(alarmType);

        bool enabled = false;

		AlarmPropertyVect::iterator it;
		
		for (it = m_proxyData.configData.alarmPropertyVect.begin(); it != m_proxyData.configData.alarmPropertyVect.end(); it++)
		{
			if ((*it).type == type)
			{
				enabled = (*it).enabled;
                break;
			}
		}

		return enabled;
	}

	bool DataPointProxy::isGeneralAlarmEnabled(unsigned long value)
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Alarm Properties not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isGeneralAlarmEnabled() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
		bool enabled = false;

		AlarmPropertyVect::iterator it;
		
		for (it = m_proxyData.configData.alarmPropertyVect.begin(); it != m_proxyData.configData.alarmPropertyVect.end(); it++)
		{
			if ((*it).type == DptGeneral && (*it).enumValue == value)
			{
				enabled = (*it).enabled;
				break;
			}
		}

		return enabled;
	}

	double DataPointProxy::getAnalogueAlarmLimit(std::string alarmType)
	{
		if(!m_proxyData.isConfigValid)
		{
			std::string text = "Info: Alarm Properties not available for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getAnalogueAlarmLimit() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
        EDataPointAlarms type = stringToDpAlarmType(alarmType);
 
        if (type != DptOverScaleHi &&
			type != DptOverScaleLo &&
			type != DptHiHi &&
            type != DptHi &&
            type != DptLo &&
			type != DptLoLo )
		{
			std::string text = "Error: Alarm is not an Analog type - " + alarmType + ".";
			//LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
			//	"getAnalogueAlarmLimit() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException(text));
		}

		double limit = 0;

		AlarmPropertyVect::iterator it;

		for (it = m_proxyData.configData.alarmPropertyVect.begin(); it != m_proxyData.configData.alarmPropertyVect.end(); it++)
		{
			if ((*it).type == type)
			{
				limit = (*it).threshold;
				break;
			}
		}

		return limit;
	}

	std::string DataPointProxy::getAgentName() const
	{
		return m_entityData->getAgentName();
	}

	bool DataPointProxy::isInAlarmState()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"isInAlarmState() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);
        
		// if the data point quality is anything else except QUALITY_BAD_NOT_CONNECTED,
		// evaluate alarm condition, else default to no alarm
		if ( QUALITY_BAD_NOT_CONNECTED != m_proxyData.dynamicData.value.quality )
		{
			return (m_proxyData.dynamicData.alarmState != DptNoAlarm);
		}
		else
		{
			return false;
		}		
	}

	std::string DataPointProxy::getAlarmStateShortString()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getAlarmStateShortString() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
		TA_THREADGUARD(m_updateLock);

		std::string alarmState;

		switch (m_proxyData.dynamicData.alarmState)
		{
		case DptNoAlarm:
			alarmState = DPTALM_NONE_SHORT;
			break;

		case DptOverScaleHi:
			alarmState = DPTALM_OH_SHORT;
			break;

		case DptOverScaleLo:
			alarmState = DPTALM_OL_SHORT;
			break;

		case DptHiHi:
			alarmState = DPTALM_HIHI_SHORT;
			break;

		case DptHi:
			alarmState = DPTALM_HI_SHORT;
			break;

		case DptLo:
			alarmState = DPTALM_LO_SHORT;
			break;

		case DptLoLo:
			alarmState = DPTALM_LOLO_SHORT;
			break;

		case DptBoolTrue:
			alarmState = DPTALM_TRUE_SHORT;
			break;

		case DptBoolFalse:
			alarmState = DPTALM_FALSE_SHORT;
			break;

		case DptGeneral:
		default:
			alarmState = DPTALM_GENERAL_SHORT;
			break;
		}

		return alarmState;
	}

	EScadaAlarmAckState DataPointProxy::getAlarmAckState()
	{
		if(!m_proxyData.isSynchronized)
		{
			std::string text = "Info: Agent is not connected for DataPoint " + m_entityData->getName() + ".";
			//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			//	"getAlarmAckState() - %s - %s", m_entityData->getName().c_str(), text.c_str());

			throw(TA_Base_Core::ScadaProxyException (text));
		}
	
        TA_THREADGUARD(m_updateLock);

        return m_proxyData.dynamicData.alarmAckState;
	}

}



