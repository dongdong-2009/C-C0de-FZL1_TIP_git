/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DerivedState.cpp $
  * @author  HoaVu
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  *	DerivedState represents a configured state of the Derived DataPoint.  A DerivedState
  * also has a reference to an output DataPoint entity
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503 4250)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/DerivedState.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedOutputAssociation.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredDerivedState.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	//TD15649, To use datapoint proxy of DDO/DAO/TDO/TAO to send output command instead of to use datapoint corba object
	//To receive control state notification event and forward to DDO/DAO/TDO/TAO's client.
	DerivedState::DerivedState ( TA_Base_Core::ConfiguredDerivedState * configuredDerivedState,
								 DataPoint * theDerivedDataPoint )
	:
	m_configuredDerivedState ( 0 ),
	m_derivedDataPoint ( 0 )
	{
		// save the handle to the ConfiguredDerivedState
		m_configuredDerivedState = configuredDerivedState;

		// save the handle to the DerivedDataPoint
		m_derivedDataPoint = theDerivedDataPoint;

		// get all output entities associated with this derived state from database
		m_listOfOutputEntities.clear();
		configuredDerivedState->getAssociatedOutputEntities ( m_listOfOutputEntities );

		m_listOfDataPointProxies.clear();
		std::vector < TA_Base_Core::DerivedOutputAssociation * >::iterator itr;
		for ( itr = m_listOfOutputEntities.begin();
			  itr != m_listOfOutputEntities.end();
			  itr++ )
		{
			// get the corba name of the output from EntityAccessFactory
			unsigned long entitykey = (*itr)->getOutputEntityKey() ;

            // create DataPointProxy
            try
            {
                TA_Base_Bus::DataPointProxySmartPtr *dp = new DataPointProxySmartPtr();
				TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy ( entitykey, *this, *dp );
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DerivedDataPoint [%lu] %s add DataPoint proxy observer %u for [%lu] %s",
					theDerivedDataPoint->getEntityKey(), 
					theDerivedDataPoint->getDataPointName().c_str(),
					this,
					entitykey,
					(*dp)->getEntityName().c_str() );
				m_listOfDataPointProxies.push_back(dp);
            }
            catch (...) 
            {
                //because this indicates a configuration error, so stop asap
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoint entity [%u] is not well configured, can't continue...", entitykey );
                TA_ASSERT(false, "DataPoint entity configuration error");
            }
        }
	}


	DerivedState::~DerivedState()
	{
		// clean up handle to ConfiguredDerivedState
		if ( 0 != m_configuredDerivedState )
		{
			delete m_configuredDerivedState;
			m_configuredDerivedState = 0;
		}

		// reset handle to the DerivedDataPoint, dont delete the object
		m_derivedDataPoint = 0;

		// clean up handel to DerivedOutputAssociation
		std::vector < TA_Base_Core::DerivedOutputAssociation * >::iterator itr1;
		for ( itr1 = m_listOfOutputEntities.begin();
			  itr1 != m_listOfOutputEntities.end();
			  itr1++ )
		{
			delete (*itr1);
			*itr1 = NULL;
		}
		m_listOfOutputEntities.clear();

		clearChildProxy();
	}

	void DerivedState::clearChildProxy()
	{
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator outputDataPoint;
		for (outputDataPoint = m_listOfDataPointProxies.begin(); outputDataPoint != m_listOfDataPointProxies.end(); outputDataPoint ++)
		{
			TA_Base_Bus::DataPointProxySmartPtr* dp = (*outputDataPoint);
			if ( dp != NULL)
			{
				delete dp;
				dp = NULL;
			}
		}
		
		m_listOfDataPointProxies.clear();

	}


	unsigned long DerivedState::getStateValue() const
	{
		return m_configuredDerivedState->getStateValue();
	}


	const std::string & DerivedState::getStateDescription() const
	{
		return m_configuredDerivedState->getStateDescription();
	}


	const std::string & DerivedState::getStateReturnCondition() const
	{
		return m_configuredDerivedState->getStateReturnCondition();
	}


	bool DerivedState::setOutputEntities ( const std::string& sessionId,
                                           bool auditEventLoggingEnabled,
										   const TA_Base_Bus::DataPointControl& triggeringDp,
										   const EControlOperation operation)
	{
		bool result = true;
		DpValue* dpValue = NULL;

		// for each of the output entities associated with this derived state
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator outputDataPoint;
		for (outputDataPoint = m_listOfDataPointProxies.begin(); outputDataPoint != m_listOfDataPointProxies.end(); outputDataPoint ++)
		{
			DataPointProxySmartPtr * dp = *outputDataPoint; 
			dpValue = new DpValue((*dp)->getValue());

			// find the pre-defined output value associated with the output entity
			// from the database object
			std::vector < TA_Base_Core::DerivedOutputAssociation * >::iterator itr1;
			for ( itr1 = m_listOfOutputEntities.begin();
				  itr1 != m_listOfOutputEntities.end();
				  itr1++ )
			{
				// if found a matching entity key
				if ((*dp)->getEntityKey() == (*itr1)->getOutputEntityKey())
				{
					// find the type of the output data point
					bool isValidValue = false;
					switch((*dp)->getValue().getType())
					{
					case DPT_BOOL_TYPE:
						// assign the value to be written to the output data point
						dpValue->setBoolean(( 0 == (int) (*itr1)->getOutputValue() ) ? false : true);

						// set the flag
						isValidValue = true;
						break;

					case DPT_ENUM_TYPE:
						// assign the value to be written to the output data point
						dpValue->setEnum( (unsigned int)(*itr1)->getOutputValue() );

						// set the flag
						isValidValue = true;
						break;
					   
					case DPT_INT16_TYPE:
					case DPT_INT32_TYPE:
					case DPT_INT16S_TYPE:
					case DPT_INT32S_TYPE:
					case DPT_UINT16_TYPE:
					case DPT_UINT32_TYPE:
					case DPT_IEEE32_TYPE:
						// assign the value to be written to the output data point
						dpValue->setFloat((double)(*itr1)->getOutputValue(), (*dp)->getValue().getPrecision() );
						
						// set the flag
						isValidValue = true;
						break;

					case DPT_TEXT_TYPE:
					case DPT_NULL_TYPE:
					default:
						break; 
					}

					if ( true == isValidValue )
					{
						try
						{
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                         "About to write to datapoint: dpValue[%s], sessionid[%s], triggeringDpkey[%lu], triggeringDpValue[%s], auditEventLoggingEnabled[%d]", 
                                         dpValue->getValueAsString().c_str(), sessionId.c_str(),
                                         triggeringDp.triggerringDpKey, triggeringDp.triggerringDpValue.c_str(),
                                         auditEventLoggingEnabled );

							if (m_derivedDataPoint && m_derivedDataPoint->getPScadaFlag())
							{
								(*dp)->setControlValue(dpValue->getValueAsString(), sessionId, operation, triggeringDp.triggerringDpKey, (char *)triggeringDp.triggerringDpValue.c_str(), auditEventLoggingEnabled);
							}
							else
							{
								(*dp)->setValue(dpValue->getValueAsString(), sessionId, triggeringDp.triggerringDpKey, (char *)triggeringDp.triggerringDpValue.c_str(), auditEventLoggingEnabled);
							}

							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Succeed to write %s to entity[%d] in agent[%s]", dpValue->getValueAsString().c_str(), (*dp)->getEntityKey(), (*dp)->getAgentName().c_str() );
						}
                        catch ( TA_Base_Core::ScadaProxyException& e)
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ScadaProxyException:%s", e.what() );
                            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								"Failed to write %s to entity[%d] in agent[%s]", dpValue->getValueAsString().c_str(), (*dp)->getEntityKey(), (*dp)->getAgentName().c_str());
                        }
						catch ( ... )
						{
							LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								"Failed to write %s to entity[%d] in agent[%s]", dpValue->getValueAsString().c_str(), (*dp)->getEntityKey(), (*dp)->getAgentName().c_str());
							
							// set the flag
							result = false;
						}
					}

					// exit inner loop
					break;
				}
			}

			delete dpValue;
			dpValue = NULL;
			// continue on to the next output data point if neccessary
		}
		return result;
	}

	void DerivedState::setOutputInhibitState ( const std::string& sessionId, TA_Base_Bus::EOutputInhibitState inhibitstate  )
	{
		// for each of the output entities associated with this derived state
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator outputDataPoint;
		for (outputDataPoint = m_listOfDataPointProxies.begin(); outputDataPoint != m_listOfDataPointProxies.end(); outputDataPoint ++)
		{
			DataPointProxySmartPtr * dp = *outputDataPoint;
			try
			{
				(*dp)->setOutputInhibitStatus(inhibitstate, sessionId);
			}
			catch ( TA_Base_Core::ScadaProxyException& e)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"in DerivedState::setOutputInhibitState, ScadaProxyException:%s", e.what() );
            }
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Failed to setOutputInhibitState [%d] to entity[%d] in agent[%s]", inhibitstate, (*dp)->getEntityKey(), (*dp)->getAgentName().c_str());
			}
		}
	}

    void DerivedState::processEntityUpdateEvent ( unsigned long entityKey,
		ScadaEntityUpdateType updateType)
    {
        // make sure the update is for this entity and operated in control mode

		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator outputDataPoint;
		for (outputDataPoint = m_listOfDataPointProxies.begin(); outputDataPoint != m_listOfDataPointProxies.end(); outputDataPoint ++)
		{
			DataPointProxySmartPtr * dp = *outputDataPoint;
			//only process the event from my children.
			if ( (*dp)->getEntityKey() == entityKey )
			{
				switch ( updateType )
				{
				// only interested in following update types
				case ControlStateUpdate:
					//only process the event which control is triggerred by me or my parents.
                    try
                    {
					    if ((*dp)->getControlTriggeringEntity() == m_derivedDataPoint->getControlState().triggerringDpKey)
					    {
						    //When LCC check failed, need to set the fail result string
						    if ((*dp)->getControlState() == LCC_failed)
						    {
							    std::string resultString = (*dp)->getLaunchingConditionResultString();
							    m_derivedDataPoint->setLaunchingConditionResultString(resultString);
						    }				
						    m_derivedDataPoint->setControlState((*dp)->getControlState());
					    }
                    }
                    catch ( TA_Base_Core::ScadaProxyException & e )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ScadaProxyException:%s", e.what() );
                    }
                    
					break;
                
					// do nothing for all other types
				default:
					break;
				}
			}
		}
    }

	
	void DerivedState::setAlarmEnabled( bool newEnabled )
	{
		// update database
		if ( 0 != m_configuredDerivedState )
		{
			m_configuredDerivedState->setAlarmEnabled ( newEnabled );
		}	
	}


	void DerivedState::setAlarmDelay ( int newDelay )
	{
		// update database
		if ( 0 != m_configuredDerivedState )
		{
			m_configuredDerivedState->setAlarmDelay ( newDelay );
		}
	}


	void DerivedState::setAlarmSeverity ( int newSeverity )
	{
		// update database
		if ( 0 != m_configuredDerivedState )
		{
			m_configuredDerivedState->setAlarmSeverity ( newSeverity );
		}
	}


	void DerivedState::setAlarmMessage ( const std::string & newMessage )
	{
		// update database
		if ( 0 != m_configuredDerivedState )
		{
			m_configuredDerivedState->setAlarmMessage ( newMessage );
		}
	}


	bool DerivedState::getAlarmEnabled() const
	{
		if ( 0 != m_configuredDerivedState )
		{
			return m_configuredDerivedState->getAlarmEnabled();
		}
		else
		{
			return false;
		}
	}


	int DerivedState::getAlarmDelay() const
	{
		if ( 0 != m_configuredDerivedState )
		{
			return m_configuredDerivedState->getAlarmDelay();
		}
		else
		{
			return 0;
		}
	}


	int DerivedState::getAlarmSeverity() const
	{
		if ( 0 != m_configuredDerivedState )
		{
			return m_configuredDerivedState->getAlarmSeverity();
		}
		else
		{
			return 0;
		}
	}


	const std::string & DerivedState::getAlarmMessage() const
	{
		if ( 0 != m_configuredDerivedState )
		{
			return m_configuredDerivedState->getAlarmMessage();
		}
		else
		{
			return m_derivedDataPoint->getDataPointName();
		}
	}


	const std::string & DerivedState::getMMSAlarmConfiguration() const
	{
		return m_configuredDerivedState->getAlarmMMS();
	}


	void DerivedState::setMMSAlarmConfiguration ( const std::string & newConfiguration )
	{
		m_configuredDerivedState->setAlarmMMS ( newConfiguration );
	}
}
