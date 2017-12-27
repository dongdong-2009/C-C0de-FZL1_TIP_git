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
// DispalyDataPointProxy.cpp: implementation of the DispalyDataPointProxy class.
//
//////////////////////////////////////////////////////////////////////
#include "app/scada/OpcBridgeLib/src/DisplayDataPointProxy.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <sstream>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{
    
    DisplayDataPointProxy::DisplayDataPointProxy(unsigned long entityKey, AbstractDataObjectAttributes * eventProcessor)
    : m_attribute(eventProcessor)
    {
        m_state.entityKey = entityKey;
		
		//default value in accordance with tag default value
        m_state.dataPointStatus = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;
        m_state.alarmState = false;
		m_state.alarmAckState = TA_Base_Bus::ScadaAlarmNotPresent;
		m_state.value.dpType = TA_Base_Bus::DPT_INT16_TYPE;
		m_state.value.floatValue = 0;
    }

    DisplayDataPointProxy::~DisplayDataPointProxy()
    {
		
    }

    bool DisplayDataPointProxy::updateState(TA_Base_Bus::DataPointDisplayState& newState)
    {
		TA_Base_Core::ThreadGuard guard(m_lock);

        m_state.value = newState.value;
        m_state.alarmState = newState.alarmState;
        m_state.alarmAckState = newState.alarmAckState;
        m_state.dataPointStatus = newState.dataPointStatus;

        return true;
    }
    
    TA_Base_Bus::DisplayDataPointValue DisplayDataPointProxy::getValue()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.value;
    }

    _variant_t DisplayDataPointProxy::getVariantValue()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);

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

    bool DisplayDataPointProxy::isInAlarmState()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.alarmState;
    }

    TA_Base_Bus::EScadaAlarmAckState DisplayDataPointProxy::getAlarmAckState()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.alarmAckState;
    }
    
    WORD DisplayDataPointProxy::getStatus()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return (WORD) m_state.dataPointStatus;
    }

    void DisplayDataPointProxy::setStatus(unsigned long newStatus)
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        m_state.dataPointStatus = newStatus;
    }

	AbstractDataObjectAttributes * DisplayDataPointProxy::getAttribute()
	{
		return m_attribute;
	}

	void DisplayDataPointProxy::setAttribute(AbstractDataObjectAttributes * ptr)
	{
		m_attribute = ptr;
	}	
}