/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPoint.cpp $
  * @author:	HoaVu
  * @version $Revision: #11 $
  *
  * Last modification : $DateTime: 2017/09/08 09:54:53 $
  * Last modified by : $Author: wang.wang $
  *
  *
  * DataPoint is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * DataPoint also realises the DataPointCorbaDef interface,
  * exposing the DataPoint's configuration to remote
  * clients.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointStateChangeEvent.h"
#include "bus/scada/datapoint_library/src/EventDetailsFactory.h"
#include "bus/scada/datapoint_library/src/CalculatedExpression.h"
#include "bus/scada/datapoint_library/src/IDataPointObserverNotification.h"
#include "bus/scada/datapoint_library/src/DataPointAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPointAlarmDetailsFactory.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/LaunchingConditionCheck.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"

#include "core/alarm/src/AlarmHelperBase.h" 
#include "core/exceptions/src/NoAlarmAgentException.h" 
#include "core/alarm/src/AlarmConstants.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/data_access_interface/entity_access/src/ScadaRootEntityData.h"
#include "core/exceptions/src/AlarmAgentException.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"
#include "core/event_processing/src/EventProcessingLibrary.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/AlarmAudit_MessageTypes.h"
#include "core/message/types/DataPointAudit_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

#include "time.h"
#include <algorithm>
#include <float.h>

using namespace TA_Base_Core;
using namespace TA_Base_Bus;


bool isSystemSessionID( const std::string& sessionID )//limin++, SystemSession should not access db
{
    bool ret = ( TA_Base_Core::SessionAccessFactory::getSuperSessionId() == sessionID ||
                 TA_Base_Bus::DPT_INTERNAL_SESSION_ID == sessionID ||
                 TA_Base_Bus::NotLoaded == sessionID );
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[sessionID=%s][return=%d]", sessionID.c_str(), ret );
    return ret;
}


namespace TA_Base_Bus
{
    static const std::string IN_SERVICE_STRING = "IN";
    static const std::string OUT_SERVICE_STRING = "OUT";

	DataPoint::DataPoint( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
						  TA_Base_Bus::EDataPointType dpType,
						  TA_Base_Bus::EDataPointDataType dpDataType,
                          ScadaPersistencePtr persistence,
                          TA_Base_Core::ScadaRootEntityData& scadaRootEntityData,            // Raymond Pau++ TD13367
                          MmsScadaUtility & mmsScadaUtility,
                          CommandProcessor& commandProcessor)
	:
m_entityKey( dataPointEntityAccess->getKey() ),
	m_name( dataPointEntityAccess->getName() ),
	m_entityTypeKey( dataPointEntityAccess->getTypeKey() ),
	m_subsystemKey( dataPointEntityAccess->getSubsystem() ),
	m_locationKey( dataPointEntityAccess->getLocation() ),
	m_locationName( dataPointEntityAccess->getLocationName() ),
	m_parentKey( dataPointEntityAccess->getParent() ),
	m_description( dataPointEntityAccess->getDescription() ),
	m_viewAssociation( dataPointEntityAccess->getViewAssociation() ),
	m_typeShortName( dataPointEntityAccess->getDataPointTypeShortName() ),
	m_OMFlag( NotLoaded ),
    m_displayOrder( 0 ),
	m_precision( 0 ),
	m_updatePriorityLevel( 0 ),
	m_writable( false ),
	m_scanTimeFactor( 0 ),
	m_wordSwap( false ),
    m_byteSwap(false),
    m_type( dpType ),
    m_dataType( dpDataType ),
	m_address( dataPointEntityAccess->getAddress() ),
	m_bitAddress( 0 ),
	m_registerAddress( 0 ),
    m_abeExpression( NotLoaded ),
	m_abeTimeoutPeriodSeconds( 0 ),
	m_lastOperatorSessionID( NotLoaded ),
    m_lastOperatorTimestamp( TA_Base_Core::AlarmConstants::defaultTime ),
    m_alarmState( TA_Base_Bus::DptNoAlarm ),
	m_previousAlarmState(TA_Base_Bus::DptNoAlarm),
    m_ackState( TA_Base_Bus::ScadaAlarmUnknown),
    m_alarmStateString( "noAlarm" ),
	m_unknownHiHiLimitAlarmID( "" ),
	m_unknownHiLimitAlarmID( "" ),
	m_unknownLoLoLimitAlarmID( "" ),
	m_unknownLoLimitAlarmID( "" ),
	m_commandLaunchingCondition( NotLoaded ),
    m_calculatedExpression( NotLoaded ),
	m_forcedState( false ),
	m_ControlInhibitedState( false ),
	m_AlarmInhibitedState( false ),
	m_MMSAlarmInhibitedState( false ),
	m_trendingEnabled( false ),
    m_virtual( false ),
    m_logChanges( false ),	
	m_clockHasStarted( false ),
	m_auditEventLoggingEnabled( false ),
	m_answerBackErrorCheckingResult( false ),	
    m_ackCloseAlarmOnInhibit( scadaRootEntityData.getAckCloseAlarmOnInhibit() ),   // Raymond Pau++ TD13367
    m_ackAnalogAlarmOnTransition( scadaRootEntityData.getAckAnalogAlarmOnTransition() ),   // TD13619
    m_forcedValue( dpDataType, m_enumLabelsMap, m_booleanLabels ),
    m_forcedEngValue( dpDataType, m_enumLabelsMap, m_booleanLabels ),
    m_forcedValueQualityStatus( QUALITY_BAD_NOT_CONNECTED ),
    m_forcedValueTimestamp( TA_Base_Core::AlarmConstants::defaultTime ),
    m_fieldValue( dpDataType, m_enumLabelsMap, m_booleanLabels ),
    m_fieldEngValue( dpDataType, m_enumLabelsMap, m_booleanLabels ),
    m_fieldValueQualityStatus( QUALITY_BAD_NOT_CONNECTED ),
	m_fieldValueTimestamp( TA_Base_Core::AlarmConstants::defaultTime ),
    m_lastKnownFieldState( m_fieldValue, 0, m_fieldValueQualityStatus ),
    m_serviceState( OUT_OF_SERVICE ),
    m_operationMode( TA_Base_Core::NotApplicable ),
    m_lastSavedOperationMode( TA_Base_Core::NotApplicable ),
    m_threadPool( LFThreadPoolSingleton::getInstance()),
    m_launchingConditionCheck( NULL ),
    m_persistQ( NULL ),
    m_dataPointEntityAccess( dataPointEntityAccess ),
    m_calculatedExpressionHandle( NULL ),
    m_auditMessageSender( NULL ),
    m_scadaUtilities( NULL ),
	m_eventDetailsFactory( NULL ),
    m_persistence( persistence ),
    m_workItem(),
	m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
    m_alarmModificationHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper()),
	m_mmsScadaUtility( mmsScadaUtility ),
    m_commandId(0),
    m_controlCommandState(CompletedWithSuccess),
    m_controlSessionId(NotLoaded),
	m_assetName(NotLoaded),
    m_commandProcessor(commandProcessor),
	m_pscadaFlag(dataPointEntityAccess->getPScadaFlag()) //Using for IEC104 Protocol
	{
        FUNCTION_ENTRY("DataPoint");

		// Initialise the vector's
		m_associations.clear();
		m_alarms.clear();
		m_listOfUpdateDetailsObservers.clear();

        //read the scan time factor from the database
        try
		{
			m_scanTimeFactor = m_dataPointEntityAccess->getScanTimeFactor();
		}
		catch ( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Failed to read scan time factor from database. set to default:0" );
		}
		
		//read the display order from the database
		try
		{
			m_displayOrder = m_dataPointEntityAccess->getDisplayOrder();
		}
		catch ( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Failed to read display order from database. set to default:-1" );
		}

        //update the engineering note
		m_engUnitsEnabled = m_dataPointEntityAccess->getEngineeringUnitsEnabled();
		if ( false == m_engUnitsEnabled )
		{
			setEngNote( "" );
		}
        else
        {
			setEngNote( m_dataPointEntityAccess->getEngineeringNote() );
		}

        //update the help URL
		if ( false == m_dataPointEntityAccess->getEnableHelpURL() )
		{
			setHelpURL( "" );
		}
        else
        {
		    setHelpURL( m_dataPointEntityAccess->getHelpURL() );
        }

		//
		// Update Precision
		//
		/*try
		{
			if ( ( dataPointEntityAccess->getPrecision() == 0 ) && ( m_dataType == TA_Base_Bus::DPT_IEEE32_TYPE ) )
			{
				// default to 2 decimal point if precision has not been setup for
				// floating point datapoint
				setPrecision( 2 );
			}
			else
			{
				setPrecision ( dataPointEntityAccess->getPrecision() );
			}
		}
		catch ( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Failed to read precision from database. set to default" );
		}
		*/
		//set the update priority level
		try
		{
			m_updatePriorityLevel = m_dataPointEntityAccess->getUpdatePriorityLevel();
		}
		catch ( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Failed to read update priority from database. set to default:0" );
		}
		// get handle to the ThreadPoolSingletonManager


		// Is this a virtual DataPoint
		std::transform ( m_address.begin(), m_address.end(),
						 m_address.begin(), toupper); // make uppercase
		m_virtual = ( m_address == DPT_CFG_VIRTUAL ) ? true : false;

        std::string tempString( IN_SERVICE_STRING );
        m_persistence->readPersistedValue( DP_SERVICE_STATE, m_dataPointEntityAccess->getKey(), tempString );
		m_serviceState = ( tempString == IN_SERVICE_STRING ? IN_SERVICE : OUT_OF_SERVICE );

		m_writable = m_dataPointEntityAccess->getAllowWrite();

        m_persistence->readPersistedValue( DP_MANUALLY_DRESSED, m_entityKey, m_forcedState );
        m_persistence->readPersistedValue( DP_CONTROL_INHIBIT, m_entityKey, m_ControlInhibitedState );
        m_persistence->readPersistedValue( DP_ALARM_INHIBIT, m_entityKey, m_AlarmInhibitedState );
		m_persistence->readPersistedValue( DP_MMS_INHIBIT, m_entityKey, m_MMSAlarmInhibitedState );

        TA_Base_Bus::InhibitState currentstate = getInhibitState();
        m_previousInputInhibitState = currentstate.inputInhibit;

		m_trendingEnabled = m_dataPointEntityAccess->getEnableTrending();
		m_logChanges = m_dataPointEntityAccess->getLogChanges();
		m_wordSwap = m_dataPointEntityAccess->getWordSwapIndication();
        m_byteSwap = m_dataPointEntityAccess->getByteSwapIndication();
		m_bitWidth = m_dataPointEntityAccess->getBitWidth();
		m_bigEndian = m_dataPointEntityAccess->getIsBigEndian();


        //initialize the control state as not loaded
	    m_controlState.lccResult = NotLoaded;
        getLaunchingConditionResultString();        
        //initialize the last operator related information
        m_persistence->readPersistedValue( DP_LAST_OPERATOR_SESSION_ID, m_entityKey, m_lastOperatorSessionID ); 
        m_persistence->readPersistedValue( DP_LAST_OPERATOR_TIMESTAMP, m_entityKey, m_lastOperatorTimestamp.time ); 

		//
		// Update Manually dressed state/values as persisted in the configuration
		// database
		//
		setForcedState ( m_forcedState );

		// convert the control status from database string to enum
		configureControlStatus();

        // Raymond Pau++ TD 13376
        m_persistQ = PersistQueue::getInstance();
        // ++Raymond Pau TD 13376

		//TD17119 if DataPoint is the TEXT DataPoint, set m_ackState to ScadaAlarmNotPresent
		if(dpType == TA_Base_Bus::DPT_TEXT)
		{
			m_ackState = TA_Base_Bus::ScadaAlarmNotPresent;
		}
        
        // initialise data
        initialiseData();
        
		//TD17119
        FUNCTION_EXIT;
	}


	DataPoint::~DataPoint()
	{
        FUNCTION_ENTRY("~DataPoint");
		// remove handle to the ThreadPoolSingletonManager
		
		m_threadPool->removeWorkItem (this);
	
        m_workItem.reset();

		// clean up associations
		clearAssociations();

        ThreadGuard guard( m_alarmslock );
		// clean up alarm details
		DpAlarmList::iterator alarmIter = m_alarms.begin();
		for( ; alarmIter != m_alarms.end(); ++alarmIter )
		{
			delete ( * alarmIter );
			*alarmIter = NULL;
		}
		m_alarms.clear();

		// clean up the list of dp observers
		m_listOfUpdateDetailsObservers.clear();

        // Raymond Pau++ TD13376
        PersistQueue::removeInstance( m_persistQ );
        // ++Raymond Pau TD13376

		// delete the ABE utility
		m_singletonAnswerBackErrorUtil.unregisterForABETimeoutClock ( this );
		m_clockHasStarted = false;

		// delete handle to CalculatedExpression
		if ( 0 != m_calculatedExpressionHandle )
		{
			delete m_calculatedExpressionHandle;
			m_calculatedExpressionHandle = 0;
		}

		// delete handle to LaunchingConditionCheck
		if ( NULL != m_launchingConditionCheck )
		{
			delete m_launchingConditionCheck;
			m_launchingConditionCheck = NULL;
		}

		// clean up audit message sender
		if ( NULL != m_auditMessageSender )
		{
			delete m_auditMessageSender;
			m_auditMessageSender = NULL;
		}

		// Removed handle to the EventDetailsFactory
		if ( NULL != m_eventDetailsFactory )
		{
			EventDetailsFactory::removeInstance( m_eventDetailsFactory );
			m_eventDetailsFactory = NULL;
		}

		// clean up ScadaUtilities
		m_scadaUtilities = NULL;

        FUNCTION_EXIT;
	}

    void DataPoint::updateSynState(const TA_Base_Bus::DpDynamicSyncUpdate& toupdate)
    {
        FUNCTION_ENTRY("updateSynState");
        //only the monitor mode datapoint required to syn the control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			throw( TA_Base_Core::OperationModeException() );
		}
		//For virtual Datapoints, do nothing.
		if( isVirtual() == true )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received update type[%d] in Monitor mode Virtual datapoint [%d]", toupdate.updateType, m_entityKey );
			return;
		}
		
		boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpupdate(new DpDynamicSyncUpdate);
		dpupdate->updateType = toupdate.updateType;
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "update [%d] for entity %u", toupdate.updateType, m_entityKey);
		m_fieldValueQualityStatus = toupdate.syncCompleteState.value.quality;
		switch(toupdate.updateType) 
        {		
           case CompleteDynamicSyncStateUpdate:
                 //alarm properties update will be handled in the corresponding child datapoint type
                m_alarmState = toupdate.syncCompleteState.alarmState;
                m_ackState = toupdate.syncCompleteState.alarmAckState;
                m_controlState = toupdate.syncCompleteState.controlState;
				dpupdate->alarmState = m_alarmState;
				dpupdate->alarmAckState = m_ackState;
				dpupdate->controlState = m_controlState;
                updateSynInhibitState(toupdate.syncCompleteState.inhibitState); 
                m_lastOperatorSessionID = toupdate.syncCompleteState.lastOperatorInfo.sessionID;
                m_lastOperatorTimestamp = toupdate.syncCompleteState.lastOperatorInfo.timeStamp;
                updateSynDataPointValue(toupdate.syncCompleteState.value);
				dpupdate->value = toupdate.value;
				dpupdate->inhibitState = toupdate.syncCompleteState.inhibitState;
				dpupdate->lastOperatorInfo.sessionID = m_lastOperatorSessionID;
				dpupdate->lastOperatorInfo.timeStamp = m_lastOperatorTimestamp;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New CompleteDynamicSyncStateUpdate for entity %u", m_entityKey);
				break;
            case ValueUpdate:
                updateSynDataPointValue(toupdate.value);                
				dpupdate->value = toupdate.value;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New value for entity %u", m_entityKey);
        	    break;
            case AlarmStateUpdate:
                m_alarmState = toupdate.alarmState;
        	    dpupdate->alarmState = m_alarmState;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New m_alarmState %d for entity %u", m_alarmState, m_entityKey);
				break;
            case AlarmAckStateUpdate:
                m_ackState = toupdate.alarmAckState;
                dpupdate->alarmAckState = m_ackState;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New m_ackState %d for entity %u", m_ackState, m_entityKey);
				break;
            case ControlUpdate:
                m_controlState = toupdate.controlState;
				dpupdate->controlState = m_controlState;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New m_controlState %d for entity %u", m_controlState.controlState, m_entityKey);
        	    break;
            case InhibitUpdate:
                updateSynInhibitState(toupdate.inhibitState);
				dpupdate->inhibitState.inputInhibit = toupdate.inhibitState.inputInhibit;
				dpupdate->inhibitState.outputInhibit = toupdate.inhibitState.outputInhibit;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New inhibitState (%d,%d) for entity %u", toupdate.inhibitState.inputInhibit, toupdate.inhibitState.outputInhibit, m_entityKey);
				break;
            case LastOperatorUpdate:
                m_lastOperatorSessionID = toupdate.lastOperatorInfo.sessionID;
                m_lastOperatorTimestamp = toupdate.lastOperatorInfo.timeStamp;
        	    dpupdate->lastOperatorInfo.sessionID = m_lastOperatorSessionID;
				dpupdate->lastOperatorInfo.timeStamp = m_lastOperatorTimestamp;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New m_lastOperatorInfo (%s,%u) for entity %u", m_lastOperatorSessionID.c_str(),m_lastOperatorTimestamp.time, m_entityKey);
				break;
            default:
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Not handling update type[%d] in Monitor mode datapoint [%d].", toupdate.updateType, m_entityKey );
                break;
				FUNCTION_EXIT;
				return;
        }
		dpupdate->syncCompleteState = *(getCompleteDynamicUpdate().get());
		notifyRegisteredObservers(dpupdate);
        FUNCTION_EXIT;
    }
    
	void DataPoint::syncOutputInhibitState(const char* sessionId, TA_Base_Bus::EOutputInhibitState inhibit)
	{
		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );

		if (inhibit == TA_Base_Bus::InhibitControl)
		{
			m_ControlInhibitedState = true;
		}
		else
		{
			m_ControlInhibitedState = false;
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Now SynInhibitState: Control(%d)", m_ControlInhibitedState);         		
	}

    void DataPoint::updateSynInhibitState(const TA_Base_Bus::InhibitState& updateinhibit, bool inputOnly)
    {
		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );

		if(inputOnly == false)
		{
			syncOutputInhibitState("", updateinhibit.outputInhibit);
		}

		if (updateinhibit.inputInhibit == TA_Base_Bus::InhibitScan )
        {
            m_serviceState = TA_Base_Bus::OUT_OF_SERVICE;
            m_AlarmInhibitedState = false;
            m_MMSAlarmInhibitedState = false;
        }
        else if (updateinhibit.inputInhibit == TA_Base_Bus::InhibitAlarm)
        {
            m_AlarmInhibitedState = true;
            m_serviceState = TA_Base_Bus::IN_SERVICE;
            m_MMSAlarmInhibitedState = false;
        }
        else if(updateinhibit.inputInhibit == TA_Base_Bus::InhibitMmsAlarm)
        {
            m_MMSAlarmInhibitedState = true;
            m_serviceState = TA_Base_Bus::IN_SERVICE;
            m_AlarmInhibitedState = false;
        }
        else
        {
            m_serviceState = TA_Base_Bus::IN_SERVICE;
            m_AlarmInhibitedState = false;
            m_MMSAlarmInhibitedState = false;
        }
        
        if (m_serviceState == TA_Base_Bus::IN_SERVICE)
        {
            if (m_forcedState)
            {
                //review, should not use last known field state to update force state, just leave it as it was before
                //setForcedValue(m_lastKnownFieldState);
            }
            else
            {
                setFieldValue(m_lastKnownFieldState);
            }
        }
        else
        {
            //also need to update the current field quality when it is inhibit scan
			m_previousFieldValueQualityStatus = m_fieldValueQualityStatus;
			m_fieldValueQualityStatus = TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE;
        }
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Now SynInhibitState: MMS(%d), Alarm(%d), Control(%d)",
			m_MMSAlarmInhibitedState, m_AlarmInhibitedState, m_ControlInhibitedState); 
    }

    void DataPoint::updateSynDataPointValue(const TA_Base_Bus::DataPointValue& updatevalue)
    {
		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );

		DpValue engVal(updatevalue, getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels(), getPrecision());
		DataPointState updateState( engVal,
								 updatevalue.timeStamp.time,
								 updatevalue.quality );
        //only syn the state when the datapoint is in service
		// ++, when not IN_SERVICE, override this datapoint, lost the override state. so add '//'.
        //if (m_serviceState == TA_Base_Bus::IN_SERVICE)
        {
            //review, should not use internal forced state to determine which value should be synchronized
            //should use the quality instead.
            bool forcedState = updatevalue.quality == QUALITY_GOOD_LOCAL_OVERRIDE;
            if (m_forcedState != forcedState)
            {
                setForcedState(forcedState);
            }

            if (m_forcedState)
            {
                setForcedValue(updateState);
            }
            else
            {
                setFieldValue(updateState);
            }
        }

		 
        switch(updatevalue.dpType)
		{
			case DPT_BOOL_TYPE:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New Bool Value %d", getCurrentEngValue().getDataPointValue().booleanValue);
				break;

			case DPT_ENUM_TYPE:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New Enum Value %u", getCurrentEngValue().getDataPointValue().enumValue);
				break;

			case DPT_TEXT_TYPE:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New text value ");//getCurrentEngValue().getDataPointValue().stringValue);
				break;

			case DPT_INT16_TYPE:
			case DPT_INT32_TYPE:
			case DPT_INT16S_TYPE:
			case DPT_INT32S_TYPE:
			case DPT_UINT16_TYPE:
			case DPT_UINT32_TYPE:
			case DPT_IEEE32_TYPE:
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "New Analogue value %f", getCurrentEngValue().getDataPointValue().floatValue);
				break;

			case DPT_NULL_TYPE:
			default:
				break;	
		}
    }

    //The "get" method for the datapoint object
	unsigned long DataPoint::getEntityKey() const
	{
		return m_entityKey;
	}

	const std::string & DataPoint::getDataPointName() const
	{
		return m_name;
	}

	const std::string & DataPoint::getDataPointAddress() const
	{
		return m_address;
	}
    
	unsigned long DataPoint::getDataPointRegisterAddress() const
	{
		return m_registerAddress;
	}

	int DataPoint::getDataPointBitAddress() const
	{
		return m_bitAddress;
	}

    const std::string & DataPoint::getDescription() const
    {
        return m_description;
    }

    const std::string &	DataPoint::getViewAssociation() const
    {
        return m_viewAssociation;
    }

    const std::string &	DataPoint::getEngineeringNote() const
    {
        return m_engNote;
    }

    const std::string &	DataPoint::getHelpURL() const
    {
        return m_helpURL;
    }

    const std::string &	DataPoint::getLaunchingConditionResultString()
	{
        FUNCTION_ENTRY("getLaunchingConditionResultString");
		// if current control status is LCC Failed
		if ( TA_Base_Bus::LCC_failed == m_controlState.controlState )
		{
			// if the result string is not loaded, try to load it from database
			if ( 0 == (m_controlState.lccResult).compare ( NotLoaded ) )
			{
                m_persistence->readPersistedValue( DP_LAUNCH_RESULT_STRING, m_entityKey, m_controlState.lccResult );
			}

		}
		return m_controlState.lccResult;
	}

    const std::string &	DataPoint::getLastOperatorSession()
    {
        return m_lastOperatorSessionID;
    }

    const timeb& DataPoint::getLastOperatorTimeStamp()
    {
        return m_lastOperatorTimestamp;
    }

	const std::string & DataPoint::getOMFlag() const
	{
        FUNCTION_ENTRY("getOMFlag");
		// if the OMF flag has not been loaded from the database yet
		if ( 0 == m_OMFlag.compare ( NotLoaded ) )
		{
			// get the latest info from database
			const_cast<DataPoint*>(this)->m_OMFlag = m_dataPointEntityAccess->getOMFlag();
		}
        FUNCTION_EXIT;
		return m_OMFlag;
	}
    
	TA_Base_Bus::EDataPointAlarms DataPoint::getAlarmState() const
	{
		return m_alarmState;
	}

    const std::string & DataPoint::getAlarmStateString() const
    {
        return m_alarmStateString;
    }

    int DataPoint::getScanFactor() const
    {
        return m_scanTimeFactor;
    }

    long DataPoint::getDisplayOrder() const
    {
        return m_displayOrder;
    }

	long DataPoint::getPrecision() const
	{
		return m_precision;
	}

    long DataPoint::getUpdatePriorityLevel() const
	{
		return m_updatePriorityLevel;
	}

	unsigned long DataPoint::getTypeKey() const
	{
		return m_entityTypeKey;
	}

	TA_Base_Bus::EDataPointType DataPoint::getDataPointType() const
	{
		return m_type;
	}

	TA_Base_Bus::EDataPointDataType DataPoint::getDataPointDataType() const
	{
		return m_dataType;
	}

	unsigned long DataPoint::getSubsystemKey() const
	{
		return m_subsystemKey;
	}

	unsigned long DataPoint::getPhysicalSubsystemKey() const
	{
		return m_dataPointEntityAccess->getPhysicalSubsystem();
	}

	unsigned long DataPoint::getLocationKey() const
	{
		return m_locationKey;
	}
	
	const std::string & DataPoint::getLocationName() const
	{
		return m_locationName;
	}

    unsigned long DataPoint::getParentEntityKey() const
	{
		return m_parentKey;
	}

	bool DataPoint::isWriteable() const
	{
		return m_writable;
	}

	bool DataPoint::isInForcedState() const
	{
		return m_forcedState;
	}

    bool DataPoint::getControlInhibitedState() const
    {
        return m_ControlInhibitedState;
    }

    bool DataPoint::getAlarmInhibitedState() const
    {
        return m_AlarmInhibitedState;
    }

    bool DataPoint::getMMSAlarmInhibitedState() const
    {
        return m_MMSAlarmInhibitedState;
    }
    
    bool DataPoint::getTrending() const
    {
        return m_trendingEnabled;
    }

	bool DataPoint::isEngUnitsEnabled() const
	{
		return m_engUnitsEnabled;
	}
    
	bool DataPoint::isVirtual() const
	{
		return m_virtual;
	}

	bool DataPoint::getLogChanges() const
    {
        return m_logChanges;
    }

	bool DataPoint::getAnswerBackErrorResult() const
	{
		return m_answerBackErrorCheckingResult;
	}

	bool DataPoint::getWordSwapIndication() const
	{
		return m_wordSwap;
	}
    
    bool DataPoint::getByteSwapIndication() const
    {
        return m_byteSwap;
    }

	long DataPoint::getBitWidth() const
	{
		return m_bitWidth;
	}

	bool  DataPoint::getIsBigEndian() const
	{
		return m_bigEndian;
	}
    
	const DpvEnumLabelsMap& DataPoint::getEnumLabelsMap() const
	{
		return m_enumLabelsMap;
	}

	const DpvBooleanLabels& DataPoint::getBooleanLabels() const
	{
		return m_booleanLabels;
	}
    
	DpValue & DataPoint::getCurrentValue()
	{
        FUNCTION_ENTRY("getCurrentValue");
		// if the data point is forced, i.e in manually edited state
		if ( true == m_forcedState )
		{
            FUNCTION_EXIT;
			// data point's current value is the forced value
			return m_forcedValue;
		}

		// if the data point is not in forced state
		else
		{
            FUNCTION_EXIT;
			// data point's current value is the field (or calculated) value
			return m_fieldValue;
		}
	}

	DpValue & DataPoint::getCurrentFieldValue()
	{
		FUNCTION_ENTRY("getCurrentFieldValue");
		return m_fieldValue;
	}

	DpValue & DataPoint::getCurrentEngValue()
	{
        FUNCTION_ENTRY("getCurrentEngValue");
		// if the data point is forced, i.e in manually edited state
		if ( true == m_forcedState )
		{
            FUNCTION_EXIT;
			// data point's current value is the forced value
			return m_forcedEngValue;
		}

		// if the data point is not in forced state
		else
		{
            FUNCTION_EXIT;
			// data point's current value is the field (or calculated) value
			return m_fieldEngValue;
		}
	}

    boost::shared_ptr<TA_Base_Bus::DataPointValue> DataPoint::getFieldValue()
    {
		FUNCTION_ENTRY("GetFieldValue");

		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw( TA_Base_Core::OperationModeException() );
		}

		boost::shared_ptr<TA_Base_Bus::DataPointValue> dpState(new TA_Base_Bus::DataPointValue);
        dpState->dpType = m_dataType;
		// assign current field value
		switch ( m_dataType )
		{
			// text
			case DPT_TEXT_TYPE:
			{
				dpState->stringValue = m_fieldEngValue.getByteSequence() ;
				break;
			}

			// boolean
			case DPT_BOOL_TYPE:
			{
				dpState->booleanValue = m_fieldEngValue.getBoolean() ;
				break;
			}

			// derived, enum, safety output, combined output
			case DPT_ENUM_TYPE:
			{
				dpState->enumValue = m_fieldEngValue.getEnum() ;
				break;
			}

			// analogue
			case DPT_INT16_TYPE:
			case DPT_INT32_TYPE:
			case DPT_INT16S_TYPE:
			case DPT_INT32S_TYPE:
			case DPT_UINT16_TYPE:
			case DPT_UINT32_TYPE:
			case DPT_IEEE32_TYPE:
            {
				// value of Analogue/Derived/Enum data points is stored as double internally
				dpState->floatValue = m_fieldEngValue.getFloat() ;
				break;
            }
			default:
				break; 
		}

        dpState->quality = getStatus();
        dpState->timeStamp = getTimebStructure();
		dpState->controlOperation = TA_Base_Bus::CONTROL_NO_OPERATION;
		FUNCTION_EXIT;
		return dpState;
        
    }

    boost::shared_ptr<TA_Base_Bus::DataPointValue>  DataPoint::getRawValue()
    {
		FUNCTION_ENTRY("getRawValue");
		
		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw ( TA_Base_Core::OperationModeException() );
		}
		
		boost::shared_ptr<TA_Base_Bus::DataPointValue> dpState(new TA_Base_Bus::DataPointValue);
        dpState->dpType = m_dataType;
		// assign current field value
		switch ( m_dataType )
		{
			// text
			case DPT_TEXT_TYPE:
			{
				dpState->stringValue = m_fieldValue.getByteSequence() ;
				break;
			}

			// boolean
			case DPT_BOOL_TYPE:
			{
				dpState->booleanValue = m_fieldValue.getBoolean() ;
				break;
			}

			// derived, enum, safety output, combined output
			case DPT_ENUM_TYPE:
			{
				dpState->enumValue = m_fieldValue.getEnum() ;
				break;
			}

			// analogue
			case DPT_INT16_TYPE:
			case DPT_INT32_TYPE:
			case DPT_INT16S_TYPE:
			case DPT_INT32S_TYPE:
			case DPT_UINT16_TYPE:
			case DPT_UINT32_TYPE:
			case DPT_IEEE32_TYPE:
            {
				// value of Analogue/Derived/Enum data points is stored as double internally
				dpState->floatValue = m_fieldValue.getFloat() ;
				break;
            }
			default:
				break; 
		}

        dpState->quality = getStatus();
        dpState->timeStamp = getTimebStructure();
				
		FUNCTION_EXIT;		
		return dpState;    
    }
    
    boost::shared_ptr<DatapointCompleteStaticConfig> DataPoint::getCompleteStaticConfig()
    {
        boost::shared_ptr<DatapointCompleteStaticConfig> toReturn(new DatapointCompleteStaticConfig);
		toReturn->entityKey = getEntityKey();            
		toReturn->name = getDataPointName();   
		toReturn->entityTypeKey = getTypeKey();
		toReturn->subsystemKey = getSubsystemKey();
		toReturn->locationKey = getLocationKey(); 
		toReturn->parentKey = getParentEntityKey();
		toReturn->description = getDescription();
		toReturn->helpURL = getHelpURL();
		toReturn->typeShortName = m_typeShortName;
		toReturn->displayOrder = getDisplayOrder();
		toReturn->precision = getPrecision();
		toReturn->writable = isWriteable();
		toReturn->type = getDataPointType();
		toReturn->dataType = getDataPointDataType();
		toReturn->enumLabelsMap = getEnumLabelsMap();
		toReturn->booleanLabels = getBooleanLabels();
        //alarm properties vector should be constructed by each different type of datapoints(analog, enum, boolean,...)
        //alarm Engineering Units should be constructed by each different type of datapoints(analog, enum, boolean,...)
        return toReturn;

    }

    boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> DataPoint::getCompleteDynamicUpdate()
    {
        boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeudpate(new DpCompleteDynamicSyncState);
        //set the current datapoint value to the update structure
        setCurrentDataPointValue(*completeudpate);
        //set the current alarm state to the update structure
        completeudpate->alarmState = m_alarmState;
        //set the current alarm ack state to the update structure
        completeudpate->alarmAckState = m_ackState;
         //set the current control state
        completeudpate->controlState = m_controlState;
        //set the current inhibit state
        completeudpate->inhibitState = getInhibitState();
        //set the current last operator info
        completeudpate->lastOperatorInfo.sessionID = getLastOperatorSession();
        completeudpate->lastOperatorInfo.timeStamp = getLastOperatorTimeStamp();
        //alarm properties vector should be constructed by each different type of datapoints(analog, enum, boolean,...)
        return completeudpate;
    }

    void DataPoint::setMeteredCorrectionValue ( const std::string& sessionId, double newValue )
	{
		FUNCTION_ENTRY("SetMeteredCorrectionValue");

		throw ( TA_Base_Core::NotAvailableException() );

		FUNCTION_EXIT;
	}

    boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> DataPoint::getDisplayState()
    {
		FUNCTION_ENTRY("GetDisplayState");

        // dont raise operation mode exception, for it will not be accessed by external system directly
		//if ( TA_Base_Core::Monitor == m_operationMode )
		//{
		//	throw( TA_Base_Core::OperationModeException() );
		//}

        boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> dpDisplayState(new TA_Base_Bus::DataPointDisplayState);

        //return the entitykey
		dpDisplayState->entityKey = getEntityKey();
        //set the dp value
        dpDisplayState->value.dpType = getDataPointDataType();
        switch(getDataPointDataType())
		{
		case DPT_BOOL_TYPE:
			dpDisplayState->value.booleanValue = getCurrentEngValue().getDataPointValue().booleanValue;
			break;

		case DPT_ENUM_TYPE:
			dpDisplayState->value.enumValue = getCurrentEngValue().getDataPointValue().enumValue;
			break;

		case DPT_TEXT_TYPE:
			dpDisplayState->value.stringValue = getCurrentEngValue().getDataPointValue().stringValue;
			break;

		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			dpDisplayState->value.floatValue = getCurrentEngValue().getDataPointValue().floatValue;
			break;

		case DPT_NULL_TYPE:
		default:
			break;	
		}

        // convert enum alarmState to boolean representation of alarmState
        dpDisplayState->alarmState = (m_alarmState != TA_Base_Bus::DptNoAlarm);
        dpDisplayState->alarmAckState = m_ackState;
        dpDisplayState->dataPointStatus = getStatus();

		FUNCTION_EXIT;
		return dpDisplayState;
    }

    unsigned long DataPoint::getTimestamp() const
	{
        FUNCTION_ENTRY("getTimestamp");
		if ( true == m_forcedState )
		{
            FUNCTION_EXIT;
			return m_forcedValueTimestamp.time;
		}

        FUNCTION_EXIT;
        return m_fieldValueTimestamp.time;
	}


	//Using for IEC104 Protocol
	/**
	* DataPoint::getPScadaFlag()
	*
	* Returns the PScada Flag of this Data Point
	*
	* @return	true indicate this is a data point used in PSCADA.
	*
	*/

	bool DataPoint::getPScadaFlag() const
	{
		FUNCTION_ENTRY("getPScadaFlag");

		FUNCTION_EXIT;
		return m_pscadaFlag;
	}

	const timeb & DataPoint::getTimebStructure() const
	{
        FUNCTION_ENTRY("getTimebStructure");
		if ( true == m_forcedState )
		{
            FUNCTION_EXIT;
			return m_forcedValueTimestamp;
		}
		
        FUNCTION_EXIT;
        return m_fieldValueTimestamp;
	}
    
    TA_Base_Bus::EQualityStatus DataPoint::getQualityStatus() const
	{
		return getStatus();
	}

	TA_Base_Bus::EQualityStatus DataPoint::getStatus() const
	{
        FUNCTION_ENTRY("getStatus");
		if ( true == m_forcedState )
		{
            FUNCTION_EXIT;
			return m_forcedValueQualityStatus;
		}

        FUNCTION_EXIT;
		return m_fieldValueQualityStatus;
	}

	TA_Base_Bus::EQualityStatus DataPoint::getFieldValueQualityStatus() const
	{
		 FUNCTION_ENTRY("getFieldValueQualityStatus");
		 return m_fieldValueQualityStatus;
	}
    
	TA_Base_Bus::EDataPointServiceState DataPoint::getServiceState() const
	{
		return m_serviceState;
	}

	bool DataPoint::isDataPointInService() const
	{
		return m_serviceState == IN_SERVICE;
	}
    
	TA_Base_Bus::DataPointControl& DataPoint::getControlState()
	{
		return m_controlState;
	}

	bool DataPoint::getIsDigitalDataPoint() const
	{
		if ( TA_Base_Bus::DPT_BOOL_TYPE == m_dataType )
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	bool DataPoint::getIsTextDataPoint() const
	{
		if ( TA_Base_Bus::DPT_TEXT_TYPE == m_dataType )
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	bool DataPoint::getIsAnalogueDataPoint() const
	{
		// true if not a text and not a boolean data point, false otherwise
		return ( !getIsTextDataPoint() && !getIsDigitalDataPoint() );
	}


	bool DataPoint::isValidUpdate( DataPointState & newState )
	{
		// by default, assume that newState is valid.  Instances of the derived
		// class to implement its own validation process
		return true;
	}
    
     TA_Base_Bus::InhibitState DataPoint::getInhibitState()
     {
        TA_Base_Bus::InhibitState toReturn;
        toReturn.inputInhibit = NoInputInhibit;
        toReturn.outputInhibit = NoOutputInhibit;
        if (m_ControlInhibitedState)
        {
            toReturn.outputInhibit = InhibitControl;
        }
        //the input inhibit, 
        //1). if it is inhibitScan, then both alarm and mms are also inhibited
        //2). if it is inhibitAlarm. then mms is also inhibited
        if (!isDataPointInService())
        {
            toReturn.inputInhibit = InhibitScan;
        }
        else if (m_AlarmInhibitedState)
        {
            toReturn.inputInhibit = InhibitAlarm;
        }
        else if(m_MMSAlarmInhibitedState)
        {
            toReturn.inputInhibit = InhibitMmsAlarm;
        }
        
        return toReturn;
     }

	bool DataPoint::isValidWrite( DpValue & newValue )
	{
		// by default, assume that newValue is valid.  Instances of the derived
		// class to implement its own validation process
		return true;
	}

    //
    //  The 'set' methods
    //
    void DataPoint::setPersistedValueFromDB()
    {
        try
        {
            std::string tempString="0";

            m_persistence->readPersistedValue( DP_CURRENT_VALUE, m_entityKey, tempString );
            m_fieldEngValue.setFromPersist(tempString, m_precision);
            convertToRaw(m_fieldEngValue, m_fieldValue);
            m_fieldValueQualityStatus = QUALITY_GOOD_NO_SPECIFIC_REASON;
			m_previousFieldValueQualityStatus = m_fieldValueQualityStatus;
        }
        catch (...) 
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Catch exception when load last persisted value of datapoint %d", m_entityKey);
        }
    }

    void DataPoint::setDescription( const std::string & description )
	{
		m_description.assign( description );
	}

	void DataPoint::setViewAssociation( const std::string & viewAssociation )
	{
		m_viewAssociation.assign( viewAssociation );
	}

	void DataPoint::setEngNote( const std::string & engNote )
	{
		m_engNote.assign( engNote );
	}

	void DataPoint::setHelpURL( const std::string & helpURL )
	{
		m_helpURL.assign( helpURL );
	}

	void DataPoint::setScanFactor( int scanFactor )
	{
		m_scanTimeFactor = scanFactor;
	}

	void DataPoint::setPrecision( long precision )
	{
		if ( precision >= 0 )
		{
			m_precision = precision;
		}
	}

	void DataPoint::setWritable( bool writeable )
	{
		m_writable = writeable;
	}

	void DataPoint::setLaunchingConditionResultString(const std::string& newResultString)
	{
		m_controlState.lccResult = newResultString;
		PersistRequest * persistReq = new PersistRequest( DP_LAUNCH_RESULT_STRING, m_entityKey, m_controlState.lccResult, m_fieldValueQualityStatus );
		m_persistQ->insert( persistReq );
	}

    void DataPoint::setInputInhibitState(const std::string& sessionId, TA_Base_Bus::EInputInhibitState inhibitstate)
    {
        //check whether the inhibit state is already set to the desired state
		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );
      
        if ( inhibitstate == m_previousInputInhibitState ) 
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                         "[XF] Input Inhibit state already set to desired value(%d). Ignoring request", m_previousInputInhibitState );
            return;
        }

		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			TA_Base_Bus::InhibitState updateinhibit;
			updateinhibit.inputInhibit = inhibitstate;
			updateSynInhibitState(updateinhibit, true);
			m_previousInputInhibitState = inhibitstate;
			//throw( TA_Base_Core::OperationModeException() );
			return;
		}

        //raise the audit message
		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
		NameValuePair pair1 ( "OnOff", ( NoInputInhibit != inhibitstate ) ? "ON" : "OFF" );
		descriptionParameters.push_back ( &pair1 );

	    struct timeb currentSystemTime;//TD19437

        switch(inhibitstate) 
        {
        case NoInputInhibit:
			  LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[XF] DataPoint::setInputInhibitState(): NoInputInhibit");
              setServiceState(sessionId, true);
              updateAlarmInhibitedStatus(sessionId,false);
              updateMMSAlarmInhibitedStatus(sessionId, false);
              break;

        case InhibitScan:
			  LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[XF] DataPoint::setInputInhibitState(): InhibitScan");
              setServiceState(sessionId, false);
              updateAlarmInhibitedStatus(sessionId,true);
              updateMMSAlarmInhibitedStatus(sessionId, true);                  
              break;

        case InhibitAlarm:
			  LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[XF] DataPoint::setInputInhibitState(): InhibitAlarm");
              setServiceState(sessionId, true);
			  //TD19437++
			  {
				//update LASTOPERATORTIMESTAMP of the dapoint first, so that to get lastoperation info to close the alarm	  
				ftime ( &currentSystemTime );
				saveLastOperatorInfo ( sessionId, currentSystemTime);
			  }
			  //++TD19437
              updateAlarmInhibitedStatus(sessionId,true);
              updateMMSAlarmInhibitedStatus(sessionId, true);                  
              break;
          
        case InhibitMmsAlarm:
              LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[XF] DataPoint::setInputInhibitState(): InhibitMmsAlarm");
              setServiceState(sessionId, true);
              updateAlarmInhibitedStatus(sessionId,false);
              updateMMSAlarmInhibitedStatus(sessionId, true);                  
              break;

        default:
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown input inhibit state: %d", inhibitstate );
			return;
        }    

		m_previousInputInhibitState = inhibitstate;

        //initialized the dynamic update data structure
        boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpState(new DpDynamicSyncUpdate);
        dpState->updateType = TA_Base_Bus::InhibitUpdate;
        dpState->inhibitState.inputInhibit = inhibitstate;
        dpState->inhibitState.outputInhibit = (m_ControlInhibitedState ? InhibitControl : NoOutputInhibit);

        submitUpdate(dpState);    
		// save the session id of the operator who initiated this operation
		//struct timeb currentSystemTime; //TD19437
		ftime ( &currentSystemTime );
		saveLastOperatorInfo ( sessionId, currentSystemTime );     
    }

	void DataPoint::setServiceState(const  std::string& sessionId, bool service )
    {
		FUNCTION_ENTRY("setServiceState");
		bool isInService = (m_serviceState == IN_SERVICE ? true : false);
		if (isInService == service)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[XF] DataPoint:(%u) service state already is %d, ignore request.", m_entityKey, service);
			return;
		}

		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
		NameValuePair pair1 ( "OnOff", ( true == service ) ? "OFF" : "ON" );
		descriptionParameters.push_back ( &pair1 );

		// if the requested action is authorised for the sessionID
		if ( true == isActionPermitted ( sessionId, ( true == service ? TA_Base_Bus::aca_DATA_POINT_SCAN_UNINHIBIT : TA_Base_Bus::aca_DATA_POINT_SCAN_INHIBIT ) ) )
		{

			if ( true == updateInServiceStatus(service) )
			{
				// log auditing event
				submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ScanInhibitionSucceeded );
			    //if the datapoint is not put into service
                if ( false == service )
			    {
				    // default quality status of the field value to out of service
					if ( true == m_ackCloseAlarmOnInhibit )
					{
						ackCloseAlarm(sessionId);
					}
			    }
            }
			else
			{
				// log auditing event
				submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ScanInhibitionPersistenceFailed );
				throw TA_Base_Bus::ScadaCorbaTypes::PersistenceFailed();
			}
		}
		else
		{
			// log auditing event
			submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ScanInhibitionPermissionDenied );
			throw( TA_Base_Core::AccessDeniedException() );
		}

		FUNCTION_EXIT;
    }

	bool DataPoint::updateInServiceStatus ( bool inService )
	{
        FUNCTION_ENTRY("updateInServiceStatus");
		bool result = true;

        result = m_persistence->writePersistedValue( DP_SERVICE_STATE, m_entityKey, 
                                                     ( inService ? IN_SERVICE_STRING : OUT_SERVICE_STRING ),
                                                     m_fieldValueQualityStatus );
		// if successful
		if ( true == result )
		{
			// modify data point's service state
			m_serviceState = ( ( true == inService ) ? IN_SERVICE : OUT_OF_SERVICE );
			
			// set quality status of the field value, leave the quality status
			// of the forced value unchanged
		
			if ( true == inService )
			{
				// if data point is put in service
				updateFieldState(m_lastKnownFieldState);
			}
			else
			{
				// default quality status of the field value to out of service
				m_fieldValueQualityStatus = QUALITY_BAD_OUT_OF_SERVICE;
				m_previousFieldValueQualityStatus = m_fieldValueQualityStatus;

				boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpupdate(new DpDynamicSyncUpdate);
                dpupdate->updateType = TA_Base_Bus::ValueUpdate;
                dpupdate->value = getCurrentEngValue().getDataPointValue();
                dpupdate->value.quality = getQualityStatus();
                dpupdate->value.timeStamp = getTimebStructure();
                    
				//send out the update to inform the observer
                submitUpdate ( dpupdate );
			}
		}
	
        FUNCTION_EXIT;
		return result;
	}

    void DataPoint::updateAlarmInhibitedStatus ( const std::string& sessionId, bool indication )
	{
        FUNCTION_ENTRY("updateAlarmInhibitedStatus");

		
		if (m_AlarmInhibitedState == indication)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[XF] DataPoint:(%u) AlarmInhibitedState already is %d, ignore request.", m_entityKey, indication);
			return;
		}

		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
		NameValuePair pair1 ( "OnOff", ( true == indication ) ? "ON" : "OFF" );
		descriptionParameters.push_back ( &pair1 );

		// if the requested action is authorised for the sessionId
		if ( true == isActionPermitted ( sessionId, ( true == indication ? TA_Base_Bus::aca_DATA_POINT_ALARM_INHIBIT : TA_Base_Bus::aca_DATA_POINT_ALARM_UNINHIBIT ) ) )
		{
			// save the current alarm inhibition state
            bool result = m_persistence->writePersistedValue( DP_ALARM_INHIBIT, m_entityKey, indication, 
                                                         m_fieldValueQualityStatus );

			if ( true == result )
			{
			    // modify data point's AlarmInhibited state
			    m_AlarmInhibitedState = indication;

				submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::AlarmInhibitionSucceeded );

			    // update alarm status to match the current value when
			    //	alarm inhibition is removed
			    if ( false == m_AlarmInhibitedState )
			    {	
                    m_previousAlarmState = m_alarmState;
				    DataPointState engState( getCurrentEngValue(),
										     getTimebStructure(),
										     getStatus() );
                    
				    updateAlarms( engState );
                    //if the alarms is different from the previous state
                    if (m_previousAlarmState != m_alarmState)
                    {
                        //the alarm status update also required if after removing the alarm inhibit, dp is in alarm
                        boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmupdate(new DpDynamicSyncUpdate);
                        alarmupdate->updateType = TA_Base_Bus::AlarmStateUpdate;
                        alarmupdate->alarmState = m_alarmState;
                        //send out the update to inform the observer
                        submitUpdate ( alarmupdate );
                    }
			    }
				else
                {
					if ( true == m_ackCloseAlarmOnInhibit )
					{
						ackCloseAlarm(sessionId);
					}
				}
			}
			else
			{
				// log auditing event
				submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::AlarmInhibitionPersistenceFailed );
				throw TA_Base_Bus::ScadaCorbaTypes::PersistenceFailed();
			}
		}
		else
		{
			// log auditing event
			submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::AlarmInhibitionPermissionDenied );
			throw( TA_Base_Core::AccessDeniedException() );
		}

        FUNCTION_EXIT;
	}


	void DataPoint::updateMMSAlarmInhibitedStatus (const  std::string& sessionId, bool indication )
	{
        FUNCTION_ENTRY("updateMMSAlarmInhibitedStatus");

		if (m_MMSAlarmInhibitedState == indication)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[XF] DataPoint:(%u) MMSAlarmInhibitedState already is %d, ignore request.", m_entityKey, indication);
			return;
		}

		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
		NameValuePair pair1 ( "OnOff", ( true == indication ) ? "ON" : "OFF" );
		descriptionParameters.push_back ( &pair1 );

		// if the requested action is authorised for the sessionId
		if ( true == isActionPermitted ( sessionId, ( true == indication ? TA_Base_Bus::aca_DATA_POINT_MMS_INHIBIT : TA_Base_Bus::aca_DATA_POINT_MMS_UNINHIBIT ) ) )
		{
            bool result = m_persistence->writePersistedValue( DP_MMS_INHIBIT, m_entityKey, indication, 
														 m_fieldValueQualityStatus );
			if ( true == result )
			{
			    // modify data point's MMSInhibited state
			    m_MMSAlarmInhibitedState = indication;

				submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::MMSInhibitSucceeded );
				// update MMS alarm status to match the current value when
				// MMS alarm inhibition is removed
				if ( false == m_MMSAlarmInhibitedState )
				{
                    m_previousAlarmState = m_alarmState;
					DataPointState engState( getCurrentEngValue(),
											 getTimebStructure(),
											 getStatus() );

					updateAlarms( engState );
                    if (m_previousAlarmState != m_alarmState)
                    {
                        //the alarm status update also required if after removing the mms inhibit, dp is in alarm
                        boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmupdate(new DpDynamicSyncUpdate);
                        alarmupdate->updateType = TA_Base_Bus::AlarmStateUpdate;
                        alarmupdate->alarmState = m_alarmState;
                        //send out the update to inform the observer
                        submitUpdate ( alarmupdate );
                    }
				}
			}
			else
			{
				// log auditing event
				submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::MMSInhibitPersistenceFailed );
				throw TA_Base_Bus::ScadaCorbaTypes::PersistenceFailed();
			}
		}
		else 
		{
			// log auditing event
			submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::MMSInhibitPermissionDenied );
			throw( TA_Base_Core::AccessDeniedException() );
		}
	
        FUNCTION_EXIT;
	}


    void DataPoint::setOutputInhibitState (const  std::string& sessionId, TA_Base_Bus::EOutputInhibitState inhibitstate)
	{
        FUNCTION_ENTRY("setOutputInhibitState");
		//if ( TA_Base_Core::Monitor == m_operationMode )
		//{
		//	throw( TA_Base_Core::OperationModeException() );
		//}

		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );
        bool indication = false;
        switch(inhibitstate) 
        {
        case NoOutputInhibit:
            indication = false; 
        	break;
        case InhibitControl:
            indication = true; 
        	break;
        default:
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                         "Invalid output inhibit state [%d]",  inhibitstate);
            return;

        }

        // Ignore if control inhibit already set to the desired value
        if ( m_ControlInhibitedState == indication )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                         "Control inhibit already set to desired value(%d). Ignoring request", m_ControlInhibitedState );
            return;
        }
		//Set value before throwing exception
		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			syncOutputInhibitState(sessionId.c_str(), inhibitstate);
			return;
			//throw( TA_Base_Core::OperationModeException() );
		}

		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
		NameValuePair pair1 ( "OnOff", ( true == indication ) ? "ON" : "OFF" );
		descriptionParameters.push_back ( &pair1 );

		// if the requested action is authorised for the sessionId
		if ( true == isActionPermitted ( sessionId, ( true == indication ? TA_Base_Bus::aca_DATA_POINT_CONTROL_INHIBIT : TA_Base_Bus::aca_DATA_POINT_CONTROL_UNINHIBIT ) ) )
		{
            bool result = m_persistence->writePersistedValue( DP_CONTROL_INHIBIT, m_entityKey, indication, 
														     m_fieldValueQualityStatus );

			if ( true == result )
			{
			    // modify data point's ControlInhibited state
			    m_ControlInhibitedState = indication;
                
                //prepared the dynamic update data structure
                boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpState(new DpDynamicSyncUpdate);
                dpState->updateType = TA_Base_Bus::InhibitUpdate;
                //also need to put in the input inhibit state
                if (m_serviceState != IN_SERVICE )
                {
				    dpState->inhibitState.inputInhibit = InhibitScan;
                }
                else if (m_AlarmInhibitedState == true) 
                {
				    dpState->inhibitState.inputInhibit = InhibitAlarm;
                }
                else if (m_MMSAlarmInhibitedState == true )
                {
				    dpState->inhibitState.inputInhibit = InhibitMmsAlarm;
                }
                else
                {
				    dpState->inhibitState.inputInhibit = NoInputInhibit;
                }

				if ( false == m_ControlInhibitedState )
				{
                    //sets the data member
                    dpState->inhibitState.outputInhibit = TA_Base_Bus::NoOutputInhibit;
                    //inform the observers
                    submitUpdate ( dpState );
				}
                else// if the data point control is inhibited by the operator
                {
                    //sets the data member
                    dpState->inhibitState.outputInhibit = TA_Base_Bus::InhibitControl;
                    //inform the observers
                    submitUpdate ( dpState );
					// save the session id of the operator who initiated this operation
					struct timeb currentSystemTime;
					ftime ( &currentSystemTime );
					saveLastOperatorInfo ( sessionId, currentSystemTime );

                }
				// CL19435++ - Will not raise event if the DP has been hidden
				if ( -1 != m_displayOrder )
				{
					// log auditing event
					submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ControlInhibitionSucceeded );
				}
				// ++CL19435
			}
			else
			{
				// CL19435++ - Will not raise event if the DP has been hidden
				if ( -1 != m_displayOrder )
				{
					// log auditing event
					submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ControlInhibitionPersistenceFailed );
				}
				// ++CL19435
		
				throw TA_Base_Bus::ScadaCorbaTypes::PersistenceFailed();
			}
		}
		else
		{
			// log auditing event
			submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ControlInhibitionPermissionDenied );

			throw( TA_Base_Core::AccessDeniedException() );
		}
		
        FUNCTION_EXIT;
	}

	void DataPoint::setTrending( bool trendingEnabled )
	{
		m_trendingEnabled = trendingEnabled;
	}

    void DataPoint::setEngUnitsEnabled( bool enabled )
	{
		m_engUnitsEnabled = enabled;
	}

	bool DataPoint::setFieldValue( const TA_Base_Bus::DataPointState & newState )
	{
        FUNCTION_ENTRY("setFieldValue");
        if (TA_Base_Core::Control == m_operationMode)
        {
            // Keep copy of old eng value and quality for checking
            // before persisting to database
            DpValue oldFieldEngValue = m_fieldEngValue;

		    // save new field value, timestamp and status
            DataPointState engState( newState );

		    // save new eng value.  Note that the field quality status will be changed
		    // if the eng value exceeds the limits
		    convertToEngUnits( newState, engState );

            m_fieldValueTimestamp = newState.getTimebStructure();
		    m_fieldValueQualityStatus = engState.getStatus();
            m_fieldValue = newState.getValue();
            m_fieldEngValue = engState.getValue();

            //
            // Persist to database, if value or quality has changed
            //
            // TD11969 - re-check value and quality since it may have changed
            //           as a result of the convertToEngUnits() call.
            //
            // persists data change only when trending enabled
            // only persist value changes if quality is good.
            if (m_trendingEnabled && (oldFieldEngValue != m_fieldEngValue) && (QUALITY_GOOD_NO_SPECIFIC_REASON == m_fieldValueQualityStatus) )
            {
                // Raymond Pau++ TD13376
                PersistRequest * persistReq = new PersistRequest( DP_CURRENT_VALUE, m_entityKey, m_fieldEngValue.getPersistString(), m_fieldValueQualityStatus );
                m_persistQ->insert( persistReq );
                // ++Raymond Pau TD13376
            }
        }
		// if not in control mode, save the new state in memory only
        else
        {
            // save new forced value, timestamp and status
            DataPointState rawState( newState );

			// save new eng value
			convertToRaw( newState.getValue(), rawState.getValue() );

            m_fieldValue = rawState.getValue();
			m_fieldValueTimestamp = rawState.getTimebStructure();
			m_fieldValueQualityStatus = rawState.getStatus();
            m_fieldEngValue = newState.getValue();
		}

        FUNCTION_EXIT;
		return true;
	}

    bool DataPoint::setForcedValue( const DataPointState & newState )
    {
        FUNCTION_ENTRY("setForcedValue");
        
        bool result = true;
        
        //if ( ( m_forcedValue != newState.getValue() ) || ( m_forcedValueQualityStatus != newState.getStatus() ) ) //limin
        //update forced timestamp, raising alarm need the timestamp.
		//if ( ( m_forcedValue != newState.getValue() ) || ( m_forcedValue.getValueAsString() != newState.getValue().getValueAsString() ) || ( m_forcedValueQualityStatus != newState.getStatus() ) )
        {
            // save new forced value, timestamp and status
            DataPointState inState( newState );
                      
            // if the data point is in control mode
            if ( TA_Base_Core::Control == m_operationMode )
            {
				convertToEngUnits( newState, inState);
            
				m_forcedValue = newState.getValue();
				m_forcedValueTimestamp = newState.getTimebStructure();
				m_forcedValueQualityStatus = inState.getStatus();
				m_forcedEngValue = inState.getValue();
                // if quality is good, then persist to database for forced value, the quality should be QUALITY_GOOD_LOCAL_OVERRIDE.
                if ( TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE == newState.getStatus() )
                {
                    result = m_persistence->writePersistedValue( DP_MANUALLY_DRESSED_VALUE, m_entityKey, inState.getValue().getPersistString(),
                        m_forcedValueQualityStatus );
                    
                    // if failed to persist forced value
                    if ( false == result )
                    {
                        std::stringstream ss;
                        ss << m_name << ": failed to persist manually dressed value " << newState.getValue().getPersistString() << " to DB";
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss.str().c_str() );
                    }
                }
            }
			else //Convert Engg->Raw in case of Monitor mode
			{
				convertToRaw( newState.getValue(), inState.getValue());
            
				m_forcedValue = newState.getValue();
				m_forcedValueTimestamp = newState.getTimebStructure();
				m_forcedValueQualityStatus = inState.getStatus();
				m_forcedEngValue = inState.getValue();	
			}
        }
        
        FUNCTION_EXIT;
        return result;
    }

    void DataPoint::SetIsForcedState (const  std::string& sessionID, bool indication )
	{
		FUNCTION_ENTRY("SetIsForcedState");

		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw ( TA_Base_Core::OperationModeException() );
		}

		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );


		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
		NameValuePair pair1 ( "OnOff", ( true == indication ) ? "ON" : "OFF" );
		descriptionParameters.push_back ( &pair1 );

		// if the requested action is authorised for the sessionID
		if ( true == isActionPermitted ( sessionID, ( true == indication ? TA_Base_Bus::aca_DATA_POINT_SET_FORCED_STATE : TA_Base_Bus::aca_DATA_POINT_RESET_FORCED_STATE ) ) )
		{
			// only proceed if states are different
			if ( m_forcedState == indication )
			{
				return;
			}
			
			// if failed to persist new state to database
			if ( false == ( m_persistence->writePersistedValue( DP_MANUALLY_DRESSED, m_entityKey, indication, m_fieldValueQualityStatus ) ) )
			{
				// log auditing event
				submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::ForcedStatePersistenceFailed );

				throw TA_Base_Bus::ScadaCorbaTypes::PersistenceFailed();
			}
			// if successful
			else
			{
				// modify data point's forced state
				setForcedState ( indication );

				// if the data point is not forced any more, check the alarm state
				// just in case if we have not received a state update from the
				// field device
                m_previousAlarmState = m_alarmState;

				if ( false == m_forcedState )
				{
					struct timeb currentSystemTime;
					ftime ( &currentSystemTime );

					DataPointState engState( getCurrentEngValue(),
											 currentSystemTime,
											 getStatus() );

					updateAlarms( engState );
				}

                if (m_previousAlarmState != m_alarmState)
                {
				    // notify client of the new value and alarm state
                    boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmupdate(new DpDynamicSyncUpdate);
                    alarmupdate->updateType = TA_Base_Bus::AlarmStateUpdate;
                    alarmupdate->alarmState = m_alarmState;
                    //send out the update to inform the observer
                    submitUpdate ( alarmupdate );
                }
                
				// log auditing event
				submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::ForcedStateSucceeded );

				// save the session id of the operator who initiated this operation
				struct timeb currentSystemTime;
				ftime ( &currentSystemTime );
				saveLastOperatorInfo ( sessionID, currentSystemTime );
				// notify client of the new value 
                boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpupdate(new DpDynamicSyncUpdate);
                dpupdate->updateType = TA_Base_Bus::ValueUpdate;
                dpupdate->value = getCurrentEngValue().getDataPointValue();
                dpupdate->value.quality = getQualityStatus();
                dpupdate->value.timeStamp = getTimebStructure();
                //send out the update to inform the observer
                submitUpdate ( dpupdate );
			}
		}
		else
		{
			// log auditing event
			submitAuditMessage( sessionID, descriptionParameters, DataPointAudit::ForcedStatePermissionDenied );

			throw( TA_Base_Core::AccessDeniedException() );
		}

		FUNCTION_EXIT;
	}

	void DataPoint::setForcedStateWithValue( const std::string& sessionId, const TA_Base_Bus::DataPointValue & value )
	{
		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw( TA_Base_Core::OperationModeException() );
		}

		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );

		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();

		// if the requested action is authorised for the sessionID
		if ( true == isActionPermitted ( sessionId, TA_Base_Bus::aca_DATA_POINT_SET_FORCED_STATE ) )
		{
			// if not already in manual override state
			if ( true != m_forcedState )
			{
				// if failed to persist new state to database
				if ( false == ( m_persistence->writePersistedValue( DP_MANUALLY_DRESSED, m_entityKey, true, m_fieldValueQualityStatus ) ) )
				{
					// log auditing event
					submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ForcedStatePersistenceFailed );

					throw ScadaCorbaTypes::PersistenceFailed();
				}
				// if successful
				else
				{
					// enable data point's override state
					setForcedState ( true );
					//TD15781 ForcedStateSucceeded event should be removed.
		            //NameValuePair pair1 ( "OnOff", ( true == m_forcedState ) ? "ON" : "OFF" );
		            //descriptionParameters.push_back ( &pair1 );
					// log auditing event
					//submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ForcedStateSucceeded );
					//TD15781
				}
			}

			// save the manually entered value
			writeDataPoint ( sessionId, value );

			// save the session id of the operator who initiated this operation
			struct timeb currentSystemTime;
			ftime ( &currentSystemTime );
			saveLastOperatorInfo ( sessionId, currentSystemTime );
		}
		else
		{
			// log auditing event
			submitAuditMessage( sessionId, descriptionParameters, DataPointAudit::ForcedStatePermissionDenied );

			throw( TA_Base_Core::AccessDeniedException() );
		}
        
	}
    
	void DataPoint::setForcedState( bool newState )
	{
		m_forcedState = newState;

		// if new state is indicating a forced state
		if ( true == newState )
		{
			m_forcedValueQualityStatus = QUALITY_GOOD_LOCAL_OVERRIDE;
        }
        else
        {
		    // default status
		    m_forcedValueQualityStatus = QUALITY_BAD_LAST_KNOWN_VALUE;
        }
	}


	void DataPoint::setControlState ( TA_Base_Bus::EDataPointControlState controlStatus, bool isRequestedBySystem ) //limin++, SystemSession should not access db
	{
        FUNCTION_ENTRY("setControlState");
        TA_Base_Bus::DataPointControl toupdate;
        //only got new control status
        toupdate.controlState = controlStatus;
        //other things remain the same
        toupdate.lccResult = m_controlState.lccResult;
        toupdate.rccResult = m_controlState.rccResult;
        toupdate.triggerringDpKey = m_controlState.triggerringDpKey;
        toupdate.triggerringDpValue = m_controlState.triggerringDpValue;
        setControlState( toupdate, isRequestedBySystem );//limin++, SystemSession should not access db
        FUNCTION_EXIT;
	}
    
	void DataPoint::setControlState ( const TA_Base_Bus::DataPointControl& controlStatus, bool isRequestedBySystem ) //limin++, SystemSession should not access db
	{
        FUNCTION_ENTRY("setControlState");
		// only updates if control status is different
		if ( m_controlState.controlState != controlStatus.controlState || 
			 m_controlState.triggerringDpKey != controlStatus.triggerringDpKey ||
			 m_controlState.triggerringDpValue != controlStatus.triggerringDpValue)
		{
			// update status
			m_controlState.controlState = controlStatus.controlState;
			m_controlState.triggerringDpKey = controlStatus.triggerringDpKey;
			m_controlState.triggerringDpValue = controlStatus.triggerringDpValue;

			std::string controlText ( "" );

            // initialise control command state
            EControlCommandState controlCommandState = m_controlCommandState;

			// only persist new state to database and then generate notification message
			// if the data point is in control mode
			if ( TA_Base_Core::Control == m_operationMode )
			{
				// reset ABE result
				m_answerBackErrorCheckingResult = false;

                switch( m_controlState.controlState )
                {
                case TA_Base_Bus::noControl:
                    controlText = "noControl";
                    break;
                    
                case TA_Base_Bus::written:
                    controlText = "written";
                    controlCommandState = JustReceived;
                    break;
                case TA_Base_Bus::pending:
                    controlText = "pending";
                    controlCommandState = BeingExecuted;
                    break;

                case TA_Base_Bus::conditionalSuccess:
                    controlText = "conditionalSuccess";
                    controlCommandState = CompletedWithSuccess;

                    // set the ABE result
					m_answerBackErrorCheckingResult = true;
                    break;
                    
                case TA_Base_Bus::success:
                    controlText = "success";
                    controlCommandState = CompletedWithSuccess;

					// set the ABE result
					m_answerBackErrorCheckingResult = true;
                    break;

                case TA_Base_Bus::LCC_failed:
                    controlText = "LCC_failed";
                    controlCommandState = CompletedWithError;
                    break;

                case TA_Base_Bus::RCC_failed:
                    controlText = "RCC_failed";
                    controlCommandState = CompletedWithError;
                    break;

                case TA_Base_Bus::comms_failed:
                    controlText = "comms_failed";
                    controlCommandState = CompletedWithError;
                    break;

                case TA_Base_Bus::failed:
                    controlText = "failed";
                    controlCommandState = CompletedWithError;
                    break;

				case TA_Base_Bus::selectCmdWritten:
					controlText = "selectCmdWritten";
					controlCommandState = BeingExecuted;
					break;

				case TA_Base_Bus::selectCmdConfirm:
					controlText = "selectCmdConfirm";
					controlCommandState = BeingExecuted;
					break;

				case TA_Base_Bus::selectCmdConfirmTimeout:
					controlText = "selectCmdConfirmTimeout";
					controlCommandState = CompletedWithError;
					break;

				case TA_Base_Bus::operateCmdTimeout:
					controlText = "operateCmdTimeout";
					controlCommandState = CompletedWithError;
					break;

				case TA_Base_Bus::execCmdWritten:
					controlText = "execCmdWritten";
					controlCommandState = BeingExecuted;
					break;

				case TA_Base_Bus::execCmdConfirm:
					controlText = "execCmdConfirm";
					controlCommandState = BeingExecuted;
					break;

				case TA_Base_Bus::execCmdConfirmTimeout:
					controlText = "execCmdConfirmTimeout";
					controlCommandState = CompletedWithError;
					break;

				case TA_Base_Bus::execCmdSuccess:
					controlText = "execCmdSuccess";
					controlCommandState = BeingExecuted;
					break;

				case TA_Base_Bus::execCmdSuccessTimeout:
					controlText = "execCmdSuccessTimeout";
					controlCommandState = CompletedWithError;
					break;

				case TA_Base_Bus::cancelCmdWritten:
					controlText = "cancelCmdWritten";
					controlCommandState = BeingExecuted;
					break;

				case TA_Base_Bus::cancelCmdConfirm:
					controlText = "cancelCmdConfirm";
					controlCommandState = CompletedWithSuccess;
					break;

				case TA_Base_Bus::cancelCmdConfirmTimeout:
					controlText = "cancelCmdConfirmTimeout";
					controlCommandState = CompletedWithError;
					break;
				case TA_Base_Bus::selectCmdCfmNegative:
					controlText = "selectCmdCfmNegative";
					controlCommandState = CompletedWithError;
					break;
				case TA_Base_Bus::execCmdCfmNegative:
					controlText = "execCmdCfmNegative";
					controlCommandState = CompletedWithError;
					break;
				case TA_Base_Bus::cancelCmdCfmNegative:
					controlText = "cancelCmdCfmNegative";
					controlCommandState = CompletedWithError;
					break;
				case TA_Base_Bus::deviceControlException:
					controlText = "deviceControlException";
					controlCommandState = CompletedWithError;
					break;


                default:
                    TA_ASSERT(false, "No control states other than enum values can be set.");
					break;
				}

                if ( false == isRequestedBySystem ) //limin++, SystemSession should not access db
                {
                    PersistRequest * persistReq = new PersistRequest( DP_CONTROL_STATUS, m_entityKey, controlText, m_fieldValueQualityStatus );
                    m_persistQ->insert( persistReq );

                    //TD16017, save the control status timestamp into last operator info
                    struct timeb currentSystemTime;
                    ftime ( &currentSystemTime );
                    saveLastOperatorInfo(m_lastOperatorSessionID, currentSystemTime ); //limin++, SystemSession should not access db
                }

				// submit an update event to indicate the data point's control status has changed
				boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails(new DpDynamicSyncUpdate);
                updateDetails->updateType = TA_Base_Bus::ControlUpdate;
                updateDetails->controlState = m_controlState;
				submitUpdate ( updateDetails);

                // if control command state changed, synchronize it to monitor mode datapoint
                if (controlCommandState != m_controlCommandState)
                {
                    m_controlCommandState = controlCommandState;
                    
                    DataPointStateUpdate dpStateUpdate;

                    dpStateUpdate.commandId = m_commandId;

                    dpStateUpdate.controlCommandState = m_controlCommandState;
                    dpStateUpdate.controlState = m_controlState.controlState;
                    dpStateUpdate.dpKey = getEntityKey();
                    dpStateUpdate.dpValue = *getFieldValue();
                    dpStateUpdate.sessionId = m_controlSessionId;
                    dpStateUpdate.auditEventLoggingEnabled = m_auditEventLoggingEnabled;
                    dpStateUpdate.triggeringDpKey = m_controlState.triggerringDpKey;
                    dpStateUpdate.triggeringDpValue = m_controlState.triggerringDpValue;
                    
                    m_commandId = m_commandProcessor.sendDataPointStateUpdate(dpStateUpdate, m_commandId == 0);

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DataPoint [%s] control state update is sent to monitor mode with commandId [%d]", m_name.c_str(), m_commandId );

                    // if the command if completed, reset m_commandId to 0
                    if (m_controlCommandState == CompletedWithError || m_controlCommandState == CompletedWithSuccess)
                    {
                        m_commandId = 0;
                    }
                }
			}
		}
	
        FUNCTION_EXIT;
	}

    void DataPoint::updateControlState( TA_Base_Bus::EDataPointControlState controlStatus, bool isRequestedBySystem /* =false */ ) //limin++, SystemSession should not access db
	{

        TA_Base_Bus::DataPointControl toupdated;
        toupdated.controlState = controlStatus;
        toupdated.lccResult = m_controlState.lccResult;
        toupdated.rccResult = m_controlState.rccResult;
        toupdated.triggerringDpKey = m_controlState.triggerringDpKey;
        toupdated.triggerringDpValue = m_controlState.triggerringDpValue;
		setControlState( toupdated, isRequestedBySystem );//limin++, SystemSession should not access db
	}

    void DataPoint::setDataPointServiceState( TA_Base_Bus::EDataPointServiceState newState )
	{
		m_serviceState = newState;
	}

    void DataPoint::addAssociation( std::string & entName, std::string & preset )
	{
		m_associations.push_back( new EntityAssociation( entName, preset ) );
	}

	void DataPoint::clearAssociations()
	{
		DpAssociationList::iterator assocIter = m_associations.begin();
		for( ; assocIter != m_associations.end(); ++assocIter )
		{
			delete ( * assocIter );
			*assocIter = NULL;
		}
		m_associations.clear();
	}


	void DataPoint::submitAuditMessage( const std::string & sessionID,
										TA_Base_Core::DescriptionParameters & description,
										const TA_Base_Core::MessageType & messageType,
										unsigned long pEntityKey)
	{
        FUNCTION_ENTRY("submitAuditMessage");

        //limin++, SystemSession should not access db
        if ( true == isSystemSessionID( sessionID ) )
        {
            FUNCTION_EXIT;
            return;
        }
        //++limin, SystemSession should not access db

		//
		// As per changes in TD# 972, this method will also submit the audit message
		// immediately.
		//

		// only if the data point is in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			// get handle to AuditMessageSender for the audit message type DataPointAudit::DataPointInternalStateChanged
			// if not already done so
			if ( 0 == m_auditMessageSender )
			{
				m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender ( TA_Base_Core::DataPointAudit::Context );
			}

			// if AuditMessageSender is initialised
			if ( 0 != m_auditMessageSender )
			{
				try
				{
					// get the handle to an instance of the EventDetailsFactory if not already done so
					if ( 0 == m_eventDetailsFactory )
					{
						m_eventDetailsFactory = EventDetailsFactory::getInstance();
					}

					unsigned long entityKey = pEntityKey == 0 ? m_entityKey : pEntityKey;
					
					const TA_Base_Core::DataPointEventDetails& eventDetails = 
						m_eventDetailsFactory->getEventDetails(entityKey, m_dataPointEntityAccess->getAgentName());

					NameValuePair pair1 ( "EventMessage", eventDetails.eventMessage );
					description.push_back ( &pair1 );

					struct timeb currentSystemTime;
					ftime ( &currentSystemTime );
					m_auditMessageSender->sendAuditMessageWithAsset ( messageType,	// message type
																	  entityKey,	// entity key
																	  description,	// description
																	  "",			// details
																	  sessionID,	// session ID
																	  "",			// alarm ID
																	  "",			// incidentkey
																	  "",			// parent event ID
																	  eventDetails.assetName, // asset name
																	  NULL,			// filterable Data
																	  currentSystemTime );
				}
				//TD12418
				catch(TA_Base_Core::TransactiveException & te)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
				}
				catch ( ... )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught unknown exception while call sendAuditMessageWithAsset.");
				}
			}
			// do nothing if AuditMessageSender is not initialised
		}
        FUNCTION_EXIT;
	}

	void DataPoint::submitAuditMessage( const DpValue& dpValue,
										const std::string & sessionID,
										TA_Base_Core::DescriptionParameters & description,
										const TA_Base_Core::MessageType & messageType)
	{
        FUNCTION_ENTRY("submitAuditMessage");

		// only if the data point is in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			NameValuePair nvpDpValue ( "DataPointValue", dpValue.getValueAsString());
			description.push_back ( &nvpDpValue );

			submitAuditMessage(sessionID, description, messageType);
		}

        FUNCTION_EXIT;
	}

	void DataPoint::submitAuditMessage( const TA_Base_Bus::DataPointControl& controlState,
										const std::string & sessionID,
										TA_Base_Core::DescriptionParameters & description,
										const TA_Base_Core::MessageType & messageType) 
	{
        FUNCTION_ENTRY("submitAuditMessage");

		// only if the data point is in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			NameValuePair nvpDpValue ( "DataPointValue", controlState.triggerringDpValue);
			description.push_back ( &nvpDpValue );
			
			submitAuditMessage(sessionID, description, messageType, controlState.triggerringDpKey);
		}
        FUNCTION_EXIT;
	}


    void DataPoint::startABETimeout(const std::string& sessionID)
	{
        FUNCTION_ENTRY("startABETimeout");

		// if the ABE expression is not empty
		if ( false == m_abeExpression.empty() )
		{
			// if the ABE timeout clock has been started for the previous output
			// command
			if ( true == m_clockHasStarted )
			{
				// stop the previous clock
				m_singletonAnswerBackErrorUtil.stopABETimeoutClock ( this );
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "%s: ABE timeout clock is stopped", m_name.c_str() );
			}

			// set the flag
			//TD15103, should set the flag first, because it may be checked or changed during calling startABETimeoutClock()
			m_clockHasStarted = true;
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "%s: ABE timeout clock is to be started", m_name.c_str() );

			// start the ABE timeout clock
			m_singletonAnswerBackErrorUtil.startABETimeoutClock ( this,
																   m_abeTimeoutPeriodSeconds * 1000, // in mseconds
																   m_abeExpression,
																   sessionID);
		}
		else
		{
			// update control status of the data point to indicate that
			// the ouput command has (conditional) successfully been carried out
            updateControlState( TA_Base_Bus::conditionalSuccess, isSystemSessionID( sessionID ) ); //limin++, SystemSession should not access db

			//TD15705, log audit message if required
			if ( true == m_auditEventLoggingEnabled )
			{
				// construct the DescriptionParameters
				TA_Base_Core::DescriptionParameters descriptionParameters;
				descriptionParameters.clear();
				NameValuePair pair1 ( "ValueString", m_controlState.triggerringDpValue );
				descriptionParameters.push_back ( &pair1 );
				
				submitAuditMessage ( m_controlState, sessionID, descriptionParameters, TA_Base_Core::DataPointAudit::SetValueSucceeded );
			}
		}
        FUNCTION_EXIT;
	}

	void DataPoint::SetAbeExpressionValue(bool bValue)
	{
		if (m_pscadaFlag)
		{
			if (bValue)
			{
				m_abeExpression = m_trueRccExpression;
			}
			else
			{
				m_abeExpression = m_falseRccExpression;
			}
		}
	}

	void DataPoint::processPostEvaluationStep ( bool answerBackErrorCheckingResult )
	{
        FUNCTION_ENTRY("processPostEvaluationStep");
		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );

		//TD15103, because the caller is not thread safe function, so must check if necessary to continue.
		if (m_clockHasStarted == false)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "recalling of processPostEvaluationStep is ingored" );
			return;
		}

		// stop the clock and set the flag
		m_singletonAnswerBackErrorUtil.stopABETimeoutClock ( this );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "%s: ABE timeout clock is stopped", m_name.c_str() );
		
		m_clockHasStarted = false;

		if ( true == answerBackErrorCheckingResult )
		{
			// update control status of the data point to indicate that
			// the ouput command has successfully been carried out
			updateControlState( TA_Base_Bus::success, isSystemSessionID( m_singletonAnswerBackErrorUtil.getSessionID() ) );//limin++, SystemSession should not access db

			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Control Succeeded"  );

			// log audit message if required
			if ( true == m_auditEventLoggingEnabled )
			{
				// construct the DescriptionParameters
				TA_Base_Core::DescriptionParameters descriptionParameters;
				descriptionParameters.clear();
				NameValuePair pair1 ( "ValueString", m_controlState.triggerringDpValue );
				descriptionParameters.push_back ( &pair1 );

				submitAuditMessage ( m_controlState, m_singletonAnswerBackErrorUtil.getSessionID(), descriptionParameters, TA_Base_Core::DataPointAudit::SetValueSucceeded );
			}
		}
		else
		{
			// update control status of the data point to indicate that
			// the output command has failed because RCC has failed
			updateControlState( TA_Base_Bus::RCC_failed, isSystemSessionID( m_singletonAnswerBackErrorUtil.getSessionID() ) );//limin++, SystemSession should not access db
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Control Failed due to Bad Return Condition"  );

			if ( true == m_auditEventLoggingEnabled )
            {
                TA_Base_Core::DescriptionParameters descriptionParameters;
                descriptionParameters.clear();
                
                submitAuditMessage( m_controlState, m_singletonAnswerBackErrorUtil.getSessionID(), descriptionParameters, TA_Base_Core::DataPointAudit::AnswerBackCheckingResult );
            }
		}

        FUNCTION_EXIT
	}


	void DataPoint::processExpressionChanges()
	{
        FUNCTION_ENTRY("processExpressionChanges");
        ThreadGuard guard( m_processExpressionChangeslock );
		//
		// the calculated expression condition has changed due to online update
		// of input data points
		//

		// only proceed if handle to CalculatedExpression is defined
		if ( 0 != m_calculatedExpressionHandle )
		{
			TA_Base_Bus::DpValue dpValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
			EQualityStatus newStatus = QUALITY_BAD_NOT_CONNECTED;

			try
			{
				switch ( m_dataType )
				{
				case TA_Base_Bus::DPT_INT16_TYPE:
				case TA_Base_Bus::DPT_INT32_TYPE:
				case TA_Base_Bus::DPT_INT16S_TYPE:
				case TA_Base_Bus::DPT_INT32S_TYPE:
				case TA_Base_Bus::DPT_UINT16_TYPE:
				case TA_Base_Bus::DPT_UINT32_TYPE:
				case TA_Base_Bus::DPT_IEEE32_TYPE:
					{
						// get the result from the real number expression
						double expressionResult = DBL_MIN;
						m_calculatedExpressionHandle->evaluateRealNumberExpression ( expressionResult, newStatus );
						dpValue.setFloat ( expressionResult, getPrecision() );
					}
					break;
				case TA_Base_Bus::DPT_ENUM_TYPE:
					{
						// get the result from the real number expression
						double expressionResult = 0;
						m_calculatedExpressionHandle->evaluateRealNumberExpression ( expressionResult, newStatus );	

						// Cast to unsigned long
						if ( expressionResult >= 0 )
						{
							unsigned long enumResult = static_cast<unsigned long>(expressionResult);
							dpValue.setEnum( enumResult);
						}
						else
						{
							newStatus = QUALITY_BAD_NOT_CONNECTED;
						}
					}
					break;
				case TA_Base_Bus::DPT_BOOL_TYPE:
					{
						// get the result from the boolean number expression
						bool expressionResult = false;
						m_calculatedExpressionHandle->evaluateBooleanExpression ( expressionResult, newStatus );
						dpValue.setBoolean ( expressionResult );
					}
					break;
				case TA_Base_Bus::DPT_TEXT_TYPE:
				default:
					break;
				}
			}
			catch ( TA_Base_Core::MathematicalEvaluationException & mee)
			{
				// do nothing
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoint [%s] caught an exception [%s].", m_name.c_str(), mee.what());
            }
			catch ( ... )
			{
				// do nothing
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoint [%s] caught an unknown exception.", m_name.c_str());
			}

			// update this data point with the calculated result
			struct timeb currentSystemTime;
			ftime ( &currentSystemTime );
			TA_Base_Bus::DataPointState newState ( dpValue, currentSystemTime, newStatus );
			updateFieldState( newState );
		}
		// do nothing
        FUNCTION_EXIT;
	}

    void DataPoint::update ( const ConfigUpdateDetails & updateEvent )
	{
        //config update is only available internally, the observer will not be notified
        FUNCTION_ENTRY("update");
		// prevent multi thread accessing the data point while its configuration
		// is updated
		ThreadGuard guard( m_lock );

		// get the list of changed parameter from the config update
		const std::vector< std::string > * configChanges = 0;
		configChanges = &updateEvent.getChangedParams();

		// if the list is not empty
		if ( ( 0 != configChanges ) && ( false == configChanges->empty() ) )
		{
			// Place DataPoint out of service and lock it to prevent it accepting
			// write requests
			EDataPointServiceState originalServiceState = getServiceState();
			setDataPointServiceState ( OUT_OF_SERVICE );

			// invalidate the ScadaPersistence and DataPointEntityData access so that latest values are
			// retrieved from the database
            m_persistence->resetPersistedState( m_entityKey );
			m_dataPointEntityAccess->invalidate();

			//
			// update the keys
			//

			m_subsystemKey = m_dataPointEntityAccess->getSubsystem();
			m_locationKey = m_dataPointEntityAccess->getLocation();
			m_name = m_dataPointEntityAccess->getName();

			// for each of the changed parameters
			for ( unsigned int i = 0; i < configChanges->size(); i++ )
			{
				std::string paramName = (*configChanges)[i];

				//
				// check for common attributes first
				//

				if ( paramName == ENT_CFG_DESCRIPTION )
				{
					// Description
					setDescription ( m_dataPointEntityAccess->getDescription() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_SCAN_TIME_FACTOR )
				{
					// Scan Time Factor
					setScanFactor( m_dataPointEntityAccess->getScanTimeFactor() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_SERVICE_STATE )
				{
					// InService state
                    std::string tempString;
                    m_persistence->readPersistedValue( DP_SERVICE_STATE, m_entityKey, tempString );
					originalServiceState = ( tempString == IN_SERVICE_STRING ? IN_SERVICE : OUT_OF_SERVICE );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ALLOW_WRITE )
				{
					// Writable
					setWritable( m_dataPointEntityAccess->getAllowWrite() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENTITY_ASSOCIATION )
				{
					// Entity Associations
					DataPointFactory::parseEntityAssociations( this, m_dataPointEntityAccess->getEntityAssociation() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_VIEW_ASSOCIATION )
				{
					// View Associations
					setViewAssociation( m_dataPointEntityAccess->getViewAssociation() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENABLE_TRENDING )
				{
					// Trending
					setTrending( m_dataPointEntityAccess->getEnableTrending() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENABLE_ENGINEERING_NOTE )
				{
					// Engineering Note Enabled
					//
					// Only interested if this disables an existing
					// eng note
					//
					if ( false == m_dataPointEntityAccess->getEnableEngineeringNote() )
					{
						setEngNote( "" );
					}
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENGINEERING_NOTE )
				{
					// Engineering Note
					setEngNote( m_dataPointEntityAccess->getEngineeringNote() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ENABLE_HELP_URL )
				{
					// Help URL Enabled
					//
					// Only interested if this disables an existing
					// help URL
					//
					if ( false == m_dataPointEntityAccess->getEnableHelpURL() )
					{
						setHelpURL( "" );
					}
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_HELP_URL )
				{
					// Help URL
					setHelpURL( m_dataPointEntityAccess->getHelpURL() );
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_DISPLAY_ORDER )
				{
					// update display order
					m_displayOrder = m_dataPointEntityAccess->getDisplayOrder();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_PRECISION )
				{
					if ( ( m_dataPointEntityAccess->getPrecision() == 0 ) && ( m_dataType == TA_Base_Bus::DPT_IEEE32_TYPE ) )
					{
						// default to 2 decimal point if precision has not been setup for
						// floating point datapoint
						setPrecision( 2 );
					}
					else
					{
						setPrecision ( m_dataPointEntityAccess->getPrecision() );
					}
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_PRIORITY )
				{
					// update priority level
					m_updatePriorityLevel = m_dataPointEntityAccess->getUpdatePriorityLevel();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_CALCULATED_EXPRESSION )
				{
					// only proceed if there is difference
					if ( 0 != m_calculatedExpression.compare ( m_dataPointEntityAccess->getCalculatedExpression() ) )
					{
						// update calculated expression
						m_calculatedExpression = m_dataPointEntityAccess->getCalculatedExpression();

						// delete the existing instance of CalculatedExpression
						delete m_calculatedExpressionHandle;
						m_calculatedExpressionHandle = 0;

						// the expression is valid
						if ( false == m_calculatedExpression.empty() )
						{
							// create handle to CalculatedExpression instance for new expression
							m_calculatedExpressionHandle = new TA_Base_Bus::CalculatedExpression ( this, m_calculatedExpression );
							m_calculatedExpressionHandle->enableCallback ( true );

							// attempt to make first evaluation of the new calculated expression
							processExpressionChanges();
						}
					}
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LAUNCH_CONDITION )
				{
					// only proceed if there is difference
					if ( 0 != m_commandLaunchingCondition.compare ( m_dataPointEntityAccess->getLaunchExpression() ) )
					{
						// update launching expression
						m_commandLaunchingCondition = m_dataPointEntityAccess->getLaunchExpression();

						// delete the existing instance of LaunchingConditionCheck
						delete m_launchingConditionCheck;
						m_launchingConditionCheck = 0;

						// the expression is valid
						if ( false == m_commandLaunchingCondition.empty() )
						{
							// create handle to LaunchingConditionCheck instance
							m_launchingConditionCheck = new TA_Base_Bus::LaunchingConditionCheck ( m_commandLaunchingCondition );

							// kick start the proxies created by the LaunchingConditionCheck
							m_launchingConditionCheck->evaluateLaunchingExpression();
						}
						// do nothing if the expression is empty
					}
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ABE_EXPRESSION )
				{
					// update ABE expression
					m_abeExpression = m_dataPointEntityAccess->getABEExpression();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_ABE_TIMEOUT_PERIOD_SECONDS )
				{
					// update ABE timeout
					m_abeTimeoutPeriodSeconds = m_dataPointEntityAccess->getABETimeoutPeriodSeconds();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_LOG_CHANGES )
				{
					// update LogChanges
					m_logChanges = m_dataPointEntityAccess->getLogChanges();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_WORD_SWAP )
				{
					// update LogChanges
					m_wordSwap = m_dataPointEntityAccess->getWordSwapIndication();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_BYTE_SWAP )
				{
					// update LogChanges
					m_byteSwap = m_dataPointEntityAccess->getByteSwapIndication();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_TYPE_SHORT_NAME )
				{
					// update the short name of the datapoint type
					m_typeShortName = m_dataPointEntityAccess->getDataPointTypeShortName();
				}
				else if ( paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_OMFLAG )
				{
					// update the OMF flag of the datapoint type
					m_OMFlag = m_dataPointEntityAccess->getOMFlag();
				}
				else if (paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_BIT_WIDTH)
				{
					m_bitWidth = m_dataPointEntityAccess->getBitWidth();
				}
				else
				{
					// do nothing
				}
			}

			// update specific attributes of specific derived data point if required
			updateSpecificConfiguration ( configChanges );

			// restore original service state (or new one, if has been modified)
			setDataPointServiceState( originalServiceState );
		}
        FUNCTION_EXIT;
	}

    void DataPoint::WriteDataPointValue( const std::string& sessionId,
						const TA_Base_Bus::DataPointValue& value,
						const unsigned long triggeringEntityKey,const std::string& triggeringEntityValue )
	{
		FUNCTION_ENTRY("WriteDataPointValue");

		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw( TA_Base_Core::OperationModeException() );
		}

		try
		{
			writeDataPoint ( sessionId, value, true, triggeringEntityKey, triggeringEntityValue );
		}
		catch(TA_Base_Core::ScadaCommonException &)
		{
			throw( TA_Base_Core::BadParameterException() );
		}


		FUNCTION_EXIT;
	}


	void DataPoint::WriteDataPointValueWithoutLoggingEvent( const std::string& sessionId,
						const TA_Base_Bus::DataPointValue& value,
						const unsigned long triggeringEntityKey,const std::string& triggeringEntityValue )
	{
		FUNCTION_ENTRY("WriteDataPointValueWithoutLoggingEvent");

		if ( TA_Base_Core::Monitor == m_operationMode )
		{
			throw( TA_Base_Core::OperationModeException() );
		}

		try
		{
			writeDataPoint ( sessionId, value, false, triggeringEntityKey, triggeringEntityValue );

		}
		catch(TA_Base_Core::ScadaCommonException &)
		{
			throw( TA_Base_Core::BadParameterException() );
		}
		
		FUNCTION_EXIT;
	}

    void DataPoint::writeDataPoint( const std::string& sessionID,
									const TA_Base_Bus::DataPointValue & value,
									bool auditEventLoggingEnabled,
									unsigned long triggeringDpKey,
									const std::string triggeringDpValue )
	{
        FUNCTION_ENTRY("writeDataPoint");

        bool isSystemSession = isSystemSessionID( sessionID );

		//TD20356
		#define HEARTBEART_TRIGGER  0xfffffffe
		if(HEARTBEART_TRIGGER == triggeringDpKey)
		{
			try
			{
				ThreadGuard guard( m_lock );
			
				DpValue engVal(value, getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels(), getPrecision());
				DpValue rawVal(engVal);
				convertToRaw( engVal, rawVal );

				// then processed by the specific device Agent
				DataPointWriteRequest * req =
					new DataPointWriteRequest( this, rawVal, sessionID, TA_Base_Bus::aca_DATA_POINT_WRITE, m_commandId );
				
				TA_Base_Bus::DataPointWriteMap::getInstance()->insert(m_entityKey, req);

			}
			catch(TA_Base_Core::ScadaCommonException &)
			{
				throw( TA_Base_Core::BadParameterException() );
			}

			return;
		}
		// prevent multi threads attempting to write at the same time
		ThreadGuard guard( m_lock );

		bool deviceRequestToBeSubmitted = false;
		bool forcedStateToBeUpdated = false;
		bool fieldStateToBeUpdated = false;
		DpValue engVal(value, getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels(), getPrecision());
		DpValue rawVal(engVal);
		
		//unsigned long trigEntity = triggeringDpKey != 0 ? triggeringDpKey : m_entityKey;
		m_auditEventLoggingEnabled = auditEventLoggingEnabled;
        m_controlSessionId = sessionID;

		//
		// Convert the requested value into the raw value that must
		// be written to the device
		// 
        convertToRaw( engVal, rawVal );

		TA_Base_Bus::DataPointControl trigDp;

		if (triggeringDpKey != 0)
		{
			trigDp.triggerringDpKey = triggeringDpKey;
			trigDp.triggerringDpValue = triggeringDpValue;
		}
		else
		{
			trigDp.triggerringDpKey = m_entityKey;
			trigDp.triggerringDpValue = engVal.getValueAsString();
		}
		
		// construct the DescriptionParameters for auditing messages
		TA_Base_Core::DescriptionParameters descriptionParameters;		
		descriptionParameters.clear();
        NameValuePair pair1 ( "ValueString", trigDp.triggerringDpValue ); 
		descriptionParameters.push_back ( &pair1 );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataPoint [%s] received write request, value [%s], session_id [%s]", m_name.c_str(), rawVal.getValueAsString().c_str(), sessionID.c_str() );
        
        //
		// authentication
		//
		bool actionPermitted = false;
		if ( true == m_forcedState )
		{
			//
			// double check to ensure the specified sessionID is authorised to set forced state, rather
			// than defaulted to true, just in case that someone might catch the AccessDeniedException
			// thrown by the SetIsForcedState operation and continue on modifying the dp state...
			//
			actionPermitted = isActionPermitted ( sessionID, TA_Base_Bus::aca_DATA_POINT_SET_FORCED_STATE );
		}
		else
		{
			//
			// check if the specified sessionID is authorised to modify the dp state
			//
			actionPermitted = isActionPermitted ( sessionID, TA_Base_Bus::aca_DATA_POINT_WRITE );
		}

		// if the specified sessionID is authorised for the requested action
		if ( true == actionPermitted )
		{
			// if the data point is in forced state, i.e the data point is being
			// manually dressed
			if ( true == m_forcedState )
			{
				// Do type-specific check on request validity
				if ( isValidWrite( engVal ) )
				{
					// update the data point's forced value
					forcedStateToBeUpdated = true;
				}
				else
				{
                    FUNCTION_EXIT;
					throw TA_Base_Core::BadParameterException( "Invalid value for write request" );
				}
			}

			// if the data point is not in forced state
			else
			{
				// if the data point is a virtual data point
				if ( true == m_virtual )
				{
					// Do type-specific check on request validity
					if ( isValidWrite( engVal ) )
					{
						// update the data point's field value
						fieldStateToBeUpdated = true;
					}
					else
					{
                        FUNCTION_EXIT;
						throw TA_Base_Core::BadParameterException( "Invalid value for write request" );
					}
				}

				// if the data point is a real data point, the data point
				// must be writable
				else if ( ( false == m_virtual ) && ( true == m_writable ) )
				{
					// only proceed if output control is not inhibited
					if ( false == m_ControlInhibitedState )
					{
						// Do type-specific check on request validity
						if ( isValidWrite( engVal ) )
						{
							// set the flag
							deviceRequestToBeSubmitted = true;
						}
						else
						{
                            FUNCTION_EXIT;
							throw TA_Base_Core::BadParameterException( "Invalid value for write request" );
						}
					}
					else
					{
						if ( true == auditEventLoggingEnabled )
						{
							// log audit message
							submitAuditMessage ( trigDp, sessionID, descriptionParameters, TA_Base_Core::DataPointAudit::SetValueFailedControlInhibited );
						}
                        FUNCTION_EXIT;
					    throw TA_Base_Core::BadParameterException("Not allow to write");
					}
				}
				else
				{
                    FUNCTION_EXIT;
					throw TA_Base_Core::BadParameterException("Not allow to write");
				}
			}
		}
		// if the specified sessionID is NOT authorised for the requested action
		else
		{
			if ( true == auditEventLoggingEnabled )
			{
				// log audit message
				if ( true == m_forcedState )
				{
					submitAuditMessage ( trigDp, sessionID, descriptionParameters, TA_Base_Core::DataPointAudit::SetForcedValueFailedPermissionDenied );
				}
				else
				{
					submitAuditMessage ( trigDp, sessionID, descriptionParameters, TA_Base_Core::DataPointAudit::SetValueFailedPermissionDenied );
				}
			}
            FUNCTION_EXIT;
			throw TA_Base_Core::AccessDeniedException();
		}

		//
		// updating the dp state with the specified value
		//

		struct timeb currentSystemTime;
		ftime ( &currentSystemTime );

		if ( true == fieldStateToBeUpdated )
		{
			// InitialiseControlStatus for vitual writable datapoint 
			if ( ( true == m_virtual ) && ( true == m_writable ) )
			{
                trigDp.controlState = TA_Base_Bus::noControl;
                setControlState ( trigDp, isSystemSession );//limin++, SystemSession should not access db
			}

			DataPointState newState( rawVal, currentSystemTime, QUALITY_GOOD_NO_SPECIFIC_REASON );

			updateFieldState ( newState );


			// Update the control state for vitual writable datapoint (TD15945)
			if ( ( true == m_virtual ) && ( true == m_writable ) )
			{
				updateControlState( TA_Base_Bus::conditionalSuccess, isSystemSessionID( sessionID ) );//limin++, SystemSession should not access db
			}

			if ( true == auditEventLoggingEnabled )
			{
				// log audit message
				submitAuditMessage ( trigDp, sessionID, descriptionParameters, TA_Base_Core::DataPointAudit::SetValueSucceeded );
			}
		}

		if ( true == forcedStateToBeUpdated )
		{
			DataPointState newState( rawVal, currentSystemTime, QUALITY_GOOD_LOCAL_OVERRIDE );

			if ( true == updateForcedState ( newState ) )
			{
				// else do nothing

				if ( true == auditEventLoggingEnabled )
				{
					// log audit message
					submitAuditMessage( trigDp, sessionID, descriptionParameters, DataPointAudit::SetForcedValueSucceeded );
				}
			}
			else
			{
				if ( true == auditEventLoggingEnabled )
				{
					// log audit message
					submitAuditMessage( trigDp, sessionID, descriptionParameters, DataPointAudit::SetForcedValueFailedPersistence );
				}
			}
		}
		
		if ( true == deviceRequestToBeSubmitted )
		{
			//the data point in PSCADA should execute a different flow.
			if (getPScadaFlag())
			{
				TA_Base_Bus::EControlOperation controlOperation = rawVal.getControlOperation();
				if (false == pscadaOperationCheck(controlOperation))
				{
					FUNCTION_EXIT;
					throw TA_Base_Core::BadParameterException( "control operation error" );
				}

				// make the field state of the output datapoint the same as the value
				// being sent to the device.
				DataPointState newState( rawVal, currentSystemTime, QUALITY_GOOD_NO_SPECIFIC_REASON );
				updateFieldState ( newState );

				bool bNeedLCCCheck = true;
				//Only Select command need LCC check
				if (controlOperation == CONTROL_SELECT)
				{					
					trigDp.controlState = TA_Base_Bus::noControl;
					setControlState ( trigDp, isSystemSession );
					bNeedLCCCheck = evaluateLaunchCondition();
				}
				// if interlocking/launching condition is correct
				if ( true == bNeedLCCCheck )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Passed the Launching Condition for %s, control operation is:%d", m_name.c_str(), controlOperation);

					// update control status
					//TD15649, set control state first
					switch(controlOperation)
					{
					case CONTROL_SELECT:
						trigDp.controlState = TA_Base_Bus::selectCmdWritten;
						break;
					case CONTROL_EXECUTE:
						trigDp.controlState = TA_Base_Bus::execCmdWritten;
					    break;
					case CONTROL_CANCEL:
						trigDp.controlState = TA_Base_Bus::cancelCmdWritten;
						break;
					default:
						trigDp.controlState = TA_Base_Bus::noControl;
					    break;
					}
					//this call will generate new command id
					setControlState ( trigDp, isSystemSession );//limin++, SystemSession should not access db

					// Form write request and put it onto the queue, which
					// then processed by the specific device Agent
					DataPointWriteRequest * req =
						new DataPointWriteRequest( this, rawVal, sessionID, TA_Base_Bus::aca_DATA_POINT_WRITE, m_commandId );

					m_commandProcessor.scheduleCommand(req);

					if ( true == isSystemSessionID( sessionID) ) //limin++, SystemSession should not access db
					{
						// save the current output request - format: [value]|[timestamp]
						std::stringstream currentRequest;
						currentRequest << engVal.getPersistString() << "|" << currentSystemTime.time;
						PersistRequest * persistReq = new PersistRequest( DP_CURRENT_CONTROL_REQUEST, m_entityKey, currentRequest.str(), m_fieldValueQualityStatus );
						m_persistQ->insert( persistReq );
					}
				}
				else
				{
					std::stringstream ss;
					ss << "Failed the Launching Condition for " << m_name;
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, ss.str().c_str() );


					if ( true == auditEventLoggingEnabled )
					{
						NameValuePair LCCResult( "LCCResult", m_controlState.lccResult );

						TA_Base_Core::DescriptionParameters descriptionParameters;
						descriptionParameters.push_back(&LCCResult);

						// Submit Audit Log
						submitAuditMessage( trigDp, sessionID, descriptionParameters, TA_Base_Core::DataPointAudit::SetValueLCCFailed );
					}

					// update control status of the data point to indicate that the
					// launching condition has failed and therefore the command cannot
					// be executed
					trigDp.controlState = TA_Base_Bus::LCC_failed;
					setControlState ( trigDp, isSystemSession );//limin++, SystemSession should not access db
				}
			}
			else
			{
				// make the field state of the output datapoint the same as the value
				// being sent to the device.
				DataPointState newState( rawVal, currentSystemTime, QUALITY_GOOD_NO_SPECIFIC_REASON );
				updateFieldState ( newState );

				// update control status
	            trigDp.controlState = TA_Base_Bus::noControl;
	            setControlState ( trigDp, isSystemSession ); //limin++, SystemSession should not access db

				// if interlocking/launching condition is correct
				if ( true == evaluateLaunchCondition() )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Passed the Launching Condition for %s", m_name.c_str() );

					// update control status
					//TD15649, set control state first
	                trigDp.controlState = TA_Base_Bus::written;
	                //this call will generate new command id
					setControlState ( trigDp, isSystemSession );//limin++, SystemSession should not access db
				
					// Form write request and put it onto the queue, which
					// then processed by the specific device Agent
					DataPointWriteRequest * req =
								  new DataPointWriteRequest( this, rawVal, sessionID, TA_Base_Bus::aca_DATA_POINT_WRITE, m_commandId );

	                m_commandProcessor.scheduleCommand(req);

	                if ( true == isSystemSessionID( sessionID) ) //limin++, SystemSession should not access db
	                {
	                    // save the current output request - format: [value]|[timestamp]
	                    std::stringstream currentRequest;
	                    currentRequest << engVal.getPersistString() << "|" << currentSystemTime.time;
						PersistRequest * persistReq = new PersistRequest( DP_CURRENT_CONTROL_REQUEST, m_entityKey, currentRequest.str(), m_fieldValueQualityStatus );
						m_persistQ->insert( persistReq );
					}
				}
				else
				{
					std::stringstream ss;
					ss << "Failed the Launching Condition for " << m_name;
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, ss.str().c_str() );


	                if ( true == auditEventLoggingEnabled )
	                {
	                    NameValuePair LCCResult( "LCCResult", m_controlState.lccResult );
                    
	                    TA_Base_Core::DescriptionParameters descriptionParameters;
	                    descriptionParameters.push_back(&LCCResult);
                    
	                    // Submit Audit Log
	                    submitAuditMessage( trigDp, sessionID, descriptionParameters, TA_Base_Core::DataPointAudit::SetValueLCCFailed );
	                }

					// update control status of the data point to indicate that the
					// launching condition has failed and therefore the command cannot
					// be executed
	                trigDp.controlState = TA_Base_Bus::LCC_failed;
					setControlState ( trigDp, isSystemSession );//limin++, SystemSession should not access db
				}
			}
		}
        FUNCTION_EXIT;
	}


	void DataPoint::setToControlMode()
	{
        FUNCTION_ENTRY("setToControlMode");
		// save the operation mode
		m_operationMode = TA_Base_Core::Control;

		// initialise data if not done so
		if ( m_lastSavedOperationMode != m_operationMode )
		{
            // only do further initialization once
            if ((m_lastSavedOperationMode != TA_Base_Core::Control) &&
                (m_lastSavedOperationMode != TA_Base_Core::Monitor))
            {
			    // schedule a work item in ThreadPool to do further initialisation on
			    // a different thread
			     //m_threadPool->queueWorkItem ( this );
				 this->executeWorkItem();
            }

            // force calculate expression ensure when came to control mode from monitor mode, 
            // the dp can show correct status
            if (0 != m_calculatedExpressionHandle)
            {
                processExpressionChanges();
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Force calculate expression for datapoint [%s]", m_name.c_str());
            }
			// peter low: force state value need to update alarm where change from monitor to control mode
			//else if (m_forcedState)
			//{
			//   m_threadPool->queueWorkItem ( this );
			//}

			// save current control mode for later usage
			m_lastSavedOperationMode = m_operationMode;

            // tell the alarm objects
            //bool inControl = true;

            {
                ThreadGuard guard( m_alarmslock );
                DpAlarmList::iterator alarmIter = m_alarms.begin();
                for( ; alarmIter != m_alarms.end(); ++alarmIter )
                {
                    (*alarmIter )->setOperationMode ( TA_Base_Core::Control );
                }
            }

            // TODO: This brings extra overhead during fail over
            // we should not send CompleteDynamicSyncStateUpdate to observers here
            // TD18751
            // if there are observers, need to notify them with complete dynamic infomation when change to control
            // to do this is because the local proxies will not be refreshed when change to control.
           
			// lym 2008-8-8 completed by synchonization

			//boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpDUpdate(new DpDynamicSyncUpdate);
            //dpDUpdate->updateType = TA_Base_Bus::CompleteDynamicSyncStateUpdate;

            //boost::shared_ptr<TA_Base_Bus::DpCompleteDynamicSyncState> completeState = getCompleteDynamicUpdate();
            //dpDUpdate->syncCompleteState = *(completeState.get());

            //notifyRegisteredObservers(dpDUpdate);
        }

        FUNCTION_EXIT;
	}


	void DataPoint::setToMonitorMode()
	{
        FUNCTION_ENTRY("setToMonitorMode");
		// save the operation mode
		m_operationMode = TA_Base_Core::Monitor;

        // if there is difference between last and current control mode
        if ( m_lastSavedOperationMode != m_operationMode )
        {
            //do not remove observers, they will call unregister to remove by themself
		    //{
		    //	// stop mutil thread modifing the list of observers
		    //	ThreadGuard guard( m_updateDetailsObserverlock );
            //
		    //	// clean up the list of dp observers
		    //	m_listOfUpdateDetailsObservers.clear();
		    //}

            // only do further initialization once
            if ((m_lastSavedOperationMode != TA_Base_Core::Control) &&
                (m_lastSavedOperationMode != TA_Base_Core::Monitor))
            {
			    // schedule a work item in ThreadPool to do further initialisation on
			    // a different thread
			     //m_threadPool->queueWorkItem ( this );
				 this->executeWorkItem();
            }

		    // save current control mode for later usage
		    m_lastSavedOperationMode = m_operationMode;

		    // tell the alarm objects
		    //bool inControl = false;
            
            {
	            ThreadGuard guard( m_alarmslock );
			    DpAlarmList::iterator alarmIter = m_alarms.begin();
			    for( ; alarmIter != m_alarms.end(); ++alarmIter )
			    {
				    (*alarmIter )->setOperationMode ( TA_Base_Core::Monitor );
			    }
            }
        }

        FUNCTION_EXIT;
	}


	void DataPoint::setToStandbyMode()
	{
		FUNCTION_ENTRY("setToMonitorMode");
		// save the operation mode
		m_operationMode = TA_Base_Core::Standby;

        // if there is difference between last and current control mode
        if ( m_lastSavedOperationMode != m_operationMode )
        {
            //do not remove observers, they will call unregister to remove by themself
		    //{
		    //	// stop mutil thread modifing the list of observers
		    //	ThreadGuard guard( m_updateDetailsObserverlock );
            //
		    //	// clean up the list of dp observers
		    //	m_listOfUpdateDetailsObservers.clear();
		    //}

            // only do further initialization once
            if ((m_lastSavedOperationMode != TA_Base_Core::Control) &&
                (m_lastSavedOperationMode != TA_Base_Core::Monitor))
            {
			    // schedule a work item in ThreadPool to do further initialisation on
			    // a different thread
			     m_threadPool->queueWorkItem ( this );
            }

		    // save current control mode for later usage
		    m_lastSavedOperationMode = m_operationMode;
            
            {
	            ThreadGuard guard( m_alarmslock );
			    DpAlarmList::iterator alarmIter = m_alarms.begin();
			    for( ; alarmIter != m_alarms.end(); ++alarmIter )
			    {
				    (*alarmIter )->setOperationMode ( TA_Base_Core::Standby );
			    }
            }
        }

        FUNCTION_EXIT;
	}

	void DataPoint::initAfterProxyFactoryReady()
	{
		// if this data point is an output data point
		if ( true == m_writable )
		{
			// get the current control request from database
			std::string currentControlRequest ( "" );
            m_persistence->readPersistedValue( DP_CURRENT_CONTROL_REQUEST, m_entityKey, currentControlRequest );

			// if not empty
			if ( false == currentControlRequest.empty() )
			{
				std::string lastOutputValue ( "" );
				unsigned long lastTimestamp = 0;
				TA_Base_Bus::DpValue value(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());

				//
				// get the output value from current control request string
				//

				std::string::size_type posValue = 0;
				posValue = currentControlRequest.find ( "|", posValue );
				if ( posValue > 0 )
				{
					lastOutputValue = currentControlRequest.substr ( 0, posValue );
				}

				//
				// get the timestamp of the output value from current control request string
				//

				lastTimestamp = (unsigned long) atol( ( currentControlRequest.substr ( posValue + 1, currentControlRequest.length() - ( posValue + 1 ) ) ).c_str() );

				//
				// update field state of this real output data point
				//

				DpValue dpValue(getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels());
				bool nextAction = true;

				// if this is an output boolean data point
				if ( true == getIsDigitalDataPoint() )
				{
					dpValue.setFromPersist ( lastOutputValue );
				}
				// if this is an output text data point
				else if ( true == getIsTextDataPoint() )
				{
					dpValue.setFromPersist ( lastOutputValue );
				}
				// if this is an output analogue data point
				else if ( true == getIsAnalogueDataPoint() )
				{
					dpValue.setFromPersist ( lastOutputValue, getPrecision() );
				}
				else
				{
					nextAction = false;
				}

				if ( true == nextAction )
				{
					TA_Base_Bus::DataPointState newState ( value, lastTimestamp , QUALITY_BAD_LAST_KNOWN_VALUE );
					updateDataPointState ( newState, true );
				}
			}
			// else do nothing
		}
		
		DpAlarmList::iterator alarmIter = m_alarms.begin();
		for( ; alarmIter != m_alarms.end(); ++alarmIter )
		{
			(*alarmIter )->setOperationMode ( TA_Base_Core::Monitor );
		}

	}
	
	bool DataPoint::isValid()
	{
		return true;
	}

	void DataPoint::start()
	{
		// do nothing
	}

	void DataPoint::stop()
	{
		// do nothing
	}

	void DataPoint::remove()
	{
        //do nothing
    }

	void DataPoint::lock()
	{
		// Don't use unless necessary
		m_lock.acquire();
	}

	void DataPoint::unlock()
	{
		// Don't use unless necessary
		m_lock.release();
	}

	void DataPoint::updateSpecificConfiguration ( const std::vector< std::string > * configChanges )
	{
		// do nothing.  Specific derived class shall implement its own version
	}

    
	void DataPoint::convertDataPointAddress ( const std::string & originDpAddress,
											  unsigned long & registerAddress,
											  int & bitAddress )
	{
        FUNCTION_ENTRY("convertDataPointAddress");
		bitAddress = 0;
		char * stopString = "\0";
		int base = 10;

		// check if the first 2 character of the address are "0x" or "0X"
		if ( ( "0x" == originDpAddress.substr ( 0, 2 ) ) ||
			 ( "0X" == originDpAddress.substr ( 0, 2 ) ) )
		{
			// hexadecimal based number
			base = 16;
		}
		else
		{
			// decimal based number
			base = 10;
		}

		// find character "." from data point address, indicating bit address within
		// the register
		std::string::size_type pos = 0;
		pos = originDpAddress.find ( ".", pos );

		// digital data point
		if ( 0 < pos )
		{
			// convert the register address
			registerAddress = strtoul ( (originDpAddress.substr( 0, pos )).c_str(), &stopString, base );

			// set the bit
			bitAddress = strtol ( originDpAddress.substr ( pos + 1, originDpAddress.length() ).c_str(), &stopString, base );
		}

		// non digital data point (analogue or text)
		else
		{
			// convert the register address
			registerAddress = strtoul ( (originDpAddress.substr( 0, pos )).c_str(), &stopString, base );
		}
        FUNCTION_EXIT;
	}

	bool DataPoint::isActionPermitted ( const std::string & sessionID, unsigned long actionKey )
	{
        FUNCTION_ENTRY("isActionPermitted");
		bool response = false;

		// action is only permitted when data point is running in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			if ( ( DPT_INTERNAL_SESSION_ID == sessionID ) || ( true == sessionID.empty() ) )
			{
				response = true;
			}
			else
			{
				// create an instance of ScadaUtilities if not done so
				if ( NULL == m_scadaUtilities )
				{
					m_scadaUtilities = &(TA_Base_Bus::ScadaUtilities::getInstance());
				}

				// get the response from the ScadaUtilities using physical subsystem key
				unsigned long physicalSubsystemKey = this->m_dataPointEntityAccess->getPhysicalSubsystem();
            	response = m_scadaUtilities->isActionAllowedWithLocSub (
                	actionKey, m_entityKey, m_locationKey, physicalSubsystemKey, sessionID );
                
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "isActionAllowedWithLocSub() returned: %s", ( true == response ) ? "TRUE" : "FALSE" );
			}
		}
        FUNCTION_EXIT;
		return response;
	}

    bool DataPoint::evaluateLaunchCondition()
	{
        FUNCTION_ENTRY("evaluateLaunchCondition");

        // return true if LaunchConditionCheck is not defined
		if ( 0 == m_launchingConditionCheck )
        {
            FUNCTION_EXIT;
            return true;
        }

		// get the result of the condition from the checker
        bool result = m_launchingConditionCheck->evaluateLaunchingExpression();

        // if failed
        if ( false == result )
        {
            // get the result string and save to database for further reference
            m_controlState.lccResult = m_launchingConditionCheck->getLaunchingConditionResultString();
			PersistRequest * persistReq = new PersistRequest( DP_LAUNCH_RESULT_STRING, m_entityKey, m_controlState.lccResult, m_fieldValueQualityStatus );
			m_persistQ->insert( persistReq );
		}

        FUNCTION_EXIT;
		return result;
	}

	void DataPoint::convertToEngUnits( const DataPointState & rawState,
									   DataPointState & engState )
	{
		// to be modified by specific data point
		engState = rawState;
	}


	void DataPoint::convertToRaw( const DpValue & engValue,
								  DpValue & rawValue )
	{
		rawValue = engValue;
	}

	bool DataPoint::isGoodValue() const
	{
        FUNCTION_ENTRY("isGoodValue");
		if ( true == m_forcedState )
		{
            FUNCTION_EXIT;
			return ( ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == m_forcedValueQualityStatus ) ||
			   ( QUALITY_GOOD_LOCAL_OVERRIDE == m_forcedValueQualityStatus ) ) ? true : false );
		}

        FUNCTION_EXIT;
        return ( ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == m_fieldValueQualityStatus ) ||
            ( QUALITY_GOOD_LOCAL_OVERRIDE == m_fieldValueQualityStatus ) ) ? true : false );
    }


	void DataPoint::initialiseData()
	{
        FUNCTION_ENTRY("initialiseData");

		// convert the data point address to register and bit address
		convertDataPointAddress ( m_address,
								  m_registerAddress,
								  m_bitAddress );

		// Initialise the timestamp to now
		m_fieldValueTimestamp = TA_Base_Core::AlarmConstants::defaultTime;
		m_forcedValueTimestamp = TA_Base_Core::AlarmConstants::defaultTime;
		m_lastOperatorTimestamp = TA_Base_Core::AlarmConstants::defaultTime;

		// if handle to CalculatedExpression is not initialised
		if ( 0 == m_calculatedExpressionHandle )
		{
			// if the calculated expression is not loaded, try to load it from database
			if ( 0 == m_calculatedExpression.compare ( NotLoaded ) )
			{
				m_calculatedExpression = m_dataPointEntityAccess->getCalculatedExpression();
			}

			// if calculated expression is defined
			if ( false == m_calculatedExpression.empty() )
			{
				// create handle to CalculatedExpression instance
				m_calculatedExpressionHandle = new TA_Base_Bus::CalculatedExpression ( this, m_calculatedExpression );
				m_calculatedExpressionHandle->enableCallback ( true );
			}
		}

		// if handle to LaunchingCondition expression is not initialised
		if ( 0 == m_launchingConditionCheck )
		{
			// if the launch expression is not loaded, try to load it from database
			if ( 0 == m_commandLaunchingCondition.compare ( NotLoaded ) )
			{
				m_commandLaunchingCondition = m_dataPointEntityAccess->getLaunchExpression();
			}

			// if there is a launching condition defined
			if ( false == m_commandLaunchingCondition.empty() )
			{
				// create handle to LaunchingConditionCheck instance
				m_launchingConditionCheck = new LaunchingConditionCheck ( m_commandLaunchingCondition );
			}
		}

		// Update abe expression from database
		m_abeExpression = m_dataPointEntityAccess->getABEExpression();
		m_trueRccExpression = m_dataPointEntityAccess->getTrueRccExpression();
		m_falseRccExpression = m_dataPointEntityAccess->getFalseRccExpression();
		if (!m_abeExpression.empty())
		{
			m_abeTimeoutPeriodSeconds = m_dataPointEntityAccess->getABETimeoutPeriodSeconds();
		}

		//TD15883, read the persisted value as default field value for virtual datapoint and output datapoint
		//the persisted value is eng value, so need to convert to raw value
		if ( 0 == m_name.compare("OCC.BMF.ODS.ODS.tioODS-SchxFileName"))
		{
			try
			{
				std::string tempString="0";
				
				m_persistence->readPersistedValue( DP_CURRENT_VALUE, m_entityKey, tempString );
				m_fieldEngValue.setFromPersist(tempString, m_precision);
				convertToRaw(m_fieldEngValue, m_fieldValue);
				
				if (isWriteable())
				{
					m_fieldValueQualityStatus = QUALITY_GOOD_NO_SPECIFIC_REASON;
					m_previousFieldValueQualityStatus = m_fieldValueQualityStatus;
				}
				else
				{
					m_fieldValueQualityStatus = QUALITY_BAD_LAST_KNOWN_VALUE;
					m_previousFieldValueQualityStatus = m_fieldValueQualityStatus;
				}
				
				//because the derived datapoint's enumLabels are not loaded yet, so can't print out the enum labels here.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Last persisted value of datapoint [%s] is %s",
					m_name.c_str(), tempString.c_str());
			}
			catch (...) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Catch exception when load last persisted value of datapoint %s", m_name.c_str());
			}
		}

        FUNCTION_EXIT;
	}


	void DataPoint::saveLastOperatorInfo ( const std::string & sessionID, const timeb & timestamp )
	{
        FUNCTION_ENTRY("saveLastOperatorInfo");

        std::string lastSessionId = m_lastOperatorSessionID;
        timeb lastTimestamp;
        lastTimestamp.time = m_lastOperatorTimestamp.time;
        lastTimestamp.millitm = m_lastOperatorTimestamp.millitm;

		m_lastOperatorSessionID = sessionID;
		m_lastOperatorTimestamp.time = timestamp.time;
		m_lastOperatorTimestamp.millitm = timestamp.millitm;

		// only if the data point is in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
            // only write to db if dp updated by operator
            if (sessionID != DPT_INTERNAL_SESSION_ID)
            {
			    // Update database with the new sessionID
                if (lastSessionId != sessionID)
                {
                    PersistRequest * persistReq = new PersistRequest( DP_LAST_OPERATOR_SESSION_ID, m_entityKey, sessionID, m_fieldValueQualityStatus );
					m_persistQ->insert( persistReq );
                }

			    // Update database with the new timestamp
                if (lastTimestamp.time != timestamp.time || lastTimestamp.millitm != timestamp.millitm)
                {
                    std::stringstream timestampSting;
					timestampSting<<m_lastOperatorTimestamp.time;
                    PersistRequest * persistReq = new PersistRequest( DP_LAST_OPERATOR_TIMESTAMP, m_entityKey, timestampSting.str(), m_fieldValueQualityStatus );
					m_persistQ->insert( persistReq );
                }
            }
		}

		// notify all registered observers of the latest changes
        if (lastSessionId != sessionID || 
            lastTimestamp.time != timestamp.time || lastTimestamp.millitm != timestamp.millitm)
		{
			boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails(new DpDynamicSyncUpdate);
            updateDetails->updateType = TA_Base_Bus::LastOperatorUpdate;
            updateDetails->lastOperatorInfo.sessionID = m_lastOperatorSessionID;
            updateDetails->lastOperatorInfo.timeStamp = m_lastOperatorTimestamp;
            
			submitUpdate( updateDetails );
		}
        FUNCTION_EXIT;
	}

	void DataPoint::configureControlStatus()
	{
        FUNCTION_ENTRY("configureControlStatus");
		std::string configControlStatus("");
        m_persistence->readPersistedValue( DP_CONTROL_STATUS, m_entityKey, configControlStatus );

		if ( "noControl" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::noControl;
		}
		else if ( "written" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::written;
		}
		else if ( "pending" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::pending;
		}
		else if ( "conditionalSuccess" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::conditionalSuccess;
		}
		else if ( "success" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::success;
		}
		else if ( "LCC_failed" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::LCC_failed;
		}
		else if ( "RCC_failed" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::RCC_failed;
		}
		else if ( "comms_failed" == configControlStatus )
		{
			m_controlState.controlState = TA_Base_Bus::comms_failed;
		}
		else
		{
			m_controlState.controlState = TA_Base_Bus::noControl;
		}

		m_controlState.triggerringDpKey = m_entityKey;
		m_controlState.triggerringDpValue = "";

        FUNCTION_EXIT;
	}

	bool DataPoint::registerForUpdateDetails ( TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver )
	{
        FUNCTION_ENTRY("registerForUpdateDetails");
		// stop mutil thread modifing the list of observers
		ThreadGuard guard( m_updateDetailsObserverlock );

		bool result = false;

		// only proceed if there is a valid observer
		if ( 0 != updateDetailsObserver )
		{
			bool existed = false;

			// check if we already have this observer in the list
			std::vector < TA_Base_Bus::IDataPointObserverNotification * >::iterator itr;
			for ( itr = m_listOfUpdateDetailsObservers.begin();
				  itr != m_listOfUpdateDetailsObservers.end();
				  itr++ )
			{
				// if found a match
				if ( updateDetailsObserver == (*itr) )
				{
					// set the flag and exit loop
					existed = true;
					break;
				}
			}

			// if found no match
			if ( false == existed )
			{
				// put the observer to the list for later usage
				m_listOfUpdateDetailsObservers.push_back ( updateDetailsObserver );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Observer [%u] is added into observer list of DataPoint [%s]", updateDetailsObserver, getDataPointName().c_str());
            }
			// else do nothing if the specified observer already in the list

			result = true;
		}
		// else do nothing if the specified observer is null
        FUNCTION_EXIT;
		return result;
	}


	bool DataPoint::deregisterForUpdateDetails ( TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver )
	{
        FUNCTION_ENTRY("deregisterForUpdateDetails");
		// stop mutil thread modifing the list of observers
		ThreadGuard guard( m_updateDetailsObserverlock );

		bool result = false;

		// only proceed if there is a valid observer
		if ( 0 != updateDetailsObserver )
		{
			bool existed = false;

			// check if we already have this observer in the list
			std::vector < TA_Base_Bus::IDataPointObserverNotification * >::iterator itr;
			for ( itr = m_listOfUpdateDetailsObservers.begin();
				  itr != m_listOfUpdateDetailsObservers.end();
				  itr++ )
			{
				// if found a match
				if ( updateDetailsObserver == (*itr) )
				{
					// set the flag and exit loop
					existed = true;
					break;
				}
			}

			// if found a match
			if ( true == existed )
			{
				// remove the observer from the list
				m_listOfUpdateDetailsObservers.erase ( itr );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Observer [%u] is removed from observer list of DataPoint [%s]", updateDetailsObserver, getDataPointName().c_str());
                
				result = true;
			}
			// else do nothing is the specified observer is not in the list
		}
		// else do nothing if the specified observer is null
        FUNCTION_EXIT;
		return result;
	}


	void DataPoint::notifyRegisteredObservers ( boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails )
	{
        FUNCTION_ENTRY("notifyRegisteredObservers");
		// stop mutil thread modifing the list of observers
		ThreadGuard guard( m_updateDetailsObserverlock );

		// for each of the registered observers in the list
		std::vector < TA_Base_Bus::IDataPointObserverNotification * >::iterator itr;
		for ( itr = m_listOfUpdateDetailsObservers.begin();
			  itr != m_listOfUpdateDetailsObservers.end();
			  itr++ )
		{
			try
			{
				(*itr)->processDataPointUpdate ( m_entityKey, updateDetails );

				// allow time for the observers to do their updates
				//TA_Base_Core::Thread::sleep ( 10 );
			}
			catch ( ... )
			{
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught UnknownException when notify observer [%u] of DataPoint [%s]", (*itr), getDataPointName().c_str());
			}
		}
        FUNCTION_EXIT;
	}
    
	void DataPoint::updateAlarms( DataPointState & newState )
	{
        FUNCTION_ENTRY("updateAlarms");
		// only if the data point is in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			// assume that none of the alarm objects are in alarm state
			bool alarmCondition = false;
			TA_Base_Bus::EDataPointAlarms alarmType = TA_Base_Bus::DptGeneral;

			try
			{
				// only updating DataPoint alarm if the new state is good
				// or if the quality is QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED
				// (applicable for analogue value over the scale limit)

				if ( ( newState.isGoodValue() ) ||
					 ( QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED == newState.getStatus() ) )
				{
					//
					// Each DataPointAlarmDetails object is responsible
					// for determining if it has entered into a new alarm
					// state or not, and then raise/close alarm accordingly
					//
					DpAlarmList::iterator alarmsIter;

					// TD19624, for derived and enum datapoint, their alarm type is DpGeneral,
					// So the closing and raising alarm sequence become important!

					if ( ( TA_Base_Bus::DPT_ENUM_TYPE == m_dataType ) ||
						 ( TA_Base_Bus::DPT_ENUM == m_type ) || 
						 ( TA_Base_Bus::DPT_DERIVED == m_type )) //19624++ cater for derived datapoint
					{
						if ( ( m_name.find( "tdiSWCR-PollingStatus" ) != std::string::npos )
							&& ( newState.getValue().getValueAsString() == "INVALID" ) )	// CL20184++ - no need close alarm for this polling status datapoint
						{						
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Meet PollingStatus Datapoint, will not close this alarm under INVALID value!!!" );
						}
						else
						{
                        	ThreadGuard guard( m_alarmslock );
							for( alarmsIter = m_alarms.begin(); alarmsIter != m_alarms.end(); ++alarmsIter )
							{
								//TD15019, close alarm with new value and timestamp
								( * alarmsIter )->closeAlarm(newState.getValue(), newState.getTimebStructure());
							}
						}
                    }

                    {
                        ThreadGuard guard( m_alarmslock );
					    for( alarmsIter = m_alarms.begin(); alarmsIter != m_alarms.end(); ++alarmsIter )
					    {
						    //
						    // set/reset the alarm condition
						    //
						    bool isAlarmInhibited = (true == m_AlarmInhibitedState ||
											         IN_SERVICE != m_serviceState );
						    
						    ( * alarmsIter )->updateState( getOMFlag(),
													       newState,
													       isAlarmInhibited );

						    // if the alarm object indicate it is in alarm
						    // and the datapoint's alarm condition is false
						    if ( ( false == alarmCondition ) &&
							     ( true == ( * alarmsIter )->isInAlarm() ) )
						    {
							    // then set the datapoint's alarm condition and the alarm type
							    alarmCondition = true;
							    alarmType = ( * alarmsIter )->getType();
						    }
					    }
                    }

					// save the alarm condition
					saveAlarmState ( alarmType, alarmCondition );

				}
			}
			catch ( ... )
			{
			}
		}
        FUNCTION_EXIT;
	}

	void DataPoint::saveAlarmState ( const TA_Base_Bus::EDataPointAlarms & alarmType,
									 bool inAlarm )
	{
        FUNCTION_ENTRY("saveAlarmState");

		// save the alarm condition
		if ( true == inAlarm )
		{
			switch ( alarmType )
			{
				case TA_Base_Bus::DptOverScaleHi:
				{
					m_alarmState = TA_Base_Bus::DptOverScaleHi;
					m_alarmStateString = "OverScaleHiAlarm";
					break;
				}
				case TA_Base_Bus::DptOverScaleLo:
				{
					m_alarmState = TA_Base_Bus::DptOverScaleLo;
					m_alarmStateString = "OverScaleLoAlarm";
					break;
				}
				case TA_Base_Bus::DptHiHi:
				{
					m_alarmState = TA_Base_Bus::DptHiHi;
					m_alarmStateString = "hihiAlarm";
					break;
				}
				case TA_Base_Bus::DptHi:
				{
					m_alarmState = TA_Base_Bus::DptHi;
					m_alarmStateString = "hiAlarm";
					break;
				}
				case TA_Base_Bus::DptLo:
				{
					m_alarmState = TA_Base_Bus::DptLo;
					m_alarmStateString = "loAlarm";
					break;
				}
				case TA_Base_Bus::DptLoLo:
				{
					m_alarmState = TA_Base_Bus::DptLoLo;
					m_alarmStateString = "loloAlarm";
					break;
				}
				case TA_Base_Bus::DptBoolTrue:
				{
					m_alarmState = TA_Base_Bus::DptBoolTrue;
					m_alarmStateString = "trueAlarm";
					break;
				}
				case TA_Base_Bus::DptBoolFalse:
				{
					m_alarmState = TA_Base_Bus::DptBoolFalse;
					m_alarmStateString = "falseAlarm";
					break;
				}
				case TA_Base_Bus::DptGeneral:
				{
					m_alarmState = TA_Base_Bus::DptGeneral;
					m_alarmStateString = "generalAlarm";
					break;
				}
				default:
				{
					m_alarmState = TA_Base_Bus::DptNoAlarm;
					m_alarmStateString = "noAlarm";
					break;
				}
			}
		}
		else
		{
			m_alarmState = TA_Base_Bus::DptNoAlarm;
			m_alarmStateString = "noAlarm";
		}
        FUNCTION_EXIT;
	}

	void DataPoint::reEvaluateAlarmCondition ( const TA_Base_Bus::EDataPointAlarms & alarmType,
												bool inAlarm )
	{
        FUNCTION_ENTRY("reEvaluateAlarmCondition");
		//
		// alarm condition has changed due to the delay in the alarm object
		//
        m_previousAlarmState = m_alarmState;

		// save the alarm state
		saveAlarmState ( alarmType, inAlarm );

        if (m_previousAlarmState != m_alarmState)
        {
            // notify client of the new value and alarm state
            boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmupdate(new DpDynamicSyncUpdate);
            alarmupdate->updateType = TA_Base_Bus::AlarmStateUpdate;
            alarmupdate->alarmState = m_alarmState;
            //send out the update to inform the observer
            submitUpdate ( alarmupdate );
        }
        FUNCTION_EXIT;
	}

    void DataPoint::ackCloseAlarm( const std::string & sessionID )
    {
        //TA_ASSERT( NULL != m_alarmHelper, "m_alarmHelper is NULL" );
        //TA_ASSERT( NULL != m_alarmModificationHelper, "m_alarmModificationHelper is NULL" );

        AlarmDetailCorbaDef* alarm = NULL;        
        std::vector<AlarmDetailCorbaDef*> alarms;
    
        try
        {
            alarm = m_alarmHelper.getMostRecentActiveAlarmForEntity( m_entityKey, m_locationKey );        
            alarms.push_back(alarm);
        
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Acknowledging alarm for ASSET: %s - %s",
                alarm->assetName.in(), alarm->alarmDescription.in());

			m_alarmModificationHelper.acknowledgeAlarms(alarms, sessionID.c_str());

			// TD15019, use current system as the close alarm timestamp
			struct timeb currentSystemTime;
			ftime ( &currentSystemTime );

            ThreadGuard guard( m_alarmslock );
            for (DpAlarmList::iterator it = m_alarms.begin(); it != m_alarms.end(); ++it)
            {
                TA_ASSERT(*it != NULL, "m_alarms contain NULL DataPointAlarmDetails pointer");

                (*it)->closeAlarm(getCurrentValue(), currentSystemTime); 
            }

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Alarm for ASSET %s successfully acknowledged and closed",
                alarm->assetName.in());
        }
        catch( const TA_Base_Core::AlarmAgentException& aae)
        {
            std::ostringstream description;
            description << "SetIsAlarmInhibited: Could not acknowledge or close alarms for datapoint "
                << m_name << " (" << m_entityKey << ")"
                << " at location " << m_locationKey
                << "; Reason: " << aae.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmAgentException", description.str().c_str());
        }
        catch( const TA_Base_Core::ObjectResolutionException& ore )
        {
            std::ostringstream description;
            description << "SetIsAlarmInhibited: Could not acknowledge or close alarms for datapoint "
                << m_name << " (" << m_entityKey << ")"
                << " at location " << m_locationKey
                << "; Reason: " << ore.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", description.str().c_str());
        }
        catch( const TA_Base_Core::NoAlarmAgentException& naae )
        {
            std::ostringstream description;
            description << "SetIsAlarmInhibited: Could not acknowledge or close alarms for datapoint "
                << m_name << " (" << m_entityKey << ")"
                << " at location " << m_locationKey
                << "; Reason: " << naae.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", description.str().c_str());
        }
		catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Alarm Exception: ", "Exception thrown while calling ackCloseAlarm(): " );
			LOGMORE(SourceInfo, ex.reason.in());
		}

		delete alarm;
		alarm = NULL;
    }

	void DataPoint::setAlarmDetails( const TA_Base_Bus::EDataPointAlarms & almType,
									 bool almEnabled,
									 int almDelay,
									 int almSeverity,
									 const std::string & almMessage,
									 const DpValue & almValue,
									 const std::string & engUnit,
									 const std::string & MMSConfiguration,
									 double alarmThresholdConstant
									 )
	{
        FUNCTION_ENTRY("setAlarmDetails");
        //
		// TD#781
		// if the alarm value is faulty then raise an alarm to inform the operator
		// and then disable, i.e. don't create the alarm object
		//
		bool alarmEnabled = almEnabled;
		std::string faultyAlarmMessage;
		switch( almType )
		{
        case TA_Base_Bus::DptHiHi:
            if ( TA_Base_Core::DataPointEntityData::FAULTY_VALUE == almValue.getFloat() )
            {
                faultyAlarmMessage = "High high alarm limit is unknown.  This alarm type will be disabled.";

                // close alarm just in case it has been raised before
                if ( false == m_unknownHiHiLimitAlarmID.empty() )
                {
                    closeNonUniqueAlarm ( m_unknownHiHiLimitAlarmID, DataPointAlarms::VH );
                    m_unknownHiHiLimitAlarmID = "";
                }
                
                // then raise alarm
                m_unknownHiHiLimitAlarmID = raiseNonUniqueAlarm ( DataPointAlarms::VH, faultyAlarmMessage, "Unknown Alarm Limit" );
            }
            else
            {
                // close alarm just in case it has been raised before
                if ( false == m_unknownHiHiLimitAlarmID.empty() )
                {
                    closeNonUniqueAlarm ( m_unknownHiHiLimitAlarmID, DataPointAlarms::VH );
                    m_unknownHiHiLimitAlarmID = "";
                }
            }
            break;

        case TA_Base_Bus::DptHi:
            if ( TA_Base_Core::DataPointEntityData::FAULTY_VALUE == almValue.getFloat() )
            {
                faultyAlarmMessage = "High alarm limit is unknown.  This alarm type will be disabled.";
                
                // close alarm just in case it has been raised before
                if ( false == m_unknownHiLimitAlarmID.empty() )
                {
                    closeNonUniqueAlarm ( m_unknownHiLimitAlarmID, DataPointAlarms::HI );
                    m_unknownHiLimitAlarmID = "";
                }
                // raise alarm if has not been done so
                m_unknownHiLimitAlarmID = raiseNonUniqueAlarm ( DataPointAlarms::HI, faultyAlarmMessage, "Unknown Alarm Limit" );
            }
            else
            {
                // close alarm just in case it has been raised before
                if ( false == m_unknownHiLimitAlarmID.empty() )
                {
                    closeNonUniqueAlarm ( m_unknownHiLimitAlarmID, DataPointAlarms::HI );
                    m_unknownHiLimitAlarmID = "";
                }
            }
            break;
			
        case TA_Base_Bus::DptLo:
            if ( TA_Base_Core::DataPointEntityData::FAULTY_VALUE == almValue.getFloat() )
            {
                faultyAlarmMessage = "Low alarm limit is unknown.  This alarm type will be disabled.";

                // close alarm just in case it has been raised before
                if ( false == m_unknownLoLimitAlarmID.empty() )
                {
                    closeNonUniqueAlarm ( m_unknownLoLimitAlarmID, DataPointAlarms::LO );
                    m_unknownLoLimitAlarmID = "";
                }
                
                // raise alarm if has not been done so
                m_unknownLoLimitAlarmID = raiseNonUniqueAlarm ( DataPointAlarms::LO, faultyAlarmMessage, "Unknown Alarm Limit" );
            }
            else
            {
                // close alarm just in case it has been raised before
                if ( false == m_unknownLoLimitAlarmID.empty() )
                {
                    closeNonUniqueAlarm ( m_unknownLoLimitAlarmID, DataPointAlarms::LO );
                    m_unknownLoLimitAlarmID = "";
                }
            }
            break;
        
        case TA_Base_Bus::DptLoLo:
			if ( TA_Base_Core::DataPointEntityData::FAULTY_VALUE == almValue.getFloat() )
			{
				faultyAlarmMessage = "Low low alarm limit is unknown.  This alarm type will be disabled.";

				// close alarm just in case it has been raised before
				if ( false == m_unknownLoLoLimitAlarmID.empty() )
				{
					closeNonUniqueAlarm ( m_unknownLoLoLimitAlarmID, DataPointAlarms::VL );
					m_unknownLoLoLimitAlarmID = "";
				}

				// raise alarm if has not been done so
				m_unknownLoLoLimitAlarmID = raiseNonUniqueAlarm ( DataPointAlarms::VL, faultyAlarmMessage, "Unknown Alarm Limit" );
			}
			else
			{
				// close alarm just in case it has been raised before
				if ( false == m_unknownLoLoLimitAlarmID.empty() )
				{
					closeNonUniqueAlarm ( m_unknownLoLoLimitAlarmID, DataPointAlarms::VL );
					m_unknownLoLoLimitAlarmID = "";
				}
			}
			break;

        case DptNoAlarm:
        case DptOverScaleHi:
        case DptOverScaleLo:
        case DptBoolTrue:
        case DptBoolFalse:
        case DptGeneral:
            // nothing to do
            // prevent stupid log
            break;
        
        default:
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Datapoint (%s), trying to create unknown alarm type (%d)",
                m_name.c_str(), almType );
            break;
		}

		if ( false == faultyAlarmMessage.empty() )
		{
			// disable the alarm since the alarm limit is faulty/unknown
			alarmEnabled = false;
		}

		DataPointAlarmDetailsFactory alarmFactory;

		//
		// Search for alarm type in currently configured alarms
		//
		bool noNewAlarmRequired = false;
        {
            ThreadGuard guard( m_alarmslock );
		    DpAlarmList::iterator almIter = m_alarms.begin();
		    for ( ; almIter != m_alarms.end(); ++almIter )
		    {
			    if ( (*almIter)->getType() == almType )
			    {
				    //
				    // This alarm type already exists - pass the current
				    // DataPointState and the new details to the factory
				    // to update.
				    //

				    // if the alarm is one of the GeneralAlarmDetails
				    if ( TA_Base_Bus::DptGeneral == almType )
				    {
					    // need to check for the alarm value if it is matched
					    if ( (*almIter)->getValue() == almValue )
					    {
						    DataPointState engState( almValue,
												     getTimebStructure(),
												     getQualityStatus() );

						    alarmFactory.updateAlarmDetails( getOMFlag(),
                                                             *almIter,
                                                             alarmEnabled,
                                                             almDelay,
                                                             almSeverity,
                                                             almMessage,
                                                             MMSConfiguration,
                                                             almValue,
                                                             engState,
                                                             m_AlarmInhibitedState,
                                                             alarmThresholdConstant );

						    // assign engUnit
						    (*almIter)->setEngUnit ( engUnit );

						    // set the flag to indicate no new alarm is needed
						    noNewAlarmRequired = true;
					    }
					    // do nothing if not matched
				    }

				    // if the alarm object is HiHi, Hi, Lo, LoLo, True or False
				    else
				    {
					    DataPointState engState( getCurrentEngValue(),
											     getTimebStructure(),
											     getQualityStatus() );

					    alarmFactory.updateAlarmDetails( getOMFlag(),
                                                         *almIter,
                                                         alarmEnabled,
                                                         almDelay,
                                                         almSeverity,
                                                         almMessage,
                                                         MMSConfiguration,
                                                         almValue,
                                                         engState,
                                                         m_AlarmInhibitedState,
                                                         alarmThresholdConstant
                                                         );

					    // assign engUnit
					    (*almIter)->setEngUnit ( engUnit );

					    // set the flag to indicate no new alarm is needed
					    noNewAlarmRequired = true;
				    }
			    }
		    }
        }

		//
		// need to create a new alarm
		//
		if ( false == noNewAlarmRequired )
		{

			// TD 9734: DataPoints belong to the "SCADA" logical subsystem (ie. Transactive application).
            //          If available, use the datapoint's physical subsystem (ie. ATS, WILD, Radio, etc)

			unsigned long subsystemKey = m_subsystemKey;

			if (!m_dataPointEntityAccess->getPhysicalSubsystemName().empty())
			{
				// Physical subsystem key exists. Use it.
				subsystemKey = m_dataPointEntityAccess->getPhysicalSubsystem();
			}

			DataPointAlarmDetails * newAlm =
				alarmFactory.createAlarmDetails( m_dataPointEntityAccess,
                                                 almType,
                                                 alarmEnabled,
                                                 almDelay,
                                                 almSeverity,
                                                 almMessage,
                                                 almValue,
                                                 m_name,
                                                 MMSConfiguration,
                                                 m_parentKey,
                                                 m_entityKey,
                                                 m_entityTypeKey,
                                                 m_locationKey,
                                                 subsystemKey,   //  TD9734
                                                 alarmThresholdConstant,
                                                 m_mmsScadaUtility
                                                 );

			if ( newAlm != NULL )
			{
				// assign engUnit
				newAlm->setEngUnit( engUnit );

				// assign the reference to this datapoint
				newAlm->setDataPointReference( this );

                ThreadGuard guard( m_alarmslock );
				// save the new alarm to the alarm list
				m_alarms.push_back( newAlm );
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "DataPointAlarmsDetailsFactory returned NULL pointer for new alarm details" );
			}
		}

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
        
	
        FUNCTION_EXIT;
	}

	std::string DataPoint::raiseNonUniqueAlarm ( const TA_Base_Core::MessageType & alarmType,
												 const std::string & alarmMessage,
												 const std::string & dpValue )
	{
        FUNCTION_ENTRY("raiseNonUniqueAlarm");
		std::string alarmID ( "" );

		try
		{
			// get the handle to an instance of the EventDetailsFactory if not already done so
			if ( 0 == m_eventDetailsFactory )
			{
				m_eventDetailsFactory = EventDetailsFactory::getInstance();
			}

			const TA_Base_Core::DataPointEventDetails& eventDetails = 
				m_eventDetailsFactory->getEventDetails(m_entityKey, m_dataPointEntityAccess->getAgentName());

			// construct the DescriptionParameters list
			DescriptionParameters descriptionParameters;
			NameValuePair pair1 ( "AlarmMessage", alarmMessage );
			NameValuePair pair2 ( "DataPointValue", dpValue );
			NameValuePair pair3 ( "OMFlag", getOMFlag() );
			descriptionParameters.push_back ( &pair1 );
			descriptionParameters.push_back ( &pair2 );
			descriptionParameters.push_back ( &pair3 );

			// generate the non-unique alarm
			alarmID = AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset ( alarmType,
												m_entityKey,
												m_entityTypeKey,
												descriptionParameters,
												m_name,
												m_locationKey,
												m_subsystemKey,
												eventDetails.assetName );
		}
		catch ( ... )
		{
		}

        FUNCTION_EXIT;
		return alarmID;
	}


	void DataPoint::closeNonUniqueAlarm ( const std::string & alarmID, const TA_Base_Core::MessageType & alarmType )
	{
        FUNCTION_ENTRY("closeNonUniqueAlarm");

		try
		{
			// TD15019, use current system as the close alarm timestamp
			struct timeb currentSystemTime;
			ftime ( &currentSystemTime );

			// close the specified non-unique alarm
			AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation ( alarmID,
												m_locationKey,
												getCurrentValue().getValueAsString(),
												currentSystemTime);
		}
		catch ( ... )
		{
		}

        FUNCTION_EXIT;
	}


    void DataPoint::updateAlarmAckState()
    {
        FUNCTION_ENTRY("updateAlarmAckState");

        //ingore when running in monitor mode
        //if ( TA_Base_Core::Monitor == m_operationMode )
        //{
        //    return;
        //}

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DataPoint [%s] receive a notification of alarm ack state change", m_name.c_str() );

        EScadaAlarmAckState ackState = ScadaAlarmUnknown;

        {
            ThreadGuard guard( m_alarmslock );
		    // for each of the alarm object belonging to this data point
		    DpAlarmList::const_iterator almIter = m_alarms.begin();
		    for( ; almIter != m_alarms.end() && ackState != ScadaAlarmNack; ++almIter )
		    {
                EScadaAlarmAckState ackStateOfType = (*almIter)->getAlarmAckState();
                switch(ackStateOfType) {
                case ScadaAlarmNack:
                    ackState = ScadaAlarmNack;
            	    break;

                case ScadaAlarmAck:
                    ackState = ScadaAlarmAck;
            	    break;

                case ScadaAlarmNotPresent:
                    if (ackState != ScadaAlarmAck)
                    {
                        ackState = ScadaAlarmNotPresent;
                    };
                    break;
                case ScadaAlarmUnknown:
                default:
                    break;
                }
		    }
        }

        if (m_ackState != ackState)
        {
            m_ackState = ackState;

            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "DataPoint [%s] alarm ack state changed to [%d]", m_name.c_str(), m_ackState );
            
            // notify client of the new value and alarm state
            boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> alarmAckUpdate(new DpDynamicSyncUpdate);
            alarmAckUpdate->updateType = TA_Base_Bus::AlarmAckStateUpdate;
            alarmAckUpdate->alarmAckState = m_ackState;
            
            //send out the update to inform the observer
            submitUpdate ( alarmAckUpdate );
        }
        
        FUNCTION_EXIT;
    }


	bool DataPoint::isInAlarm( const TA_Base_Bus::EDataPointAlarms & dpAlmType ) const
	{
        FUNCTION_ENTRY("isInAlarm");
		bool inAlarm = false;

        ThreadGuard guard( m_alarmslock );
		// for each of the alarm object belonging to this data point
		DpAlarmList::const_iterator almIter = m_alarms.begin();
		for( ; almIter != m_alarms.end(); ++almIter )
		{
			// if the alarm type matches
			if ( (*almIter)->getType() == dpAlmType )
			{
				// only proceed if the inAlarm flag has not been set
				if ( true != inAlarm )
				{
					// get the alarm status of the current alarm object
					inAlarm = (*almIter)->isInAlarm();
				}
				// else do nothing because we know that the data point is already in alarm
			}
		}
        FUNCTION_EXIT;
		return inAlarm;
	}


    void DataPoint::submitUpdate(boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> dpState )
	{
        FUNCTION_ENTRY("submitUpdate");

		// only if the data point is in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			            
            // following actions only applicable if the logging of the field value
            // is enabled
            if ( ( false == m_forcedState ) && ( true == m_logChanges ) && 
                (TA_Base_Bus::ValueUpdate == dpState->updateType) )//TD18579, only the value update is considered as state change
            {
				// Only log if we are not raising or normalizing an alarm
				// These events are handled by Alarm Agent
                if ( (TA_Base_Bus::DptNoAlarm == m_previousAlarmState && 
                      TA_Base_Bus::DptNoAlarm == m_alarmState) ||
                     (TA_Base_Bus::DptNoAlarm != m_previousAlarmState && 
                      TA_Base_Bus::DptNoAlarm != m_alarmState) )
                {
                    TA_Base_Core::IEvent* stateChange  = new DataPointStateChangeEvent( dpState, *this, TA_Base_Core::DataPointAudit::DataPointStateChange );
                    m_auditMessageSender->sendAuditMessage(stateChange);
                }
            }

			if ( TA_Base_Bus::ValueUpdate == dpState->updateType )
			{
				m_previousFieldValueQualityStatus = m_fieldValueQualityStatus;
			}
			// notify all registered observers of the latest changes
			notifyRegisteredObservers ( dpState );
		}
		else
		{
			// peter low: always notify observers for alarmAck state to fix proxies ack state update synchronization issue during startup stage
			if ((TA_Base_Bus::AlarmAckStateUpdate == dpState->updateType)|| 
				(TA_Base_Bus::ValueUpdate == dpState->updateType)||
				(TA_Base_Bus::CompleteDynamicSyncStateUpdate == dpState->updateType))
			{
				notifyRegisteredObservers ( dpState );
			}
		}		
        FUNCTION_EXIT;
	}

	void DataPoint::updateFieldState( DataPointState & newState )
	{
        FUNCTION_ENTRY("updateFieldState");

		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );

		if ( TA_Base_Core::Control == m_operationMode )
		{
			// store latest field state
			m_lastKnownFieldState = newState;

			// only process newState if the data point is not scan inhibited.
			if ( IN_SERVICE == m_serviceState ) 
			{
				// Make type-specific determination of the validity of this new state
				if ( isValidUpdate( newState ) )
				{
					// update the datapoint with the specified new state
					updateDataPointState ( newState, true );
				}
			}
		}

        FUNCTION_EXIT;
	}


	bool DataPoint::updateForcedState( DataPointState & newState )
	{
        FUNCTION_ENTRY("updateForcedState");

		// lock the data point to prevent it from being modified while its current
		// state is being updated
		ThreadGuard guard( m_lock );

		bool result = false;

		result = updateDataPointState ( newState, false ); // ++wenching (TD12132)
	
        FUNCTION_EXIT;
		return result;
	}


	bool DataPoint::updateDataPointState ( DataPointState & newState, bool fieldValue )
	{
        FUNCTION_ENTRY("updateDataPointState");
		bool result = false;
		bool newStateToBeSubmitted = false;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPoint [%s] updates its state: value [%s], quality [%u].", 
            getDataPointName().c_str(), newState.getValue().getValueAsString().c_str(), newState.getStatus());
        
		// dealing with field state
		if ( true == fieldValue )
		{
			// if this data point is writeable, i.e an output
			if ( true == m_writable )
			{
				// update the current field state with the new state
				result = setFieldValue( newState );

				// always generate an update event each time the data point is updated with
				// a new value
				newStateToBeSubmitted = true;
			}

			// if this data point is not writeable, i.e an input
			else
			{
				// NOTE: if the data point is in forced state, the field value is to be updated but
				//		 there will be no event/alarm generated

				// if the new field status is different with the current status
				// or the new field value is different from the current field value WHEN QUALITY IS GOOD
				if ( m_fieldValueQualityStatus != newState.getStatus() ||
					 ( newState.isGoodValue() || newState.getStatus() == QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED) && m_fieldValue != newState.getValue() )
				{
					// update the current field state with the new state
					result = setFieldValue( newState );

					// generate an update event, if and only if not in forced state
					if ( ! m_forcedState )
					{
						newStateToBeSubmitted = true;
					}
					// else do not submit an update message
				}
			}
		}
		
		// dealing with forced state
		else
		{
			// update the current forced state with the new state
			result = setForcedValue( newState );

			// always generate an update event each time a forced value is entered
			newStateToBeSubmitted = true;
		}

		//
		// submit new state if required to do so
		//

		if ( true == newStateToBeSubmitted )
		{
			//
			// check alarms state of the current eng value, either
			// forced or field eng. value, depending on the manual switch
			//
			m_previousAlarmState = m_alarmState;
			DataPointState engState( getCurrentEngValue(),
									 newState.getTimebStructure(),
									 newState.getStatus() );

			updateAlarms( engState );

            if (m_previousAlarmState != m_alarmState)
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
            //set the current datapoint value to the update structure
            dpupdate->value = getCurrentEngValue().getDataPointValue();
            dpupdate->value.quality = getQualityStatus();
            dpupdate->value.timeStamp = getTimebStructure();
            //send out the update to inform the observer
            submitUpdate ( dpupdate );

        }
        FUNCTION_EXIT;
		return result;
	}

    void DataPoint::setCurrentDataPointValue( TA_Base_Bus::DpCompleteDynamicSyncState& dpvalue)
    {
        //set the current datapoint value to the update structure
        dpvalue.value.dpType = getDataPointDataType();
        switch(getDataPointDataType())
		{
		case DPT_BOOL_TYPE:
			dpvalue.value.booleanValue = getCurrentEngValue().getDataPointValue().booleanValue;
			break;

		case DPT_ENUM_TYPE:
			dpvalue.value.enumValue = getCurrentEngValue().getDataPointValue().enumValue;
			break;

		case DPT_TEXT_TYPE:
			dpvalue.value.stringValue = getCurrentEngValue().getDataPointValue().stringValue;
			break;

		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			dpvalue.value.floatValue = getCurrentEngValue().getDataPointValue().floatValue;
			break;

		case DPT_NULL_TYPE:
		default:
			break;	
		}
        dpvalue.value.quality = getStatus();
        dpvalue.value.timeStamp = getTimebStructure();
		dpvalue.value.controlOperation = TA_Base_Bus::CONTROL_NO_OPERATION;
    }

    void DataPoint::updateEngUnitForAlarms ( const std::string & engUnit )
	{
        FUNCTION_ENTRY("updateEngUnitForAlarms");
        
        ThreadGuard guard( m_alarmslock );
		// for each of the alarm associated with this data point
		DpAlarmList::iterator alarmIter = m_alarms.begin();
		for( ; alarmIter != m_alarms.end(); ++alarmIter )
		{
			// assign eng unit
			(*alarmIter)->setEngUnit ( engUnit );
		}
        FUNCTION_EXIT;
	}

    void DataPoint::syncControlState(DataPointStateUpdate& dpStateUpdate)
    {
        if (m_operationMode == TA_Base_Core::Monitor)
        {
            //to set control state
            m_controlState.controlState = dpStateUpdate.controlState;
            m_controlState.triggerringDpKey = dpStateUpdate.triggeringDpKey;
            m_controlState.triggerringDpValue = dpStateUpdate.triggeringDpValue;
            m_auditEventLoggingEnabled = dpStateUpdate.auditEventLoggingEnabled;

            //to set control update
            m_commandId = dpStateUpdate.commandId;
            m_controlCommandState = dpStateUpdate.controlCommandState;
            m_controlSessionId = dpStateUpdate.sessionId;

            //to set the field value
            m_fieldValue = DpValue(dpStateUpdate.dpValue, getDataPointDataType(), getEnumLabelsMap(), getBooleanLabels(), getPrecision());
            m_fieldValueQualityStatus = dpStateUpdate.dpValue.quality;
			m_previousFieldValueQualityStatus = m_fieldValueQualityStatus;
            m_fieldValueTimestamp = dpStateUpdate.dpValue.timeStamp;

            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Monitor mode DataPoint [%s] synchronized control update with commandId [%u].", m_name.c_str(), dpStateUpdate.commandId );
        }
        else
        {
            //should not get here
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Control mode DataPoint [%s] ingored control update info with commandId [%u].", m_name.c_str(), dpStateUpdate.commandId );
        }
    }

    bool DataPoint::isRCCConfigured()
    {
        return (!m_abeExpression.empty());
    }

	void DataPoint::setSession(std::string strSession)
	{
		m_strSession = strSession;
	}

	std::string DataPoint::getSession()
	{
		return m_strSession;
	}

	void DataPoint::executeWorkItem()
	{
        FUNCTION_ENTRY("executeWorkItem");
		//
		// callback from ThreadPool for scheduled work
		//

        // if datapoint is initialised with forced value, need to evaluate its alarm states
        if (m_forcedState)
        {
            m_previousAlarmState = m_alarmState;
            
            DataPointState engState( getCurrentEngValue(),
                getTimebStructure(),
                getStatus() );
            
            updateAlarms( engState );
            //send out the alarm update only if the datapoint is in control mode
            //if (TA_Base_Core::Control == m_operationMode)
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

		// attempt to make first evaluation of the calculated expression
		if ( 0 != m_calculatedExpressionHandle )
		{			
			processExpressionChanges();
		}

		// kick start the proxies created by the LaunchingConditionCheck
		if ( 0 != m_launchingConditionCheck )
		{			
			m_launchingConditionCheck->evaluateLaunchingExpression();
		}

        FUNCTION_EXIT;
	}

	std::string DataPoint::getEventMessage()
	{
		return m_dataPointEntityAccess->getEventMessage();
	}
	
	bool DataPoint::pscadaOperationCheck(TA_Base_Bus::EControlOperation operation)
	{
		bool ret = false;
		if (CONTROL_SELECT == operation &&
			(CompletedWithError == m_controlCommandState || CompletedWithSuccess == m_controlCommandState))
		{
			ret = true;
		}
		if (CONTROL_EXECUTE == operation && selectCmdConfirm == m_controlState.controlState)
		{
			ret = true;
		}
		if (CONTROL_CANCEL == operation && selectCmdConfirm == m_controlState.controlState)
		{
			ret = true;
		}
		return ret;
	}

	DpValue  DataPoint::getRawDpValue()
	{
		FUNCTION_ENTRY("getRawDpValue");
		FUNCTION_EXIT;		
		return m_fieldValue;
	}
	
	const std::string & DataPoint::getAssetName()
	{
		if (NotLoaded == m_assetName)
		{
			//TA_Base_Core::DataNodeEntityData dnData(m_dataPointEntityAccess->getParent());
			TA_Base_Core::IEntityDataPtr parentEntityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(m_dataPointEntityAccess->getParent());

			if (parentEntityData.get() != NULL)
			{
                //TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(loadedEntityData);
                TA_Base_Core::DataNodeEntityDataPtr dnEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(parentEntityData);        
                if (dnEntityData != NULL)
                {
					m_assetName = dnEntityData->getAssetName();
					return m_assetName;
                }
			}
			
			TA_Base_Core::DataNodeEntityData dnData(m_dataPointEntityAccess->getParent());
			m_assetName = dnData.getAssetName();
		}

		return m_assetName;
	}

	std::string DataPoint::getAlarmDescription(TA_Base_Bus::EDataPointAlarms alarmType, const DpValue& dpValue)
	{
		std::string alarmMessage("");

		ThreadGuard guard( m_alarmslock );
		bool bGeneralAlarm = (alarmType == TA_Base_Bus::DptGeneral);
		DpAlarmList::iterator almIter = m_alarms.begin();
		for (; almIter != m_alarms.end(); ++almIter )
		{
			bool sameType = ((*almIter)->getType() == alarmType);
			if ( (sameType && !bGeneralAlarm) ||
				 (sameType && bGeneralAlarm && ((*almIter)->getValue() == dpValue)))
			{
				alarmMessage = (*almIter)->getMessage();
				break;
			}
		}

		return alarmMessage;
	}

	std::string DataPoint::getEventDescription()
	{
		if ( 0 == m_eventDetailsFactory )
		{
			m_eventDetailsFactory = EventDetailsFactory::getInstance();
		}

		const TA_Base_Core::DataPointEventDetails& eventDetails = 
			m_eventDetailsFactory->getEventDetails(m_entityKey, m_dataPointEntityAccess->getAgentName());

		return eventDetails.eventMessage;
	}
  
    void DataPoint::setControlCommandState(EControlCommandState state)
    {
        m_controlCommandState = state;
    }

}

