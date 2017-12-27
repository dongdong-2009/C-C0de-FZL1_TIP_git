#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER



#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/MultiBitDataPoint.h"

#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include <math.h>
#include <time.h>

using namespace TA_Base_Core;



namespace TA_Base_Bus
{
		MultiBitDataPoint::MultiBitDataPoint ( TA_Base_Core::DataPointEntityDataPtr		            dataPointEntityAccess,
						   TA_Base_Bus::EDataPointType		                    dpType,
						   TA_Base_Bus::EDataPointDataType	                    dpDataType,
                           ScadaPersistencePtr                                  persistence,
                           TA_Base_Core::ScadaRootEntityData&                   scadaRootEntityData,
                           MmsScadaUtility&                                     mmsScadaUtility,
                           CommandProcessor&                                    commandProcessor )
		:
    DataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor ),
    m_MultiBitDataPointEntityData( NULL )
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Construct MutiBitDataPoint,data type=%d",dpDataType);
        // Check Datapoint Data TypE
        if (dpDataType != TA_Base_Bus::DPT_ENUM_TYPE)
        {
            std::ostringstream desc;
            desc << "Invalid Configuration; DerivedDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
        }
		m_enumLabelsMap.clear();
		this->initialise();
    }
		
		
		MultiBitDataPoint::~MultiBitDataPoint()
		{
			if(m_MultiBitDataPointEntityData)
			{
				delete m_MultiBitDataPointEntityData;
				m_MultiBitDataPointEntityData=NULL;
			}

			std::vector < ConfiguredDerivedState * >::iterator cdsItr;
			for ( cdsItr = m_listOfConfiguredDerivedStates.begin(); cdsItr != m_listOfConfiguredDerivedStates.end(); cdsItr++ )
            {
				delete (*cdsItr);
				*cdsItr = NULL;
			}
			m_listOfConfiguredDerivedStates.clear();
		}
		
		bool MultiBitDataPoint::getIsBitAddressMSB()
		{
			return m_isBigEndian;
		}
		
		void MultiBitDataPoint::initialise()
		{
			if ( NULL == m_MultiBitDataPointEntityData )
			{
				m_MultiBitDataPointEntityData = new DerivedDataPointEntityData ( m_entityKey, m_name );//m_dataPointEntityAccess->getAllowWrite());
			}
			this->initialiseEnumMap();
			
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
		
		void MultiBitDataPoint::initialiseEnumMap()
		{
			m_enumLabelsMap.clear();
			
			m_listOfConfiguredDerivedStates.clear();
            m_MultiBitDataPointEntityData->getConfiguredDerivedStates ( m_listOfConfiguredDerivedStates );

           
            std::vector < ConfiguredDerivedState * >::iterator cdsItr;
            for ( cdsItr = m_listOfConfiguredDerivedStates.begin(); cdsItr != m_listOfConfiguredDerivedStates.end(); cdsItr++ )
            {
                m_enumLabelsMap[(*cdsItr)->getStateValue()] = (*cdsItr)->getStateDescription();
                DpValue alarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
                alarmValue.setEnum( (*cdsItr)->getStateValue() );
                setAlarmDetails ( TA_Base_Bus::DptGeneral, // alarm type
                                        (*cdsItr)->getAlarmEnabled(),    // alarm enabled
                                        (*cdsItr)->getAlarmDelay(),      // alarm delay
                                        (*cdsItr)->getAlarmSeverity(),   // alarm severity
                                        (*cdsItr)->getAlarmMessage(),    // alarm message
                                        alarmValue,                             // alarm value
                                        (*cdsItr)->getStateDescription(),        // label
                                        (*cdsItr)->getAlarmMMS() ); // MMS alarm configuration


            }
			
		}

		void MultiBitDataPoint::setAlarmProperties( const std::string& sessionId, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
		{
			if ( TA_Base_Core::Monitor == m_operationMode )
			{
				throw TA_Base_Core::OperationModeException();
			}

			std::ostringstream propertyvalue; 
			propertyvalue << newAlarmProperties.enumValue;
			DpValue almValue (propertyvalue.str(), getDataPointDataType(),
							  getEnumLabelsMap(), getBooleanLabels(), getPrecision());

			bool almEnabled = newAlarmProperties.enabled;

			// update the database if there is a difference
			switch ( newAlarmProperties.type )
			{
				case TA_Base_Bus::DptGeneral:
				{
					// for each of the DerivedState associated with this derived data point
					std::vector < ConfiguredDerivedState * >::iterator cdsItr;
					for ( cdsItr = m_listOfConfiguredDerivedStates.begin(); cdsItr != m_listOfConfiguredDerivedStates.end(); cdsItr++ )
					{
						// if the state value equals to the alarm value
						if ( newAlarmProperties.enumValue == (unsigned long) (*cdsItr)->getStateValue() )
						{
							// update alarm settings
							(*cdsItr)->setAlarmEnabled ( almEnabled );

							// update the alarm detail
							setAlarmDetails ( newAlarmProperties.type,
												 almEnabled,
												 (*cdsItr)->getAlarmDelay(),
												 (*cdsItr)->getAlarmSeverity(),
												 (*cdsItr)->getAlarmMessage(),
												 almValue,
												 (*cdsItr)->getStateDescription(),
												 (*cdsItr)->getAlarmMMS() );
						}
					}

					break;
				}
				case TA_Base_Bus::DptHiHi:      // fall through
				case TA_Base_Bus::DptHi:            // fall through
				case TA_Base_Bus::DptLo:            // fall through
				case TA_Base_Bus::DptLoLo:      // fall through
				case TA_Base_Bus::DptBoolTrue:  // fall through
				case TA_Base_Bus::DptBoolFalse: // fall through
				default:
				{
					// throw exception to indicate cannot change the alarm details
					throw TA_Base_Core::BadParameterException();

					break;
				}
			}

			// submit an update event for configuration changes so that clients
			// of the data point is aware of the changes
			boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmconfigupdate(new DpDynamicSyncUpdate);
	        
			alarmconfigupdate->updateType = TA_Base_Bus::AlarmPropertiesUpdate;
			alarmconfigupdate->alarmProperties.resize(1);
			alarmconfigupdate->alarmProperties.at(0).type = newAlarmProperties.type;
			alarmconfigupdate->alarmProperties.at(0).enabled = newAlarmProperties.enabled;
			alarmconfigupdate->alarmProperties.at(0).threshold = 0;
			alarmconfigupdate->alarmProperties.at(0).enumValue = newAlarmProperties.enumValue;
			submitUpdate ( alarmconfigupdate );


			// save the session id of the operator who initiated this operation
			struct timeb currentSystemTime;
			ftime ( &currentSystemTime );
			saveLastOperatorInfo ( sessionId, currentSystemTime );
		}

		void MultiBitDataPoint::getEngineeringUnits(TA_Base_Bus::DataPointEngineeringUnits & eng )
		{
			// there is no engineering unit for a Derived DataPoint
			eng.EngineeringUnitsEnabled = false;
			eng.LowLimit = 0;
			eng.HighLimit = 0;
			eng.Units = "";
		}

		TA_Base_Bus::AlarmPropertyVect MultiBitDataPoint::getAlarmPropertiesVect()
		{
			//
			// Alarm Properties sequence
			//
			// set the size of the sequence
			 TA_Base_Bus::AlarmPropertyVect alarmProperties;
			// for each of the DerivedState associated with this derived data point
			std::vector < ConfiguredDerivedState * >::iterator cdsItr;
			for ( cdsItr = m_listOfConfiguredDerivedStates.begin(); cdsItr != m_listOfConfiguredDerivedStates.end(); cdsItr++ )
			{
				// create an AlarmProperties on stack
				TA_Base_Bus::AlarmProperty almProperty;

				// alarm type
				almProperty.type = TA_Base_Bus::DptGeneral;

				// alarm enabled
				almProperty.enabled = (*cdsItr)->getAlarmEnabled();

				//set up the thresold
				almProperty.threshold = 0;
				almProperty.enumValue = (*cdsItr)->getStateValue();
				// assign to sequence
				alarmProperties.push_back(almProperty);
			}

			return alarmProperties;
		}
		
		std::string MultiBitDataPoint::getDataPointValueAsString( const DpValue & dpValue )
		{
			FUNCTION_ENTRY("getDataPointValueAsString");
			if ( TA_Base_Core::Monitor == m_operationMode )
			{
				throw TA_Base_Core::OperationModeException();
			}

			// if there are no DerivedState defined for this DerivedDataPoint
			if ( true == m_listOfConfiguredDerivedStates.empty() )
			{
				throw TA_Base_Core::NotAvailableException();
			}

			std::vector < ConfiguredDerivedState * >::iterator cdsItr;
			for ( cdsItr = m_listOfConfiguredDerivedStates.begin(); cdsItr != m_listOfConfiguredDerivedStates.end(); cdsItr++ )
			{
				if( (*cdsItr)->getStateValue() == dpValue.getEnum())
				{
					return (*cdsItr)->getStateDescription();
				}
				
			}
			std::ostringstream ss;
			ss << "Unknown state (" << dpValue.getEnum() << ") for datapoint " << getDataPointName();
	
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "%s", ss.str().c_str());
			throw TA_Base_Core::BadParameterException(ss.str().c_str());
		}
		
		void MultiBitDataPoint::updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
		{
			std::ostringstream propertyvalue; 
			propertyvalue << newAlarmProperties.enumValue;
			DpValue almValue (propertyvalue.str(), getDataPointDataType(),
							  getEnumLabelsMap(), getBooleanLabels(), getPrecision());

			bool almEnabled = newAlarmProperties.enabled;

			// update the database if there is a difference
			switch ( newAlarmProperties.type )
			{
				case TA_Base_Bus::DptGeneral:
				{
					// for each of the DerivedState associated with this derived data point
					std::vector < ConfiguredDerivedState * >::iterator cdsItr;
					for ( cdsItr = m_listOfConfiguredDerivedStates.begin(); cdsItr != m_listOfConfiguredDerivedStates.end(); cdsItr++ )
					{
						// if the state value equals to the alarm value
						if ( newAlarmProperties.enumValue == (unsigned long)(*cdsItr)->getStateValue() )
						{
							// update alarm settings
							(*cdsItr)->setAlarmEnabled ( almEnabled );

							// update the alarm detail
							setAlarmDetails ( newAlarmProperties.type,
												 almEnabled,
												 (*cdsItr)->getAlarmDelay(),
												 (*cdsItr)->getAlarmSeverity(),
												 (*cdsItr)->getAlarmMessage(),
												 almValue,
												 (*cdsItr)->getStateDescription(),
												 (*cdsItr)->getAlarmMMS());
						}
					}

					break;
				}
				case TA_Base_Bus::DptHiHi:      // fall through
				case TA_Base_Bus::DptHi:            // fall through
				case TA_Base_Bus::DptLo:            // fall through
				case TA_Base_Bus::DptLoLo:      // fall through
				case TA_Base_Bus::DptBoolTrue:  // fall through
				case TA_Base_Bus::DptBoolFalse: // fall through
					break;
				default:
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unkown alarm property type[%d] received in datapoint [%d]", newAlarmProperties.type, m_entityKey );
					break;
			}
		}

		bool MultiBitDataPoint::getIsDigitalDataPoint() const
		{
			return true;
		}
}
