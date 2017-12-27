/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.cpp $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *	DataPointAccessCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */


#include "time.h"

#include "core/naming/src/Naming.h"
#include "core/exceptions/src/CorbaException.h"

#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/MeteredDataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"


//TD15618, use timer to trigger if the observers are timeout
namespace TA_Base_Bus
{

	DataPointAccessCorbaDef_Impl::DataPointAccessCorbaDef_Impl ( const std::string & agentName, TA_Base_Bus::IDataPointAgent * specificDataPointAgent)
	:
	m_agentName ( agentName ),
	m_operationMode ( TA_Base_Core::NotApplicable ),
	m_specificAgent ( 0 )
	{
		// save the pointer to the data point agent for later usage
		m_specificAgent = specificDataPointAgent;

		// activate the ServantBase of the class DataPointAccessCorbaDef_Impl
		activateServantWithName ( m_agentName + TA_Base_Bus::DP_ACCESS_INTERFACE );
	}


	DataPointAccessCorbaDef_Impl::~DataPointAccessCorbaDef_Impl()
	{
		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificAgent = 0;
		// CL-17309 ++
		deactivateServant();
		// ++ CL-17309
		//deactivateAndDeleteServant();
	}

	void DataPointAccessCorbaDef_Impl::setToControl()
	{

		m_operationMode = TA_Base_Core::Control;
        m_specificAgent->getDataPointFactory()->setToControl();
	}


	void DataPointAccessCorbaDef_Impl::setToMonitor()
	{


		m_operationMode = TA_Base_Core::Monitor;
        m_specificAgent->getDataPointFactory()->setToMonitor();
	}

	void DataPointAccessCorbaDef_Impl::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		m_operationMode = mode;
		m_specificAgent->getDataPointFactory()->setOperationMode(mode);
	}

    void DataPointAccessCorbaDef_Impl::registerMonitorAgentForStateUpdate(const char* uniqueId, IDataPointStateUpdateCorbaDef_ptr dpStateUpdateServantRef)
    {
        FUNCTION_ENTRY("registerMonitorAgentForStateUpdate");
        
        if(m_operationMode == TA_Base_Core::Control)
        {
        m_specificAgent->getDataPointFactory()->registerMonitorAgent(uniqueId, dpStateUpdateServantRef);
        }
        else
        {
            TA_THROW_CORBA(TA_Base_Core::OperationModeException());
        }
        
        FUNCTION_EXIT;
    }
    
    
	void DataPointAccessCorbaDef_Impl::setAlarmProperty(CORBA::ULong dpKey, 
		const char* sessionId, const DataPointCorbaTypes::SAlarmProperty& newAlarmProperty)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::setAlarmProperty");

		if(m_operationMode == TA_Base_Core::Control)
		{
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
			if(0 != dataPoint)
			{
				TA_Base_Bus::AlarmProperty alarmProperty;
				convertAlarmProperty(newAlarmProperty, alarmProperty);
				dataPoint->setAlarmProperties(sessionId, alarmProperty);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	ScadaCorbaTypes::UTag* DataPointAccessCorbaDef_Impl::getFieldValue(CORBA::ULong dpKey)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::getFieldValue");

		if(m_operationMode != TA_Base_Core::Control)
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
		if(0 == dataPoint)
		{
			TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
		}

		boost::shared_ptr<TA_Base_Bus::DataPointValue> fieldValue = dataPoint->getFieldValue();
		TA_Base_Bus::ScadaCorbaTypes::UTag* ret = new TA_Base_Bus::ScadaCorbaTypes::UTag;
		convertDataPointValue(*fieldValue , *ret);

		FUNCTION_EXIT;
		return ret;
	}

	ScadaCorbaTypes::UTag* DataPointAccessCorbaDef_Impl::getRawValue(CORBA::ULong dpKey)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::getRawValue");

		if(m_operationMode != TA_Base_Core::Control)
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
		if(0 == dataPoint)
		{
			TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
		}

		boost::shared_ptr<TA_Base_Bus::DataPointValue> rawValue = dataPoint->getRawValue();
		TA_Base_Bus::ScadaCorbaTypes::UTag* ret = new TA_Base_Bus::ScadaCorbaTypes::UTag;
		convertDataPointValue(*rawValue, *ret);

		FUNCTION_EXIT;
		return ret;		
	}

	void DataPointAccessCorbaDef_Impl::writeDataPointValue(CORBA::ULong dpKey, const char* sessionId, 
		const ScadaCorbaTypes::UTag& value, CORBA::ULong triggeringEntity, const char* triggeringEntityValue)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::writeDataPointValue");

		if(m_operationMode == TA_Base_Core::Control)
		{
		
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);

			if(0 != dataPoint)
			{
				TA_Base_Bus::DataPointValue datapointValue;
				convertDataPointValue(value, datapointValue);
				dataPoint->WriteDataPointValue(sessionId, datapointValue, triggeringEntity, triggeringEntityValue);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataPointAccessCorbaDef_Impl::writeDataPointValueWithoutLoggingEvent(CORBA::ULong dpKey, const char* sessionId,
		const ScadaCorbaTypes::UTag& value, CORBA::ULong triggeringEntity, const char* triggeringEntityValue)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::writeDataPointValueWithoutLoggingEvent");

		if(m_operationMode == TA_Base_Core::Control)
		{

		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
			
			if(0 != dataPoint)
			{
				TA_Base_Bus::DataPointValue datapointValue;
				convertDataPointValue(value, datapointValue);
				dataPoint->WriteDataPointValueWithoutLoggingEvent(sessionId, datapointValue, 
											triggeringEntity, triggeringEntityValue);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataPointAccessCorbaDef_Impl::setOverride(CORBA::ULong dpKey, 
		const char* sessionId, const ScadaCorbaTypes::UTag& value)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::setOverride");

		if(m_operationMode == TA_Base_Core::Control)
		{

		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
			
			if(0 != dataPoint)
			{
				TA_Base_Bus::DataPointValue datapointValue;
				convertDataPointValue(value, datapointValue);
				
				dataPoint->setForcedStateWithValue(sessionId, datapointValue);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataPointAccessCorbaDef_Impl::removeOverride(CORBA::ULong dpKey, const char* sessionId)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::setOverride");

		if(m_operationMode == TA_Base_Core::Control)
		{

		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
			
			if(0 != dataPoint)
			{
				TA_Base_Bus::DataPointValue datapointValue;

				dataPoint->SetIsForcedState(sessionId, false);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataPointAccessCorbaDef_Impl::setMeterCorrectionValue(CORBA::ULong dpKey, 
		const char* sessionId, CORBA::Double correctionValue)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::setMeterCorrectionValue");

		if(m_operationMode == TA_Base_Core::Control)
		{

		TA_Base_Bus::MeteredDataPoint* meteredDataPoint  = dynamic_cast<TA_Base_Bus::MeteredDataPoint*>(m_specificAgent->getDataPoint(dpKey));
			
			if(0 == meteredDataPoint)
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
			else 
			{
				meteredDataPoint->setMeteredCorrectionValue(sessionId, correctionValue);
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataPointAccessCorbaDef_Impl::setInputInhibitState(CORBA::ULong dpKey, 
		const char* sessionId, ScadaCorbaTypes::EInputInhibitState inhibit)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::setInputInhibitState");

		if(m_operationMode == TA_Base_Core::Control)
		{

		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
			
			if(0 != dataPoint)
			{
				TA_Base_Bus::EInputInhibitState newInhibitState;
				convertInputInhibitState(inhibit, newInhibitState);
				dataPoint->setInputInhibitState(sessionId, newInhibitState);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;		
	}

	void DataPointAccessCorbaDef_Impl::setOutputInhibitState(CORBA::ULong dpKey, 
		const char* sessionId, ScadaCorbaTypes::EOutputInhibitState inhibit)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::setInputInhibitState");

		if(m_operationMode == TA_Base_Core::Control)
		{

		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
			
			if(0 != dataPoint)
			{
				TA_Base_Bus::EOutputInhibitState newInhibitState;
				convertOutputInhibitState(inhibit, newInhibitState);
				dataPoint->setOutputInhibitState(sessionId, newInhibitState);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;		
	}

	CORBA::Boolean DataPointAccessCorbaDef_Impl::getIsMMSAlarmInhibited(CORBA::ULong dpKey)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::getIsMMSAlarmInhibited");

		if(m_operationMode != TA_Base_Core::Control)
		{			
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
		
		if(0 == dataPoint)
		{
			TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
		}

		FUNCTION_EXIT;
		return ( dataPoint->getMMSAlarmInhibitedState() ? 1 :0 );		
	}

	ScadaCorbaTypes::EInputInhibitState DataPointAccessCorbaDef_Impl::getInputInhibitState(CORBA::ULong dpKey)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::getInputInhibitState");

		if(m_operationMode != TA_Base_Core::Control)
		{			
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
		
		if(0 == dataPoint)
		{
			TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
		}

		TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState inputInhibitState;
		TA_Base_Bus::InhibitState inhibitState = dataPoint->getInhibitState();

		convertInputInhibitState(inhibitState.inputInhibit, inputInhibitState);
		
		FUNCTION_EXIT;
		return inputInhibitState;
	}

	ScadaCorbaTypes::EOutputInhibitState DataPointAccessCorbaDef_Impl::getOutputInhibitState(CORBA::ULong dpKey)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::getOutputInhibitState");

		if(m_operationMode != TA_Base_Core::Control)
		{			
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
		
		if(0 == dataPoint)
		{
			TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
		}
		
		TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState outputInhibitState;
		TA_Base_Bus::InhibitState inhibitState = dataPoint->getInhibitState();
		
		convertOutputInhibitState(inhibitState.outputInhibit, outputInhibitState);
			
		FUNCTION_EXIT;
		return outputInhibitState;
	}

	CORBA::Boolean DataPointAccessCorbaDef_Impl::isWriteable(CORBA::ULong dpKey)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::isWriteable");

		if(m_operationMode != TA_Base_Core::Control)
		{			
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
		
		if(0 == dataPoint)
		{
			TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
		}
		
		FUNCTION_EXIT;
		return ( dataPoint->isWriteable() ? 1 :0 );		
	}

	ScadaCorbaTypes::SLastOperatorInfo* DataPointAccessCorbaDef_Impl::getLastOperatorInfo(CORBA::ULong dpKey)
	{
		FUNCTION_ENTRY("DataPointAccessCorbaDef_Impl::isWriteable");

		if(m_operationMode != TA_Base_Core::Control)
		{			
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		TA_Base_Bus::DataPoint* dataPoint = m_specificAgent->getDataPoint(dpKey);
		
		if(0 == dataPoint)
		{
			TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
		}

		TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo* ret = NULL;
		ret = new TA_Base_Bus::ScadaCorbaTypes::SLastOperatorInfo();

		TA_Base_Bus::LastOperatorInfo lastOperator;
		lastOperator.sessionID = dataPoint->getLastOperatorSession();
		lastOperator.timeStamp = dataPoint->getLastOperatorTimeStamp();
		
		convertLastOperatorInfo(lastOperator, *ret);
		
		FUNCTION_EXIT;
		return ret;		
	}
}
