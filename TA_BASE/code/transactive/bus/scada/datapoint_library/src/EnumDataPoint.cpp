/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/EnumDataPoint.cpp $
  * @author  HoaVu
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  * EnumDataPoint is a multi-state data point.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER


#include "bus/scada/datapoint_library/src/EnumDataPoint.h"
#include "bus/scada/datapoint_library/src/DerivedState.h"

#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include "time.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

    EnumDataPoint::EnumDataPoint( TA_Base_Core::DataPointEntityDataPtr              dataPointEntityAccess,
                                  TA_Base_Bus::EDataPointType     dpType,
                                  TA_Base_Bus::EDataPointDataType dpDataType,
                                  ScadaPersistencePtr                               persistence,
                                  TA_Base_Core::ScadaRootEntityData&                scadaRootEntityData,
                                  MmsScadaUtility&                                  mmsScadaUtility,
                                  CommandProcessor&                                 commandProcessor )
    :
    DataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor ),
    m_dummyReturnCondition ( "" ),
    m_initialised ( false ),
    m_derivedDataPointEntityData ( 0 )
    {
        // 
        // Check Datapoint Data Type
        //
        if (dpDataType != TA_Base_Bus::DPT_ENUM_TYPE)
        {
            std::ostringstream desc;
            desc << "Invalid Configuration; EnumDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
            TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
        }

        m_listOfDerivedStates.clear();
    }


    EnumDataPoint::~EnumDataPoint()
    {
        cleanup();
    }


    void EnumDataPoint::initialise()
    {
        // make sure only one thread can access this object
        ThreadGuard guard ( m_ownLock );

        // create handle to DerivedDataPointEntityData
        m_derivedDataPointEntityData = new DerivedDataPointEntityData ( m_entityKey,
                                                                        m_name, 
                                                                        m_dataPointEntityAccess->getAllowWrite());
        // create references to states configured for this EnumDataPoint
        createDerivedState();

        //
        // Set up the forced value
        //
		//Only the input data point can configurate alarm and force value.
		if (false == m_writable)
		{        
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


    void EnumDataPoint::cleanup()
    {
        // make sure only one thread can access this object
        ThreadGuard guard ( m_ownLock );

        // clean up the list of derived states configured for this EnumDataPoint
        DerivedStatesMap::iterator cds_itr;
        for ( cds_itr = m_listOfDerivedStates.begin();
              cds_itr != m_listOfDerivedStates.end();
              cds_itr++ )
        {
            delete cds_itr->second;
			cds_itr->second = NULL;
        }
        m_listOfDerivedStates.clear();

        // clean up handle to DerivedDataPointEntityData
        if ( 0 != m_derivedDataPointEntityData )
        {
            delete m_derivedDataPointEntityData;
            m_derivedDataPointEntityData = 0;
        }
    }


    void EnumDataPoint::createDerivedState()
    {
        if ( 0 != m_derivedDataPointEntityData )
        {
            // get all derived states configured for this DerivedDataPoint from database
            std::vector < ConfiguredDerivedState * > m_listOfConfiguredDerivedStates;
            m_derivedDataPointEntityData->getConfiguredDerivedStates ( m_listOfConfiguredDerivedStates );

            // for each of the configured derived states from database
            m_listOfDerivedStates.clear();
            m_enumLabelsMap.clear();
            std::vector < ConfiguredDerivedState * >::iterator cdsItr;
            for ( cdsItr = m_listOfConfiguredDerivedStates.begin();
                  cdsItr != m_listOfConfiguredDerivedStates.end();
                  cdsItr++ )
            {
                // create a DerivedState object
                DerivedState * derivedState = 0;
                derivedState = new DerivedState ( (*cdsItr), this );

                m_enumLabelsMap[ (unsigned long) derivedState->getStateValue() ] = derivedState->getStateDescription();

		 		//Only the input data point can configurate alarm and force value.
				if (false == m_writable)
				{
					// create alarm object associated with the DerivedState for this data point
	                if(derivedState->getAlarmEnabled())
	                {
		                DpValue alarmValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels()); 
		                alarmValue.setEnum( derivedState->getStateValue() );

		                setAlarmDetails( TA_Base_Bus::DptGeneral,   // alarm type
		                                 derivedState->getAlarmEnabled(),           // alarm enabled
		                                 derivedState->getAlarmDelay(),             // alarm delay
		                                 derivedState->getAlarmSeverity(),          // alarm severity
		                                 derivedState->getAlarmMessage(),           // alarm message
		                                 alarmValue,                                // alarm value
		                                 derivedState->getStateDescription(),       // label
		                                 derivedState->getMMSAlarmConfiguration()); // MMS alarm configuration
	                }
				}

                // put the derived state to the list for further reference
                m_listOfDerivedStates [ (unsigned long) derivedState->getStateValue() ] = derivedState;
            }
            m_listOfConfiguredDerivedStates.clear();
        }
    }


    void EnumDataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
    {
        //
        // check online config update for this Enum data point
        //

        bool reInitialised = false;

        // for each of the changed parameters
        for ( unsigned int i = 0; i < configChanges->size(); i++ )
        {
            std::string paramName = (*configChanges)[i];

            // if any parameters specific to the EnumDataPoint has been changed
            if ( ( std::string::npos != paramName.find ( "Output Association" ) ) ||
                 ( std::string::npos != paramName.find ( "State" ) ) )
            {
                // set the flag to indicate that the states of the EnumDataPoint
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


    bool EnumDataPoint::isValidStateValue ( const DpValue & dpValue )
    {
        bool result = false;

        //
        // looking for a unsigned long value only, text or boolean is error.
        // don't check for quality status as a valid value could have
        // either bad or good quality status.
        //

        if (dpValue.getType() == DPT_ENUM_TYPE)
        {       
            // find if any of the derived states configured for this Enum DataPoint
            // has state value matches with the specified value
            DerivedStatesMap::iterator itr = m_listOfDerivedStates.find ( (unsigned long) dpValue.getEnum() );

            // if found one
            if ( itr != m_listOfDerivedStates.end() )
            {
                result = true;
            }
        }

        return result;
    }


    bool EnumDataPoint::isValidUpdate( DataPointState & newState )
    {
        bool result = false;

        // check the value against the configured states of this Enum data point
        result = isValidStateValue ( newState.getValue() );

        // if failed to validate the specified state, output error to log file
        if ( false == result )
        {
            std::stringstream ss;
            ss << "Invalid input value (" << newState.getValue().getValueAsString() << ") for " << m_name;
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss.str().c_str() );
        }

        return result;
    }


    bool EnumDataPoint::isValidWrite( DpValue & newValue )
    {
        bool result = false;

        // check the value against the configured states of this Enum data point
        result = isValidStateValue ( newValue );

        // if failed to validate the specified value, output error to log file
        if ( false == result )
        {
            std::stringstream ss;
            ss << "Invalid output value (" << newValue.getValueAsString() << ") for " << m_name;
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss.str().c_str() );
        }

        return result;
    }


    void EnumDataPoint::setToControlMode()
    {
        // initialise all dependencies associated with this Enum DataPoint
        if ( false == m_initialised )
        {
            m_initialised = true;
            initialise();
        }

        // call the base class first
        DataPoint::setToControlMode();
    }


    void EnumDataPoint::setToMonitorMode()
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
    }


    const std::string & EnumDataPoint::getStateReturnCondition ( const DpValue & dpValue )
    {
        bool result = false;
        DerivedStatesMap::iterator itr;

        //
        // looking for a unsigned long value only, text or boolean is error.
        // don't check for quality status as a valid value could have
        // either bad or good quality status.
        //

        if ( dpValue.getType() == DPT_ENUM_TYPE )
        {
            // find if any of the derived states configured for this Enum DataPoint
            // has state value matches with the specified value
            itr = m_listOfDerivedStates.find ( (unsigned long) dpValue.getEnum() );

            // if found one
            if ( itr != m_listOfDerivedStates.end() )
            {
                result = true;
            }
        }

        if ( true == result )
        {
            return itr->second->getStateReturnCondition();
        }
        else
        {
            return m_dummyReturnCondition;
        }
    }


    void EnumDataPoint::startABETimeout(const std::string& sessionID)
    {
        // get the ReturnCondition associated with the current output field value
        std::string stateReturnCondition = getStateReturnCondition ( getCurrentEngValue() );

        // if the ReturnCondition is defined
        if ( false == stateReturnCondition.empty() )
        {
            // assign new ABE expression
            m_abeExpression = stateReturnCondition;
        }
        // else use the ReturnCondition defined at DataPoint level

        // call the base class
        DataPoint::startABETimeout(sessionID);
    }

    const std::string & EnumDataPoint::getStateLabel(unsigned long state) const
    {
        if ( TA_Base_Core::Monitor == m_operationMode )
        {
            throw TA_Base_Core::OperationModeException();
        }

        // if there are no DerivedState defined for this EnumDataPoint
        if ( true == m_listOfDerivedStates.empty() )
        {
            throw TA_Base_Core::NotAvailableException();
        }

        DerivedStatesMap::const_iterator cds_itr = m_listOfDerivedStates.find(state);

        if (cds_itr == m_listOfDerivedStates.end())
        {
            std::ostringstream ss;
            ss << "Unknown state (" << state << ") for datapoint " << getDataPointName();

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, ss.str().c_str());
            throw TA_Base_Core::BadParameterException(ss.str().c_str());
        }

        return cds_itr->second->getStateDescription();
    }

    void EnumDataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
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

    void EnumDataPoint::updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
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
                // find if any of the derived states configured for this Enum DataPoint
                // has state value matches with the specified alarm value
                DerivedStatesMap::const_iterator itr = m_listOfDerivedStates.find ( newAlarmProperties.enumValue );
                // if found one
                if ( itr != m_listOfDerivedStates.end() )
                {
                    // update alarm settings
                    itr->second->setAlarmEnabled ( almEnabled );
                    // update the alarm detail
                    setAlarmDetails ( newAlarmProperties.type,
                                         almEnabled,
                                         itr->second->getAlarmDelay(),
                                         itr->second->getAlarmSeverity(),
                                         itr->second->getAlarmMessage(),
                                         almValue,
                                         itr->second->getStateDescription(),
                                         itr->second->getMMSAlarmConfiguration() );
                }
                // else do nothing

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
            {
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unkown alarm property type[%d] received in datapoint [%d]", newAlarmProperties.type, m_entityKey );
                break;
            }
        }
   }

    void EnumDataPoint::setAlarmProperties ( const std::string& sessionID, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
        if ( TA_Base_Core::Monitor == m_operationMode )
        {
            throw TA_Base_Core::OperationModeException();
        }

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
                // find if any of the derived states configured for this Enum DataPoint
                // has state value matches with the specified alarm value
                DerivedStatesMap::const_iterator itr = m_listOfDerivedStates.find ( newAlarmProperties.enumValue );

                // if found one
                if ( itr != m_listOfDerivedStates.end() )
                {
                    // update alarm settings
                    itr->second->setAlarmEnabled ( almEnabled );
                    // update the alarm detail
                    setAlarmDetails ( newAlarmProperties.type,
                                         almEnabled,
                                         itr->second->getAlarmDelay(),
                                         itr->second->getAlarmSeverity(),
                                         itr->second->getAlarmMessage(),
                                         almValue,
                                         itr->second->getStateDescription(),
                                         itr->second->getMMSAlarmConfiguration() );
                }
                // else do nothing

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


    boost::shared_ptr<DatapointCompleteStaticConfig> EnumDataPoint::getCompleteStaticConfig()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<DatapointCompleteStaticConfig> toReturn = DataPoint::getCompleteStaticConfig();
        getEngineeringUnits(toReturn->engUnits);
        toReturn->alarmPropertyVect = getAlarmPropertiesVect();
        return toReturn;
    }

    boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> EnumDataPoint::getCompleteDynamicUpdate()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeudpate = DataPoint::getCompleteDynamicUpdate();
        //set the current alarm properties
        completeudpate->alarmProperties = getAlarmPropertiesVect();
        return completeudpate;
    }

    TA_Base_Bus::AlarmPropertyVect EnumDataPoint::getAlarmPropertiesVect()
    {
		//
		// Alarm Properties sequence
		//
		// set the size of the sequence
		 TA_Base_Bus::AlarmPropertyVect alarmProperties;
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

    void EnumDataPoint::getEngineeringUnits ( TA_Base_Bus::DataPointEngineeringUnits & eng )
    {
        std::string emptyStr ( "" );

        // there is no engineering unit for an Enum DataPoint
        eng.EngineeringUnitsEnabled = false;
        eng.LowLimit = 0;
        eng.HighLimit = 0;
        eng.Units = emptyStr.c_str();
    }

    std::string EnumDataPoint::getDataPointValueAsString( const DpValue & dpValue )
    {
        FUNCTION_ENTRY("getDataPointValueAsString");
        FUNCTION_EXIT;
        return getStateLabel( dpValue.getEnum() );
    }

	void EnumDataPoint::initAfterProxyFactoryReady()
	{
		if ( false == m_initialised )
		{
			m_initialised = true;
			initialise();
			
			DataPoint::initAfterProxyFactoryReady();
		}
	}
}
