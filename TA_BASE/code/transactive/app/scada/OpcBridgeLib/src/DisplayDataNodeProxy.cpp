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
// DispalyDataNodeProxy.cpp: implementation of the DispalyDataNodeProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "app/scada/OpcBridgeLib/src/DisplayDataNodeProxy.h"
#include "core/synchronisation/src/ThreadGuard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{
    
    DisplayDataNodeProxy::DisplayDataNodeProxy(unsigned long entityKey, AbstractDataObjectAttributes * eventProcessor)
    : m_attribute(eventProcessor)
    {
        m_state.entityKey = entityKey;
        
		//default value in accordance with tag default value
        m_state.alarmAckState.value = (unsigned long)TA_Base_Bus::ScadaAlarmNotPresent;
        m_state.alarmAckState.quality = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;

        m_state.alarmState.value = false;
        m_state.alarmState.quality = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;

        m_state.quality.value = (unsigned long)TA_Base_Bus::QUALITY_BAD;
        m_state.quality.quality = TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED;

        m_state.ONITP = "";
    }

    DisplayDataNodeProxy::~DisplayDataNodeProxy()
    {

    }

    bool DisplayDataNodeProxy::updateState(TA_Base_Bus::DataNodeDisplayState& newState)
    {
		TA_Base_Core::ThreadGuard guard(m_lock);

        m_state.alarmAckState = newState.alarmAckState;
        m_state.alarmState = newState.alarmState;
        m_state.ONITP = newState.ONITP;
        m_state.quality = newState.quality;
        
        return true;
    }
        
    std::string DisplayDataNodeProxy::getONITP()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.ONITP;
    }
    
    _variant_t DisplayDataNodeProxy::getVariantONITP()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.ONITP.c_str();
    }

    TA_Base_Bus::BooleanTag DisplayDataNodeProxy::getAlarmSummary()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.alarmState;
    }

    TA_Base_Bus::EnumTag DisplayDataNodeProxy::getAlarmAckStateSummary()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.alarmAckState;
    }
    
    TA_Base_Bus::EnumTag DisplayDataNodeProxy::getQualitySummary()
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        return m_state.quality;
    }

	unsigned long DisplayDataNodeProxy::getQualityValue()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_state.quality.value;
	}
    
    void DisplayDataNodeProxy::setStatus(unsigned long newStatus)
    {
		TA_Base_Core::ThreadGuard guard(m_lock);
        m_state.quality.value = newStatus;
        m_state.quality.quality = (TA_Base_Bus::EQualityStatus)newStatus;
    }

	AbstractDataObjectAttributes * DisplayDataNodeProxy::getAttribute()
	{
		return m_attribute;
	}

	void DisplayDataNodeProxy::setAttribute(AbstractDataObjectAttributes * ptr)
	{
		m_attribute = ptr;
	}	
}
