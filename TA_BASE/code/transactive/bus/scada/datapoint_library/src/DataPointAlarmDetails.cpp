/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointAlarmDetails.cpp $
  * @author  J. Welton
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  *	DataPointAlarmDetails is the abstract base class for alarm rules that
  *	need to be evaluated with each update to a DataPoint's state.
  *	DataPointAlarmDetails evaluate state's for possible alarm conditions,
  *	maintain current alarm status and submit/close the alarm
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "time.h"
#include "ace/OS.h"

#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"
#include "bus/scada/datapoint_library/src/EventDetailsFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"
#include "core/message/types/DataPointAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

    DataPointAlarmDetails::DataPointAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                                                  const TA_Base_Core::MessageType& alarmType,
                                                  unsigned long			dpParentEntityKey,
												  unsigned long			dpEntityKey,
												  unsigned long			dpEntityTypeKey,
												  unsigned long			locationKey,
												  unsigned long			subsystemKey,
												  const std::string &	entityName,
												  bool 					almEnabled,
												  int  					almDelay,
												  int  					almSeverity,
												  const std::string & 	almMessage,
												  const std::string &	MMSConfiguration,
												  const TA_Base_Bus::EDataPointAlarms & almType,
                                                  const DpValue&           almValue,
                                                  MmsScadaUtility&      mmsScadaUtility )
	:
    m_alarmType( alarmType ),
	m_dataPointParentEntityKey( dpParentEntityKey ),
	m_dataPointEntityKey( dpEntityKey ),
	m_dataPointEntityTypeKey( dpEntityTypeKey ),
	m_locationKey( locationKey ),
	m_subsystemKey( subsystemKey ),
    m_timestamp( TA_Base_Core::AlarmConstants::defaultTime ),
    m_currentStateTimestamp( TA_Base_Core::AlarmConstants::defaultTime ),
	m_entityName( entityName ),
	m_message( almMessage ),
    m_engUnit( "" ),			// default to "" unless it is explicitly set
    m_MMSConfiguration( MMSConfiguration ),
    m_OMFlag( "" ),
    m_ackState( ScadaAlarmUnknown ),
    m_ackStateSubscribed( false ),
    m_alarmOn( true ),		// assume alarm is on when created
	m_enabled( almEnabled ),
    m_knownState( false ),	// assume unknown state when created
    m_clockHasStarted( false ), // assume there is no delay timer is used
	m_inAlarmValueAndAlarmInhibited( false ),
    m_currentAlarmInhibited( false ),
	m_delay( almDelay ),
	m_severity( almSeverity ),
	m_alarmThresholdConstant( 0.0 ),
    m_val(DPT_NULL_TYPE, m_dummyEnumLabelsMap, m_dummyBooleanLabelsMap),
	m_currentStateValue(DPT_NULL_TYPE, m_dummyEnumLabelsMap, m_dummyBooleanLabelsMap),
	m_dataPoint( NULL ),
    m_dataPointEntityAccess(dataPointEntityAccess),
	m_type( almType ),
	m_alarmHelper(AlarmHelperManager::getInstance().getAlarmHelper()),
	m_eventDetailsFactory( NULL ),
	m_mmsScadaUtility( mmsScadaUtility ),
    //m_isInControl(false)
	m_isFireAlarm(false),
	m_fireAlarmType(TA_Base_Core::DataPointAlarms::FireAlarm),
    m_currentMode(TA_Base_Core::Standby)
	{
        setValue( almValue );
		std::string strSpecialAlarmType = m_dataPointEntityAccess->getSpecialAlarmType();

		if (0 == strSpecialAlarmType.compare("FireAlarm"))
		{
			m_isFireAlarm = true ;
		}
	}


	DataPointAlarmDetails::~DataPointAlarmDetails()
	{
        // to check if the alarm type is DpGeneral for EnumDataType DataPoint
        bool isDpGeneral = atol(m_alarmType.getTypeKey().c_str()) == atol(TA_Base_Core::DataPointAlarms::DpGeneral.getTypeKey().c_str());

        // unsubscribe alarm ack state
        AlarmAckStateProcessor::getInstance().unsubscribeToAlarmType(
            m_alarmType,
            isDpGeneral ? getValue().getValueAsString() : "NotUsed",
            m_dataPointEntityKey,
            m_locationKey);


		if ( 0 < m_delay )
		{
			// make sure there is no timeout clock allocated for this object
			SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
			m_clockHasStarted = false;
		}

		// Removed handle to the EventDetailsFactory
		if ( NULL != m_eventDetailsFactory )
		{
			EventDetailsFactory::removeInstance ( m_eventDetailsFactory );
			m_eventDetailsFactory = NULL;
		}

		// Removed reference to the DataPoint, but don't delete the object
		m_dataPoint = NULL;
	}

    void DataPointAlarmDetails::updateAlarmAckState(EScadaAlarmAckState ackState)
    {
        if (m_ackState != ackState)
        {
            m_ackState = ackState;
            
            if ( NULL != m_dataPoint )
            {
                m_dataPoint->updateAlarmAckState();
            }
        }
    }
    
    EScadaAlarmAckState DataPointAlarmDetails::getAlarmAckState()
    {
        return m_ackState;
    }
    
	void DataPointAlarmDetails::updateState( const std::string & OMFlag,
											 const DataPointState & newState,
											 bool alarmInhibited )
	{
		// only proceed if alarm is enabled...
		if ( m_enabled )
		{
			{
				// prevent multiple threads attempting to modify alarm data
				TA_Base_Core::ThreadGuard guard( m_lock );

				// save the current alarm data
				m_currentStateValue = newState.getValue();
				m_currentStateTimestamp = newState.getTimebStructure();
				m_currentAlarmInhibited = alarmInhibited;
				m_OMFlag = OMFlag;
			}

			// if alarm flag is on
			if ( m_alarmOn )
			{
				// if the new state is in alarm
				if ( inAlarm ( newState ) )
				{
					// and the alarm object is in unknown state, i.e this
					// is the first time it is called to update state
					if ( false == m_knownState || true == m_inAlarmValueAndAlarmInhibited)
					{
						// schedule a work item in ThreadPool for this latest update
						//m_threadPool->queueWorkItem ( m_workItem );
						generateAlarm(newState.getValue(), newState.getTimebStructure(), alarmInhibited, OMFlag);
						// set the flag to indicate it is in known state
						m_knownState = true;
					}
					// else do nothing because there is already an alarm
					// being raised
				}

				// if the new state is not in alarm
				else
				{
					// no longer in alarm state, reset the alarm flag
					m_alarmOn = false;

					// schedule a work item in ThreadPool for this latest update
					//m_threadPool->queueWorkItem ( m_workItem );
					generateAlarm(newState.getValue(), newState.getTimebStructure(), alarmInhibited, OMFlag);
				}
			}

			// if the alarm flag is not on
			else
			{
				// if the new state is in alarm
				if ( inAlarm( newState ) )
				{
					//
					// if there is a configured alarm delay
					//
					if ( m_delay > 0 )
					{
						// if the timer has not been started
						if ( false == m_clockHasStarted )
						{
							// start the timeout clock
							SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, m_delay * 1000 );
							m_clockHasStarted = true;
						}
						// else do nothing, wait for the timer to expired
					}

					//
					// if there is no configured alarm delay, raise the alarm now
					//
					else
					{
						// set the flag
						m_alarmOn = true;

						// schedule a work item in ThreadPool for this latest update
						//m_threadPool->queueWorkItem ( m_workItem );
						generateAlarm(newState.getValue(), newState.getTimebStructure(), alarmInhibited, OMFlag);
					}
				}

				// if the new state is not in alarm
				else
				{
					// if there is alarm delay and the timer has been started
					if ( ( m_delay > 0 ) && ( true == m_clockHasStarted ) )
					{
						// stop the timer
						SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock ( this );
						m_clockHasStarted = false;
					}
					// else do nothing
				}
			}
		}	
	}

	//TD15019
	std::string DataPointAlarmDetails::getDataPointValueAsString(const DpValue& dpValue)
	{
		// if eng unit is defined
		std::stringstream ss;
		ss << dpValue.getValueAsString();

		if ( false == m_engUnit.empty() )
		{
			// if reference to the data point is defined
			if ( NULL != m_dataPoint )
			{
				// if the data point is of type boolean/derived/enum
				if ( ( false == m_dataPoint->getIsDigitalDataPoint() ) &&
                    ( TA_Base_Bus::DPT_DERIVED != m_dataPoint->getDataPointType() ) &&
					( TA_Base_Bus::DPT_ENUM != m_dataPoint->getDataPointType() ) )
				{
					// attach the eng unit to the value, e.g '100 (Kwh)'
					ss << " (" << m_engUnit << ")";
				}
			}
			else
			{
				// attach the eng unit to the value, e.g '100 (Kwh)'
				ss << " (" << m_engUnit << ")";
			}
		}

		return ss.str();
		
	}

	void DataPointAlarmDetails::generateAlarm( const DpValue dataPointValue, const timeb & timestamp, bool alarmInhibited, const std::string & OMFlag )
	{
        FUNCTION_ENTRY("generateAlarm");

		// get the handle to an instance of the EventDetailsFactory
		// PW 3046
		if ( NULL == m_eventDetailsFactory )
		{
			m_eventDetailsFactory = EventDetailsFactory::getInstance();
		}

		const TA_Base_Core::DataPointEventDetails& eventDetails = 
				m_eventDetailsFactory->getEventDetails(m_dataPointEntityKey, m_dataPointEntityAccess->getAgentName());

		// check if MMS alarm is inhibited from the ScadaUtility

		// get handle to MMS ScadaUtility if not done so
		bool isDataPointMmsInhibited = false;

        try
        {
            // change the way for MMS inhibition status checking since dp itself should know its own inhibition status.
            // isDataPointMmsInhibited = m_mmsScadaUtility.isDataPointMmsInhibited( m_dataPointEntityKey, m_dataPointEntityAccess );
            // check if this DP itself is MMS inhibited
            isDataPointMmsInhibited = m_dataPoint->getMMSAlarmInhibitedState();
            if (!isDataPointMmsInhibited)
            {
                // check if the parent equipment DN is MMS inhibited which normally hold by the same agent itself.
                isDataPointMmsInhibited = m_mmsScadaUtility.isMmsSubsystemInhibited(m_dataPoint->getParentEntityKey());
            }
            if (!isDataPointMmsInhibited)
            {
                // check if global MMS inhibited.
                isDataPointMmsInhibited = m_mmsScadaUtility.isMmsGlobalInhibited();
            }
	    }
        catch (const TA_Base_Core::TransactiveException& te )
        {
            std::ostringstream desc;
            desc << "Caught TransactiveException while trying to check datapoint (" 
                 << m_entityName
                 << ") MMS Inhibited status; Reason: "
                 << te.what();
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", desc.str().c_str() );
        }
	    catch ( ... )
	    {
            std::ostringstream desc;
            desc << "Caught unknown exception while trying to check datapoint(" 
                 << m_entityName
                 << ") MMS Inhibited status";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc.str().c_str() );
	    }

        //if (!m_isInControl)
        if(m_currentMode != TA_Base_Core::Control)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Alarm is ingored in monitor mode." );
            return;
        }
		//
		// raise alarm only if it is not alarm inhibited 
		//
		if ( false == alarmInhibited )
		{
			// reset the flag so event(ValueInAlarmButAlarmInhibited) will not be blocked
			if ( true == m_inAlarmValueAndAlarmInhibited )
			{
				m_inAlarmValueAndAlarmInhibited = false;
			}

			//if ( 0 != m_alarmHelper )
			{
				time_t ltime;
				struct tm gmt;
				ltime = timestamp.time;
                ACE_OS::gmtime_r( &ltime, &gmt );
				std::stringstream ss;

                //const unsigned int bufLen = 27;
                const unsigned int bufLen = 35;
                char buf[bufLen];

				if ( true == m_alarmOn )
				{
					if ( 0 == m_severity )
					{
                        ss << m_entityName << ": raise alarm without severity, value = " << dataPointValue.getValueAsString() 
							<< ", alarm type = " << getAlarmTypeStr() << ", UTC timestamp = " << ACE_OS::asctime_r( &gmt, buf, bufLen );
					}
					else
					{
						ss << m_entityName << ": raise alarm with severity = " << m_severity << ", value = " << dataPointValue.getValueAsString() 
							<< ", alarm type = " << getAlarmTypeStr() << ", UTC timestamp = " << ACE_OS::asctime_r( &gmt, buf, bufLen );
					}
				}
				else
				{
					ss << m_entityName << ": close alarm, value = " << dataPointValue.getValueAsString() 
						<< ", alarm type = " << getAlarmTypeStr() << ", UTC timestamp = " << ACE_OS::asctime_r( &gmt, buf, bufLen );
				}

				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, ss.str().c_str() );

				// construct the DescriptionParameters list
				// PW 3047: attached eng unit to the value where appropriated
				// PW 3158: added new parameter OMFlag
				DescriptionParameters descriptionParameters;
				descriptionParameters.clear();

				NameValuePair pair1 ( "AlarmMessage", m_message );
				NameValuePair pair2 ( "DataPointValue", getDataPointValueAsString(dataPointValue) );
				NameValuePair pair3 ( "OMFlag", OMFlag );
				descriptionParameters.push_back ( &pair1 );
				descriptionParameters.push_back ( &pair2 );
				descriptionParameters.push_back ( &pair3 );

				// save the timestamp of the alarm
				m_timestamp = timestamp;

				TA_Base_Core::MmsStateType mmsState = MMS_NONE;
				if ( true == isDataPointMmsInhibited )
				{
					mmsState = MMS_INHIBIT;
				}
				else
				{
					if ( TA_Base_Core::DataPointEntityData::DATAPOINT_MMS_AUTO_STRING == m_MMSConfiguration )
					{
						mmsState = MMS_AUTO;
					}
					else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_MMS_SEMI_STRING == m_MMSConfiguration )
					{
						mmsState = MMS_SEMI;
					}
					else
					{
						mmsState = MMS_NONE;
					}
				}

				//Weimin ++ TD13619.
				try
				{
					raiseAlarm( dataPointValue, descriptionParameters, eventDetails, mmsState );
				}
				catch( const TA_Base_Core::TransactiveException & te)
				{
	                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
				}
				catch ( ... )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught unknown exception while raise alarm.");
				}
				//++ Weimin TD13619
			}
			// do nothing if alarm helper is not available
		}
		// if alarm functionality is inhibited
		else
		{
			//
			// generate an event for the new state
			//
			std::string alarmTypeStr;

			//Weimin ++ TD13619, to call derived class's function instead of write switch statement.
			alarmTypeStr = getAlarmTypeStr();

			// construct the DescriptionParameters
			TA_Base_Core::DescriptionParameters descriptionParameters;
			descriptionParameters.clear();
			NameValuePair pair1 ( "AlarmType", alarmTypeStr );
			NameValuePair pair2 ( "ValueString", ( m_dataPoint->getCurrentEngValue().getValueAsString() ) );
			descriptionParameters.push_back ( &pair2 );
			descriptionParameters.push_back ( &pair1 );

			// if the new state is in alarm
			if ( true == m_alarmOn )
			{
				// only if not already done
				if ( false == m_inAlarmValueAndAlarmInhibited )
				{
					m_dataPoint->submitAuditMessage ( "", descriptionParameters, TA_Base_Core::DataPointAudit::ValueInAlarmButAlarmInhibited );

					// set the flag
					m_inAlarmValueAndAlarmInhibited = true;
				}
			}

			// if the new state is not in alarm
			else
			{
				// only if an event has been generated
				if ( true == m_inAlarmValueAndAlarmInhibited )
				{
					m_dataPoint->submitAuditMessage ( "", descriptionParameters, TA_Base_Core::DataPointAudit::ValueOutOfAlarmButAlarmInhibited );

					// reset the flag
					m_inAlarmValueAndAlarmInhibited = false;
				}				
			}
		}
        FUNCTION_EXIT;
	}

	void DataPointAlarmDetails::setEngUnit ( const std::string & engUnit )
	{
		m_engUnit = engUnit;
	}


	void DataPointAlarmDetails::setMMSConfiguration( const std::string & MMSConfiguration )
	{
		m_MMSConfiguration = MMSConfiguration;
	}


	void DataPointAlarmDetails::setDataPointReference( DataPoint * theDataPoint )
	{
		// save the reference to the specified data point
		m_dataPoint = theDataPoint;
	}


	void DataPointAlarmDetails::threadedTimerExpired(long timerId, void* userData)
	{
	
		// the delay period has expired, stop the timer
		SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock ( this );
		m_clockHasStarted = false;

	
		// check if the data point's current state is still in alarm
		TA_Base_Bus::DataPointState newState ( m_dataPoint->getCurrentValue(),
											   m_dataPoint->getTimebStructure(),
											   m_dataPoint->getQualityStatus() );

		// if the data point current state is in alarm
		if ( inAlarm ( newState ) )
		{
			// generate the delayed alarm now
			m_alarmOn = true;

			bool isAlarmInhibited = ( true == m_dataPoint->getAlarmInhibitedState() ||
									  false == m_dataPoint->isDataPointInService() );

			generateAlarm ( m_dataPoint->getCurrentValue(),
							m_dataPoint->getTimebStructure(),
							isAlarmInhibited,
							m_dataPoint->getOMFlag() );

			// request the data point to re evaluate the data point alarm condition,
			// based on its current eng value
			m_dataPoint->reEvaluateAlarmCondition ( m_type, m_alarmOn );
		}
	}


	void DataPointAlarmDetails::executeWorkItem()
	{

		// callback from ThreadPool for scheduled work
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "alarms are triggered here" );

        // use stack variables to avoid blocking other threads
        DpValue  					currentStateValue(DPT_NULL_TYPE, m_dummyEnumLabelsMap, m_dummyBooleanLabelsMap);
        timeb						currentStateTimestamp;
        bool						currentAlarmInhibited;
        std::string					strOMFlag;

        {
		    // prevent multiple threads attempting to modify alarm data
            TA_Base_Core::ThreadGuard guard( m_lock );

            currentStateValue = m_currentStateValue;
            currentStateTimestamp = m_currentStateTimestamp;
            currentAlarmInhibited = m_currentAlarmInhibited;
            strOMFlag = m_OMFlag;
        }

        generateAlarm ( currentStateValue,
						currentStateTimestamp,
						currentAlarmInhibited,
						strOMFlag );
	}

	int	DataPointAlarmDetails::getDelay() const
	{
		return m_delay;
	}


	void DataPointAlarmDetails::setDelay ( int newDelay )
	{
		// check of the new delay value
		int delay = newDelay;
		if ( 0 > delay )
		{
			// default to 0 if the new delay is set to -ve value
			delay = 0;

			// if there is a current alarm delay and the timer has been started
			if ( ( m_delay > 0 ) && ( true == m_clockHasStarted ) )
			{
				// fast forward the clock to generate the delayed alarm
				timerExpired(0,NULL);

				// make sure there is no timeout clock allocated for this object
				SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock ( this );
				m_clockHasStarted = false;
			}
		}

		// save the new delay
		m_delay = delay;
	}

	void DataPointAlarmDetails::setOperationMode ( TA_Base_Core::EOperationMode mode )
	{
		if(mode == m_currentMode)
		{
			return;
		}
		
		switch(mode)
		{
			case TA_Base_Core::Control:
			case TA_Base_Core::Monitor:
				if (!m_ackStateSubscribed)
				{
					AlarmAckStateProcessor::getInstance().requestLoadAlarms(m_locationKey);

                	// to check if the alarm type is DpGeneral for EnumDataType DataPoint
                	bool isDpGeneral = atol(m_alarmType.getTypeKey().c_str()) == atol(TA_Base_Core::DataPointAlarms::DpGeneral.getTypeKey().c_str());

                	LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "subcribe alarm for: entity [%u]", m_dataPointEntityKey);
                	m_ackStateSubscribed = true;

                	// subscribe alarm ack state
                	AlarmAckStateProcessor::getInstance().subscribeToAlarmType(
                    m_alarmType,
                    isDpGeneral ? getValue().getValueAsString() : "NotUsed",
                    this,
                    m_dataPointEntityKey,
                    m_locationKey);
            	}
				m_currentMode = mode;
				break;
			case TA_Base_Core::Standby:
				m_ackStateSubscribed = false;
				m_currentMode = mode;
				break;
			default:
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "unknown mode %d", mode);
				break;
		}
	}


    void DataPointAlarmDetails::closeAlarm( const DpValue & dataPointValue, const timeb & timestamp )
    {
        //if (!m_isInControl)
		if(m_currentMode != TA_Base_Core::Control)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "CloseAlarm is ingored in monitor mode." );
            return;
        }

        if ( m_alarmOn )
        {
            m_alarmOn = false;
            
			if (m_isFireAlarm)
			{
				// close all FalsAlarms for entity associated with this request
				m_alarmHelper.closeAlarmAtLocation (
					m_fireAlarmType,
					m_dataPointEntityKey,
					m_locationKey,
					getDataPointValueAsString(dataPointValue), //TD15019
					timestamp);
			}
			else
			{
				// close all FalsAlarms for entity associated with this request
				m_alarmHelper.closeAlarmAtLocation (
					m_alarmType,
					m_dataPointEntityKey,
					m_locationKey,
					getDataPointValueAsString(dataPointValue), //TD15019
					timestamp);
			}
        }
        
        m_timestamp = timestamp;
    }
    

    void DataPointAlarmDetails::raiseAlarm ( const DpValue & dataPointValue, 
        const TA_Base_Core::DescriptionParameters & descriptionParameters,
        const TA_Base_Core::DataPointEventDetails & eventDetails,
        const TA_Base_Core::MmsStateType & mmsState
        )
    {
        FUNCTION_ENTRY("raiseAlarm");

		if (m_isFireAlarm)
		{
			if ( true == m_alarmOn )
			{
				if ( 0 == m_severity )
				{
					//submit an unique DpGeneral alarm without severity
					m_alarmHelper.submitAlarmWithAsset(
						m_fireAlarmType,
						m_dataPointEntityKey,
						m_dataPointEntityTypeKey,
						descriptionParameters,
						m_entityName,
						m_locationKey,
						m_subsystemKey,
						eventDetails.assetName,
						m_timestamp );
				}
				else
				{
					//submit an unique DpGeneral
					m_alarmHelper.submitAlarmWithSeverityAndAsset(
						m_fireAlarmType,
						m_dataPointEntityKey,
						m_dataPointEntityTypeKey,
						descriptionParameters,
						m_entityName,
						m_locationKey,
						m_subsystemKey,
						m_severity,
						eventDetails.assetName,
						m_timestamp,
						mmsState );
				}
			}
			else
			{
				// close all DpFalse for entity associated with this request
				m_alarmHelper.closeAlarmAtLocation(
					m_fireAlarmType,
					m_dataPointEntityKey,
					m_locationKey,
					getDataPointValueAsString(dataPointValue), //TD15019
					m_timestamp);
			}
		}
		else
		{
			if ( true == m_alarmOn )
			{
				if ( 0 == m_severity )
				{
					//submit an unique DpGeneral alarm without severity
					m_alarmHelper.submitAlarmWithAsset(
						m_alarmType,
						m_dataPointEntityKey,
						m_dataPointEntityTypeKey,
						descriptionParameters,
						m_entityName,
						m_locationKey,
						m_subsystemKey,
						eventDetails.assetName,
						m_timestamp );
				}
				else
				{
					//submit an unique DpGeneral
					m_alarmHelper.submitAlarmWithSeverityAndAsset(
						m_alarmType,
						m_dataPointEntityKey,
						m_dataPointEntityTypeKey,
						descriptionParameters,
						m_entityName,
						m_locationKey,
						m_subsystemKey,
						m_severity,
						eventDetails.assetName,
						m_timestamp,
						mmsState );
				}
			}
			else
			{
				// close all DpFalse for entity associated with this request
				m_alarmHelper.closeAlarmAtLocation(
					m_alarmType,
					m_dataPointEntityKey,
					m_locationKey,
					getDataPointValueAsString(dataPointValue), //TD15019
					m_timestamp);
			}
		}
        FUNCTION_EXIT;
    }
    
    
    TA_Base_Core::AlarmDetailCorbaDef * DataPointAlarmDetails::getAlarmDetailPointer()
    {
		if (m_isFireAlarm)
		{
			return 	m_alarmHelper.getActiveAlarmForEntityOfType(
				m_fireAlarmType,
				m_dataPointEntityKey,
				m_locationKey );
		}
		else
		{
			return 	m_alarmHelper.getActiveAlarmForEntityOfType(
				m_alarmType,
				m_dataPointEntityKey,
				m_locationKey );
		}
        
    }
    
}
