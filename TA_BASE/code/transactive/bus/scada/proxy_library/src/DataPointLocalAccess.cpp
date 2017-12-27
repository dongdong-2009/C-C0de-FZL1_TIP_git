/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointLocalAccess.cpp $
  * @author:	Darren Sampson
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *	DataPoint CORBA interface, used to work with DataPoints
  *
  */

#if defined(_MSC_VER)
#pragma warning (disable:4284 4786 4146 4018 4250 4290 4503)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/DataPointLocalAccess.h"
#include "bus/scada/proxy_library/src/DataPointProxy.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

namespace TA_Base_Bus
{

    DataPointLocalAccess::DataPointLocalAccess(TA_Base_Bus::DataPoint* dataPoint)
    :
    m_dataPointLocalObject(dataPoint)
	{
	};

	DataPointLocalAccess::~DataPointLocalAccess()
	{
	};


	/**
	 *
	 * GetFieldValue
	 *
	 * Gets the  field value of the DataPoint
	 *
	 */
	boost::shared_ptr<TA_Base_Bus::DataPointValue> DataPointLocalAccess::getFieldValue()
	{
        return m_dataPointLocalObject->getFieldValue();
	};

	/**
	 *
	 * WriteDataPointValue
	 *
	 * Change the value associated with the point
	 * (audit message to be logged)
	 *
	 */
    void DataPointLocalAccess::writeDataPointValue ( const std::string&  sessionId, TA_Base_Bus::DataPointValue& value, unsigned long triggeringEntityKey, const char * triggeringEntityValue ) //TD12418
	{
		if (NULL == triggeringEntityValue)
		{
			m_dataPointLocalObject->WriteDataPointValue(sessionId, value, triggeringEntityKey, "");
		}
		else
		{
			m_dataPointLocalObject->WriteDataPointValue(sessionId, value, triggeringEntityKey, triggeringEntityValue);
		}
	};

	/**
	 *
	 * WriteDataPointValueWithoutLoggingEvent
	 *
	 * Change the value associated with the point
	 * (audit message NOT to be logged)
	 *
	 */
    void DataPointLocalAccess::writeDataPointValueWithoutLoggingEvent ( const std::string&  sessionId, TA_Base_Bus::DataPointValue& value, unsigned long triggeringEntityKey, const char * triggeringEntityValue ) //TD12418
	{
		if (NULL == triggeringEntityValue)
		{
			m_dataPointLocalObject->WriteDataPointValueWithoutLoggingEvent(sessionId, value, triggeringEntityKey, "");
		}
		else
		{
			m_dataPointLocalObject->WriteDataPointValueWithoutLoggingEvent(sessionId, value, triggeringEntityKey, triggeringEntityValue);
		}
	};



	/**
	 *
	 * SetIsForcedState
	 *
	 * Set the data point manual state to the specified indication.
	 * Throw AccessDeniedException exception if the client has no authorisation to do so.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    indication	True to enable data point's manual override state,
	 *						false otherwise.
	 *
	*/
	void DataPointLocalAccess::setIsForcedState ( const std::string&  sessionId, bool indication )
	{
        TA_ASSERT(indication == false, "To enalbe override, should call setForcedStateWithValue");
        
        m_dataPointLocalObject->SetIsForcedState(sessionId.c_str(), indication);
	};


	/**
	 *
	 * setForcedStateWithValue
	 *
	 * Force the data point to manual state and update data point's value with the
	 * specified manually entered value.  Throw the following exceptions:
	 *
	 *		OperationModeException, if the data point is in monitor mode,
	 *		AccessDeniedException, if the client has no authorisation to do so,
	 *		NotWriteAble, if the specified value cannot be updated
	 *		BadParameterException, if the specified value is out of range.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    value		The manually entered value to be written to the data point
	 *
	*/
	void DataPointLocalAccess::setForcedStateWithValue ( const std::string&  sessionId, const TA_Base_Bus::DataPointValue& value )
	{
		m_dataPointLocalObject->setForcedStateWithValue ( sessionId, value );
	}


	/**
	  * setAlarmProperties
	  *
	  * This method changes the alarm properties for the datapoint.
	  * Throw the following exceptions:
	  *
	  *		OperationModeException, if the data point is in monitor mode,
	  * 	BadParameterException, if the specified value is out of range.
	  *
	  * @param	newAlarmProperty	The new alarm property of the data point
	  * @param	sessionId			The operator's session id
	  *
	  */
	void DataPointLocalAccess::setAlarmProperty ( const std::string&  sessionId, const TA_Base_Bus::AlarmProperty& newAlarmProperty )
	{
		m_dataPointLocalObject->setAlarmProperties ( sessionId, newAlarmProperty );
	}

		
	/**
	 *
	 * SetServiceState
	 *
	 * Change the data point's service state.  Throw AccessDeniedException exception
	 * if the client has no authorisation to do so.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    indication	True to put the data point in service, false
	 *						to take the data point out of service
	 *
	 */
	void DataPointLocalAccess::setInputInhibitState ( const std::string&  sessionId, EInputInhibitState inputInhibitState )
	{
        m_dataPointLocalObject->setInputInhibitState(sessionId, inputInhibitState);
	};

	/**
	 *
	 * SetIsControlInhibited
	 *
	 * Inhibit or un-inhibit the data point's control.  Throw AccessDeniedException exception
	 * if the client has no authorisation to do so.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    indication	True to inhibit the data point's control, false
	 *						to un-inhibit the control
	 *
	 */
	void DataPointLocalAccess::setOutputInhibitState ( const std::string&  sessionId, EOutputInhibitState outputInhibitState )
	{
        m_dataPointLocalObject->setOutputInhibitState(sessionId, outputInhibitState);
    };

	/**
	 *
	 * SetMeteredCorrectionValue
	 *
	 * Set the correction value of the Metered DataPoint.
	 *
	 * @param    newValue	The new correction value
	 *
	 */
	void DataPointLocalAccess::setMeteredCorrectionValue ( const std::string& sessionId, double newValue )
	{
		m_dataPointLocalObject->setMeteredCorrectionValue(sessionId, newValue);
	}
}
