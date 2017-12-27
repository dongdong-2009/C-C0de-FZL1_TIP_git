/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/InputAssociation.cpp $
  * @author  HoaVu
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  *	InputAssociation represent the DataPoint entity that is associated
  * with a Derive DataPoint.
  *
  */

#include <sstream>
#include "time.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

#include "bus/scada/datapoint_library/src/DataPointState.h"
#include "bus/scada/datapoint_library/src/InputAssociation.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/naming/src/INamedObject.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

namespace TA_Base_Bus
{
	InputAssociation::InputAssociation ( TA_Base_Core::ConfiguredInputAssociation * configuredInputAssociation )
	:
	m_currentState ( 0 ),
	m_configuredInputAssociation ( 0 ),
    m_scadaProxyFactory ( 0 ),
	m_derivedDataPointCallBack ( 0 ),
	m_threadPool ( LFThreadPoolSingleton::getInstance() ),
	m_controlMode ( false ),
    m_workItem ()
	{
		// initialise the state.
		struct timeb currentSystemTime;
		ftime ( &currentSystemTime );
		DpvEnumLabelsMap dummyEnumLabelsMap;
		DpvBooleanLabels dummyBooleanLabels;
		DpValue dpv(TA_Base_Bus::DPT_NULL_TYPE, dummyEnumLabelsMap, dummyBooleanLabels);
		m_currentState = new DataPointState ( dpv, currentSystemTime, QUALITY_BAD_NOT_CONNECTED );

		// save the handle to the ConfiguredInputAssociation
		m_configuredInputAssociation = configuredInputAssociation;

		// get the entitykey of the input
		unsigned long entitykey = configuredInputAssociation->getInputEntityKey();

		try
		{
			// create DataPointProxy
			TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy ( entitykey, *this, m_inputDataPointRef );
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Add DataPoint proxy observer %u for [%lu] %s",
				this,
				entitykey,
				m_inputDataPointRef->getEntityName().c_str() );			
		}
		catch ( ... )
		{
            //because this indicates a configuration error, so stop asap
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoint entity [%u] is not well configured, can't continue...", entitykey );
            TA_ASSERT(false, "DataPoint entity configuration error");
		}
	}


	InputAssociation::~InputAssociation()
	{
		// prevent multiple threads attempting to access private member
		// while we clean up
		TA_Base_Core::ThreadGuard guard( m_lock );

		// remove handle to the ThreadPoolSingletonManager
		m_threadPool->removeWorkItem ( this);

        m_workItem.reset();

		// reset pointer to callback without deleting itself
		m_derivedDataPointCallBack = 0;

		// clean up state of the input data point
		if ( 0 != m_currentState )
		{
			delete m_currentState;
			m_currentState = 0;
		}

		// clean up handle to ConfiguredInputAssociation
		delete m_configuredInputAssociation;
		m_configuredInputAssociation = 0;
	}


	unsigned long InputAssociation::getInputDataPointKey() const
	{
		return m_configuredInputAssociation->getInputEntityKey();
	}


	const std::string & InputAssociation::getInputDataPointName() const
	{
		return m_configuredInputAssociation->getInputEntityName();
	}


	const DataPointState & InputAssociation::getCurrentState() const
	{
		return *m_currentState;
	}


	std::string InputAssociation::getValueAsString()
	{
        std::string valueString("");

        try
        {
            valueString = m_inputDataPointRef->getValueAsString();
        }
        catch ( TA_Base_Core::ScadaProxyException & e )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ScadaProxyException:%s", e.what() );
        }
        
		return valueString;
	}


	const std::string & InputAssociation::getDescription()
	{
		return m_inputDataPointRef->getDescription();
	}


	EDataPointDataType InputAssociation::getInputDataPointDataType() const
	{
		return m_currentState->getValue().getType();
	}


	int InputAssociation::getInputCalculationOrder() const
	{
		return m_configuredInputAssociation->getInputCalculationOrder();
	}


	unsigned long InputAssociation::getInputDataPointSubsystemKey() const
	{
		return m_configuredInputAssociation->getInputEntitySubsystemKey();
	}


	unsigned long InputAssociation::getInputDataPointLocationKey() const
	{
		return m_configuredInputAssociation->getInputEntityLocationKey();
	}


	void InputAssociation::resolveCurrentDataPointState()
	{
		//bool valueExtracted = true;
		bool schedulingNeeded = false;
		bool exceptionReceived = true;
		
        if ( m_inputDataPointRef->isConfigValid() )
        {
            try
            {
                // get the current value of the input data point
			    DpValue value = m_inputDataPointRef->getValue();

			    // update current state with the value from the proxy
			    schedulingNeeded = updateCurrentState ( value,
								     m_inputDataPointRef->getTimestampStructure(),
								     (EQualityStatus) m_inputDataPointRef->getStatus() );

				
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "resolveCurrentDataPointState  NAME:%s, VALUE:%s, Quality %d", getInputDataPointName().c_str(), value.getValueAsString().c_str(), m_inputDataPointRef->getStatus());
				
			    // set the flag for a scheduling
			    //schedulingNeeded = true;
                exceptionReceived = false;
		    }
		    catch ( TA_Base_Core::ScadaProxyException & )
		    {
		    }
		    catch ( ... )
		    {
		    }
        }

		// if exception occurred
		if ( true == exceptionReceived )
		{
			// if current quality status is not QUALITY_BAD_NOT_CONNECTED
			if ( QUALITY_BAD_NOT_CONNECTED != m_currentState->getStatus() )
			{
				// set quality status to QUALITY_BAD_LAST_KNOWN_VALUE
				// since we are unable to get value/timestamp/quality from the proxy
				struct timeb currentSystemTime;
				ftime ( &currentSystemTime );
				m_currentState->setTimestamp ( currentSystemTime );
				m_currentState->setStatus ( QUALITY_BAD_LAST_KNOWN_VALUE );

				// set the flag for a scheduling
				schedulingNeeded = true;
			}
			// else do nothing
		}

		// if scheduling is required
		if ( true == schedulingNeeded )
		{
			// schedule a work item in ThreadPool for this latest update
			m_threadPool->queueWorkItem ( this );	
		}
		// else do nothing
	}


	bool InputAssociation::updateCurrentState ( const DpValue & dpv,
												const timeb & tmstamp,
												EQualityStatus status )
	{
		// just update the current state with the new info
		if(!(m_currentState->getValue() == dpv && m_currentState->getStatus() == status) )
		{
			m_currentState->setValue ( dpv );
			m_currentState->setTimestamp ( tmstamp );
			m_currentState->setStatus ( status );
			return true;
		}
		return false;
	}


	void InputAssociation::registerForStateChangeNotification ( TA_Base_Bus::IEntityUpdateEventProcessor * callback )
	{
		TA_Base_Core::ThreadGuard guard( m_lock );
		m_derivedDataPointCallBack = callback;
	}


    void InputAssociation::processEntityUpdateEvent ( unsigned long entityKey,
                                                      ScadaEntityUpdateType updateType)
    {
        // make sure the update is for this entity and operated in control mode
        if ( ( getInputDataPointKey() == entityKey ) &&
             ( true == m_controlMode ) )
        {
            switch ( updateType )
            {
                // only interested in following update types
            case ValueStateUpdate:
           // case AlarmSummaryUpdate://limin++, TD20740
           // case AckSummaryUpdate:  //limin++, TD20740
            case QualitySummaryUpdate:
            //case ConfigAvailable:
                resolveCurrentDataPointState();
                break;
                
            // do nothing for all other types
            default:
                break;
            }
        }
        // else do nothing
    }


	void InputAssociation::executeWorkItem()
	{
		//
		// callback from ThreadPool for scheduled work
		//

		// prevent multiple threads attempting to access private member
		TA_Base_Core::ThreadGuard guard( m_lock );

		// notify the callback object for the change in the InputAssociation
		if ( 0 != m_derivedDataPointCallBack )
		{
			//TD13275, change LOG to LOG_GENERIC
			//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start of callback for latest changes of DataPoint, NAME:%s", getInputDataPointName().c_str());
			
			try
			{
				m_derivedDataPointCallBack->processEntityUpdateEvent ( m_configuredInputAssociation->getInputEntityKey(), ValueStateUpdate );
			}
			catch ( ... )
			{
			}

			//TD13275, change LOG to LOG_GENERIC
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End of callback for latest changes of DataPoint, NAME:%s", getInputDataPointName().c_str());
		}
		// else do nothing		
	}


	void InputAssociation::setOperationMode ( bool control )
	{
		// proceed if different
		if ( control != m_controlMode )
		{
			// save operation mode
			m_controlMode = control;

			// if going to control mode
			if ( true == m_controlMode )
			{
				// get current state of the input data point
				resolveCurrentDataPointState();
			}

		}
	}
}
