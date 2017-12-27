/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointState.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	Summarises the state of a DataPoint.
  *
  *
  */

#ifndef DATAPOINTSTATE_H
#define DATAPOINTSTATE_H

#include <sys/timeb.h>

#include "core/alarm/src/AlarmConstants.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/src/DpValue.h"

namespace TA_Base_Bus
{

	class DataPointState
	{
	public:

		
		/**
		 * DataPointState::DataPointState()
		 *
		 * A DataPointState consists of a value, a timestamp indicating
		 * the time of state change and a status of whether the DataPoint
		 * is in a 'good' state or not.
		 *
		 * @param dpv				The value of the DataPoint
		 * @param tmstamp			The time (in secs since the start of the ipoch)
		 *							that the state change was detected
		 * @param status			The status of the DataPoint
		 * @param miliSeconds		The fraction of the second if defined
		 *
		 */
		DataPointState( const DpValue & dpv,
						unsigned long tmstamp,
                        TA_Base_Bus::EQualityStatus status,
						unsigned short miliSeconds = 0 )
		:
		m_value( dpv ),
        m_status( status ),
		m_timestamp ( TA_Base_Core::AlarmConstants::defaultTime )
		{
			m_timestamp.time = tmstamp;
			m_timestamp.millitm = miliSeconds;
		}


		/**
		 * DataPointState::DataPointState()
		 *
		 * A DataPointState consists of a value, a timestamp indicating
		 * the time of state change and a status of whether the DataPoint
		 * is in a 'good' state or not.
		 *
		 * @param dpv				The value of the DataPoint
		 * @param tmstamp			The time (in secs, including miliseconds, since the start of the ipoch)
		 *							that the state change was detected
		 * @param status			The status of the DataPoint
		 *
		 */
		DataPointState( const DpValue & dpv,
						const timeb & tmstamp,
						TA_Base_Bus::EQualityStatus status )
		:
		m_value( dpv ),
		m_status( status ),
		m_timestamp ( tmstamp )
		{
		}

        DataPointState( const DataPointState & dps )
		:
		m_value( dps.m_value ),
		m_status( dps.m_status ),
		m_timestamp ( dps.m_timestamp )
		{
		}


		virtual ~DataPointState()
		{
		}

        void operator=( const DataPointState & dps )
        {
            if (this == &dps)
            {
                // Don't do anything if it's trying to copy itself
                return;
            }
            
            m_value = dps.m_value;
            m_status = dps.m_status;
		    m_timestamp = dps.m_timestamp;
        }


		/**
		 * DataPointState::getValue()
		 *
		 * @return	the value of the DataPoint
		 *
		 */
		DpValue & getValue()
		{ 
			return m_value; 
		}


		/**
		 * DataPointState::getValue()
		 *
		 * @return	the value of the DataPoint
		 *
		 */
		const DpValue & getValue() const
		{ 
			return m_value; 
		}


		/**
		 * DataPointState::getTimebStructure()
		 *
		 * @return	the time (in secs and msecs since the start of epoch) that the state change was detected
		 *
		 */
		const timeb & getTimebStructure() const
		{ 
			return m_timestamp; 
		}


		/**
		 * DataPointState::getTimestamp()
		 *
		 * @return	the time (in secs since the start of epoch) that the state change was detected
		 *
		 */
		unsigned long getTimestamp() const
		{ 
			return m_timestamp.time; 
		}


		/**
		 * DataPointState::getMsecsTimestamp()
		 *
		 * @return	the msecs of the current time stamp
		 *
		 */
		unsigned long getMsecsTimestamp() const
		{ 
			return m_timestamp.millitm; 
		}


		/**
		 * DataPointState::getStatus()
		 *
		 * @return	the status of the DataPoint's value
		 *
		 */
		TA_Base_Bus::EQualityStatus getStatus() const
		{ 
			return m_status; 
		}


		/**
		 * DataPointState::setValue()
		 *
		 * @param newValue	the new value for this DataPointState
		 *
		 */
		void setValue( const DpValue & newValue ) 
		{ 
			m_value = newValue; 
		}
		

		/**
		 * DataPointState::setTimestamp()
		 *
		 * @param newTimestamp	the new timestamp (in secs) for this DataPointState
		 *
		 */
		void setTimestamp( unsigned long newTimestamp, unsigned short miliSeconds = 0 ) 
		{ 
			m_timestamp.time = newTimestamp; 
			m_timestamp.millitm = miliSeconds;
		}
		

		/**
		 * DataPointState::setTimestamp()
		 *
		 * @param newTimestamp	the new timestamp (in secs and msecs) for this DataPointState
		 *
		 */
		void setTimestamp( const timeb & newTimestamp ) 
		{ 
			m_timestamp = newTimestamp; 
		}
		

		/**
		 * DataPointState::setStatus()
		 *
		 * @param newStatus	the new status of the DataPoint's value
		 *
		 */
		void setStatus ( TA_Base_Bus::EQualityStatus newStatus )
		{ 
			m_status = newStatus;
		} 


		/**
		 * DataPointState::isGoodValue()
		 *
		 * @return	whether the DataPoint is in a 'good' state or not
		 *
		 */
		bool isGoodValue()
		{ 
			return ( ( ( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == m_status ) ||
					   ( TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE == m_status ) ) ? true : false );
		}


	private:

		DpValue m_value;
		TA_Base_Bus::EQualityStatus m_status;
		timeb m_timestamp;
	};

}

#endif
