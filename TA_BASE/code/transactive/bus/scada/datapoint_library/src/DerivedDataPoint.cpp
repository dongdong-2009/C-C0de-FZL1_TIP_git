/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DerivedDataPoint.cpp $
  * @author  HoaVu
  * @version $Revision: #8 $
  * Last modification : $DateTime: 2017/07/03 11:12:52 $
  * Last modified by : $Author: dhanshri $
  *
  *
  * DerivedDataPoint, in SCADA term, is an equivalent of an enum.  It
  * combines several inputs into a single data point to represent a multi state
  * device.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER


#include "bus/scada/datapoint_library/src/DerivedDataPoint.h"
#include "bus/scada/datapoint_library/src/DerivedState.h"
#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/InputAssociation.h"

#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include <math.h>
#include <time.h>

using namespace TA_Base_Core;


namespace TA_Base_Bus
{

    DerivedDataPoint::DerivedDataPoint( TA_Base_Core::DataPointEntityDataPtr                dataPointEntityAccess,
                                        TA_Base_Bus::EDataPointType                         dpType,
                                        TA_Base_Bus::EDataPointDataType                     dpDataType,
                                        ScadaPersistencePtr                                 persistence,
                                        TA_Base_Core::ScadaRootEntityData&                  scadaRootEntityData,
                                        MmsScadaUtility&                                    mmsScadaUtility,
                                        CommandProcessor&                                   commandProcessor )
    :
    DataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor ),
    m_initialised( false ),
    m_currentState( NULL ),
    m_currentStatus( QUALITY_GOOD_NO_SPECIFIC_REASON ),
    m_derivedDataPointEntityData( 0 )
    {
        // 
        // Check Datapoint Data Type
        //
        if (dpDataType != TA_Base_Bus::DPT_ENUM_TYPE)
        {
            std::ostringstream desc;
            desc << "Invalid Configuration; DerivedDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
        }

        // initialise the state.
        DpValue dpv(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());

        struct timeb currentSystemTime;
        ftime ( &currentSystemTime );
        m_currentState = new DataPointState( dpv, currentSystemTime, m_currentStatus );
    }


    DerivedDataPoint::~DerivedDataPoint()
    {
        cleanup();

        // clean up handle to DerivedDataPointEntityData
        if ( NULL != m_derivedDataPointEntityData )
        {
            delete m_derivedDataPointEntityData;
            m_derivedDataPointEntityData = NULL;
        }

        // clean up state of this DerivedDataPoint
        if ( NULL != m_currentState )
        {
            delete m_currentState;
            m_currentState = NULL;
        }
    }


    void DerivedDataPoint::initialise()
    {
        // make sure only one thread can access this object
        ThreadGuard guard ( m_ownLock );

        // create handle to DerivedDataPointEntityData if not done so
        if ( NULL == m_derivedDataPointEntityData )
        {
            // create handle to DerivedDataPointEntityData
            m_derivedDataPointEntityData = new DerivedDataPointEntityData ( m_entityKey,
                                                                 m_name, m_dataPointEntityAccess->getAllowWrite());
        }

        // create references to input data point associated with this Derived DataPoint
        createDerivedInputAssociation();

        // create references to states configured for this Derived DataPoint
        createDerivedState();

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

        // calculate the combined value of this Derived DataPoint based on the
        // current values of the input data points
        unsigned long combinedValue = calculateCombinedValue();

        // check the current calculated value against configured derived states
        // Raise alarm and set output if necessary
        checkCombinedValue( combinedValue );

        // for each of the input data point
        std::vector < InputAssociation * >::iterator itr;
        for ( itr = m_listOfDerivedInputAssociations.begin();
              itr != m_listOfDerivedInputAssociations.end();
              itr++ )
        {
			(*itr)->setOperationMode ( true );
        }		
    }


    void DerivedDataPoint::cleanup()
    {
        // make sure only one thread can access this object
        ThreadGuard guard ( m_ownLock );

        // clean up the list of input entities associated with this DerivedDataPoint
        std::vector < InputAssociation * >::iterator dia_itr;
        for ( dia_itr = m_listOfDerivedInputAssociations.begin();
              dia_itr != m_listOfDerivedInputAssociations.end();
              dia_itr++ )
        {
            delete (*dia_itr);
			*dia_itr = NULL;
        }
        m_listOfDerivedInputAssociations.clear();

        // clean up the list of derived states configured for this DerivedDataPoint
        DerivedStatesMap::iterator cds_itr;
        for ( cds_itr = m_listOfDerivedStates.begin();
              cds_itr != m_listOfDerivedStates.end();
              cds_itr++ )
        {
            delete cds_itr->second;
			cds_itr->second = NULL;
        }
        m_listOfDerivedStates.clear();

    }

    boost::shared_ptr<DatapointCompleteStaticConfig> DerivedDataPoint::getCompleteStaticConfig()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<DatapointCompleteStaticConfig> toReturn = DataPoint::getCompleteStaticConfig();
        getEngineeringUnits(toReturn->engUnits);
        toReturn->alarmPropertyVect = getAlarmPropertiesVect();
        return toReturn;
    }

    boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> DerivedDataPoint::getCompleteDynamicUpdate()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeudpate = DataPoint::getCompleteDynamicUpdate();
        //set the current alarm properties
        completeudpate->alarmProperties = getAlarmPropertiesVect();
        return completeudpate;
    }

    TA_Base_Bus::AlarmPropertyVect DerivedDataPoint::getAlarmPropertiesVect()
    {
		//
		// Alarm Properties sequence
		//
        // set the size of the sequence
		 TA_Base_Bus::AlarmPropertyVect alarmProperties;
        // for each of the DerivedState associated with this derived data point
        for ( DerivedStatesMap::const_iterator cds_itr = m_listOfDerivedStates.begin();
              cds_itr != m_listOfDerivedStates.end();
              ++cds_itr )
		{
			// create an AlarmProperties on stack
			TA_Base_Bus::AlarmProperty almProperty;

			// alarm type
			almProperty.type = TA_Base_Bus::DptGeneral;

			// alarm enabled
			almProperty.enabled = cds_itr->second->getAlarmEnabled();

            //set up the thresold
            almProperty.threshold = 0;
            almProperty.enumValue = cds_itr->second->getStateValue();
			// assign to sequence
			alarmProperties.push_back(almProperty);
		}

        return alarmProperties;
    }

    void DerivedDataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
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

    void DerivedDataPoint::updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
        std::ostringstream propertyvalue; 
        propertyvalue << newAlarmProperties.enumValue;
        DpValue almValue (propertyvalue.str(), getDataPointDataType(),
                          getEnumLabelsMap(), getBooleanLabels(), getPrecision());
		
		unsigned long oldValue = almValue.getEnum();
		almValue.setEnum(newAlarmProperties.enumValue);
		bool almEnabled = newAlarmProperties.enabled;

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Datapoint (%s), newAlarmProperties.enumValue(%d), oldValue(%d), almValue.getEnum(%d), almValue.getValueAsString(%s)",
			m_name.c_str(), newAlarmProperties.enumValue, oldValue, almValue.getEnum(), almValue.getValueAsString().c_str());

        // update the database if there is a difference
        switch ( newAlarmProperties.type )
        {
            case TA_Base_Bus::DptGeneral:
            {
                // for each of the DerivedState associated with this derived data point
                DerivedStatesMap::iterator cds_itr;
                for ( cds_itr = m_listOfDerivedStates.begin();
                      cds_itr != m_listOfDerivedStates.end();
                      cds_itr++ )
                {
                    // if the state value equals to the alarm value
                    if ( newAlarmProperties.enumValue == (unsigned long) cds_itr->second->getStateValue() )
                    {
                        // update alarm settings
                        cds_itr->second->setAlarmEnabled ( almEnabled );

                        // update the alarm detail
                        setAlarmDetails ( newAlarmProperties.type,
                                             almEnabled,
                                             cds_itr->second->getAlarmDelay(),
                                             cds_itr->second->getAlarmSeverity(),
                                             cds_itr->second->getAlarmMessage(),
                                             almValue,
                                             cds_itr->second->getStateDescription(),
                                             cds_itr->second->getMMSAlarmConfiguration() );
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

    void DerivedDataPoint::setAlarmProperties ( const std::string& sessionID, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
		/*if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw TA_Base_Core::OperationModeException();
		}*/

        std::ostringstream propertyvalue; 
        propertyvalue << newAlarmProperties.enumValue;
        DpValue almValue (propertyvalue.str(), getDataPointDataType(),
                          getEnumLabelsMap(), getBooleanLabels(), getPrecision());

		unsigned long oldValue = almValue.getEnum();
		almValue.setEnum(newAlarmProperties.enumValue);
		bool almEnabled = newAlarmProperties.enabled;

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Datapoint (%s), newAlarmProperties.enumValue(%d), oldValue(%d), almValue.getEnum(%d), almValue.getValueAsString(%s)",
			m_name.c_str(), newAlarmProperties.enumValue, oldValue, almValue.getEnum(), almValue.getValueAsString().c_str());

        // update the database if there is a difference
        switch ( newAlarmProperties.type )
        {
            case TA_Base_Bus::DptGeneral:
            {
                // for each of the DerivedState associated with this derived data point
                DerivedStatesMap::iterator cds_itr;
                for ( cds_itr = m_listOfDerivedStates.begin();
                      cds_itr != m_listOfDerivedStates.end();
                      cds_itr++ )
                {
                    // if the state value equals to the alarm value
                    if ( newAlarmProperties.enumValue == (unsigned long) cds_itr->second->getStateValue() )
                    {
                        // update alarm settings
                        cds_itr->second->setAlarmEnabled ( almEnabled );

                        // update the alarm detail
                        setAlarmDetails ( newAlarmProperties.type,
                                             almEnabled,
                                             cds_itr->second->getAlarmDelay(),
                                             cds_itr->second->getAlarmSeverity(),
                                             cds_itr->second->getAlarmMessage(),
                                             almValue,
                                             cds_itr->second->getStateDescription(),
                                             cds_itr->second->getMMSAlarmConfiguration() );
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
        saveLastOperatorInfo ( sessionID, currentSystemTime );
    }


    void DerivedDataPoint::createDerivedInputAssociation()
    {
        if ( NULL != m_derivedDataPointEntityData )
        {
            // get all input entities configured with this DerivedDataPoint from database
            std::vector < ConfiguredInputAssociation * > m_listOfInputAssociations;
            m_derivedDataPointEntityData->getInputAssociations( m_listOfInputAssociations );

            // for each of the configured input association from database
            m_listOfDerivedInputAssociations.clear();
            std::vector < ConfiguredInputAssociation * >::iterator ciaItr;
            for ( ciaItr = m_listOfInputAssociations.begin();
                  ciaItr != m_listOfInputAssociations.end();
                  ciaItr++ )
            {
                // create a InputAssociation object and put in the list for
                // further reference
                InputAssociation * ipAssocation = new InputAssociation( (*ciaItr) );
                if ( NULL != ipAssocation )
                {
                    // pass the callback object to the InputAssociation
                    ipAssocation->registerForStateChangeNotification( this );

                    m_listOfDerivedInputAssociations.push_back( ipAssocation );
                }
            }
            m_listOfInputAssociations.clear();
        }
    }


	void DerivedDataPoint::createDerivedState()
    {
        if ( NULL != m_derivedDataPointEntityData )
        {
            // get all derived states configured for this DerivedDataPoint from database
            std::vector < ConfiguredDerivedState * > listOfConfiguredDerivedStates;
            m_derivedDataPointEntityData->getConfiguredDerivedStates ( listOfConfiguredDerivedStates );

            // for each of the configured derived states from database
            m_listOfDerivedStates.clear();
            m_enumLabelsMap.clear();
            std::vector < ConfiguredDerivedState * >::iterator cdsItr;
            for ( cdsItr = listOfConfiguredDerivedStates.begin();
                  cdsItr != listOfConfiguredDerivedStates.end();
                  cdsItr++ )
            {
                DerivedState * derivedState = 0;
				//ugly code,the pointer is passed to DerivedState, and managed by DerivedState ;
                derivedState = new DerivedState ( (*cdsItr), this );

				// put the derived state to the list for further reference,assure that the same state value not cause mem leak
                if( !m_listOfDerivedStates.insert(DerivedStatesMap::value_type(derivedState->getStateValue(),derivedState)).second )
                {
                	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"Entity %ld has the same derivedstate %d",this->getEntityKey(),derivedState->getStateValue());
                	delete derivedState;
					continue;
                }

                m_enumLabelsMap[derivedState->getStateValue()] = derivedState->getStateDescription();

                // create alarm object associated with the DerivedState for this data point
                if(derivedState->getAlarmEnabled())
                {
	                DpValue alarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
	                alarmValue.setEnum( derivedState->getStateValue() );
	                setAlarmDetails ( TA_Base_Bus::DptGeneral, // alarm type
	                                        derivedState->getAlarmEnabled(),    // alarm enabled
	                                        derivedState->getAlarmDelay(),      // alarm delay
	                                        derivedState->getAlarmSeverity(),   // alarm severity
	                                        derivedState->getAlarmMessage(),    // alarm message
	                                        alarmValue,                             // alarm value
	                                        derivedState->getStateDescription(),        // label
	                                        derivedState->getMMSAlarmConfiguration() ); // MMS alarm configuration
                }

            }
            listOfConfiguredDerivedStates.clear();
        }
    }


    unsigned long DerivedDataPoint::calculateCombinedValue()
    {
        unsigned long combinedValue = 0;

        // only proceed if in Control mode
        //if ( TA_Base_Core::Control == m_operationMode )
        {        
            m_currentStatus = QUALITY_GOOD_NO_SPECIFIC_REASON;
			
            // Raymond Pau++
            // TD13543 Corrected rounding error when computing derived datapoint value
            try
            {
                //
                // formulae:
                //  combinedValue = DP(n-1)*2**(n-1) + .... + DP(1)*2**(1) + DP(0)*2**(0)

                // for each of the input data point associated with the Derived DataPoint
                std::vector<timeb> vSaveTime; // we will save all the time to this vector, than choose the latest value.
                for ( std::vector< InputAssociation * >::iterator itr = m_listOfDerivedInputAssociations.begin();
                      itr != m_listOfDerivedInputAssociations.end(); ++itr )
                {
                    InputAssociation* input = (*itr);

                    if ( TA_Base_Bus::DPT_BOOL_TYPE != input->getInputDataPointDataType() )
                    {
                        // default current status of this DeriveDataPoint to this
                        m_currentStatus = QUALITY_BAD_CONFIGURATION_ERROR;

                        // Raymond Pau++ TD14922
                        struct timeb currentSystemTime;
                        ftime ( &currentSystemTime );
                        m_currentState->setTimestamp( currentSystemTime );
                        // ++Raymond Pau

                        // and exit the loop - no need to do further calculation
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                            "Derived DataPoint: %s has Input Association with invalid data type: %s with data type: %d",
                            getDataPointName().c_str(),
                            input->getInputDataPointName().c_str(),
                            input->getInputDataPointDataType() );
                            break;
                    }
                
                    int calculationOrder = input->getInputCalculationOrder();
                    const DataPointState & dpState = input->getCurrentState();
                    bool value = dpState.getValue().getBoolean();
                    
                    const timeb & dpStateTS = dpState.getTimebStructure();
                    const timeb & currentStateTS = m_currentState->getTimebStructure();
                    vSaveTime.push_back(dpStateTS);

                    // calculate combined value
                    if (true == value)
                    {
                        combinedValue |= (1 << calculationOrder);
                    }
                
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "%s: combined value=%d - Input: %s value=%s, order = %lu, status = %d",
                        m_name.c_str(),
                        combinedValue,
                        input->getInputDataPointName().c_str(),
                        value ? "TRUE" : "FALSE",
                        calculationOrder,
                        dpState.getStatus() );
                }
                
                // we choose the latest time to update m_currentState. m_currentState can be updating every time.
                if (0 != vSaveTime.size())
                {                    
                    timeb tempTime = vSaveTime[0];
                    for (int i = 1; i < vSaveTime.size(); i++)
                    {
                        if (vSaveTime[i].time > tempTime.time ||
                            ((vSaveTime[i].time == tempTime.time) &&
                            (vSaveTime[i].millitm > tempTime.millitm)))
                        {
                            tempTime = vSaveTime[i];
                        }
                    }
                    m_currentState->setTimestamp(tempTime);
                }
                
            }
            catch ( const TA_Base_Core::TransactiveException & te )
            {
                combinedValue = 0;
                m_currentStatus = QUALITY_BAD_NO_SPECIFIC_REASON;

                // Raymond Pau++ TD14922
                struct timeb currentSystemTime;
                ftime ( &currentSystemTime );
                m_currentState->setTimestamp( currentSystemTime );
                // ++Raymond Pau
            
                std::ostringstream desc;
                desc << "While computing combined value for Derived DataPoint: " << getDataPointName()
                    << " reason: " << te.what();
            
                LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", desc.str().c_str() );
            }
            catch ( ... )
            {
                combinedValue = 0;
                m_currentStatus = QUALITY_BAD_NO_SPECIFIC_REASON;

                // Raymond Pau++ TD14922
                struct timeb currentSystemTime;
                ftime ( &currentSystemTime );
                m_currentState->setTimestamp( currentSystemTime );
                // ++Raymond Pau

                std::ostringstream desc;
                desc << "Caught unknown exception while computing combined value for Derived DataPoint: " << getDataPointName();
            
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc.str().c_str() );
            }
            // TD13543 Corrected rounding error when computing derived datapoint value
            // ++Raymond Pau
        }

        return combinedValue;
    }


    void DerivedDataPoint::checkCombinedValue( unsigned long combinedValue )
    {
        // only in Control Mode
        //if ( TA_Base_Core::Control == m_operationMode )
        {
            // if status is bad
            if ( QUALITY_BAD_NOT_CONNECTED == m_currentStatus )
            {
                // attempt to recalculate the combined value
                calculateCombinedValue();
            }

            // if still bad quality
            if ( QUALITY_BAD_NOT_CONNECTED == m_currentStatus )
            {
                // set the value of this Derived DataPoint to the new calculated value
                // so that events are generated for client notification
                // NOTE: only update the field value of the Derived DataPoint and don't
                //       touch the forced value
                updateFieldState( *m_currentState );

                return;
            }

            // for each of the derived states configured for this Derived DataPoint
            DerivedStatesMap::iterator itr;
            for ( itr = m_listOfDerivedStates.begin();
                  itr != m_listOfDerivedStates.end();
                  itr++ )
            {
                // if the calculated value matches the derived state configured for this
                // Derived DataPoint
                if ( combinedValue == itr->second->getStateValue() )
                {
                    // update the current DataPointState of this DerivedDataPoint with the new
                    // calculated value and status
                    updateCurrentState( combinedValue, itr->second->getStateDescription() );

                    // set the value of this Derived DataPoint to the new calculated value
                    // so that events are generated for client notification
                    // NOTE: only update the field value of the Derived DataPoint and don't
                    //       touch the forced value
                    updateFieldState( *m_currentState );

                    // set the output associated with the derived state if applicable
                    itr->second->setOutputEntities(DPT_INTERNAL_SESSION_ID, m_auditEventLoggingEnabled, m_controlState); // TD12418

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Updated state for %s value = %lu, status = %d",
                        m_name.c_str(),
                        m_currentState->getValue().getEnum(),
                        m_currentStatus );
                }
            }
        }
    }


    void DerivedDataPoint::updateCurrentState( unsigned long stateValue, const std::string & stateDescription )
    {
        // only in Control mode
        //if ( TA_Base_Core::Control == m_operationMode )
        {
            // calculate the current status of this DerivedDataPoint
            calculateCombinedStatus();

            // create DpValue
            DpValue dpv(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
            dpv.setEnum( stateValue );

            // update with the new info
            m_currentState->setValue( dpv );
            m_currentState->setStatus( m_currentStatus );
        }
    }


    void DerivedDataPoint::calculateCombinedStatus()
    {
        EQualityStatus worstStatus = QUALITY_GOOD_NO_SPECIFIC_REASON;
        bool forcedInputDataPoint = false;

        // for each of the input data point
        std::vector < InputAssociation * >::iterator itr;
        for ( itr = m_listOfDerivedInputAssociations.begin();
              itr != m_listOfDerivedInputAssociations.end();
              ++itr )
        {
            // if current status of input data point is less than the
            // worst status, reassign the worst status
            if ( (*itr)->getCurrentState().getStatus() < worstStatus )
            {
                worstStatus = (*itr)->getCurrentState().getStatus();
            }

            // if current status of the input data point is forced
            if ( QUALITY_GOOD_LOCAL_OVERRIDE == (*itr)->getCurrentState().getStatus() )
            {
                // set the flag to indicate so
                forcedInputDataPoint = true;
            }
        }

        // assign the current status of the derived data point to the
        // worst status
        m_currentStatus = worstStatus;

        // if all input data points have good status
        // and if one of the input data point is forced
        if ( QUALITY_GOOD_NO_SPECIFIC_REASON == worstStatus &&
             true == forcedInputDataPoint )
        {
            // assign the current status of the derived data point to this
            m_currentStatus = QUALITY_GOOD_LOCAL_OVERRIDE;
        }
    }


    void DerivedDataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
    {
        bool reInitialised = false;

        // for each of the changed parameters
        for ( unsigned int i = 0; i < configChanges->size(); i++ )
        {
            std::string paramName = (*configChanges)[i];

            // if any parameters specific to the DerivedDataPoint has been changed
            if ( ( std::string::npos != paramName.find ( "Input Association" ) ) ||
                 ( std::string::npos != paramName.find ( "Output Association" ) ) ||
                 ( std::string::npos != paramName.find ( "State" ) ) )
            {
                // set the flag to indicate that the states of the DerivedDataPoint
                // needs to be reloaded
                reInitialised = true;

                // then exit loop, no need to go further
                break;
            }
        }

        // if the flag is set
        if ( true == reInitialised )
        {
            // only if the data point is in control mode
            if ( TA_Base_Core::Control == m_operationMode )
            {
                cleanup();
                initialise();
            }
        }
    }


    void DerivedDataPoint::processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType)
    {
        // make sure only one thread can access this object
        ThreadGuard guard ( m_ownLock );

        // only in Control mode and not scan inhibited
        if ( //TA_Base_Core::Control == m_operationMode && 
             IN_SERVICE == m_serviceState )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Received update event from entity pkey: %d", entityKey );

            // calculate the combined value of this Derived DataPoint based on the
            // current values of the input data points
            unsigned long combinedValue = calculateCombinedValue();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Updated combined value for %s", m_name.c_str() );
            
            // check the current calculated value against configured derived states
            // Raise alarm and set output if necessary
            checkCombinedValue( combinedValue );
        }
    }


    const std::string & DerivedDataPoint::getStateLabel(unsigned long state) const
    {
        // if in Monitor mode
        /*if ( TA_Base_Core::Monitor == m_operationMode )
        {
            throw TA_Base_Core::OperationModeException();
        }*/

        // if there are no DerivedState defined for this DerivedDataPoint
        if ( true == m_listOfDerivedStates.empty() )
        {
            throw TA_Base_Core::NotAvailableException();
        }

        DerivedStatesMap::const_iterator cds_itr = m_listOfDerivedStates.find(state);

        if (cds_itr == m_listOfDerivedStates.end())
        {
            std::ostringstream ss;
            ss << "Unknown state (" << state << ") for datapoint " << getDataPointName();

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "%s", ss.str().c_str());
            throw TA_Base_Core::BadParameterException(ss.str().c_str());
        }

        return cds_itr->second->getStateDescription();
    }


    void DerivedDataPoint::getEngineeringUnits( TA_Base_Bus::DataPointEngineeringUnits & eng )
    {
        // there is no engineering unit for a Derived DataPoint
        eng.EngineeringUnitsEnabled = false;
        eng.LowLimit = 0;
        eng.HighLimit = 0;
		eng.Units = "";
    }


    std::string DerivedDataPoint::getDataPointValueAsString( const DpValue & dpValue )
    {
        FUNCTION_ENTRY("getDataPointValueAsString");
        FUNCTION_EXIT;
        return getStateLabel( dpValue.getEnum() );
    }


    void DerivedDataPoint::setToControlMode()
    {
        // initialise all dependencies associated with this derived data point
        // if not done so
        if ( false == m_initialised )
        {
            m_initialised = true;
            initialise();
        }

        // call the base class first
        DataPoint::setToControlMode();

        // for each of the input data point
        
        std::vector < InputAssociation * >::iterator itr;
        for ( itr = m_listOfDerivedInputAssociations.begin();
              itr != m_listOfDerivedInputAssociations.end();
              itr++ )
        {
            (*itr)->resolveCurrentDataPointState();
        }
        
    }


    void DerivedDataPoint::setToMonitorMode()
    {
        // initialise all dependencies associated with this derived data point
        // if not done so
        if ( false == m_initialised )
        {
            m_initialised = true;
            initialise();
        }

        // call the base class first
        DataPoint::setToMonitorMode();

        // for each of the input data point
        /*
        std::vector < InputAssociation * >::iterator itr;
        for ( itr = m_listOfDerivedInputAssociations.begin();
              itr != m_listOfDerivedInputAssociations.end();
              itr++ )
        {
            (*itr)->setOperationMode ( false );
        }
        */
    }


    void DerivedDataPoint::updateFieldState( DataPointState & newState )
    {
		// store latest field state
		m_lastKnownFieldState = newState;

		// only process newState if the data point is not scan inhibited.
		if ( IN_SERVICE == m_serviceState )
		{
			//
			// Make type-specific determination of the validity of this new state
			//
			if ( isValidUpdate( newState ) )
			{
				// if valid state then update the datapoint state
				updateDataPointState( newState, true );
			}
		}
    }


    bool DerivedDataPoint::updateInServiceStatus( bool inService )
    {
        bool result = false;

        // call the base class first
        result = DataPoint::updateInServiceStatus ( inService );

        // if successfull
        if ( true == result )
        {
            // if data point is in service, force the recalculation of the derived states
            if ( true == inService )
            {
                // calculate the combined value of this Derived DataPoint based on the
                // current values of the input data points
                unsigned long combinedValue = calculateCombinedValue();

                // check the current calculated value against configured derived states
                // Raise alarm and set output if necessary
                checkCombinedValue ( combinedValue );
            }
        }

        return result;
    }

	void DerivedDataPoint::initAfterProxyFactoryReady()
	{
		if ( false == m_initialised )
		{
			m_initialised = true;
			initialise();

			DataPoint::initAfterProxyFactoryReady();
		}
	}
}
