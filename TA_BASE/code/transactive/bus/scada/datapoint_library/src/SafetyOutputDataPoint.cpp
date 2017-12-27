/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/SafetyOutputDataPoint.cpp $
  * @author  HoaVu
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  *	A Safety Output Datapoint maps a single value to several target
  * datapoints.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/SafetyOutputDataPoint.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include <math.h>
#include "time.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	const unsigned long TIMEOUT_PERIOD = 200; // in mseconds


	SafetyOutputDataPoint::SafetyOutputDataPoint( TA_Base_Core::DataPointEntityDataPtr		            dataPointEntityAccess,
									  			  TA_Base_Bus::EDataPointType               			dpType,
												  TA_Base_Bus::EDataPointDataType                       dpDataType,
                                                  ScadaPersistencePtr                                   persistence,
                                                  TA_Base_Core::ScadaRootEntityData&                    scadaRootEntityData,
                                                  MmsScadaUtility&                                      mmsScadaUtility,
                                                  CommandProcessor&                                     commandProcessor )
	:
	DataPoint( dataPointEntityAccess, dpType, dpDataType, persistence, scadaRootEntityData, mmsScadaUtility, commandProcessor ),
    m_initialised ( false ),
	m_derivedDataPointEntityData ( 0 ),
	m_selectedDerivedState ( 0 )
	{
		// 
		// Check Datapoint Data Type
		//
		if (dpDataType != TA_Base_Bus::DPT_ENUM_TYPE)
		{
            std::ostringstream desc;
            desc << "Invalid Configuration; SafetyOutputDataPoint [" << m_name << "] data type (" << dpDataType << ") invalid.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
		}
	}


	SafetyOutputDataPoint::~SafetyOutputDataPoint()
	{
		cleanup();
	}


	void SafetyOutputDataPoint::initialise()
	{
		// make sure only one thread can access this object
		ThreadGuard guard ( m_ownLock );

		// create handle to DerivedDataPointEntityData
		m_derivedDataPointEntityData = new DerivedDataPointEntityData ( m_entityKey,
																	m_name,
																		m_dataPointEntityAccess->getAllowWrite());

		// create references to states configured for this SafetyOutputDataPoint
		createDerivedState();

	}


	void SafetyOutputDataPoint::cleanup()
	{
		// make sure only one thread can access this object
		ThreadGuard guard ( m_ownLock );

		// clean up the list of derived states configured for this SafetyOutputDataPoint
		DerivedStatesMap::iterator cds_itr;
		for ( cds_itr = m_listOfDerivedStates.begin();
			  cds_itr != m_listOfDerivedStates.end();
			  cds_itr++ )
		{
			delete (*cds_itr).second;
			(*cds_itr).second = NULL;
		}
		m_selectedDerivedState = 0;
		m_listOfDerivedStates.clear();

		// clean up handle to DerivedDataPointEntityData
		if ( 0 != m_derivedDataPointEntityData )
		{
			delete m_derivedDataPointEntityData;
			m_derivedDataPointEntityData = 0;
		}
	}

    boost::shared_ptr<DatapointCompleteStaticConfig> SafetyOutputDataPoint::getCompleteStaticConfig()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<DatapointCompleteStaticConfig> toReturn = DataPoint::getCompleteStaticConfig();
        getEngineeringUnits(toReturn->engUnits);
        toReturn->alarmPropertyVect = getAlarmPropertiesVect();
        return toReturn;
    }

    boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> SafetyOutputDataPoint::getCompleteDynamicUpdate()
    {
        //get those common things from the datapoint class
        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeudpate = DataPoint::getCompleteDynamicUpdate();
        //set the current alarm properties
        completeudpate->alarmProperties = getAlarmPropertiesVect();
        return completeudpate;
    }

    void SafetyOutputDataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
    {
        FUNCTION_ENTRY("SafetyOutputDataPoint::updateSynState");
        DataPoint::updateSynState(toupdate);
        FUNCTION_EXIT;
    }

    TA_Base_Bus::AlarmPropertyVect SafetyOutputDataPoint::getAlarmPropertiesVect()
    {
		//
		// Alarm Properties sequence
		//
		// set the size of the sequence
		 TA_Base_Bus::AlarmPropertyVect alarmProperties;
		// for each of the alarm defined for this data point
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
    
    void SafetyOutputDataPoint::updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
        //the alarm properties is not required to be synchronized
		throw ( TA_Base_Core::NotAvailableException() );        
    }

    void SafetyOutputDataPoint::setAlarmProperties ( const std::string& sessionID, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
    {
        //the alarm properties can not be set for SafetyOutputDataPoint
		throw ( TA_Base_Core::NotAvailableException() );        
    }

	void SafetyOutputDataPoint::createDerivedState()
	{
		if ( 0 != m_derivedDataPointEntityData )
		{
			// get all derived states configured for this SafetyOutputDataPoint from database
			std::vector < ConfiguredDerivedState * > m_listOfConfiguredDerivedStates;
			m_derivedDataPointEntityData->getConfiguredDerivedStates ( m_listOfConfiguredDerivedStates );

			// for each of the configured derived states from database
			m_listOfDerivedStates.clear();
			m_enumLabelsMap.clear();
			
			std::vector < ConfiguredDerivedState * >::iterator cdsItr;
			for ( cdsItr = m_listOfConfiguredDerivedStates.begin();
				  cdsItr != m_listOfConfiguredDerivedStates.end();
				  ++cdsItr )
			{
				// create a DerivedState object and put in the list for
				// further reference
				DerivedState* derivedState = new DerivedState ( (*cdsItr), this );

				m_listOfDerivedStates[derivedState->getStateValue()] = derivedState;
				m_enumLabelsMap[derivedState->getStateValue()] = derivedState->getStateDescription();
			}
			m_listOfConfiguredDerivedStates.clear();
		}
	}


	void SafetyOutputDataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
	{
		bool reInitialised = false;

		// for each of the changed parameters
		for ( unsigned int i = 0; i < configChanges->size(); i++ )
		{
			std::string paramName = (*configChanges)[i];

			// if any parameters specific to the DerivedDataPoint has been changed
			if ( ( std::string::npos != paramName.find ( "Output Association" ) ) ||
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


	void SafetyOutputDataPoint::getEngineeringUnits ( TA_Base_Bus::DataPointEngineeringUnits & eng )
	{
		// there is no engineering unit for a SafetyOutputDataPoint
		eng.EngineeringUnitsEnabled = false;
		eng.LowLimit = 0;
		eng.HighLimit = 0;
		eng.Units = "";
	}

    std::string SafetyOutputDataPoint::getDataPointValueAsString( const DpValue & dpValue )
    {
        FUNCTION_ENTRY("getDataPointValueAsString");

        std::ostringstream valueStr;
        valueStr << dpValue.getEnum();
        
        FUNCTION_EXIT;
        return valueStr.str();
    }


    void SafetyOutputDataPoint::writeDataPoint( const std::string& sessionID,
					 							const TA_Base_Bus::DataPointValue & value,
					 							bool auditEventLoggingEnabled,
												unsigned long triggeringDpKey,
												const std::string triggeringDpValue)
	{
		// make sure only one thread can access this object
		ThreadGuard guard ( m_ownLock );

		std::string logMsg ( "" );
		std::string valueString ( "" );
		unsigned long combinedValue = 0;
		bool foundDerivedState = false;
		DpValue dpValue (value, getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());

		TA_Base_Bus::DataPointControl trigDp;
		if (triggeringDpKey != 0)
		{
			trigDp.triggerringDpKey = triggeringDpKey;
			trigDp.triggerringDpValue = triggeringDpValue;
		}
		else
		{
			trigDp.triggerringDpKey = m_entityKey;
			trigDp.triggerringDpValue = dpValue.getValueAsString();
		}

		//unsigned long trigEntity = triggeringEntity != 0 ? triggeringEntity : m_entityKey;
		m_auditEventLoggingEnabled = auditEventLoggingEnabled;
        m_controlSessionId = sessionID;
        
		// looking for a enum only, everything else is error
		switch( value.dpType )
		{
			case TA_Base_Bus::DPT_ENUM_TYPE :
			{
				combinedValue = value.enumValue;
				std::stringstream ss;
				ss << combinedValue;
				valueString = ss.str();

				// for each of the derived states configured for this SafetyOutputDataPoint
				DerivedStatesMap::iterator itr 
					= m_listOfDerivedStates.find(combinedValue);
				
				// if the value to be set matches the derived state configured for this
				// SafetyOutputDataPoint
				if (itr != m_listOfDerivedStates.end())
				{
					// get the description
					valueString = (*itr).second->getStateDescription();

					// save the selected DerivedState;
					m_selectedDerivedState = (*itr).second;

					// set the flag
					foundDerivedState = true;
				}

				break;
			}

			default:
			{
				break;
			}
		}

		// found a state matching with the specified value
		if ( true == foundDerivedState )
		{
			// construct the DescriptionParameters for auditing messages
			TA_Base_Core::DescriptionParameters descriptionParameters;		
			descriptionParameters.clear();
			NameValuePair pair1 ( "ValueString", valueString );
			descriptionParameters.push_back ( &pair1 );

			// if the requested action is authorised for the sessionID
			if ( true == isActionPermitted ( sessionID, TA_Base_Bus::aca_DATA_POINT_WRITE ) )
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Received DataPoint write request" );

				// only proceed if control is not inhibited
				if ( false == m_ControlInhibitedState )
				{
					if ( true == auditEventLoggingEnabled )
					{
						// log auditing event
						// submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::SetValueSucceeded );
					}

					// make the field state of the output datapoint the same as the value
					// being written.
					struct timeb currentSystemTime;
					ftime ( &currentSystemTime );

					DpValue dpValue (value, getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels(), getPrecision());

					DataPointState newState( dpValue, currentSystemTime, QUALITY_GOOD_NO_SPECIFIC_REASON );
					updateFieldState ( newState );

					// update control status to indicate this SafetyOutputDataPoint is
					// being written
                    trigDp.controlState = written;
					setControlState ( trigDp );

					// set the output associated with the derived state if applicable
					if ( true == m_selectedDerivedState->setOutputEntities ( sessionID.c_str(), auditEventLoggingEnabled, trigDp, value.controlOperation) )
					{
						//TD15649, need not to set the following control state if no RCC check, 
						//because the derived state will set the control state when receive control state events from the underlying output datapoints.
						// if there is an ABE expression defined for this SafetyOutputDataPoint
						if ( false == m_abeExpression.empty() )
						{
							// update control status to indicate this SafetyOutputDataPoint
							// is waiting for the result of its outputs
                            trigDp.controlState = pending;
							setControlState ( trigDp );

							// then start the ABE timeout mechanism
							startABETimeout(sessionID);
						}
					}
					else
					{
						// update control status to indicate this SafetyOutputDataPoint
						// failed to write the ouput datapoints associated with it, possibly
						// because of comms error
                        trigDp.controlState = comms_failed;
						setControlState ( trigDp );
					}
				}
				else
				{
					if ( true == auditEventLoggingEnabled )
					{
						// log auditing event
						//submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::SetValueFailedControlInhibited );
					}

					throw TA_Base_Core::BadParameterException("Not allowed to write");
				}
			}
			else
			{
				if ( true == auditEventLoggingEnabled )
				{
					// log auditing event
					//submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::SetValueFailedPermissionDenied );
				}

				throw TA_Base_Core::AccessDeniedException();
			}
		}
		else
		{
			throw TA_Base_Core::BadParameterException();
		}
	}

	void SafetyOutputDataPoint::setOutputInhibitState ( const std::string& sessionId, TA_Base_Bus::EOutputInhibitState inhibitstate  )
	{
		FUNCTION_ENTRY("setOutputInhibitState");

		DataPoint::setOutputInhibitState(sessionId, inhibitstate);

		DerivedStatesMap::const_iterator itr;
		for (itr = m_listOfDerivedStates.begin(); m_listOfDerivedStates.end() != itr; ++itr)
		{
			itr->second->setOutputInhibitState(sessionId, inhibitstate);
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"finished to setOutputInhibitState for [%s] to [%d]", m_name.c_str(), inhibitstate);
		FUNCTION_EXIT;
	}

	void SafetyOutputDataPoint::setToControlMode()
	{
		// initialise all dependencies associated with this derived data point
		if ( false == m_initialised )
		{
			cleanup();
            m_initialised = true;
            initialise();
		}
        
		// call the base class first
		DataPoint::setToControlMode();
	}


	void SafetyOutputDataPoint::setToMonitorMode()
	{
		// initialise all dependencies associated with this derived data point
		// if not done so
		if ( false == m_initialised )
		{
			cleanup();
            m_initialised = true;
            initialise();
		}

		// call the base class first
		DataPoint::setToMonitorMode();
	}

	void SafetyOutputDataPoint::initAfterProxyFactoryReady()
	{
		if ( false == m_initialised )
		{
			cleanup();
			m_initialised = true;
			initialise();

			DataPoint::initAfterProxyFactoryReady();
		}
	}
}
