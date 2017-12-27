
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/BooleanDataPoint.cpp $
  * @author  J. Welton
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  * BooleanDataPoint represents a single boolean value. It
  * inherits from the DataPoint base class and also
  * includes functionality for alarm rules and conversion
  * to/from engineering units.
  *
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/BooleanDataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/DataPointAudit_MessageTypes.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	BooleanDataPoint::BooleanDataPoint( TA_Base_Core::DataPointEntityDataPtr				dataPointEntityAccess,
										TA_Base_Bus::EDataPointType                 		dpType,
										TA_Base_Bus::EDataPointDataType                 	dpDataType,
                                        ScadaPersistencePtr                                 persistence,
                                        TA_Base_Core::ScadaRootEntityData&                  scadaRootEntityData,
                                        MmsScadaUtility&                                    mmsScadaUtility,
                                        CommandProcessor&                                   commandProcessor )
	:
	DataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor )
	{
		// 
		// Check Datapoint Data Type
		//
		if (dpDataType != TA_Base_Bus::DPT_BOOL_TYPE)
		{
            std::ostringstream desc;
            desc << "Invalid Configuration; BooleanDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}

		//
		// Get the labels for true and false value of the boolean data point.
		// Note that for a boolean data point, there is no meaning of engineering
		// unit, i.e the value is always either 0 or 1.  However, each of the state
		// of the boolean value can have a label defined for it, e.g 0/OFF and 1/ON
		//

		setEngUnitsEnabled( false );
		setTrueLabel( dataPointEntityAccess->getTrueLabel() );
		setFalseLabel( dataPointEntityAccess->getFalseLabel() );

		//
		// set up different alarm types associated with boolean data point
		//
		//Only the input data point can configurate alarm and force value.
		if (false == m_writable)
		{
			configureAlarms();

			//
			// Set up the forced value
			//
			if ( m_forcedState )
	        {
				std::string tempString;

	            DpValue forcedValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
	            m_persistence->readPersistedValue( DP_MANUALLY_DRESSED_VALUE, m_entityKey, tempString );
	            
				forcedValue.setFromPersist( tempString );
			
	            convertToRaw( forcedValue, m_forcedValue );
	            m_forcedEngValue = forcedValue;
	        }
		}
	}

	BooleanDataPoint::~BooleanDataPoint()
	{
	}

    void BooleanDataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
    {
        FUNCTION_ENTRY("updateSynState");
        //only the monitor mode datapoint required to syn the control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			throw( TA_Base_Core::OperationModeException() );
		}

        switch(toupdate.updateType)
        {
        case CompleteDynamicSyncStateUpdate:
            {
                AlarmPropertyVect::const_iterator alarmitr;
                for (alarmitr = toupdate.syncCompleteState.alarmProperties.begin(); alarmitr != toupdate.syncCompleteState.alarmProperties.end(); ++alarmitr) 
                {
                    updateSynAlarmProperties( (*alarmitr) );
                }
            }
            break;
        case AlarmPropertiesUpdate:
            {
                AlarmPropertyVect::const_iterator alarmitr;
                for (alarmitr = toupdate.alarmProperties.begin(); alarmitr != toupdate.alarmProperties.end(); ++alarmitr) 
                {
                    updateSynAlarmProperties( (*alarmitr) );
                }
            }
        	break;
		default:
			break;
        }
        //go to the base class to update the rest
        DataPoint::updateSynState(toupdate);
    }

    void BooleanDataPoint::updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
        // only update when there is difference
        bool foundDifference = false;
        {
            ThreadGuard guard( m_alarmslock );
            for (unsigned long i = 0; i < m_alarms.size() && (!foundDifference); ++ i)
            {
                if (newAlarmProperties.type == m_alarms[i]->getType() &&
                    (newAlarmProperties.enabled != m_alarms[i]->isEnabled()))
                {
                    foundDifference = true;
                }
            }
        }
        
        if (!foundDifference)
        {
            return;
        }
        
        std::vector< std::string > configChanges;
		configChanges.clear();

		switch ( newAlarmProperties.type )
		{
			case TA_Base_Bus::DptBoolTrue:
			{
				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_ENABLED );
				break;
			}
			case TA_Base_Bus::DptBoolFalse:
			{
                configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_ENABLED );
				break;
			}
			case TA_Base_Bus::DptOverScaleHi:
			case TA_Base_Bus::DptOverScaleLo:
			case TA_Base_Bus::DptHiHi:
			case TA_Base_Bus::DptHi:
			case TA_Base_Bus::DptLo:
			case TA_Base_Bus::DptLoLo:		
			case TA_Base_Bus::DptGeneral:	// fall through
                break;
			default:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unkown alarm property type[%d] received in datapoint [%d]", newAlarmProperties.type, m_entityKey );
				break;
		}            
        
        if (configChanges.size() > 0)
        {
            // before update alarm properties, need to refresh its configuration from database
            m_persistence->resetPersistedState(m_entityKey);
            
            // update the alarm detail
            updateSpecificConfiguration ( &configChanges );
        }
    }

	void BooleanDataPoint::setTrueLabel ( const std::string & trueEngUnits )
	{
		m_booleanLabels.first.assign( trueEngUnits );

        ThreadGuard guard( m_alarmslock );
		// for each of the alarm associated with this data point
		DpAlarmList::iterator alarmIter = m_alarms.begin();
		for( ; alarmIter != m_alarms.end(); ++alarmIter )
		{
			// find the alarm object for true value
			if ( TA_Base_Bus::DptBoolTrue == (*alarmIter)->getType() )
			{
				// assign the eng unit to this alarm object
				(*alarmIter)->setEngUnit ( trueEngUnits );
			}
		}
	}


	void BooleanDataPoint::setFalseLabel( const std::string & falseEngUnits )
	{
		m_booleanLabels.second.assign( falseEngUnits );

        ThreadGuard guard( m_alarmslock );
		// for each of the alarm associated with this data point
		DpAlarmList::iterator alarmIter = m_alarms.begin();
		for( ; alarmIter != m_alarms.end(); ++alarmIter )
		{
			// find the alarm object for true value
			if ( TA_Base_Bus::DptBoolFalse == (*alarmIter)->getType() )
			{
				// assign the eng unit to this alarm object
				(*alarmIter)->setEngUnit ( falseEngUnits );
			}
		}
	}

    boost::shared_ptr<DatapointCompleteStaticConfig> BooleanDataPoint::getCompleteStaticConfig()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<DatapointCompleteStaticConfig> toReturn = DataPoint::getCompleteStaticConfig();
        getEngineeringUnits(toReturn->engUnits);
        toReturn->alarmPropertyVect = getAlarmPropertiesVect();
        return toReturn;

    }

    boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> BooleanDataPoint::getCompleteDynamicUpdate()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeudpate = DataPoint::getCompleteDynamicUpdate();
        //set the current alarm properties
        completeudpate->alarmProperties = getAlarmPropertiesVect();
        return completeudpate;
    }

    TA_Base_Bus::AlarmPropertyVect BooleanDataPoint::getAlarmPropertiesVect()
    {
		//
		// Alarm Properties sequence
		//

        ThreadGuard guard( m_alarmslock );
		// set the size of the sequence
		 TA_Base_Bus::AlarmPropertyVect alarmProperties;
		// for each of the alarm defined for this data point
		for (int index = 0; index < (int) m_alarms.size(); index++ )
		{
			// create an AlarmProperties on stack
			TA_Base_Bus::AlarmProperty almProperty;

			// alarm type
			almProperty.type = m_alarms[index]->getType();

			// alarm enabled
			almProperty.enabled = m_alarms[index]->isEnabled();

            //set up the thresold
            almProperty.threshold = 0;
            almProperty.enumValue = 0;
			// assign to sequence
			alarmProperties.push_back(almProperty);
		}

        return alarmProperties;
    }

    void BooleanDataPoint::setAlarmProperties( const std::string& sessionID, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}


		std::vector< std::string > configChanges;
		configChanges.clear();

		bool almEnabled = newAlarmProperties.enabled;

        bool tempBool;

		// update the database if there is a difference
		bool result = true;
		switch ( newAlarmProperties.type )
		{
			case TA_Base_Bus::DptBoolTrue:
			{
                //TD15446
                m_persistence->readPersistedValue( DP_TRUE_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_TRUE_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_TRUE_ALARM_ENABLED, m_entityKey, almEnabled ) )
					{
						submitAuditMessage( sessionID, alarmEnabledDescriptionParameters,
                                            almEnabled ? DataPointAudit::DataPointLimitsAdd : 
                                                         DataPointAudit::DataPointLimitsRemove );
					}
					// if failed
					else
					{
                        NameValuePair alarmParam2( "LimitAction", almEnabled ? "Enabled" : "Disabled" );
                        alarmEnabledDescriptionParameters.push_back ( &alarmParam2 );

						submitAuditMessage( sessionID, alarmEnabledDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_ENABLED );

				break;
			}
			case TA_Base_Bus::DptBoolFalse:
			{
                m_persistence->readPersistedValue( DP_FALSE_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_FALSE_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_FALSE_ALARM_ENABLED, m_entityKey, almEnabled ) )
					{
						submitAuditMessage( sessionID, alarmEnabledDescriptionParameters,
                                            almEnabled ? DataPointAudit::DataPointLimitsAdd : 
                                                         DataPointAudit::DataPointLimitsRemove );
					}
					// if failed
					else
					{
                        NameValuePair alarmParam2( "LimitAction", almEnabled ? "Enabled" : "Disabled" );
                        alarmEnabledDescriptionParameters.push_back ( &alarmParam2 );

						submitAuditMessage( sessionID, alarmEnabledDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_ENABLED );

				break;
			}
			case TA_Base_Bus::DptOverScaleHi:
			case TA_Base_Bus::DptOverScaleLo:
			case TA_Base_Bus::DptHiHi:
			case TA_Base_Bus::DptHi:
			case TA_Base_Bus::DptLo:
			case TA_Base_Bus::DptLoLo:		
			case TA_Base_Bus::DptGeneral:	// fall through
			default:
			{
				// throw exception to indicate cannot change the alarm details
				throw TA_Base_Core::BadParameterException();

				break;
			}
		}

		// if database operations are ok
		if ( true == result )
		{
			// update the alarm detail
			updateSpecificConfiguration ( &configChanges );

			// submit an update event for configuration changes so that clients
			// of the data point is aware of the changes
			configChanges.clear();
            
            //the alarm status update also required if after changing the dp value, dp is in alarm
            boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmconfigupdate(new DpDynamicSyncUpdate);
            
            alarmconfigupdate->updateType = TA_Base_Bus::AlarmPropertiesUpdate;
            alarmconfigupdate->alarmProperties.resize(1);
            alarmconfigupdate->alarmProperties.at(0).type = newAlarmProperties.type;
            alarmconfigupdate->alarmProperties.at(0).enabled = almEnabled;
            alarmconfigupdate->alarmProperties.at(0).threshold = 0;
            alarmconfigupdate->alarmProperties.at(0).enumValue = 0;
			submitUpdate ( alarmconfigupdate );

			// save the session id of the operator who initiated this operation
			struct timeb currentSystemTime;
			ftime ( &currentSystemTime );
			saveLastOperatorInfo ( sessionID, currentSystemTime );
		}
		else
		{
			throw TA_Base_Bus::ScadaCorbaTypes::PersistenceFailed();
		}
    }
    
	void BooleanDataPoint::getEngineeringUnits(
			 TA_Base_Bus::DataPointEngineeringUnits & eng )
	{
		// there is no engineering units for a boolean data point
		eng.EngineeringUnitsEnabled = false;
		eng.LowLimit = 0;
		eng.HighLimit = 0;
		eng.Units = "" ;
	}


    std::string BooleanDataPoint::getDataPointValueAsString( const DpValue & dpValue )
    {
        FUNCTION_ENTRY("getDataPointValueAsString");

        if ( dpValue.getBoolean() )
        {
            FUNCTION_EXIT;
            return getTrueLabel();
        }
        
        FUNCTION_EXIT;
        return getFalseLabel();
    }


    void BooleanDataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
	{
		// for each of the changed parameters
		for ( unsigned int i = 0; i < configChanges->size(); i++ )
		{
			std::string paramName = (*configChanges)[i];

			if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_UNITS_ENABLED )
			{
				// do nothing
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ENGINEERING_UNIT )
			{
				// label for true value
				setTrueLabel( m_dataPointEntityAccess->getTrueLabel() );
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ENGINEERING_UNIT )
			{
				// label for true value
				setFalseLabel( m_dataPointEntityAccess->getFalseLabel() );
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_ENABLED ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TRUE_ALARM_SEVERITY )
			{
				//TD15446
				configureTrueAlarm();
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_ENABLED ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_FALSE_ALARM_SEVERITY )
			{
				//TD15446
				configureFalseAlarm();
			}
			else
			{
				// do nothing
			}
		}
	}

	const std::string & BooleanDataPoint::getTrueLabel() const
	{
		return m_booleanLabels.first;
	}


	const std::string & BooleanDataPoint::getFalseLabel() const
	{
		return m_booleanLabels.second;
	}



	void BooleanDataPoint::setToControlMode()
	{
		// call the base class
		DataPoint::setToControlMode();
	}


	void BooleanDataPoint::setToMonitorMode()
	{
		// call the base class
		DataPoint::setToMonitorMode();
	}


	void BooleanDataPoint::configureAlarms()
	{
		//for booleanDataPoint only one alarm is enaled.
		bool trueAlarmEnabled = false;
        m_persistence->readPersistedValue( DP_TRUE_ALARM_ENABLED, m_entityKey, trueAlarmEnabled );
		if(trueAlarmEnabled)
		{
			configureTrueAlarm();
		}
		else
		{
			bool falseAlarmEnabled;
        	m_persistence->readPersistedValue( DP_FALSE_ALARM_ENABLED, m_entityKey, falseAlarmEnabled );
			if(falseAlarmEnabled)
			{
				configureFalseAlarm();
			}
		}
	}


	void BooleanDataPoint::configureTrueAlarm()
	{
		DpValue almValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		almValue.setBoolean( true );

		setAlarmDetails( TA_Base_Bus::DptBoolTrue,
						 true, 
						 (int) m_dataPointEntityAccess->getTrueAlarmDelay(),
						 m_dataPointEntityAccess->getTrueAlarmSeverity(),
						 m_dataPointEntityAccess->getTrueAlarmMessage(),
						 almValue,
						 m_booleanLabels.first,
						 m_dataPointEntityAccess->getTrueAlarmMMS() );
	}


	void BooleanDataPoint::configureFalseAlarm()
	{
		DpValue almValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
        almValue.setBoolean( false );

		setAlarmDetails( TA_Base_Bus::DptBoolFalse,
						 true, 
						 (int) m_dataPointEntityAccess->getFalseAlarmDelay(),
						 m_dataPointEntityAccess->getFalseAlarmSeverity(),
						 m_dataPointEntityAccess->getFalseAlarmMessage(),
						 almValue,
						 m_booleanLabels.second,
						 m_dataPointEntityAccess->getFalseAlarmMMS() );
	}

}

