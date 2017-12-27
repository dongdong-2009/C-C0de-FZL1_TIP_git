/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/AnalogueDataPoint.cpp $
  * @author  J. Welton
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  *	AnalogueDataPoint represents a single numeric
  *	value. It inherits from the DataPoint base class,
  *	and also includes details representing raw and
  * engineering ranges, as well as a possible update
  * threshold (the % of the raw range that must change
  * before the DataPoint is updated).
  *
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/AnalogueDataPoint.h"
#include "bus/scada/datapoint_library/src/AnalogueAlarmDetails.h"

#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/AlarmAgentException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/DataPointAudit_MessageTypes.h"
#include "core/exceptions/src/ObjectResolutionException.h"

#include <cmath>
#include <climits>
#include <cfloat>
#include <sstream>
using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	AnalogueDataPoint::AnalogueDataPoint( TA_Base_Core::DataPointEntityDataPtr					dataPointEntityAccess,
										  TA_Base_Bus::EDataPointType               			dpType,
										  TA_Base_Bus::EDataPointDataType                   	dpDataType,
                                          ScadaPersistencePtr                                   persistence,
                                          TA_Base_Core::ScadaRootEntityData&                    scadaRootEntityData,
                                          MmsScadaUtility&                                      mmsScadaUtility,
                                          CommandProcessor&                                     commandProcessor )
	:
	DataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor ),
    m_updateThreshold( 0 ),
	m_updateTimeLimit( 0 ),
	m_updateTimestamp( 0 ),
	m_alarmThresholdConstant ( 0 ),
	m_engConstant( -1 ),
	m_rawHighLimit( 0 ),
	m_rawLowLimit( 0 ),
	m_rawRange( -1 ),
	m_engHighLimit( 0 ),
	m_engLowLimit( 0 ),
	m_zeroValueConstant( 0 )
	{
		// 
		// Check Datapoint Data Type
		//
		if (dpDataType != TA_Base_Bus::DPT_INT16_TYPE &&
			dpDataType != TA_Base_Bus::DPT_INT32_TYPE &&
			dpDataType != TA_Base_Bus::DPT_INT16S_TYPE &&
			dpDataType != TA_Base_Bus::DPT_INT32S_TYPE &&
			dpDataType != TA_Base_Bus::DPT_UINT16_TYPE &&
			dpDataType != TA_Base_Bus::DPT_UINT32_TYPE &&
			dpDataType != TA_Base_Bus::DPT_IEEE32_TYPE)
		{
            std::ostringstream desc;
            desc << "Invalid Configuration; AnalogueDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}

		//
		// Update Precision
		//
		try
		{
			int nPrecision = dataPointEntityAccess->getPrecision();
			if ( ( nPrecision == 0 ) && ( m_dataType == TA_Base_Bus::DPT_IEEE32_TYPE ) )
			{
				// default to 2 decimal point if precision has not been setup for
				// floating point datapoint
				setPrecision( 2 );
			}
			else
			{
				setPrecision ( nPrecision );
			}
		}
		catch ( ... )
		
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Failed to read precision from database. set to default" );
		}

		//
		// Update engineering units
		//

		setEngUnitsEnabled( dataPointEntityAccess->getEngineeringUnitsEnabled() );

		setEngineeringUnits( dataPointEntityAccess->getEngineeringUnits(),
							 dataPointEntityAccess->getRawLimitHigh(),
							 dataPointEntityAccess->getRawLimitLow(),
							 dataPointEntityAccess->getEngineeringLimitHigh(),
							 dataPointEntityAccess->getEngineeringLimitLow() );

		//
		// Update threshold and time limit
		//

		if ( 0 < dataPointEntityAccess->getUpdateThreshold() )
		{
    		setUpdateThreshold( dataPointEntityAccess->getUpdateThreshold(), dataPointEntityAccess->getUpdateTimeLimit() );
		}

		//
		// Update Zero value threshold
		//

		if ( 0 < dataPointEntityAccess->getZeroValueThreshold() )
		{
			setZeroValueThreshold ( dataPointEntityAccess->getZeroValueThreshold() );
		}

		//
		// set up different alarm types associated with this analogue data point
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
	            
				forcedValue.setFromPersist( tempString, getPrecision() );
				
	            convertToRaw( forcedValue, m_forcedValue );
	            m_forcedEngValue = forcedValue;
	        }
		}
	}


	AnalogueDataPoint::~AnalogueDataPoint()
	{

	}


	void AnalogueDataPoint::setEngineeringUnits( const std::string & engUnits,
                          						 double        rawHi,
                          						 double        rawLo,
                          						 double        engHi,
                          						 double        engLo )
	{
		// update eng unit
		m_engineeringUnits.assign( engUnits );
		updateEngUnitForAlarms ( engUnits );

		// if the ranges are correctly defined
		if ( ( rawHi > rawLo ) && ( engHi > engLo ) )
		{
			m_rawHighLimit = rawHi;
			m_rawLowLimit = rawLo;
			m_engHighLimit = engHi;
			m_engLowLimit = engLo;

			//
			// Calculating these now saves doing it everytime
			// ConvertTo*() methods are called
			//
			m_rawRange = m_rawHighLimit - m_rawLowLimit;
			m_engConstant = ( m_engHighLimit - m_engLowLimit ) / m_rawRange;
		}
		// else force the eng unit conversion to off
		else
		{
			setEngUnitsEnabled( false );
		}

		//
		// calculate alarm threshold constant based on the engineering limits
		//
		if ( m_engHighLimit > m_engLowLimit )
		{
			double alarmThreshold = m_dataPointEntityAccess->getAlarmThreshold();
			m_alarmThresholdConstant = ( alarmThreshold * ( m_engHighLimit - m_engLowLimit ) ) / 100;
		}
		else
		{
			m_alarmThresholdConstant = 0;
		}
	}


	bool AnalogueDataPoint::isValidWrite( DpValue & writeValue )
	{
		return isValidEngValue(writeValue) && isValidDataTypeValue(writeValue);
	}
	
	
	bool AnalogueDataPoint::isValidEngValue( const DpValue & dpValue )
	{
		double floatVal = dpValue.getFloat();
		
		if ( isEngUnitsEnabled() )
		{
			if ( m_engHighLimit > m_engLowLimit )
			{
				return ( floatVal >= m_engLowLimit &&
					floatVal <= m_engHighLimit  );
			}
		}
		
		// if engineering limits are not configured, return true
		return true;
	}
	
	
	bool AnalogueDataPoint::isValidRawValue( const DpValue & dpValue )
	{
		double floatVal = dpValue.getFloat();
		
		if ( isEngUnitsEnabled() )
		{
			if ( m_rawHighLimit > m_rawLowLimit )
			{
				return ( floatVal >= m_rawLowLimit &&
					floatVal <= m_rawHighLimit  );
			}
		}
		
		// if engineering limits are not configured, return true
		return true;
	}
	
	
	bool AnalogueDataPoint::isValidDataTypeValue( const DpValue & dpValue )
	{
		double floatVal = dpValue.getFloat();
		
		return floatVal >= getDataTypeLimitValue(false) && floatVal <= getDataTypeLimitValue(true);
	}
	

	double AnalogueDataPoint::getDataTypeLimitValue(bool isMaxLimit)
	{
		switch( getDataPointDataType() )
		{
		case TA_Base_Bus::DPT_INT16_TYPE:
		case TA_Base_Bus::DPT_INT16S_TYPE:
			
			return (isMaxLimit ? SHRT_MAX : SHRT_MIN); break;
			
		case TA_Base_Bus::DPT_INT32_TYPE :
		case TA_Base_Bus::DPT_INT32S_TYPE :
			
			return (isMaxLimit ? LONG_MAX : LONG_MIN); break;
			
		case TA_Base_Bus::DPT_UINT16_TYPE :
			
			return (isMaxLimit ? USHRT_MAX : 0); break;
			
		case TA_Base_Bus::DPT_UINT32_TYPE :
			
			return (isMaxLimit ? ULONG_MAX : 0); break;
			
		case TA_Base_Bus::DPT_IEEE32_TYPE :
			
			return (isMaxLimit ? FLT_MAX : -FLT_MAX); break; //FLT_MIN means the minimum positive value
			
		default :
			{
				// Unsupported type, should never reach here.
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Unsupported analogue type detected" );
				
				return isMaxLimit ? 0 : 1; break;
			}
		}
	}


    boost::shared_ptr<DatapointCompleteStaticConfig> AnalogueDataPoint::getCompleteStaticConfig()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<DatapointCompleteStaticConfig> toReturn = DataPoint::getCompleteStaticConfig();
        getEngineeringUnits(toReturn->engUnits);
        toReturn->alarmPropertyVect = getAlarmPropertiesVect();
        return toReturn;
    }

    boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> AnalogueDataPoint::getCompleteDynamicUpdate()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeudpate = DataPoint::getCompleteDynamicUpdate();
        //set the current alarm properties
        completeudpate->alarmProperties = getAlarmPropertiesVect();
        return completeudpate;
    }

    TA_Base_Bus::AlarmPropertyVect AnalogueDataPoint::getAlarmPropertiesVect()
    {
		//
		// Alarm Properties sequence
		//

		// set the size of the sequence
		 TA_Base_Bus::AlarmPropertyVect alarmProperties;
         ThreadGuard guard( m_alarmslock );
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
            almProperty.threshold = m_alarms[index]->getValue().getFloat();
            almProperty.enumValue = 0;
			// assign to sequence
			alarmProperties.push_back(almProperty);
		}

        return alarmProperties;
    }
    
    void AnalogueDataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
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

    void AnalogueDataPoint::updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
        // only update when there is difference
        bool foundDifference = false;
        {
            ThreadGuard guard( m_alarmslock );
            for (unsigned long i = 0; i < m_alarms.size() && (!foundDifference); ++ i)
            {
                if (newAlarmProperties.type == m_alarms[i]->getType() &&
                    (newAlarmProperties.enabled != m_alarms[i]->isEnabled() ||
                    newAlarmProperties.threshold != m_alarms[i]->getValue().getFloat()))
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

		bool almEnabled = newAlarmProperties.enabled;
        double almthreshold = newAlarmProperties.threshold;
        std::stringstream valuestr;
        valuestr << almthreshold;

		// update the database if there is a difference
		bool result = true;
		switch ( newAlarmProperties.type )
		{
			case TA_Base_Bus::DptOverScaleHi:
			{
				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptOverScaleLo:
			{
				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptHiHi:
			{
				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptHi:
			{
                configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptLo:
			{
				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptLoLo:		
			{
				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptBoolTrue:
			case TA_Base_Bus::DptBoolFalse:
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

    
    void AnalogueDataPoint::setAlarmProperties( const std::string& sessionID, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}


		std::vector< std::string > configChanges;
		configChanges.clear();

		bool almEnabled = newAlarmProperties.enabled;
        double almthreshold = newAlarmProperties.threshold;
        std::stringstream valuestr;
        valuestr << almthreshold;

        bool tempBool;
        double tempDouble;

		// update the database if there is a difference
		bool result = true;
		switch ( newAlarmProperties.type )
		{
			case TA_Base_Bus::DptOverScaleHi:
			{
                m_persistence->readPersistedValue( DP_OHI_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_OH_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_OHI_ALARM_ENABLED, m_entityKey, almEnabled ) )
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
                m_persistence->readPersistedValue( DP_OHI_ALARM_VALUE, m_entityKey, tempDouble );
				if ( tempDouble != almthreshold )
				{
					TA_Base_Core::DescriptionParameters alarmValueDescriptionParameters;
					alarmValueDescriptionParameters.clear();
					NameValuePair AlarmValuePair1 ( "AlarmType", DPTALM_OH_SHORT );
					alarmValueDescriptionParameters.push_back ( &AlarmValuePair1 );
					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_OHI_ALARM_VALUE, m_entityKey, almthreshold ) )
					{
					    NameValuePair AlarmValuePair2 ( "ValueString",  valuestr.str());
					    alarmValueDescriptionParameters.insert (alarmValueDescriptionParameters.begin(), &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, DataPointAudit::DataPointLimitsChanged );
					}
					// if failed
					else
					{
					    NameValuePair AlarmValuePair2 ( "LimitAction", "Set " + valuestr.str() );
					    alarmValueDescriptionParameters.push_back ( &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptOverScaleLo:
			{
                //TD15446
                m_persistence->readPersistedValue( DP_OLO_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_OL_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_OLO_ALARM_ENABLED, m_entityKey, almEnabled ) )
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

				m_persistence->readPersistedValue( DP_OLO_ALARM_VALUE, m_entityKey, tempDouble );
				if ( tempDouble != almthreshold )
				{
					TA_Base_Core::DescriptionParameters alarmValueDescriptionParameters;
					alarmValueDescriptionParameters.clear();
					NameValuePair AlarmValuePair1 ( "AlarmType", DPTALM_OL_SHORT );
					alarmValueDescriptionParameters.push_back ( &AlarmValuePair1 );
					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_OLO_ALARM_VALUE, m_entityKey, almthreshold ) )
					{
                        NameValuePair AlarmValuePair2 ( "ValueString", valuestr.str() );
                        alarmValueDescriptionParameters.insert (alarmValueDescriptionParameters.begin(), &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, DataPointAudit::DataPointLimitsChanged );
					}
					// if failed
					else
					{
					    NameValuePair AlarmValuePair2 ( "LimitAction", "Set " + valuestr.str() );
					    alarmValueDescriptionParameters.push_back ( &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptHiHi:
			{
                m_persistence->readPersistedValue( DP_HIHI_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_HIHI_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_HIHI_ALARM_ENABLED, m_entityKey, almEnabled ) )
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

                m_persistence->readPersistedValue( DP_HIHI_ALARM_VALUE, m_entityKey, tempDouble );
				if ( tempDouble != almthreshold )
				{
					TA_Base_Core::DescriptionParameters alarmValueDescriptionParameters;
					alarmValueDescriptionParameters.clear();
					NameValuePair AlarmValuePair1 ( "AlarmType", DPTALM_HIHI_SHORT );
					alarmValueDescriptionParameters.push_back ( &AlarmValuePair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_HIHI_ALARM_VALUE, m_entityKey, almthreshold ) )
					{
                        NameValuePair AlarmValuePair2 ( "ValueString", valuestr.str() );
                        alarmValueDescriptionParameters.insert (alarmValueDescriptionParameters.begin(), &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, DataPointAudit::DataPointLimitsChanged );
					}
					// if failed
					else
					{
					    NameValuePair AlarmValuePair2 ( "LimitAction", "Set " + valuestr.str() );
					    alarmValueDescriptionParameters.push_back ( &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_ENABLED );
				break;
			}		
			case TA_Base_Bus::DptHi:
			{
                //TD15446
                m_persistence->readPersistedValue( DP_HI_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_HI_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_HI_ALARM_ENABLED, m_entityKey, almEnabled ) )
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

                m_persistence->readPersistedValue( DP_HI_ALARM_VALUE, m_entityKey, tempDouble );
				if ( tempDouble != almthreshold )
				{
					TA_Base_Core::DescriptionParameters alarmValueDescriptionParameters;
					alarmValueDescriptionParameters.clear();
					NameValuePair AlarmValuePair1 ( "AlarmType", DPTALM_HI_SHORT );
					alarmValueDescriptionParameters.push_back ( &AlarmValuePair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_HI_ALARM_VALUE, m_entityKey, almthreshold ) )
					{
                        NameValuePair AlarmValuePair2 ( "ValueString", valuestr.str() );
                        alarmValueDescriptionParameters.insert (alarmValueDescriptionParameters.begin(), &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, DataPointAudit::DataPointLimitsChanged );
					}
					// if failed
					else
					{
					    NameValuePair AlarmValuePair2 ( "LimitAction", "Set " + valuestr.str() );
					    alarmValueDescriptionParameters.push_back ( &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

                configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_ENABLED );

				break;
			}		
			case TA_Base_Bus::DptLo:
			{
                //TD15446
                m_persistence->readPersistedValue( DP_LO_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_LO_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_LO_ALARM_ENABLED, m_entityKey, almEnabled ) )
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

                m_persistence->readPersistedValue( DP_LO_ALARM_VALUE, m_entityKey, tempDouble );
				if ( tempDouble != almthreshold )
				{
					TA_Base_Core::DescriptionParameters alarmValueDescriptionParameters;
					alarmValueDescriptionParameters.clear();
					NameValuePair AlarmValuePair1 ( "AlarmType", DPTALM_LO_SHORT );
					alarmValueDescriptionParameters.push_back ( &AlarmValuePair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_LO_ALARM_VALUE, m_entityKey, almthreshold ) )
					{
                        NameValuePair AlarmValuePair2 ( "ValueString", valuestr.str() );
                        alarmValueDescriptionParameters.insert (alarmValueDescriptionParameters.begin(), &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, DataPointAudit::DataPointLimitsChanged );
					}
					// if failed
					else
					{
					    NameValuePair AlarmValuePair2 ( "LimitAction", "Set " + valuestr.str() );
					    alarmValueDescriptionParameters.push_back ( &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_ENABLED );

				break;
			}		
			case TA_Base_Bus::DptLoLo:		
			{
                //TD15446
                m_persistence->readPersistedValue( DP_LOLO_ALARM_ENABLED, m_entityKey, tempBool );
				if ( tempBool != almEnabled )
				{
					TA_Base_Core::DescriptionParameters alarmEnabledDescriptionParameters;
					alarmEnabledDescriptionParameters.clear();
					NameValuePair AlarmEnabledPair1 ( "AlarmType", DPTALM_LOLO_SHORT );
					alarmEnabledDescriptionParameters.push_back ( &AlarmEnabledPair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_LOLO_ALARM_ENABLED, m_entityKey, almEnabled ) )
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

                m_persistence->readPersistedValue( DP_LOLO_ALARM_VALUE, m_entityKey, tempDouble );
				if ( tempDouble != almthreshold )
				{
					TA_Base_Core::DescriptionParameters alarmValueDescriptionParameters;
					alarmValueDescriptionParameters.clear();
					NameValuePair AlarmValuePair1 ( "AlarmType", DPTALM_LOLO_SHORT );
					alarmValueDescriptionParameters.push_back ( &AlarmValuePair1 );

					// if successful to update the database
					if ( true == m_persistence->writePersistedValue( DP_LOLO_ALARM_VALUE, m_entityKey, almthreshold ) )
					{
                        NameValuePair AlarmValuePair2 ( "ValueString", valuestr.str() );
                        alarmValueDescriptionParameters.insert (alarmValueDescriptionParameters.begin(), &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, DataPointAudit::DataPointLimitsChanged );
					}
					// if failed
					else
					{
					    NameValuePair AlarmValuePair2 ( "LimitAction", "Set " + valuestr.str() );
					    alarmValueDescriptionParameters.push_back ( &AlarmValuePair2 );

						submitAuditMessage( sessionID, alarmValueDescriptionParameters, 
                                            DataPointAudit::DataPointLimitsPersistenceFailed );

						// no need to go any further
						result = false;
						break;
					}
				}

				configChanges.push_back ( TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_ENABLED );

				break;
			}		
			case TA_Base_Bus::DptBoolTrue:
			case TA_Base_Bus::DptBoolFalse:
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
            alarmconfigupdate->alarmProperties.at(0).enabled = newAlarmProperties.enabled;
            alarmconfigupdate->alarmProperties.at(0).threshold = almthreshold;
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

    void AnalogueDataPoint::updateEngineeringUnits( bool enabled,
													std::string * engUnits,
													double * rawHi,
													double * rawLo,
													double * engHi,
													double * engLo )
	{
		//
		// update current configurations with new data
		//

		setEngUnitsEnabled( enabled );

		if ( engUnits != NULL )
		{
			m_engineeringUnits.assign( *engUnits );
		}

		if ( rawHi != NULL )
		{
			m_rawHighLimit = *rawHi;
		}

		if ( rawLo != NULL )
		{
			m_rawLowLimit = *rawLo;
		}

		if ( engHi != NULL )
		{
			m_engHighLimit = *engHi;
		}

		if ( engLo != NULL )
		{
			m_engLowLimit = *engLo;
		}

		setEngineeringUnits ( m_engineeringUnits,
							  m_rawHighLimit,
							  m_rawLowLimit,
							  m_engHighLimit,
							  m_engLowLimit );
	}

	void AnalogueDataPoint::setUpdateThreshold( double threshold, int timeLimit )
	{
		if ( threshold >= 0 && m_rawRange > 0 )
		{
			m_updateThreshold = threshold;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Datapoint %s update threshold not set, current update threshold is %d", 
							m_name.c_str(), m_updateThreshold);
		}

		if ( timeLimit >= 0 )
		{
			m_updateTimeLimit = timeLimit;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Datapoint %s update time limit not set, current update time limit is %d", 
							m_name.c_str(), m_updateTimeLimit);
		}
	}


	void AnalogueDataPoint::setZeroValueThreshold( double threshold )
	{
		if ( threshold >= 0  &&  m_rawRange > 0 )
		{
			m_zeroValueConstant = ( threshold / 100 ) * m_rawRange;
		}
	}


	bool AnalogueDataPoint::isValidUpdate( DataPointState & updateState )
	{
		//
		// These checks are all based on the new state's value, thus we
		// can skip them if this is a "bad" state, whose value is irrelevant
		// anyway.
		//
		// Both thresholds rely on raw limits being configured so if engineering
		// units are disabled we can move on.
		//
		if ( updateState.isGoodValue() && isEngUnitsEnabled() )
		{
			//
			// If the zero value threshold is set and the value is
			// within the zero value threshold, make it zero
			//
			if ( m_zeroValueConstant > 0  &&
				 fabs( updateState.getValue().getFloat() ) <= m_zeroValueConstant )
			{
				updateState.getValue().setFloat( 0, getPrecision() );
			}


			if ( m_updateThreshold > 0 && isGoodValue() )
			{
				//
				// The new state is Good and an update threshold is configured.
				// Check if the threshold has been exceeded by the new value or
				// if a timelimit is configured, has that been exceeded
				//
				return ( isUpdateThresholdExceeded( updateState ) ||
			 			 isUpdateTimeLimitExceeded( updateState ) );
			}
		}

		return true;
	}

	void AnalogueDataPoint::getEngineeringUnits(
							TA_Base_Bus::DataPointEngineeringUnits & eng )
	{
		if ( isEngUnitsEnabled() )
		{
			eng.EngineeringUnitsEnabled = true;
			eng.LowLimit = m_engLowLimit;
			eng.HighLimit = m_engHighLimit;
			eng.Units =  m_engineeringUnits ;
		}
		else
		{
			eng.EngineeringUnitsEnabled = false;
			eng.LowLimit = 0;
			eng.HighLimit = 0;
			eng.Units = "";
		}
	}


    std::string AnalogueDataPoint::getDataPointValueAsString( const DpValue & dpValue )
    {
        FUNCTION_ENTRY("getDataPointValueAsString");

        std::ostringstream valueStr;
        valueStr << dpValue.getValueAsString(); //wenching++ (TD14422)

        // Append units here...
        // (TD 15181: but only if the EngUnits are enabled and not blank)
        if ( true == isEngUnitsEnabled() && (!m_engineeringUnits.empty()) )
        {
            valueStr << " (" << m_engineeringUnits << ")"; //wenching++ (TD14422)
        }

        FUNCTION_EXIT;
        return valueStr.str();
	}


	bool AnalogueDataPoint::isUpdateThresholdExceeded( DataPointState & state )
	{
		if ( m_rawRange == 0 )
		{
			return false;
		}

		//
		// Calculate the absolute difference between the new
		// value and the current. Compare against the precalculated
		// thresholdConstant.
		//
		DpValue value = state.getValue();

		double newValue = value.getFloat();
		double currentValue = getCurrentValue().getFloat();
		double valueDiff = fabs( newValue - currentValue );

		//19620++ Convert ValueDiff from raw value to eng value
		valueDiff = valueDiff * m_engConstant;
		//++19620

		//19620++ calculate update threshold constant based on the engineering limits
		if ( valueDiff < (( m_updateThreshold * ( m_engHighLimit - m_engLowLimit ) ) / 100) ) //++19620
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"Datapoint %s updateThreshold not exceeded; newValue=%f currentValue=%f valueDiff=%f m_updateThreshold=%f",
				m_name.c_str(), newValue, currentValue, valueDiff, m_updateThreshold );
			return false;
		}

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
			"Datapoint %s updateThreshold exceeded; newValue=%f currentValue=%f valueDiff=%f m_updateThreshold=%f",
			m_name.c_str(), newValue, currentValue, valueDiff, m_updateThreshold );
		
		// Update timestamp
		m_updateTimestamp = state.getTimestamp();
		return true;
	}


	bool AnalogueDataPoint::isUpdateTimeLimitExceeded( DataPointState & state )
	{
		//
		// Check whether a time limit is configured and whether the new
		// state has a different value or not
		//

		DpValue value = state.getValue();
		double newValue = value.getFloat();
		double currentValue = getCurrentValue().getFloat();
		
		
		if ( m_updateTimeLimit <= 0 || value == getCurrentValue() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"Datapoint %s updateTimeLimit not exceeded; newValue=%f currentValue=%f m_updateTimeLimit=%d",
				m_name.c_str(), newValue, currentValue, m_updateTimeLimit );

			return false;
		}
	
		//
		// Update time limit is configured. If this is the
		// "first" update, just record the time and return false.
		// Otherwise, check whether the time limit has been
		// exceeded
		//
		time_t newTime = state.getTimestamp();
		long timeDiff = newTime - m_updateTimestamp;

		if ( timeDiff > m_updateTimeLimit )
		{

			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"Datapoint %s updateTimeLimit exceeded; newValue=%f currentValue=%f timeDiff=%d m_updateTimeLimit=%d",
				m_name.c_str(), newValue, currentValue, timeDiff, m_updateTimeLimit );

			m_updateTimestamp = newTime;
			return true;
		}

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
				"Datapoint %s updateTimeLimit not exceeded; newValue=%f currentValue=%f newTime:%d m_updateTimestamp:%d timeDiff=%d m_updateTimeLimit=%d",
				m_name.c_str(), newValue, currentValue, newTime, m_updateTimestamp, timeDiff, m_updateTimeLimit );

		return false;
	}

	//
	// Engineering Conversion is:
	//
	//	EngVal = ( ( ( RawVal - RawLo ) / ( RawRange) ) * EngRange ) + EngLo )
	//
	//		where RawRange = RawHi - RawLo , EngRange = EngHi - EngLo
	//
	void AnalogueDataPoint::convertToEngUnits( const DataPointState & rawState,
									           DataPointState & engState )
	{
        engState.setTimestamp( rawState.getTimebStructure() );
        engState.setStatus( rawState.getStatus() );

		// if conversion is enabled
		if ( isEngUnitsEnabled() && m_engConstant > 0 )
		{
			double engTemp =
				( ( rawState.getValue().getFloat() - m_rawLowLimit ) * m_engConstant ) +
				m_engLowLimit;

			engState.getValue().setFloat( engTemp, getPrecision() );
		}

		// if conversion is not enabled
		else
		{
			engState.getValue().setFloat( rawState.getValue().getFloat(), getPrecision() );
		}

		// only proceed if the data point's eng unit is enabled
		if (! isValidEngValue(engState.getValue()))
		{
            engState.setStatus( QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED );
		}
	}


	//
	// Raw Conversion is:
	//
	//	RawVal = ( ( ( engVal - EngLo ) / ( EngRange) ) * RawRange ) + RawLo )
	//
	//		where RawRange = RawHi - RawLo , EngRange = EngHi - EngLo
	//
	void AnalogueDataPoint::convertToRaw( const DpValue & engVal,
										  DpValue & rawVal )
	{
		if ( isEngUnitsEnabled() && m_engConstant != 0 )
		{
			double rawTemp =
				( ( engVal.getFloat() - m_engLowLimit ) / m_engConstant ) +
				m_rawLowLimit;

			rawVal.setFloat( rawTemp, m_precision );
		}
		else
		{
			rawVal.setFloat( engVal.getFloat(), getPrecision() );
		}
	}

	void AnalogueDataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
	{
		FUNCTION_ENTRY("updateSpecificConfiguration");

		// for each of the changed parameters
		for ( unsigned int i = 0; i < configChanges->size(); i++ )
		{
			std::string paramName = (*configChanges)[i];

			if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_UNITS_ENABLED ||
				 paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_UNITS ||
				 paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_RAW_LIMIT_LOW ||
				 paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_RAW_LIMIT_HIGH ||
				 paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_LIMIT_LOW ||
				 paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_LIMIT_HIGH ||
				 paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ALARM_THRESHOLD )
			{
				// re calculate engineering limits
				updateAnalogueEngineeringUnits ( paramName );

				// update alarm details with new alarm threshold constant which might have
				// been changed based on the engineering limits
		
				// OverScaleHi Alarm Details
				configureOverScaleHiAlarm();

				// Hi-Hi Alarm Details
                configureHiHiAlarm();

				// Hi Alarm Details
                configureHiAlarm();

				// Lo Alarm Details
                configureLoAlarm();

				// Lo-Lo Alarm Details
                configureLoLoAlarm();

				// OverScaleLo Alarm Details
				configureOverScaleLoAlarm();

			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_ENABLED ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_VALUE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_SEVERITY )
			{
				configureOverScaleHiAlarm();
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_ENABLED ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_VALUE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_SEVERITY )
			{
				configureOverScaleLoAlarm();
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_ENABLED ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_VALUE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_SEVERITY )
			{
				configureHiHiAlarm();
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_ENABLED || //TD15160
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_VALUE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_SEVERITY )
			{
                configureHiAlarm();
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_ENABLED ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_VALUE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_SEVERITY )
			{
				configureLoAlarm();
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_ENABLED ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_VALUE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_MESSAGE ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_DELAY ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_MMS ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_SEVERITY )
			{
				configureLoLoAlarm();      
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_THRESHOLD ||
					  paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_TIME_LIMIT )
			{
				// Update Threshold / Time Limit
				updateAnalogueUpdateThreshold ( paramName );
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ZERO_VALUE_THRESHOLD )
			{
				// Zero Value Threshold
				setZeroValueThreshold ( m_dataPointEntityAccess->getZeroValueThreshold() );
			}
			else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_PRECISION )
			{
				// Precision
				setPrecision( m_dataPointEntityAccess->getPrecision() );
			}

            // TD16033 Because these parameters will affect other alarm's other limit, 
            // so need to set other limit and re-evaluate all alarm states
            if (paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_ENABLED ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OH_ALARM_VALUE ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_ENABLED ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HIHI_ALARM_VALUE ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_ENABLED ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HI_ALARM_VALUE ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_ENABLED ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LO_ALARM_VALUE ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_ENABLED ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOLO_ALARM_VALUE ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_ENABLED ||
                paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OL_ALARM_VALUE)
            {
			    // Weimin, TD13619
			    // Set Alarm Details' other Limit.
			    setAlarmsOtherLimit();
                // Update alarms
                m_previousAlarmState = m_alarmState;
            
                DataPointState engState( getCurrentEngValue(),
                    getTimebStructure(),
                    getStatus() );
            
                updateAlarms( engState );
                //send out the alarm update only if the datapoint is in control mode
                if (TA_Base_Core::Control == m_operationMode)
                {
                    if (m_previousAlarmState != m_alarmState)
                    {
                        //update the alarm state
                        boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmupdate(new DpDynamicSyncUpdate);
                        alarmupdate->updateType = TA_Base_Bus::AlarmStateUpdate;
                        alarmupdate->alarmState = m_alarmState;
                        //send out the update to inform the observer
                        submitUpdate ( alarmupdate );
                    }
                }
            }
            
			FUNCTION_EXIT;
		}
	}


	void AnalogueDataPoint::updateAnalogueEngineeringUnits ( const std::string & paramName )
	{
		bool engEnabled = true;
		std::string tmpUnits;
		double tmpRawHi;
		double tmpRawLo;
		double tmpEngHi;
		double tmpEngLo;

		std::string * engUnits = NULL;
		double * rawHi = NULL;
		double * rawLo = NULL;
		double * engHi = NULL;
		double * engLo = NULL;

		// Engineering Units
		if ( TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_UNITS == paramName )
		{
			tmpUnits.assign( m_dataPointEntityAccess->getEngineeringUnits() );
			engUnits = & tmpUnits;
		}
		// Raw Low Limit
		else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_RAW_LIMIT_LOW == paramName )
		{
			tmpRawLo = m_dataPointEntityAccess->getRawLimitLow();
			rawLo = & tmpRawLo;
		}
		// Raw High Limit
		else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_RAW_LIMIT_HIGH == paramName )
		{
			tmpRawHi = m_dataPointEntityAccess->getRawLimitHigh();
			rawHi = & tmpRawHi;
		}
		// Engineering High Limit
		else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_LIMIT_HIGH == paramName )
		{
			tmpEngHi = m_dataPointEntityAccess->getEngineeringLimitHigh();
			engHi = & tmpEngHi;
		}
		// Engineering Low Limit
		else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_LIMIT_LOW == paramName )
		{
			tmpEngLo = m_dataPointEntityAccess->getEngineeringLimitLow();
			engLo = & tmpEngLo;
		}
		// Engineering Units Enabled
		else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_UNITS_ENABLED == paramName )
		{
			engEnabled = m_dataPointEntityAccess->getEngineeringUnitsEnabled();
		}

		updateEngineeringUnits( engEnabled, engUnits, rawHi, rawLo, engHi, engLo );
	}

	void AnalogueDataPoint::updateAnalogueUpdateThreshold( const std::string & paramName )
	{
		double threshold = -1;
		double timeLimit = -1;

		// Update Threshold
		if ( TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_THRESHOLD == paramName )
		{
			threshold = m_dataPointEntityAccess->getUpdateThreshold();
		}
		// Update Time Limit
		else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_TIME_LIMIT == paramName )
		{
			timeLimit = m_dataPointEntityAccess->getUpdateTimeLimit();
		}

		setUpdateThreshold( threshold, (int) timeLimit );
	}



	void AnalogueDataPoint::setToControlMode()
	{
        
		// call the base class
		DataPoint::setToControlMode();
	}


	void AnalogueDataPoint::setToMonitorMode()
	{
		// call the base class
		DataPoint::setToMonitorMode();
	}


	double AnalogueDataPoint::getAlarmThresholdConstant() const
	{
		return m_alarmThresholdConstant;
	}


	void AnalogueDataPoint::updateFieldState( DataPointState & newState )
	{
		// TD 8307: Analog datapoints should send an update whenever they cross the
		// alarm boundaries, ie regardless of whether the update threshold is exceeded.

		// lock the data point to prevent it from being modified while its current
		// state is being updated
		//ThreadGuard guard( m_lock );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPoint [%s] updates its state: value [%s], quality [%u].", 
            getDataPointName().c_str(), newState.getValue().getValueAsString().c_str(), newState.getStatus());
        
		// store latest field state
		m_lastKnownFieldState = newState;

		// only process newState if the data point is in service state
		if ( IN_SERVICE == m_serviceState )
		{
			// TD19657, if the new value exceeds datatype limit, set quality to bad_uncertain.
			if (!isValidDataTypeValue(newState.getValue()))
			{
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Try to set DataPoint [%s] field value to [%s], which exceeds datatype limit.", 
                    getDataPointName().c_str(), newState.getValue().getValueAsString().c_str());

				//set the value to limit value
				if (newState.getValue().getFloat() < getDataTypeLimitValue(false))
				{
					newState.getValue().setFloat(getDataTypeLimitValue(false));
				}
				else
				{
					newState.getValue().setFloat(getDataTypeLimitValue(true));
				}

				//set quality to uncertain
                newState.setStatus( QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED );
			}

            DataPointState engState( newState );
            
			// check the alarm condition first, based on alarm threshold constant,
			// only proceed if not in forced state
			if ( ! m_forcedState )
			{
				// convert the specified raw value to engineering value as we only
				// raise/close alarm on engineering value
				convertToEngUnits( newState, engState );

				// save the current alarm state as previous alarm state, which
				// we will compare against the new alarm state after updating the alarms
				m_previousAlarmState = m_alarmState;

				// update the alarm status
				updateAlarms( engState );

				// if there is a different before and after we evaluate the alarm
				// condition
				if ( m_previousAlarmState != m_alarmState )
				{
					// update the current field state with the new state
					setFieldValue ( newState );

					// notify client of the new value and alarm state
                    boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmupdate(new DpDynamicSyncUpdate);
                    alarmupdate->updateType = TA_Base_Bus::AlarmStateUpdate;
                    alarmupdate->alarmState = m_alarmState;
                    //send out the update to inform the observer
                    submitUpdate ( alarmupdate );
                    //the new state also required to be update
                    boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpupdate(new DpDynamicSyncUpdate);
                    dpupdate->updateType = TA_Base_Bus::ValueUpdate;
                    dpupdate->value = getCurrentEngValue().getDataPointValue();
                    dpupdate->value.quality = getQualityStatus();
                    dpupdate->value.timeStamp = getTimebStructure();
                    //send out the update to inform the observer
                    submitUpdate ( dpupdate );
					return;
				}
			}

			// if alarm condition has not changed, check the value based on
			// update threshold constant

		
			// Make type-specific determination of the validity of this new state
			if ( isValidUpdate( newState ) )
			{
				// NOTE: if the data point is in forced state, the field value is to
				//		 be updated but there will be no event/alarm generated

				bool newStateToBeSubmitted = false;

				// if this data point is writeable, i.e an output
				if ( true == m_writable )
				{
					// update the current field state with the new state
					setFieldValue( newState );

					// always generate an update event each time the data point is updated
					// with a new value
					newStateToBeSubmitted = true;
				}

				// if this data point is not writeable, i.e an input
				else
				{
					// if the new field status is different with the current status
					// or the new field value is different from the current field value
					if ( ( m_fieldValueQualityStatus != engState.getStatus() ) ||
						 ( m_fieldValue != newState.getValue() ) )
					{
						// update the current field state with the new state
						setFieldValue( newState );

						// in case engineering units have been exceeded
						// and the datapoint is forced
                        // weimin, why need to update the forced value quality here? 
                        //      and this will cause the IP and schematics lose the override state
                        //      and display the field value as forced value.
						//if ( true == m_forcedState )
						//{
						//	m_forcedValueQualityStatus = engState.getStatus();
						//}

						// generate an update event, if and only if not in forced state
						if ( ! m_forcedState )
						{
							newStateToBeSubmitted = true;
						}
					}
				}

				if ( true == newStateToBeSubmitted )
				{
				    if ( m_previousAlarmState != m_alarmState )
				    {
                        //the alarm status update also required if after changing the dp value, dp is in alarm
                        boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmupdate(new DpDynamicSyncUpdate);
                        alarmupdate->updateType = TA_Base_Bus::AlarmStateUpdate;
                        alarmupdate->alarmState = m_alarmState;
                        //send out the update to inform the observer
                        submitUpdate ( alarmupdate );
                    }
                    //the new state also required to be update
                    boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpupdate(new DpDynamicSyncUpdate);
                    dpupdate->updateType = TA_Base_Bus::ValueUpdate;
                    dpupdate->value = getCurrentEngValue().getDataPointValue();
                    dpupdate->value.quality = getQualityStatus();
                    dpupdate->value.timeStamp = getTimebStructure();
                    //send out the update to inform the observer
                    submitUpdate ( dpupdate );
				}
			}
		}
	}


	void AnalogueDataPoint::configureAlarms()
	{
		//
		// want to create the alarms in this specific order
		//

		// OverScaleHi
		configureOverScaleHiAlarm();

		// HiHi
		configureHiHiAlarm();

		// Hi
		configureHiAlarm();

		// Lo
		configureLoAlarm();

		// LoLo
		configureLoLoAlarm();

		// OverScaleLo
		configureOverScaleLoAlarm();
	
		// Set Alarm Details' other Limit.
		setAlarmsOtherLimit();
    }
    
	void AnalogueDataPoint::setAlarmsOtherLimit()
	{
		AnalogueAlarmDetails * pHiAlarm				= NULL;
		AnalogueAlarmDetails * pHiHiAlarm			= NULL;
		AnalogueAlarmDetails * pOverScaleHiAlarm	= NULL;
		AnalogueAlarmDetails * pLoAlarm				= NULL;
		AnalogueAlarmDetails * pLoLoAlarm			= NULL;
		AnalogueAlarmDetails * pOverScaleLoAlarm	= NULL;
		
		// retrive the alarm detail for easy use.
		DpAlarmList::iterator alarmsIter;
        {
            ThreadGuard guard( m_alarmslock );
		for( alarmsIter = m_alarms.begin(); alarmsIter != m_alarms.end(); ++alarmsIter )
		{
			switch ((*alarmsIter )->getType())
			{
			case TA_Base_Bus::DptHi:
				pHiAlarm = dynamic_cast<AnalogueAlarmDetails *> (* alarmsIter);
				TA_ASSERT(NULL != pHiAlarm, "Failed to cast alarm into AnalogueAlarmDetails*");
				break;
			case TA_Base_Bus::DptHiHi:
				pHiHiAlarm = dynamic_cast<AnalogueAlarmDetails *> (* alarmsIter);
				TA_ASSERT(NULL != pHiHiAlarm, "Failed to cast alarm into AnalogueAlarmDetails*");
				break;
			case TA_Base_Bus::DptOverScaleHi:
				pOverScaleHiAlarm = dynamic_cast<AnalogueAlarmDetails *> (* alarmsIter);
				TA_ASSERT(NULL != pOverScaleHiAlarm, "Failed to cast alarm into AnalogueAlarmDetails*");
				break;
			case TA_Base_Bus::DptLo:
				pLoAlarm = dynamic_cast<AnalogueAlarmDetails *> (* alarmsIter);
				TA_ASSERT(NULL != pLoAlarm, "Failed to cast alarm into AnalogueAlarmDetails*");
				break;
			case TA_Base_Bus::DptLoLo:
				pLoLoAlarm = dynamic_cast<AnalogueAlarmDetails *> (* alarmsIter);
				TA_ASSERT(NULL != pLoLoAlarm, "Failed to cast alarm into AnalogueAlarmDetails*");
				break;
			case TA_Base_Bus::DptOverScaleLo:
				pOverScaleLoAlarm = dynamic_cast<AnalogueAlarmDetails *> (* alarmsIter);
				TA_ASSERT(NULL != pOverScaleLoAlarm, "Failed to cast alarm into AnalogueAlarmDetails*");
				break;

            case TA_Base_Bus::DptBoolTrue:
            case TA_Base_Bus::DptBoolFalse:
            case TA_Base_Bus::DptGeneral:
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "[AnalogueDataPoint::setAlarmsOtherLimit] Invalid alarm type(%d)",
                    (*alarmsIter )->getType());
                break;

            default:
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "[AnalogueDataPoint::setAlarmsOtherLimit] Unhandled alarm type(%d)",
                    (*alarmsIter )->getType());
				break;
			    }
			}
		}
		
		//TD15180, Overscale value is exclude from alarm range, so just set a minimum change unit larger/smaller as the alarm other limit.
		DpValue alarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		double minDeltaValue = 0;
		int precision = 0;
		
		alarmValue.setFloat( 0, getPrecision() ); //just for getting precision
		precision = alarmValue.getPrecision();

		minDeltaValue = pow( (double)10, -precision);

		// If OH alarm is enabled, set the overscale value + min delta value as the other limit.
		if (NULL != pOverScaleHiAlarm && true == pOverScaleHiAlarm->isEnabled())
		{
			alarmValue.setFloat(pOverScaleHiAlarm->getValue().getFloat() + minDeltaValue, precision);
			
			if (NULL != pHiHiAlarm && true == pHiHiAlarm->isEnabled())
			{ // if VH alarm is enabled, set VH's other limit with OH's
				pHiHiAlarm->setOtherLimitValue(alarmValue);
			}
			else if (NULL != pHiAlarm && true == pHiAlarm->isEnabled())
			{ // if VH alarm is disabled, set HI's other limit with OH's
				pHiAlarm->setOtherLimitValue(alarmValue);
			}
		}
		// If VH alarm is enabled 
		if (NULL != pHiHiAlarm && true == pHiHiAlarm->isEnabled())
		{
			if (NULL != pHiAlarm && true == pHiAlarm->isEnabled())
			{ // if HI alarm is enabled, set HI's other limit with VH's
				pHiAlarm->setOtherLimitValue(pHiHiAlarm->getValue());
			}
		}
		
		// If OL alarm is enabled, set the overscale value - min delta value as the other limit.
		if (NULL != pOverScaleLoAlarm && true == pOverScaleLoAlarm->isEnabled())
		{
			alarmValue.setFloat(pOverScaleLoAlarm->getValue().getFloat() - minDeltaValue, precision);

			if (NULL != pLoLoAlarm && true == pLoLoAlarm->isEnabled())
			{ // if VL alarm is enabled, set VH's other limit with OL's
				pLoLoAlarm->setOtherLimitValue(alarmValue);
			}
			else if (NULL != pLoAlarm && true == pLoAlarm->isEnabled())
			{ // if VL alarm is disabled, set LO's other limit with OL's
				pLoAlarm->setOtherLimitValue(alarmValue);
			}
		}
		// If VL alarm is enabled 
		if (NULL != pLoLoAlarm && true == pLoLoAlarm->isEnabled())
		{
			if (NULL != pHiAlarm && true == pHiAlarm->isEnabled())
			{ // if LO alarm is enabled, set LO's other limit with LO's
				pLoAlarm->setOtherLimitValue(pLoLoAlarm->getValue());
			}
		}		
	}
	
	void AnalogueDataPoint::configureHiHiAlarm()
	{
		FUNCTION_ENTRY("configureHiHiAlarm");

        double hihiValue;
        m_persistence->readPersistedValue( DP_HIHI_ALARM_VALUE, m_entityKey, hihiValue );

        bool hihiEnabled;
        m_persistence->readPersistedValue( DP_HIHI_ALARM_ENABLED, m_entityKey, hihiEnabled );

		DpValue hiHiAlarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		hiHiAlarmValue.setFloat(hihiValue, getPrecision());

		setAlarmDetails( TA_Base_Bus::DptHiHi,
						 hihiEnabled, //TD15446
						 (int) m_dataPointEntityAccess->getHiHiAlarmDelay(),
						 m_dataPointEntityAccess->getHiHiAlarmSeverity(),
						 m_dataPointEntityAccess->getHiHiAlarmMessage(),
						 hiHiAlarmValue,
						 m_engineeringUnits,
						 m_dataPointEntityAccess->getHiHiAlarmMMS(),
						 m_alarmThresholdConstant
						 );
		FUNCTION_EXIT;
	}


	void AnalogueDataPoint::configureHiAlarm()
	{
		FUNCTION_ENTRY("configureHiAlarm");

        double hiValue;
        m_persistence->readPersistedValue( DP_HI_ALARM_VALUE, m_entityKey, hiValue );

        bool hiEnabled;
        m_persistence->readPersistedValue( DP_HI_ALARM_ENABLED, m_entityKey, hiEnabled );

		DpValue hiAlarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		hiAlarmValue.setFloat( hiValue, getPrecision() );

		setAlarmDetails( TA_Base_Bus::DptHi,
						 hiEnabled, //TD15446
						 (int) m_dataPointEntityAccess->getHiAlarmDelay(),
						 m_dataPointEntityAccess->getHiAlarmSeverity(),
						 m_dataPointEntityAccess->getHiAlarmMessage(),
						 hiAlarmValue,
						 m_engineeringUnits,
						 m_dataPointEntityAccess->getHiAlarmMMS(),
						 m_alarmThresholdConstant
						 );
		FUNCTION_EXIT;
	}


	void AnalogueDataPoint::configureLoAlarm()
	{
		FUNCTION_ENTRY("configureLoAlarm");

        double loValue;
        m_persistence->readPersistedValue( DP_LO_ALARM_VALUE, m_entityKey, loValue );

        bool loEnabled;
        m_persistence->readPersistedValue( DP_LO_ALARM_ENABLED, m_entityKey, loEnabled );

		DpValue loAlarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		loAlarmValue.setFloat( loValue, getPrecision() );

		setAlarmDetails( TA_Base_Bus::DptLo,
						 loEnabled, //TD15446
						 (int) m_dataPointEntityAccess->getLoAlarmDelay(),
						 m_dataPointEntityAccess->getLoAlarmSeverity(),
						 m_dataPointEntityAccess->getLoAlarmMessage(),
						 loAlarmValue,
						 m_engineeringUnits,
						 m_dataPointEntityAccess->getLoAlarmMMS(),
						 m_alarmThresholdConstant
						 );
		FUNCTION_EXIT;
	}


	void AnalogueDataPoint::configureLoLoAlarm()
	{
		FUNCTION_ENTRY("configureLoLoAlarm");

        double loloValue;
        m_persistence->readPersistedValue( DP_LOLO_ALARM_VALUE, m_entityKey, loloValue );

        bool loloEnabled;
        m_persistence->readPersistedValue( DP_LOLO_ALARM_ENABLED, m_entityKey, loloEnabled );


		DpValue loLoAlarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		loLoAlarmValue.setFloat( loloValue, getPrecision() );

		setAlarmDetails( TA_Base_Bus::DptLoLo,
						 loloEnabled, //TD15446
						 (int) m_dataPointEntityAccess->getLoLoAlarmDelay(),
						 m_dataPointEntityAccess->getLoLoAlarmSeverity(),
						 m_dataPointEntityAccess->getLoLoAlarmMessage(),
						 loLoAlarmValue,
						 m_engineeringUnits,
						 m_dataPointEntityAccess->getLoLoAlarmMMS(),
						 m_alarmThresholdConstant
						 );
		FUNCTION_EXIT;
	}


	void AnalogueDataPoint::configureOverScaleHiAlarm()
	{
		// TD14391.1
		FUNCTION_ENTRY("configureOverScaleHiAlarm");

        double ohiValue;
        m_persistence->readPersistedValue( DP_OHI_ALARM_VALUE, m_entityKey, ohiValue );

        bool ohiEnabled;
        m_persistence->readPersistedValue( DP_OHI_ALARM_ENABLED, m_entityKey, ohiEnabled );

		DpValue overScaleHiAlarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		overScaleHiAlarmValue.setFloat( ohiValue, getPrecision() );
		
		setAlarmDetails( TA_Base_Bus::DptOverScaleHi,
			ohiEnabled, //TD15446
			(int) m_dataPointEntityAccess->getOverScaleHiAlarmDelay(),
			m_dataPointEntityAccess->getOverScaleHiAlarmSeverity(),
			m_dataPointEntityAccess->getOverScaleHiAlarmMessage(),
			overScaleHiAlarmValue,
			m_engineeringUnits,
			m_dataPointEntityAccess->getOverScaleHiAlarmMMS(),
			m_alarmThresholdConstant
			);

		FUNCTION_EXIT;
	}


	void AnalogueDataPoint::configureOverScaleLoAlarm()
	{
		// TD14391.1
		FUNCTION_ENTRY("configureOverScaleLoAlarm");

        double oloValue;
        m_persistence->readPersistedValue( DP_OLO_ALARM_VALUE, m_entityKey, oloValue );

        bool oloEnabled;
        m_persistence->readPersistedValue( DP_OLO_ALARM_ENABLED, m_entityKey, oloEnabled );

		DpValue overScaleLoAlarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		overScaleLoAlarmValue.setFloat( oloValue, getPrecision() );
		
		setAlarmDetails( TA_Base_Bus::DptOverScaleLo,
			oloEnabled, //TD15446
			(int) m_dataPointEntityAccess->getOverScaleLoAlarmDelay(),
			m_dataPointEntityAccess->getOverScaleLoAlarmSeverity(),
			m_dataPointEntityAccess->getOverScaleLoAlarmMessage(),
			overScaleLoAlarmValue,
			m_engineeringUnits,
			m_dataPointEntityAccess->getOverScaleLoAlarmMMS(),
			m_alarmThresholdConstant
			);

		FUNCTION_EXIT;
	}


    void AnalogueDataPoint::updateAlarms( DataPointState & newState )
	{
        FUNCTION_ENTRY("updateAlarms");

        if ( TA_Base_Core::Control == m_operationMode )
		{
			if ( ( newState.isGoodValue() ) ||
				 ( QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED == newState.getStatus() ) )
			{
			    bool alarmCondition = false; // to indicate the alarm state
                bool hasAlarmStateChange = false; // to indicate if the alarm state changed
			    TA_Base_Bus::EDataPointAlarms alarmType = TA_Base_Bus::DptGeneral;

                {
                    ThreadGuard guard( m_alarmslock );
				    for(DpAlarmList::iterator alarmsIter = m_alarms.begin(); alarmsIter != m_alarms.end(); ++alarmsIter )
				    {
						bool isAlarmInhibited = (true == m_AlarmInhibitedState || IN_SERVICE != m_serviceState );
	                    bool isPreInAlarm = (* alarmsIter)->isInAlarm();
						
						(* alarmsIter)->updateState( getOMFlag(), newState, isAlarmInhibited );

	                    bool isInAlarm = (* alarmsIter)->isInAlarm();

						if (false == alarmCondition && true == isInAlarm )
						{
							alarmCondition = true;
							alarmType = (* alarmsIter)->getType();
						}

	                    if (false == hasAlarmStateChange && isPreInAlarm != isInAlarm)
	                    {
	                        hasAlarmStateChange = true;
	                    }
					}
                }

				saveAlarmState ( alarmType, alarmCondition );

				// if AckAnalogAlarmOnTransition is configured to true,
				// to ack the other unacked AND closed alarms if there is a NEW unacked AND opened one.
				if (m_ackAnalogAlarmOnTransition && alarmCondition && hasAlarmStateChange)
				{
					AlarmDetailCorbaDef* alarm = NULL;        
					std::vector<AlarmDetailCorbaDef*> alarms;

                    {
                        ThreadGuard guard( m_alarmslock );
					    for(DpAlarmList::iterator alarmsIter = m_alarms.begin(); alarmsIter != m_alarms.end(); alarmsIter ++)
					{ // to get the closed alarms
						if (false == (* alarmsIter)->isInAlarm())
						{
							try
							{
								alarm = NULL;
								alarm = (* alarmsIter)->getAlarmDetailPointer();  
								if (alarm)
								{
									alarms.push_back(alarm);
								}
							}
							catch( const TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ae)
							{
								std::ostringstream description;
								description << "getAlarmDetailPointer Error[name:" << m_name << "][AlarmDetail Type:" << (* alarmsIter)->getAlarmTypeStr() << "].";
								description << "; Reason: " << ae.reason.in();
								
								LOG_EXCEPTION_CATCH(SourceInfo, "AlarmAgentException", description.str().c_str());
							}
							catch( const TA_Base_Core::AlarmAgentException& aae)
							{
								std::ostringstream description;
								description << "getAlarmDetailPointer Error[name:" << m_name << "][AlarmDetail Type:" << (* alarmsIter)->getAlarmTypeStr() << "].";
								description << "; Reason: " << aae.what();
								
								LOG_EXCEPTION_CATCH(SourceInfo, "AlarmAgentException", description.str().c_str());
							}
							catch( const TA_Base_Core::ObjectResolutionException& ore )
							{
								std::ostringstream description;
								description << "getAlarmDetailPointer Error[name:" << m_name << "][AlarmDetail Type:" << (* alarmsIter)->getAlarmTypeStr() << "].";
								description << "; Reason: " << ore.what();
								
								LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", description.str().c_str());
							}
							catch( const TA_Base_Core::NoAlarmAgentException& naae )
							{
								std::ostringstream description;
								description << "getAlarmDetailPointer Error[name:" << m_name << "][AlarmDetail Type:" << (* alarmsIter)->getAlarmTypeStr() << "].";
								description << "; Reason: " << naae.what();
								
								LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", description.str().c_str());
							}
							catch(TA_Base_Core::TransactiveException & te)
							{
								std::ostringstream description;
								description << "getAlarmDetailPointer Error[name:" << m_name << "][AlarmDetail Type:" << (* alarmsIter)->getAlarmTypeStr() << "].";
								description << "; Reason: " << te.what();
								
								LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", description.str().c_str());
							}
							catch(...)
							{
								std::ostringstream description;
								description << "getAlarmDetailPointer Error[name:" << m_name << "][AlarmDetail Type:" << (* alarmsIter)->getAlarmTypeStr() << "].";
								
								LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", description.str().c_str());
							}

						}
                        }
					}
					
					if (false == alarms.empty())
					{ // to ack the inactive alarms
						try
						{  
							// to get the supper session id
							std::string session = TA_Base_Core::SessionAccessFactory::getInstance().getSuperSessionId();
							if (false == session.empty())
							{
								// this function will acknowledge the input alarms, and then clear it!
								m_alarmModificationHelper.acknowledgeAlarms(alarms, session.c_str());
							}
						}
						catch( const TA_Base_Core::AlarmAgentException& exception)  //TD15491, exception catch code cause agent crash when alarm agent raise exception
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "AlarmAgentException", exception.what());
						}
						catch( const TA_Base_Core::ObjectResolutionException& exception )
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", exception.what());
						}
						catch( const TA_Base_Core::NoAlarmAgentException& exception )
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", exception.what());
						}
						catch(TA_Base_Core::TransactiveException & exception)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", exception.what());
						}
						catch(...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown reason.");
						}
						
						// according to AlarmModificationHelp's changes, alarms should be deleted after they are acknowledged.
						std::vector<AlarmDetailCorbaDef*>::iterator it;
						for (it = alarms.begin(); it != alarms.end(); it ++)
						{ // debug info, to see which alarms are acknowledged.
							std::ostringstream description;
							description << "Alarm is acknoledged, DataPoint[name:" << m_name << "] Alarm ID:" << (* it)->alarmID;
							LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, description.str().c_str() );

							delete (* it);
						}
						
					}
					
				}
				
			}
		}
        FUNCTION_EXIT;

	}

}
