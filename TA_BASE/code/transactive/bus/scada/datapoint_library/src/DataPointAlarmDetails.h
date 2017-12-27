/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointAlarmDetails.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2015/01/21 14:56:06 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	DataPointAlarmDetails is the abstract base class for alarm rules that
  *	need to be evaluated with each update to a DataPoint's state.
  *	DataPointAlarmDetails evaluate state's for possible alarm conditions,
  *	maintain current alarm status and submit/close the alarm
  *
  */

#ifndef DATAPOINTALARMDETAILS_H
#define DATAPOINTALARMDETAILS_H

#include <string>
#include <sys/timeb.h>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/datapoint_library/src/EventDetailsFactory.h"
#include "bus/scada/datapoint_library/src/IAlarmAckStateUser.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"

#include "core/threads/src/IWorkItem.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"

namespace TA_Base_Core
{
	class AlarmHelper;
	class ThreadPoolSingletonManager;
};

namespace TA_Base_Bus
{
    // forward  declaration
	class DataPointState;
	class EventDetailsFactory;
	class DataPoint;
    class MmsScadaUtility;

    class DataPointAlarmDetails : public TA_Base_Core::AbstractThreadedTimeoutUser,
                                    public IAlarmAckStateUser
	{

	public:

		/**
		 * DataPointAlarmDetails::DataPointAlarmDetails()
		 *
		 * @param	dpEntityKey		the pkey for the DataPoint entity that this
		 *							alarm pertains to
		 *
		 * @param	almEnabled		whether or not to process this alarm
		 *
		 * @param	almDelay		the alarm state must persist for this period
		 *							of time (in seconds) before an alarm is
		 *							generated
		 *
		 * @param	almSeverity		the severity of alarms generated as a result
		 *							of this alarm condition
		 *
		 * @param	almMessage		the message of alarms generated as a result
		 *							of this alarm condition
		 *
		 * @param	almType			the type of alarm
		 *
		 */
		DataPointAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                               const TA_Base_Core::MessageType& alarmType,
                               unsigned long			dpParentEntityKey,
							   unsigned long	 		dpEntityKey,
							   unsigned long			dpEntityTypeKey,
							   unsigned long			regionKey,
							   unsigned long			subystemKey,
							   const std::string &		entityName,
							   bool              		almEnabled,
                       		   int              		almDelay,
                       		   int               		almSeverity,
                       		   const std::string & 	   	almMessage,
							   const std::string & 	   	MMSConfiguration,
							   const TA_Base_Bus::EDataPointAlarms &  almType,
                               const DpValue&           almValue,
                               MmsScadaUtility&         mmsScadaUtility );

		virtual ~DataPointAlarmDetails();


		//
		// operations required by the AbstractThreadedTimeoutUser class
		//


		/**
		*
		* threadedTimerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData);


		/**
		*
		* updateAlarmAckState
		*
		* This method is used as a callback mechnism by the AlarmAckStateProcessor to notify
		* the subscriber of the current status that is specified in the AlarmState object.
		*
		* @param	currentAlarmState	The current alarm status for processing
		*
		*/
		virtual void updateAlarmAckState(EScadaAlarmAckState ackState);


		/**
		*
		* getAlarmAckState
		*
		* This method is used to get alarm ack state
		*
		*/
        virtual EScadaAlarmAckState getAlarmAckState();

        //
		// Operations specific to IWorkItem
		//


		virtual void executeWorkItem();


		/*
		 * DataPointAlarmDetails::updateState()
		 *
		 * Tests a DataPointState for possible alarm condition, or
		 * possible out-of-alarm condition. Submits a DataPointAlarm
		 * to the PLCAlarmEventProcessor if necessary.
		 *
		 * @param OMFlag			The OMF flag associated with the data point
		 * @param newState			The state to be tested
		 * @param alarmInhibited	The flag to indicate if the alarm
		 *							shall not be raised
		 *
		 */
		virtual void updateState ( const std::string & OMFlag, const DataPointState & newState, bool alarmInhibited );

		/*
		 * DataPointAlarmDetails::isEnabled()
		 *
		 * @return whether or not to do any processing of states
		 *
		 */
		virtual bool isEnabled() const { return m_enabled; }

		/*
		 * DataPointAlarmDetails::setEnabled()
		 *
		 * @param newEnabled 	the new enabled setting
		 *
		 */
		virtual void setEnabled( bool newEnabled ) { m_enabled=newEnabled; }


		/*
		 * DataPointAlarmDetails::getDelay()
		 *
		 * @return the configured delay in seconds before an alarm will
		 * 		   be raised. ( 0 is no configured delay )
		 *
		 */
		virtual int	getDelay() const;

		virtual void setDelay( int newDelay );

		/*
		 * DataPointAlarmDetails::getSeverity()
		 *
		 * @return the severity of the alarm
		 *
		 */
		virtual int	getSeverity() const { return m_severity; }

		virtual void setSeverity( int newSeverity ) { m_severity = newSeverity; }


		/*
		 * DataPointAlarmDetails::getSeverity()
		 *
		 * @return the severity of the alarm
		 *
		 */
		virtual const std::string & getMessage() const { return m_message; }

		virtual void setMessage( const std::string & newMessage )
		{
			m_message.assign( newMessage );
		}

		/*
		 * DataPointAlarmDetails::getTimestamp()
		 *
		 * @return the timestamp of the alarm (in secs since the start of UTC epoch)
		 *
		 */
		virtual unsigned long getTimestamp() const { return m_timestamp.time; }


		/*
		 * DataPointAlarmDetails::getTimebStructure()
		 *
		 * @return the timestamp of the alarm (in secs since the start of UTC epoch)
		 *
		 */
		virtual const timeb & getTimebStructure() const { return m_timestamp; }



		/*
		 * DataPointAlarmDetails::getDataPointValueAsString()
		 *
		 * @return	the value string include the unit
		 * @param dpValue	the datapoint value to be convert
		 *
		 */
		virtual std::string getDataPointValueAsString(const DpValue& dpValue);

		/*
		 * DataPointAlarmDetails::isInAlarm()
		 *
		 * @return the current alarm state
		 *
		 */
		virtual bool isInAlarm() const
		{
			if ( true == m_enabled )
			{
				return m_alarmOn;
			}
            
            return false;
		}


		/*
		 * DataPointAlarmDetails::getType()
		 *
		 * @return the type of the alarm (used in the AlarmType field
		 * 		   of generated alarm)
		 *
		 */
		virtual const TA_Base_Bus::EDataPointAlarms & getType() const { return m_type; }


		/*
		 * DataPointAlarmDetails::setState()
		 *
		 * @param state		sets whether the alarm is on or off
		 *
		 */
		virtual void setState( bool state ) { m_alarmOn = state; }


		/*
		 * setEngUnit
		 *
		 * @param engUnit	The eng unit associated with the alarm value
		 *
		 */
		virtual void setEngUnit( const std::string & engUnit );


		/*
		 * setMMSConfiguration
		 *
		 * @param MMSConfiguration	The MMS alarm configuration associated with the alarm value
		 *
		 */
		virtual void setMMSConfiguration( const std::string & MMSConfiguration );


		/*
		 * setDataPointReference
		 *
		 * @param theDataPoint	The data point to call back when the alarm
		 *						condition has changed
		 *
		 */
		virtual void setDataPointReference( DataPoint * theDataPoint );


		/*
		 * closeAlarm()
		 *
		 * If this AlarmDetails is in alarm, then a command is issued to
		 * close the alarm. This function is called prior to modifying the
		 * configuration of a DataPointAlarmDetails object.
		 *
		 * @param dataPointValue	The data point value when alarm is closed
		 * @param timestamp			The timestamp of alarm is closed
		 *
		 */
		virtual void closeAlarm( const DpValue & dataPointValue, const timeb & timestamp);//TD15019


		/*
		 * setOperationMode
		 *
		 * Tell the DataPointAlarmDetails that the DataPoint it belongs to
		 * is currently in Control or Monitor mode.
		 *
		 * @param inControl	The flag indicating the current operation mode,
		 *					true for control and false for monitor
		 *
		 */
		virtual void setOperationMode ( TA_Base_Core::EOperationMode mode );

		/*
		 * DataPointAlarmDetails::getValue()
		 *
		 * @return the value for the alarm rule
		 *
		 */
		virtual const DpValue &	getValue() const { return m_val; }


		/*
		 * DataPointAlarmDetails::setValue()
		 *
		 * @param newValue	the value used in future evaluation of alarm
		 * 					condition
		 *
		 */
		virtual void setValue( const DpValue & newValue ) { m_val = newValue; }

		/*
		 * setAlarmThresholdConstant
		 *
		 * @param alarmThresholdConstant	the threshold constant that the value
		 *									must be below before the alarm condition
		 *									can be normalised
		 *
		 */
		virtual void setAlarmThresholdConstant ( double alarmThresholdConstant )
		{
			m_alarmThresholdConstant = alarmThresholdConstant;
		}

		// Weimin ++ TD13619
		virtual TA_Base_Core::AlarmDetailCorbaDef * getAlarmDetailPointer();
		virtual std::string getAlarmTypeStr() = 0;
		// ++ Weimin TD13619
		
		
	protected:

		/*
		 * DataPointAlarmDetails::generateAlarm()
		 *
		 * Raise or close the alarm with the specified DataPoint value
		 *
		 */
		virtual void generateAlarm ( const DpValue dataPointValue, const timeb & timestamp, bool alarmInhibited, const std::string & OMFlag );


		/*
		 * DataPointAlarmDetails::inAlarm()
		 *
		 * Determines whether a DataPointState constitutes an alarm
		 * state or not. To be implemented by subclasses.
		 *
		 * @param state 	the state to be tested
		 *
		 */
		virtual bool inAlarm( const DataPointState & state ) = 0;

		//Weimin ++ TD13619
		virtual void raiseAlarm ( 
						const DpValue & dataPointValue, 
						const TA_Base_Core::DescriptionParameters & descriptionParameters,
						const TA_Base_Core::DataPointEventDetails & eventDetails,
						const TA_Base_Core::MmsStateType & mmsState
						);

		//++ Weimin TD13619

        const TA_Base_Core::MessageType& m_alarmType;
		const TA_Base_Core::MessageType& m_fireAlarmType;

		unsigned long				m_dataPointParentEntityKey;
		unsigned long				m_dataPointEntityKey;
		unsigned long				m_dataPointEntityTypeKey;
		unsigned long				m_locationKey;
		unsigned long				m_subsystemKey;

		timeb				        m_timestamp;
		timeb						m_currentStateTimestamp;

		std::string					m_entityName;
		std::string					m_message;
		std::string					m_engUnit;
		std::string					m_MMSConfiguration;
		std::string					m_OMFlag;

        EScadaAlarmAckState         m_ackState;
        bool                        m_ackStateSubscribed;

		bool						m_alarmOn;
		bool						m_enabled;
		bool						m_knownState;
		bool						m_clockHasStarted;
		bool						m_inAlarmValueAndAlarmInhibited;
		bool						m_currentAlarmInhibited;

		int							m_delay;
		int							m_severity;

		double						m_alarmThresholdConstant;

		DpvEnumLabelsMap			m_dummyEnumLabelsMap;
		DpvBooleanLabels			m_dummyBooleanLabelsMap;
		DpValue  					m_val;
		DpValue  					m_currentStateValue;
		DataPoint *												m_dataPoint;
        TA_Base_Core::DataPointEntityDataPtr    	            m_dataPointEntityAccess;
		TA_Base_Bus::EDataPointAlarms		                    m_type;
		TA_Base_Core::AlarmHelper &								m_alarmHelper;
		EventDetailsFactory *									m_eventDetailsFactory;
		
		MmsScadaUtility&	                        m_mmsScadaUtility;
		TA_Base_Core::ReEntrantThreadLockable		m_lock;
        //bool                        m_isInControl;
        TA_Base_Core::EOperationMode				m_currentMode;
		bool						m_isFireAlarm;
	};

}

#endif
