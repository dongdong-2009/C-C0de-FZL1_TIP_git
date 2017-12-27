/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/MeteredDataPoint.cpp $
  * @author  HoaVu
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  * Metered DataPoints are accumulators that are updated periodically
  * to provide a totalised meter unit count.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER


#include "bus/scada/datapoint_library/src/MeteredDataPoint.h"
#include "bus/scada/datapoint_library/src/InputAssociation.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h"
#include "core/exceptions/src/CorbaException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/types/DataPointAudit_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "time.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	MeteredDataPoint::MeteredDataPoint( TA_Base_Core::DataPointEntityDataPtr					dataPointEntityAccess,
										TA_Base_Bus::EDataPointType			                    dpType,
										TA_Base_Bus::EDataPointDataType		                    dpDataType,
                                        ScadaPersistencePtr                                     persistence,
                                        TA_Base_Core::ScadaRootEntityData&                      scadaRootEntityData,
                                        MmsScadaUtility&                                        mmsScadaUtility,
                                        CommandProcessor&                                       commandProcessor )
	:
	AnalogueDataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor ),
	m_initialised( false ),
    m_EngineeringLabel ( "" ),
    m_lastInputValue ( 0.0 ),
    m_inputDependencyPKey ( 0 ),
	m_rolloverConstant ( 0.0 ),
	m_accumulatedInputValue ( 0.0 ),
	m_meterFactor ( 1.0 ), //TD15121
	m_currentStatus ( QUALITY_BAD_NOT_CONNECTED ),
	m_currentState ( NULL ),
    m_inputAssociation ( NULL )	
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
            desc << "Invalid Configuration; MeteredDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}

		// initialise the state.
		struct timeb currentSystemTime;
		ftime ( &currentSystemTime );
		DpValue dpv(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		dpv.setFloat(0, getPrecision());
		m_currentState = new DataPointState ( dpv, currentSystemTime, m_currentStatus );
	}


	MeteredDataPoint::~MeteredDataPoint()
	{
		cleanup();

		// clean up state of this DerivedDataPoint
		if ( 0 != m_currentState )
		{
			delete m_currentState;
			m_currentState = 0;
		}
	}


	void MeteredDataPoint::updateInputAssociation ( unsigned long inputKey )
	{
		// only proceed if the pkeys are different
		if ( m_inputDependencyPKey != inputKey )
		{
			// save the new pkey
			m_inputDependencyPKey = inputKey;

			// create handle to the input data point associated with 
			// this Metered data point
			createDerivedInputAssociation();

			// calculate the totalised value from the current value of the input
			//TD15121, should not calculateValue before InputAssociation will be ready.
			//It will be called in processEntityUpdateEvent() when InputAssociation got ready
			//calculateValue();
		}
	}


	void MeteredDataPoint::initialise()
	{
		// this will ensure this block is only execute once and only once
		if ( 0 == m_inputDependencyPKey )
		{
            m_persistence->readPersistedValue( DP_LAST_INPUT_VALUE, m_entityKey, m_lastInputValue );
            m_persistence->readPersistedValue( DP_ACCUMULATED_INPUT_VALUE, m_entityKey, m_accumulatedInputValue );
            
			double correctionValue = 0;
			m_persistence->readPersistedValue( DP_CORRECTION_VALUE, m_entityKey, correctionValue );

			m_rolloverConstant = m_dataPointEntityAccess->getRolloverConstant();
			m_meterFactor = m_dataPointEntityAccess->getMeterFactor();

			correct(correctionValue, "");
		}

		// create the input object associated with this Metered DataPoint
		updateInputAssociation ( m_dataPointEntityAccess->getMIDataPointPKey() );
	}


	void MeteredDataPoint::cleanup()
	{
		// make sure only one thread can access this object
		ThreadGuard guard ( m_ownLock );

		// clean up handle to Input data point associated with
		// this Metered data point
		if ( 0 != m_inputAssociation )
		{
			delete m_inputAssociation;
			m_inputAssociation = NULL;
			m_inputDependencyPKey = 0;
		}
	}


	void MeteredDataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
	{
		// call update specific configuration of AnalogueDataPoint
		AnalogueDataPoint::updateSpecificConfiguration ( configChanges );

		// for each of the changed parameters
		for ( unsigned int i = 0; i < configChanges->size(); i++ )
		{
			std::string paramName = (*configChanges)[i];
			
			// input associated with this Metered DataPonit has been changed
			if ( TA_Base_Core::DataPointEntityData::DATAPOINT_MI_DATAPOINT_PKEY == paramName )
			{
				initialise();
			}

			// Rollover Constant associated with this Metered DataPoint has been changed
			else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_ROLLOVER_CONSTANT == paramName )
			{
				m_rolloverConstant = m_dataPointEntityAccess->getRolloverConstant();

				// calculate the totalised value from the current value of the inputs
				calculateValue();
			}

			// Correction Value associated with this Metered DataPoint has been edited
			else if ( mapEPersistedItem2DataRecordingId(DP_CORRECTION_VALUE) == paramName )
			{
                double correctionValue = 0;
				m_persistence->readPersistedValue( DP_CORRECTION_VALUE, m_entityKey, correctionValue );

				correct(correctionValue, "");
			}
			else if ( TA_Base_Core::DataPointEntityData::DATAPOINT_METER_FACTOR == paramName )
			{
				m_meterFactor = m_dataPointEntityAccess->getMeterFactor();

				// calculate the totalised value from the current value of the inputs
				calculateValue();
			}
			else 
			{
				// do nothing
			}
		}
	}


	void MeteredDataPoint::createDerivedInputAssociation()
	{
		// make sure only one thread can access this object
		ThreadGuard guard ( m_ownLock );

		IEntityData * inputEntity = 0;
		ConfiguredInputAssociation * cia = 0;

		// if the pointer to the associated input has been initialised
		// then delete the previous object
		if ( 0 != m_inputAssociation )
		{
			delete m_inputAssociation;
			m_inputAssociation = 0;
		}

		try
		{
            std::string inputDataPointName;
            std::string inputDataPointAgentName;
            unsigned long inputDataPointSubsystemKey = 0;
            unsigned long inputDataPointLocationKey = 0;
            
            // first try to get the entity from DataNodeAccessFactory cache
            TA_Base_Core::IEntityDataPtr entityData = DataNodeAccessFactory::getInstance()->getEntityData(m_inputDependencyPKey);
            if (entityData.get() != NULL)
            {
                inputDataPointName = entityData->getName();
                inputDataPointAgentName = entityData->getAgentName();
                inputDataPointSubsystemKey = entityData->getSubsystem();
                inputDataPointLocationKey = entityData->getLocation();
            }
            else
            {
                // if not found, load from db
			    // get the database entity of the input object
			    std::auto_ptr<IEntityData> inputEntity(EntityAccessFactory::getInstance().getEntity ( m_inputDependencyPKey ));
                inputDataPointName = inputEntity->getName();
                inputDataPointAgentName = inputEntity->getAgentName();
                inputDataPointSubsystemKey = inputEntity->getSubsystem();
                inputDataPointLocationKey = inputEntity->getLocation();
            }

			// create an instance of the ConfiguredInputAssociation with intial values
			// obtained from database
			cia = new ConfiguredInputAssociation ( m_inputDependencyPKey,
												   inputDataPointName,
												   inputDataPointAgentName,
												   inputDataPointSubsystemKey,
												   inputDataPointLocationKey );

			// intialise the input object
			m_inputAssociation = new InputAssociation ( cia );
			if ( 0 != m_inputAssociation )
			{
				// pass the callback object to the InputAssociation
				m_inputAssociation->registerForStateChangeNotification ( this );
			}

			// clean up database entity
			delete inputEntity;
			inputEntity = 0;
		}
		catch ( ... )
		{
			//
			// clean up debris
			//

			if ( 0 != inputEntity )
			{
				delete inputEntity;
				inputEntity = 0;
			}

			if ( 0 != cia )
			{
				delete cia;
				cia = 0;
			}

			if ( 0 != m_inputAssociation )
			{
				delete m_inputAssociation;
				m_inputAssociation = 0;
			}
		}
	}


	void MeteredDataPoint::processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType)
	{
		// calculate the totalised value from the current value of the input
		calculateValue();
	}


	void MeteredDataPoint::calculateValue()
	{
		// make sure only one thread can access this object
		ThreadGuard guard ( m_ownLock );

		// only in Control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			double currentValue = 0;
			m_currentStatus = QUALITY_BAD_NOT_CONNECTED;

			// if the input associated with this metered data point is intialised
			if ( 0 != m_inputAssociation )
			{
				// determine the type of the data to deal with
				switch ( m_inputAssociation->getInputDataPointDataType() )
				{
					case DPT_NULL_TYPE:
					{
						// do nothing, the input entity associated with this
						// Metered DataPoint has not been resolved yet

						break;
					}
					case DPT_TEXT_TYPE:
					{
						// do nothing, not applicable: an MI value cannot be a text

						break;
					}
					case DPT_BOOL_TYPE:
					{
						// do nothing, not applicable: an MI value cannot be a boolean

						break;
					}
					case DPT_ENUM_TYPE:
					{
						// do nothing, not applicable: an MI value cannot be an enum type

						break;
					}
					default:
					{
						// get the current value of the input data point
						currentValue = m_inputAssociation->getCurrentState().getValue().getFloat();

						// get the current status of the input data point
						m_currentStatus = m_inputAssociation->getCurrentState().getStatus();

						break;
					}
				}
			}

			// calculate the totalised value and update current state
			// of this Metered DataPoint
			updateCurrentState ( totalising ( currentValue ) );
		}
		// else if in Monitor mode, do nothing
	}


	void MeteredDataPoint::updateCurrentState ( double stateValue )
	{
		// create DpValue
		DpValue dpv(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		dpv.setFloat( stateValue, getPrecision() );

		// create timestamp
		struct timeb currentSystemTime;
		ftime ( &currentSystemTime );

		// update with the new info
		m_currentState->setValue ( dpv );
		m_currentState->setTimestamp ( currentSystemTime );
		m_currentState->setStatus ( m_currentStatus );

		// set the value of this Metered DataPoint to the new calculated value
		// so that events are generated for client notification
		updateFieldState ( *m_currentState );
	}


	double MeteredDataPoint::totalising ( double inputValue )
	{
		if (m_currentStatus == QUALITY_GOOD_NO_SPECIFIC_REASON)
		{ //TD15121, only update value when current status is good.
			double increments = 0;

			// calculate the difference between current and last input value,
			// taking into consideration of input value being overflowed
			if ( inputValue < m_lastInputValue )
			{
				//increments = ( m_rolloverConstant - m_lastInputValue ) + inputValue;
				//DAR_955_ICD_2015-B_Interface_Control_Doc_for_C955-C953(Power).doc
				//DTL-291 
				increments = ( m_rolloverConstant - m_lastInputValue ) + inputValue + 1;
			}
			else
			{
				increments = inputValue - m_lastInputValue;
			}

			// calculate the accumulated value, taking into consideration of
			// correction value as well
			m_accumulatedInputValue += increments;

			// save the current input value for next calculation, persist the value
			// to database in case if the system is down
			persistLastInputValue ( inputValue );
			persistAccumulatedValue();
		}
		
		// convert the accumulated value to eng. value, using the meter factor
		return ( m_accumulatedInputValue * m_meterFactor );
	}


	void MeteredDataPoint::persistLastInputValue ( double lastInputValue )
	{
		// save the last input value;
		m_lastInputValue = lastInputValue;

		// persist last input value to database
		if ( false == m_persistence->writePersistedValue( DP_LAST_INPUT_VALUE, m_entityKey, (long) lastInputValue, m_fieldValueQualityStatus ) )
		{
			char buffer [ 100 ] = {0};
			sprintf ( buffer, "Failed to persist Last Input Value %f for %s", m_lastInputValue, m_name.c_str() );
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, buffer );
		}
	}


	bool MeteredDataPoint::persistAccumulatedValue()
	{
		if ( false == m_persistence->writePersistedValue( DP_ACCUMULATED_INPUT_VALUE, m_entityKey, m_accumulatedInputValue, m_fieldValueQualityStatus ) )
		{
			char buffer [ 100 ] = {0};
			sprintf ( buffer, "Failed to persist Accumulated Value %f for %s", m_accumulatedInputValue, m_name.c_str() );
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, buffer );
			return false;
		}
		return true;
	}


	void MeteredDataPoint::persistCorrectionValue ( double correctionValue )
	{
		if ( false == m_persistence->writePersistedValue( DP_CORRECTION_VALUE, m_entityKey, correctionValue, m_fieldValueQualityStatus ) )
		{
			char buffer [ 100 ] = {0};
			sprintf ( buffer, "Failed to persist Correction Value %f for %s", correctionValue, m_name.c_str() );
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, buffer );
		}
	}


	void MeteredDataPoint::setToControlMode()
	{
		// initialise all dependencies associated with this derived data point
		// if not done so
		if ( false == m_initialised )
		{
            m_initialised = true;
			initialise();
		}

		// call the base class first
		AnalogueDataPoint::setToControlMode();

		m_inputAssociation->setOperationMode ( true );
	}


	void MeteredDataPoint::setToMonitorMode()
	{
		// initialise all dependencies associated with this derived data point
		// if not done so
		if ( false == m_initialised )
		{
            m_initialised = true;
			initialise();
		}

		// call the base class first
		AnalogueDataPoint::setToMonitorMode();

		m_inputAssociation->setOperationMode ( false );
	}


    void MeteredDataPoint::setMeteredCorrectionValue ( const std::string& sessionID, double newValue )
	{
		FUNCTION_ENTRY("SetMeteredCorrectionValue");

		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}


		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
		std::stringstream ss;
		//14420++
		//ss.setf(ios::fixed);
		ss.precision(12);
		//++14420
		ss << newValue;

		NameValuePair pair1 ( "ValueString", ss.str() );
		descriptionParameters.push_back ( &pair1 );

		// if the requested action is authorised for the sessionID
		if ( true == isActionPermitted ( sessionID, TA_Base_Bus::aca_DATA_POINT_METER_ADJUST ) )
		{
			bool bTemp = correct(newValue, sessionID);
			if (false == bTemp)
			{
				// log auditing event
				//submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::MeteringAdjustmentPersistenceFailed );
				TA_THROW_CORBA( TA_Base_Bus::ScadaCorbaTypes::PersistenceFailed());
			}
		}
		else
		{
			// log auditing event
			submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::MeteringAdjustmentPermissionDenied );

			TA_THROW_CORBA( TA_Base_Core::AccessDeniedException() );
		}

		FUNCTION_EXIT;
	}

	bool MeteredDataPoint::correct ( double newValue, std::string sessionId )
	{
		// make sure only one thread can access this object
		ThreadGuard guard ( m_ownLock );
		
		// Get the eng correction value
		DpValue engCorrectionValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
		engCorrectionValue.setFloat( newValue, getPrecision() );

		// Get the raw correction value
		DpValue rawCorrectionValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());	
		convertToRaw(engCorrectionValue, rawCorrectionValue);

		// Remove the meter factor
		double correctionValue = rawCorrectionValue.getFloat();
		if (m_meterFactor > 0.000001) // TD15121, To avoid overflow.
		{
			correctionValue = rawCorrectionValue.getFloat() / m_meterFactor;
		}

		// Apply the correction
		double lastAccumulatedInputValue = m_accumulatedInputValue;
		m_accumulatedInputValue += correctionValue;
		if (lastAccumulatedInputValue != m_accumulatedInputValue)
		{
			bool persistOk = persistAccumulatedValue();
			if (false == persistOk)
			{
				m_accumulatedInputValue -= correctionValue;	//
				return false;
			}
		}
		// update the state - this updates both DP raw and corrected values
		updateCurrentState ( m_accumulatedInputValue * m_meterFactor );

		double preCorrectionValue = 0;
		m_persistence->readPersistedValue( DP_CORRECTION_VALUE, m_entityKey, preCorrectionValue );
		if ( !(preCorrectionValue >= -0.000001 && preCorrectionValue <= 0.000001) )
		{
			// reset the correction value
			persistCorrectionValue ( 0 );
		}
		
		// log auditing event
		if ("" != sessionId)
		{
			// construct the DescriptionParameters for auditing messages
			TA_Base_Core::DescriptionParameters descriptionParameters;		
			descriptionParameters.clear();
			std::stringstream ss;
			//14420++
			//ss.setf(ios::fixed);
			ss.precision(12);
			//++14420
			ss << newValue;
			NameValuePair pair1 ( "ValueString", ss.str() );
			descriptionParameters.push_back ( &pair1 );

			submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::MeteringAdjustmentSucceeded );
		}
		return true;
	}
    
    void MeteredDataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
    {
        FUNCTION_ENTRY("updateSynState");
        //only the monitor mode datapoint required to syn the control mode
        if ( TA_Base_Core::Control == m_operationMode )
        {
            throw( TA_Base_Core::OperationModeException() );
        }
        
        // for metered datapoint, should update other configuration parameters as well
        // because the value may got update by their changes
        switch(toupdate.updateType)
        {
        case CompleteDynamicSyncStateUpdate:
        case ValueUpdate:
            {
                m_persistence->resetPersistedState(m_entityKey);
                m_persistence->readPersistedValue( DP_LAST_INPUT_VALUE, m_entityKey, m_lastInputValue );
                m_persistence->readPersistedValue( DP_ACCUMULATED_INPUT_VALUE, m_entityKey, m_accumulatedInputValue );
            }
            break;
		default:
			break;
        }
        //go to the base class to update the rest
        AnalogueDataPoint::updateSynState(toupdate);
    }
    
    void MeteredDataPoint::updateFieldState( DataPointState & newState )
    {
        //if the new state exceeds datatype range, rollover it.
        if (! isValidDataTypeValue(newState.getValue()))
        {
            double newValue = newState.getValue().getFloat();

           // rollover until the newValue is less than maximum value.
            while (newValue > getDataTypeLimitValue(true))
            {
                newValue -= (getDataTypeLimitValue(true) - getDataTypeLimitValue(false));
            }
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "MeteredDataPoint [%s] rollover its accumulated value from [%s] to [%f], which exceeds datatype limit.", 
                getDataPointName().c_str(), newState.getValue().getValueAsString().c_str(), newValue);

            newState.getValue().setFloat(newValue);
        }

        //go to the base class to update the rest
        AnalogueDataPoint::updateFieldState( newState );
    }
	
	void MeteredDataPoint::initAfterProxyFactoryReady()
	{
		if ( false == m_initialised )
		{
			m_initialised = true;
			initialise();
			
			DataPoint::initAfterProxyFactoryReady();
		}
	}
}

