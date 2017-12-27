/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointWriteRequest.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/21 14:56:06 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	Represents a request for the PLCWorker to execute
  * a write operation to the IProtocol driver.
  *
  *
  */

#ifndef DATAPOINTWRITEREQUEST_H
#define DATAPOINTWRITEREQUEST_H

#include "bus/scada/common_library/src/DpValue.h"


namespace TA_Base_Bus
{
	class DataPoint;
}


namespace TA_Base_Bus
{

	class DataPointWriteRequest
	{

	public:

		/**
		 * DataPointWriteRequest::DataPointWriteRequest()
		 *
		 * @param point		the DataPoint representing the value to
		 *					be written to
		 *
		 * @param value		the value to write
		 *
		 */
		DataPointWriteRequest( DataPoint * point,
							   DpValue & value,
							   const std::string & sessionID,
							   unsigned long actionKey,
                               unsigned long commandId)
		:
		m_point( point ),
		m_value( value ),
		m_sessionID ( sessionID ),
		m_actionKey ( actionKey ),
        m_commandId ( commandId )
		{
			// Nothing else to do
		}


		/**
		 * DataPointWriteRequest::getDataPoint()
		 *
		 * @return	the DataPoint representing the value to be written to
		 *
		 */
		DataPoint *	getDataPoint()
		{
			return m_point;
		}


		/**
		 * DataPointWriteRequest::getValue()
		 *
		 * @return	the value to write
		 *
		 */
		DpValue & getValue()
		{
			return m_value;
		}

		std::string getSessionID()
		{
			return m_sessionID;
		}

		unsigned long getActionKey()
		{
			return m_actionKey;
		}

        void setCommandId(unsigned long commandId)
		{
			m_commandId = commandId;
		}

        unsigned long getCommandId()
		{
			return m_commandId;
		}

	private:

		DataPoint *		m_point;
		DpValue	m_value;
		std::string		m_sessionID;
		unsigned long	m_actionKey;
        unsigned long   m_commandId;
	};

}

#endif
