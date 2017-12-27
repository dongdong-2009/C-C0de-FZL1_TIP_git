/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ross Tucker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifdef _MSC_VER
  #pragma warning(disable: 4786)
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include <sstream>
#include <algorithm>
#include <time.h>

#include "app/alarm/alarm_agent/src/AlarmQueueWrapper.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "app/alarm/alarm_agent/src/AlarmQueue.h"
#include "app/alarm/alarm_agent/src/DatabaseLoader.h"
#include "app/alarm/alarm_agent/src/AbstractRule.h"
#include "app/alarm/alarm_agent/src/RuleRepository.h"
#include "app/alarm/alarm_agent/src/PlanExecutionRule.h"
#include "app/alarm/alarm_agent/src/SuppressionRule.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/AlarmAgentEntityData.h"
#include "core/data_access_interface/src/AlarmAccessFactory.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/data_access_interface/src/AlarmData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h"
#include "core/data_access_interface/plans/src/IAlarmPlanAssocData.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocData.h"
#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"


#include "core/alarm/src/AlarmConfig.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/src/AlarmSender.h"
#include "core/alarm/src/AlarmPublicationManager.h"
// TD 7378
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/AlarmAgentAlarm_MessageTypes.h"
// TD 7378
#include "core/exceptions/src/AlarmAgentException.h"
#include "core/exceptions/src/AlarmCacheCapacityExceededException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
// TD 13530 Audit message to be sent by AlarmQueue after sending alarm DB updates
 #include "core/message/src/AuditMessageSender.h"  //20448++
#include "core/message/src/NameValuePair.h" //20448++
#include "core/message/src/NameValuePair.h"
#include "core/message/types/AlarmUpdate_MessageTypes.h"
#include "core/message/types/AlarmStateUpdate_MessageTypes.h"
#include "core/message/types/AlarmAudit_MessageTypes.h" //20448++
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/src/MessageConfig.h"
#include "core/utilities/src/Timer.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
//Mintao ++
//TD13650
#include "bus/scada/common_library/src/ScadaUtilities.h"
//Mintao ++
//TD13650



using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::Timer;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::AlarmAgentException;
using TA_Base_Core::AlarmCacheCapacityExceededException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::AlarmSequence;
using TA_Base_Core::AlarmMessageCorbaDef;
using TA_Base_Core::AlarmDetailCorbaDef;
using TA_Base_Core::AlarmUpdateCorbaDef;
using TA_Base_Core::AlarmAgentQueueItemCorbaDef;
using TA_Base_Core::AlarmAgentQueueItemSequence;
using TA_Base_Core::AlarmSender;
using TA_Base_Core::AlarmPublicationManager;
using TA_Base_Core::MessagePublicationManager;
using TA_Base_Core::IAlarmData;
using TA_Base_Core::IEntityData;
using TA_Base_Core::AlarmAccessFactory;
using TA_Base_Core::EntityAccessFactory;

namespace
{
	//TD14082
	//marvin++
    static const short ALARM_AGENT_TIMEOUT(300); // secs
	//TD14082
	//++marvin
}

namespace TA_Base_App
{
	inline void initAlarmUpdateCorbaDef(AlarmUpdateCorbaDef* updateItem)
	{
	/*
		// AlarmUpdateCorbaDef theUpdate;
		//    CORBA::String_member messageTypeKey;
		updateItem->messageTypeKey					= CORBA::string_dup("");//C830 this is ULong
		//    CORBA::String_member messageContext;
		updateItem->messageContext					= CORBA::string_dup("");
		//    CORBA::ULong assocEntityKey;
		updateItem->assocEntityKey					= 0;
		//    CORBA::ULong locationKey;
		updateItem->locationKey						= 0;
		//    CORBA::String_member alarmID;
		updateItem->alarmID 						= CORBA::string_dup("");
		//    CORBA::String_member alarmDescription;
		updateItem->alarmDescription				= CORBA::string_dup("");
		//    CORBA::String_member assetName;
		updateItem->assetName						= CORBA::string_dup("");
		//    CORBA::String_member alarmComments;
		updateItem->alarmComments					= CORBA::string_dup("");
		//    CORBA::String_member alarmAcknowledgedBy;
		updateItem->alarmAcknowledgedBy				= CORBA::string_dup("");
		//    DateTime ackTime;
		updateItem->ackTime							= 0;
		//    DateTime closeTime;
		updateItem->closeTime						= 0;
		//    CORBA::String_member closeValue;
		updateItem->closeValue						= CORBA::string_dup("");
		//    CORBA::String_member session;
		updateItem->session							= CORBA::string_dup("");
	*/
		//    DecisionSupportStateType decisionSupportState;
		updateItem->decisionSupportState			= TA_Base_Core::NoRelatedPlan;
		//    MmsStateType mmsState;
		updateItem->mmsState						= TA_Base_Core::MMS_NONE;
	/*
		//    CORBA::ULong messageSequenceNumber;
		updateItem->messageSequenceNumber			= 0;
		//    CORBA::Boolean alarmHasBeenAcknowledged;
		updateItem->alarmHasBeenAcknowledged		= false;
		//    CORBA::Boolean alarmHasBeenOpened;
		updateItem->alarmHasBeenOpened				= false;
		//    CORBA::Boolean alarmHasBeenClosed;
		updateItem->alarmHasBeenClosed				= false;
		//    CORBA::Boolean commentsWereAdded;
		updateItem->commentsWereAdded				= false;
		//    CORBA::Boolean decisionSupportStateHasChanged;
		updateItem->decisionSupportStateHasChanged	= false;
		//    CORBA::Boolean mmsStateHasChanged;
		updateItem->mmsStateHasChanged				= false;
	*/
	}


	inline void initAlarmAgentQueueItemCorbaDef(AlarmAgentQueueItemCorbaDef* item)
	{
	/*
		// CORBA::Boolean containsAlarmDetailCorbaDef;
		item->containsAlarmDetailCorbaDef 				= false;
		// CORBA::Boolean containsAlarmUpdateCorbaDef;
		item->containsAlarmUpdateCorbaDef 				= false;
		// CORBA::Boolean containsAlarmDetailForPrintingCorbaDef;
		item->containsAlarmDetailForPrintingCorbaDef 	= false;
		// CORBA::Boolean isOperatorTriggered;
		item->isOperatorTriggered						= false;
		
		// AlarmDetailCorbaDef theMessage;
		//    MillisecondTime sourceTime;
		item->theMessage.sourceTime.time 				= 0;
		item->theMessage.sourceTime.millitm 			= 0;
		//    MillisecondTime transactiveTime;
		item->theMessage.transactiveTime.time 			= 0;
		item->theMessage.transactiveTime.millitm 		= 0;
		//    DateTime ackTime;
		item->theMessage.ackTime						= 0;
		//    DateTime closeTime;
		item->theMessage.closeTime						= 0;
		//    MillisecondTime displayTime;
		item->theMessage.displayTime.time			 	= 0;
		item->theMessage.displayTime.millitm 			= 0;
		//    CORBA::String_member messageTypeKey;
		item->theMessage.messageTypeKey				 	= CORBA::string_dup(""); //C830 this is ULong
		//    CORBA::String_member messageContext;
		item->theMessage.messageContext				 	= CORBA::string_dup("");
		//    CORBA::ULong assocEntityKey;
		item->theMessage.assocEntityKey				 	= 0;
		//    CORBA::ULong assocEntityTypeKey;
		item->theMessage.assocEntityTypeKey				= 0;
		//    CORBA::String_member assetName;
		item->theMessage.assetName				 		= CORBA::string_dup("");
		//    CORBA::String_member alarmID;
		item->theMessage.alarmID				 		= CORBA::string_dup("");
		//    CORBA::ULong alarmSeverity;
		item->theMessage.alarmSeverity				 	= 0;
		//    CORBA::String_member alarmDescription;
		item->theMessage.alarmDescription				= CORBA::string_dup("");
		//    CORBA::String_member alarmParameterList;
		item->theMessage.alarmParameterList				= CORBA::string_dup("");
		//    CORBA::String_member alarmAcknowledgedBy;
		item->theMessage.alarmAcknowledgedBy			= CORBA::string_dup("");
		//    CORBA::String_member alarmAcknowledgedSession;
		item->theMessage.alarmAcknowledgedSession	 	= CORBA::string_dup("");
	*/
		//    AlarmState state;
		item->theMessage.state				 			= TA_Base_Core::AlarmOpen;
	/*
		//    CORBA::String_member alarmComments;
		item->theMessage.alarmComments				 	= CORBA::string_dup("");
		//    CORBA::String_member entityName;
		item->theMessage.entityName				 		= CORBA::string_dup("");
		//    CORBA::ULong locationKey;
		item->theMessage.locationKey					= 0;
		//    CORBA::ULong subsystemKey;
		item->theMessage.subsystemKey				 	= 0;
		//    CORBA::String_member parentAlarmID;
		item->theMessage.parentAlarmID				 	= CORBA::string_dup("");
		//    CORBA::String_member avalancheHeadID;
		item->theMessage.avalancheHeadID				= CORBA::string_dup("");
		//    CORBA::Boolean isHeadOfAvalanche;
		item->theMessage.isHeadOfAvalanche				= false;
		//    CORBA::Boolean isChildOfAvalanche;
		item->theMessage.isChildOfAvalanche				= false;
		//    CORBA::Boolean isUniqueAlarm;
		item->theMessage.isUniqueAlarm				 	= false;
	*/
		//    DecisionSupportStateType decisionSupportState;
		item->theMessage.decisionSupportState	 		= TA_Base_Core::NoRelatedPlan;
		//    MmsStateType mmsState;
		item->theMessage.mmsState						= TA_Base_Core::MMS_NONE;
	/*
		//    CORBA::ULong messageSequenceNumber;
		item->theMessage.messageSequenceNumber 			= 0;
		//    CORBA::String_member alarmValue;
		item->theMessage.alarmValue						= CORBA::string_dup("");
	*/
		
		// AlarmUpdateCorbaDef theUpdate;
		initAlarmUpdateCorbaDef( &(item->theUpdate) );
	}


	// Max number of 40000 alarms to cache but the alarm list control should limit the displayable alarms
    const unsigned int AlarmCache::MAX_CACHED_ALARMS = 40000;

    AlarmCache* AlarmCache::m_instance = 0;
    TA_Base_Core::NonReEntrantThreadLockable AlarmCache::m_singletonLock;
   
    //
    // getInstance
    //
    AlarmCache* AlarmCache::getInstance()
    {
        ThreadGuard guard(m_singletonLock);
        
        if( m_instance == 0 )
        {
            m_instance = new AlarmCache();
        }
        return m_instance;
    }


	//
    // ReleaseInstance
    //
    void AlarmCache::releaseInstance()
    {
        ThreadGuard guard(m_singletonLock);
        
        if( m_instance != 0 )
        {
            delete m_instance ;
        }
        m_instance=NULL;
    }


    //
    // Private Constructor
    //
    AlarmCache::AlarmCache() :
		m_corbaTimeout(0),
		m_maximumReturnableAlarms(15000),
		m_dataLoaded(false),
		m_stateMessageSequenceCount(1),
		m_messageSequenceCount(1)
    {
        m_dbLoader = 0;

        loadAlarmPlanAssociations();

        m_queue = new AlarmQueue();
        m_queue->start();

		// TD 13530 Audit message to be sent by AlarmQueue
        //20448++
		m_auditSender = MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::AlarmAudit::Context);

        m_stateSender = MessagePublicationManager::getInstance().getStateUpdateMessageSender(
                            TA_Base_Core::AlarmStateUpdate::Context);

        m_alarmSender = AlarmPublicationManager::getInstance().getAlarmSender(
                            TA_Base_Core::Alarm::Context);

        m_authLib = new TA_Base_Bus::AuthenticationLibrary();

        // Need to register for online updates for alarm-plan-associations so these can be updated for the alarm if they change
        TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::ALARM_PLAN_ASSOCIATION ,*this );
		//Tianxiang speed up processing alarms
		TA_Base_Core::AlarmTypeTableAccessor::getInstance().loadAllAlarmTypes(); // load all alarm type into cache
		//Tianxiang speed up processing alarms
		
    }


    //
    // Destructor
    //
    AlarmCache::~AlarmCache()
    {
        TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::ALARM_PLAN_ASSOCIATION, *this );

        if( m_queue != NULL )
        {
            m_queue->terminateAndWait();
            delete m_queue;
            m_queue = 0;
        }
		
		//delete AlarmMMSSubmitter instance
		AlarmMMSSubmitter::getInstance()->terminateAndWait();
		AlarmMMSSubmitter::releaseInstance();

		// TD 13530 Audit message to be sent by AlarmQueue
        //20448++
		if ( m_auditSender != NULL )
        {
            delete m_auditSender;
            m_auditSender = 0;
        }

        if( m_stateSender != NULL )
        {
            delete m_stateSender;
            m_stateSender = 0;
        }

        if( m_alarmSender != NULL )
        {
            delete m_alarmSender;
            m_alarmSender = 0;
        }

        if( m_dbLoader != NULL )
        {
            m_dbLoader->terminateAndWait();
            delete m_dbLoader;
            m_dbLoader = 0;
        }

        std::map<unsigned long, TA_Base_Core::IAlarmPlanAssocData*>::iterator iter = m_alarmPlanAssociations.begin();
        for(; iter != m_alarmPlanAssociations.end(); ++iter)
        {
            if(iter->second != NULL)
            {
                delete iter->second;
                iter->second = NULL;
            }
        }

        m_entityToAlarmMap.clear();
		//Clear deque 
		while(!m_CurrentAlarmsToBeClosed.empty())
		{
			m_CurrentAlarmsToBeClosed.pop_front();
		}

        for( AlarmIdToAlarmMap::iterator p = m_cache.begin(); p != m_cache.end(); p++ )
        {
            if( p->second != NULL )
            {
                delete p->second;
                p->second = 0;
            }
        }
        m_cache.clear();

		//Clear Operator Cache
		std::map<std::string, TA_Base_Core::IOperator*>::iterator itrDelOp = m_operatorMap.begin();
		for(; itrDelOp != m_operatorMap.end(); ++itrDelOp)
		{
			if(itrDelOp->second != NULL)
			{
				delete itrDelOp->second;
				itrDelOp->second = NULL;
			}
		}
		m_operatorMap.clear();

        if( m_authLib != NULL )
        {
            delete m_authLib;
            m_authLib = 0;
        }
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
     }


    //
    // terminate
    //
    void AlarmCache::terminate()
    {
        m_queue->terminateAndWait();
    }


    //
    // insert
    //
    bool AlarmCache::insert( TA_Base_Core::AlarmDetailCorbaDef& p_alarm,
                             bool p_multipleOccurancesAllowed )
    {
        FUNCTION_ENTRY( "insert" );

        ThreadGuard guard( m_cacheLock );
		bool bRetVal = true;

        AlarmIdToAlarmMap::iterator cacheItr = m_cache.find(p_alarm.alarmID.in());
        if( cacheItr != m_cache.end())
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                 "Alarm with id %s already exists", p_alarm.alarmID.in());
            return false;
        }

		if (m_alarmHistoricalQueue.isAlarmSubmitted(p_alarm.alarmID.in()))
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
				"Alarm with id %s has already been submitted", p_alarm.alarmID.in());
			return false;
		}

        
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
             "Inserting an alarm with uuid %s, entitykey %u, type %u and asset %s",
             p_alarm.alarmID.in(),
			 p_alarm.assocEntityKey,
             p_alarm.messageTypeKey,
			 p_alarm.assetName.in());

        if( !p_multipleOccurancesAllowed )
        {
            if( alarmAlreadyRaised(p_alarm) )
            {
                std::stringstream eKey;
                eKey << (unsigned long)p_alarm.assocEntityKey;

				std::stringstream msg;
                msg << "The alarm with the following entity key: " ;
				msg << eKey.str();
				msg << " and alarm type: ";
				msg << p_alarm.messageTypeKey;
				msg << " already exists";
                TA_THROW( AlarmAgentException(msg.str()) );
            }
        }

		//Move code here to allow agent to check if unique alarm has already been raised.
		if (m_cache.size() > MAX_CACHED_ALARMS)
		{
			raiseExceededMaxAlarm("insert new alarm");
			TA_THROW( AlarmCacheCapacityExceededException("Maximum alarm cache level reached. Further alarm submissions will not be accepted until cache size reduced.") );
		}

        // Make a copy of the alarm. DONT DELETE the new alarm pointer because it has
        // been copied into our cache. It will either be deleted when the alarm is
        // closed and acknowledged or when the AlarmCache cleans up.
        AlarmDetailCorbaDef* alarm = new TA_Base_Core::AlarmDetailCorbaDef();

        *alarm = p_alarm;
		//Mintao++
		//TD13650
		//check the global inhibitation state and update it to the correct state
		//check if the MMS inhibit state is ok
		bool isInhibited = false;
		try
		{
			isInhibited = TA_Base_Bus::ScadaUtilities::getInstance().isMmsGlobalInhibited(); //tianxiang speed up alarm update
		}
		catch(...)
		{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					 "Could not determine the MMS inhibition state.");
		}
        
		if(isInhibited)
		{
			if((alarm->mmsState == TA_Base_Core::MMS_AUTO) || (alarm->mmsState == TA_Base_Core::MMS_SEMI))
			{
				// TD 14082 Logging string type incorrect use std::string.c_str()
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					"Global inhibit is already applied, update the mms state from %d to inhibit for alarm: %s ",p_alarm.mmsState , std::string(p_alarm.alarmID.in()).c_str() );
				alarm->mmsState = TA_Base_Core::MMS_INHIBIT;
				p_alarm.mmsState =TA_Base_Core::MMS_INHIBIT;  
			}
		}
		//Mintao++
		//TD13650

        TA_ASSERT(alarm->messageTypeKey != 0, "Alarm message type cannot be 0");

        if( RuleRepository::getInstance()->isAlarmAnAvalancheHead(p_alarm) )
        {
            alarm->isHeadOfAvalanche = true;
        }

        timeb taTime;
        ftime( &taTime );
        alarm->transactiveTime.time = taTime.time;
        alarm->transactiveTime.millitm = taTime.millitm;

        // Truncate the milliseconds to 3 digits.
        alarm->sourceTime.millitm = truncateTo3Digits(alarm->sourceTime.millitm);
		// update the display time depending on close time
		if (alarm->state == TA_Base_Core::AlarmClosed)
		{
			alarm->displayTime.time = alarm->closeTime;
			alarm->displayTime.millitm = 0;
		}
		else
		{
			alarm->displayTime = alarm->sourceTime;
		}
        alarm->transactiveTime.millitm = truncateTo3Digits(alarm->transactiveTime.millitm);

        // TD 7534: Update the decision support state if required
        alarm->decisionSupportState = checkDecisionSupportStateType(alarm->decisionSupportState, alarm->assocEntityTypeKey,
                                                                  alarm->assocEntityKey, alarm->messageTypeKey);

        m_cache[p_alarm.alarmID.in()] = alarm;

        m_entityToAlarmMap.insert(std::pair<unsigned long, AlarmDetailCorbaDef*>
            (p_alarm.assocEntityKey, alarm));

		//++Noel
		m_alarmHistoricalQueue.insert(p_alarm.alarmID.in());

		alarm->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm->messageTypeKey,  std::string(alarm->alarmParameterList.in())).c_str());

        // Queue item for database queue
        AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
        initAlarmAgentQueueItemCorbaDef(queueItem);
        queueItem->containsAlarmDetailCorbaDef = true;
        queueItem->containsAlarmUpdateCorbaDef = false;
        queueItem->containsAlarmDetailForPrintingCorbaDef = false;
		queueItem->isOperatorTriggered = false;
        queueItem->theMessage = *alarm;

        // CL-21508 begin
        AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *alarm );
//         m_queue->push(queueItem);
        m_queue->push(pNewWrapper);
        // CL-21508 end

		// Wenguang++ Alarm Message Size Reduction
		alarm->alarmDescription = CORBA::string_dup("");

		// Note that the event (or audit message) takes the data from the database directly
		// So if the queue item is slower than the event or audit message the M, D & A will not be updated according
		// Do message sending will help in alleviating this problem but it is still not fool proof
        doMessageSendingForNewAlarm(alarm); 

        FUNCTION_EXIT;
		return bRetVal;
    }


    //
    // insert
    //
    void AlarmCache::insertSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm )
    {
        FUNCTION_ENTRY( "insertSuppressed" );

        ThreadGuard guard( m_cacheLock );

        if (m_cache.size() > MAX_CACHED_ALARMS)
        {
			raiseExceededMaxAlarm("insert suppressed alarm");
            TA_THROW( AlarmCacheCapacityExceededException("Maximum alarm cache level reached. Further alarm submissions will not be accepted until cache size reduced.") );
        }

        AlarmIdToAlarmMap::iterator cacheItr = m_cache.find(p_alarm.alarmID.in());
        if( cacheItr != m_cache.end())
        {
            // check alarm state and suppress it if not already so
            if ( p_alarm.state != TA_Base_Core::AlarmSuppressed )
            {
                (cacheItr->second)->state = TA_Base_Core::AlarmSuppressed;
            }
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                 "Alarm with id %s already exists", p_alarm.alarmID.in());
            return;
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Inserting a suppressed alarm with uuid %s and type %u",
             p_alarm.alarmID.in(),
             p_alarm.messageTypeKey);

        // Make a copy of the alarm. DONT DELETE the new alarm pointer because it has
        // been copied into our cache. It will either be deleted when the alarm is
        // closed and acknowledged or when the AlarmCache cleans up.
        AlarmDetailCorbaDef* alarm = new TA_Base_Core::AlarmDetailCorbaDef();

        *alarm = p_alarm;

        TA_ASSERT(alarm->messageTypeKey != 0, "Alarm message type cannot be 0");

        timeb taTime;
        ftime( &taTime );
        alarm->transactiveTime.time = taTime.time;
        alarm->transactiveTime.millitm = taTime.millitm;

        // Truncate the milliseconds to 3 digits.
        alarm->sourceTime.millitm = truncateTo3Digits(alarm->sourceTime.millitm);
        alarm->transactiveTime.millitm = truncateTo3Digits(alarm->transactiveTime.millitm);

		//Initial display time
		alarm->displayTime = alarm->sourceTime;
         // TD 7534: Update the decision support state if required
 
        alarm->decisionSupportState = checkDecisionSupportStateType(alarm->decisionSupportState, alarm->assocEntityTypeKey,
            alarm->assocEntityKey, alarm->messageTypeKey);

        m_cache[p_alarm.alarmID.in()] = alarm;

        m_entityToAlarmMap.insert(std::pair<unsigned long, AlarmDetailCorbaDef*>
            (p_alarm.assocEntityKey, alarm));

		// CL 15035 ++
		alarm->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm->messageTypeKey,  std::string(alarm->alarmParameterList.in())).c_str());
		// ++ CL 15035
		
        // Queue item for database queue
        AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
        initAlarmAgentQueueItemCorbaDef(queueItem);
        queueItem->containsAlarmDetailCorbaDef = true;
        queueItem->containsAlarmUpdateCorbaDef = false;
        queueItem->containsAlarmDetailForPrintingCorbaDef = false;
		queueItem->isOperatorTriggered = false;
        queueItem->theMessage = *alarm;

        // CL-21508 begin
        AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *alarm );
//         m_queue->push(queueItem);
        m_queue->push( pNewWrapper );
        // CL-21508 end

		// Note that the event (or audit message) takes the data from the database directly
		// So if the queue item is slower than the event or audit message the M, D & A will not be updated according
		// Do message sending will help in alleviating this problem but it is still not fool proof
        doMessageSendingForNewAlarm(alarm); // TD 112240 Suppressed alarm should not be sent

        FUNCTION_EXIT;
    }


    //
    // getAll
    //
    TA_Base_Core::AlarmSequence* AlarmCache::getAll()
    {
        FUNCTION_ENTRY( "getAll" );

        Timer timer;
        // Get the CORBA timeout and reduce by a few seconds to ensure we have time to return
        unsigned long timeoutInSeconds = getCORBATimeout() - 5;

        ThreadGuard guard(m_cacheLock);

        AlarmSequence* cachedAlarms = new AlarmSequence();
        cachedAlarms->length(m_cache.size());

        AlarmIdToAlarmMap::const_iterator p = m_cache.begin();
        AlarmIdToAlarmMap::const_iterator end = m_cache.end();

        int i = 0;

        for( p = m_cache.begin(); p!= end; p++ )
        {
            const TA_Base_Core::AlarmDetailCorbaDef* alarm = p->second;

            (*cachedAlarms)[i] = *alarm;
            i++;

            if ( static_cast< unsigned long >((unsigned long)timer.elapsedTime()) > timeoutInSeconds )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                     "CORBA Timeout reached! Only %d out of %d alarms returned", i, m_cache.size());
                break;
            }
        }

        FUNCTION_EXIT;

        return cachedAlarms;
    }


    //
    // getByEntityAndAlarmType
    //
    TA_Base_Core::AlarmSequence* AlarmCache::getByEntityAndAlarmType( unsigned long p_entity, unsigned long p_alarmType )
    {
        FUNCTION_ENTRY( "getByEntityAndAlarmType" );

        Timer timer;
        // Get the CORBA timeout and reduce by a few seconds to ensure we have time to return
        unsigned long timeoutInSeconds = getCORBATimeout() - 5;
        unsigned long length = 0;

        ThreadGuard guard(m_cacheLock);

        AlarmSequence* cachedAlarms = new AlarmSequence();
        cachedAlarms->length(m_entityToAlarmMap.size());

        EntityToAlarmMap::const_iterator p = m_entityToAlarmMap.find(p_entity);
        EntityToAlarmMap::const_iterator end = m_entityToAlarmMap.upper_bound(p_entity);
        EntityToAlarmMap::const_iterator endOfMap = m_entityToAlarmMap.end();

        while( (p != end) && (p != endOfMap) && (length < m_maximumReturnableAlarms) )
        {
            const AlarmDetailCorbaDef* alarm = p->second;

            TA_ASSERT((alarm != NULL), "Alarm cannot be NULL");
            TA_ASSERT(alarm->messageTypeKey != 0, "Message type key cannot be 0");

            unsigned long storedType = alarm->messageTypeKey;

            if( p_alarmType == storedType )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                     "Returning alarm with ID %s, Type %u, Entity key %d", alarm->alarmID.in(),
                                                                           alarm->messageTypeKey,
                                                                           alarm->assocEntityKey);

                (*cachedAlarms)[length] = *alarm;

                length++;
            }

            p++;

            if ( static_cast< unsigned long >( (unsigned long)timer.elapsedTime() ) > timeoutInSeconds )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                     "CORBA Timeout reached! Only %d out of %d alarms returned", length, m_entityToAlarmMap.size());
                break;
            }
        }

        cachedAlarms->length(length);

        FUNCTION_EXIT;

        return cachedAlarms;
    }


    //
    // getByEntity
    //
    TA_Base_Core::AlarmSequence* AlarmCache::getByEntity( unsigned long p_entity )
    {
        FUNCTION_ENTRY( "getByEntity" );

        Timer timer;
        // Get the CORBA timeout and reduce by a few seconds to ensure we have time to return
        unsigned long timeoutInSeconds = getCORBATimeout() - 5;
        unsigned long length = 0;

        ThreadGuard guard(m_cacheLock);

        AlarmSequence* cachedAlarms = new AlarmSequence();
        cachedAlarms->length(m_entityToAlarmMap.size());

		if (p_entity==0)
		{
            //the following flow is used to replace the "getActiveAlarmForEntityOfType" call from SCADA data point library.		
			//when data point library initialize, the data point library will call to 
			//   getActiveAlarmForEntityOfType of alarm agent for getting alarm info of data point;
			//   the old implementation makes alarm agent need to handle a large number of CORBA requests in a short time, 
			//   and it cause alarm agent cannot provide service to other functions, like name resolve...
			//so we use the "getByEntity" with a special parameter 0 replace the "getActiveAlarmForEntityOfType".
			//          the "getByEntity" will return all alarm info interested in data point library.
			//the "getByEntity" is warped in TA_BASE_CORE::AlarmHelper::getActiveAlarms.
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "AlarmCache::getByEntity by zero 0");			
		
			 std::map<std::string, int> activeAlarms;

			 EntityToAlarmMap::const_iterator p = m_entityToAlarmMap.begin();
			 EntityToAlarmMap::const_iterator endOfMap = m_entityToAlarmMap.end();

			 for (;p!=endOfMap;p++)
			 {				 
				 std::string alarmAckedBy(p->second->alarmAcknowledgedBy.in());				 
				 if ( ( p->second->state == TA_Base_Core::AlarmOpen || true == alarmAckedBy.empty() ) && p->second->isUniqueAlarm==true  ) 
				 {					 
					 const AlarmDetailCorbaDef* openAlarm = p->second;				 
					 std::stringstream ss;
					 ss<<openAlarm->assocEntityKey<<"-"<<openAlarm->messageTypeKey;
					 std::string key=ss.str();
					 std::map<std::string, int>::iterator pCache;
                     pCache=activeAlarms.find(key);
					 if (pCache==activeAlarms.end())
					 {
						 activeAlarms.insert(std::pair<std::string,  int>(key, 0) );
						 (*cachedAlarms)[length] = *openAlarm;
						 length++;
					 }				      
				 }
				 if ( static_cast< unsigned long >( (unsigned long)timer.elapsedTime() ) > timeoutInSeconds )
				 {
					 LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
						 "CORBA Timeout reached! Only %d out of %d alarms returned", length, m_entityToAlarmMap.size());
					 break;
				 }
			 }     
             activeAlarms.clear();
			 LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "AlarmCache::getByEntity by zero count=%d",length);
		}
        else
		{	       
			EntityToAlarmMap::const_iterator p = m_entityToAlarmMap.find(p_entity);
			EntityToAlarmMap::const_iterator end = m_entityToAlarmMap.upper_bound(p_entity);
			EntityToAlarmMap::const_iterator endOfMap = m_entityToAlarmMap.end();

			while( (p != end) && (p != endOfMap) && (length < m_maximumReturnableAlarms) )
			{
				const TA_Base_Core::AlarmDetailCorbaDef* alarm = p->second;

				TA_ASSERT((alarm != NULL), "Alarm cannot be NULL");

				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					 "Returning alarm with ID %s, Type %u, Entity key %d", alarm->alarmID.in(),
																		   alarm->messageTypeKey,
																		   alarm->assocEntityKey);

				(*cachedAlarms)[length] = *alarm;
				p++;
				length++;

				if ( static_cast< unsigned long >( (unsigned long)timer.elapsedTime() ) > timeoutInSeconds )
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
						 "CORBA Timeout reached! Only %d out of %d alarms returned", length, m_entityToAlarmMap.size());
					break;
				}
            }
        }

        cachedAlarms->length(length);

        FUNCTION_EXIT;

        return cachedAlarms;
    }


    //
    // getMostRecentByEntity
    //
    TA_Base_Core::AlarmDetailCorbaDef* AlarmCache::getMostRecentByEntity( unsigned long p_entity )
    {
        FUNCTION_ENTRY( "getMostRecentByEntity" );

        ThreadGuard guard(m_cacheLock);

        EntityToAlarmMap::const_iterator p = m_entityToAlarmMap.find(p_entity);
        EntityToAlarmMap::const_iterator mostRecent = m_entityToAlarmMap.find(p_entity);
        EntityToAlarmMap::const_iterator end = m_entityToAlarmMap.upper_bound(p_entity);
        EntityToAlarmMap::const_iterator endOfMap = m_entityToAlarmMap.end();

        if( p == endOfMap )
        {
            return NULL;
        }

        do
        {
            TA_ASSERT((p->second != NULL), "Alarm cannot be NULL");

            if( p->second->sourceTime.time > mostRecent->second->sourceTime.time ||
                (p->second->sourceTime.time == mostRecent->second->sourceTime.time &&
                p->second->sourceTime.millitm >= mostRecent->second->sourceTime.millitm))
            {
                mostRecent = p;
            }
            p++;
        }while( (p != end) && (p != endOfMap) );

        AlarmDetailCorbaDef* alarm = new AlarmDetailCorbaDef();

        const AlarmDetailCorbaDef* mostRecentAlarm = mostRecent->second;

        *alarm = *mostRecentAlarm;

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Returning alarm with ID %s, Type %u, Entity key %d", alarm->alarmID.in(),
                                                                   alarm->messageTypeKey,
                                                                   alarm->assocEntityKey);

        FUNCTION_EXIT;

        return alarm;
    }

    //
    // getActiveAlarmForEntityOfType
    //
    TA_Base_Core::AlarmDetailCorbaDef* AlarmCache::getActiveAlarmForEntityOfType( unsigned long p_alarmTypeKey, unsigned long p_entity )
    {
        FUNCTION_ENTRY( "getActiveAlarmForEntityOfType" );

        ThreadGuard guard(m_cacheLock);
		EntityToAlarmMap::const_iterator p = m_entityToAlarmMap.begin();

		AlarmDetailCorbaDef* alarm = new AlarmDetailCorbaDef();
		bool found = false;

		p = m_entityToAlarmMap.find(p_entity);
		if (p == m_entityToAlarmMap.end())
		{
			delete alarm;
            alarm = NULL;

            std::string msg = "No active alarm exists";
            TA_THROW( AlarmAgentException(msg) );
		}
		unsigned long nCount = m_entityToAlarmMap.count(p_entity);
		for (unsigned long i = 0; i < nCount; i++)
		{
			TA_ASSERT((p->second != NULL), "Alarm cannot be NULL");
			TA_ASSERT((p->second->messageTypeKey != 0), "Message type key cannot be 0");
			
			std::string alarmAckedBy(p->second->alarmAcknowledgedBy.in());
            if ( (p->second->state == TA_Base_Core::AlarmOpen || true == alarmAckedBy.empty()) && p->second->messageTypeKey ==p_alarmTypeKey )
			{
				const AlarmDetailCorbaDef* openAlarm = p->second;
				*alarm = *openAlarm;
				found = true;
				break;
			}
			p++;			
		}

		if (found == true)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
				 "Returning alarm with ID %s, Type %u, Entity key %d", alarm->alarmID.in(),
																	   alarm->messageTypeKey,
																	   alarm->assocEntityKey);
			FUNCTION_EXIT;
			return alarm;
		}
		else
		{
			// delete the alarm
			delete alarm;
			alarm = NULL;
			
            std::string msg = "No active alarm exists";
            TA_THROW( AlarmAgentException(msg) );
		}
    }

    //
    // getAlarmById
    //
    TA_Base_Core::AlarmDetailCorbaDef* AlarmCache::getAlarmById( const std::string& alarmId )
    {
        FUNCTION_ENTRY( "getAlarmById" );

        ThreadGuard guard(m_cacheLock);

        const AlarmDetailCorbaDef* alarm = find(alarmId);

        if( alarm == NULL )
        {
            LOG( SourceInfo, DebugUtil::FunctionExit, "getAlarmById - returning null" );

            return NULL;
        }

        AlarmDetailCorbaDef* copy = new AlarmDetailCorbaDef();

        *copy = *alarm;

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Returning alarm with ID %s, Type %u, Entity key %d", copy->alarmID.in(),
                                                                   copy->messageTypeKey,
                                                                   copy->assocEntityKey);

        FUNCTION_EXIT;

        return copy;
    }


    //
    // openAlarms
    //
    void AlarmCache::openAlarms( const std::vector<std::string>& p_alarmIds )
    {
        FUNCTION_ENTRY( "openAlarms" );

        std::vector<std::string>::const_iterator id = p_alarmIds.begin();
        while (id != p_alarmIds.end() )
        {
            openAlarm( *id );
            id++;
        }
        FUNCTION_EXIT;
    }

    //
    // openAlarm
    //
    void AlarmCache::openAlarm( const std::string& p_alarmId )
    {
        FUNCTION_ENTRY( "openAlarm - by id" );

        ThreadGuard guard( m_cacheLock );
        AlarmDetailCorbaDef* alarm = find(p_alarmId);

        if( alarm == NULL )
        {
			FUNCTION_EXIT;
            return; // Alarm does not exist
        }

        if( RuleRepository::getInstance()->isAlarmSuppressed(*alarm) )
        {
            // alarm still suppressed by another rule
			FUNCTION_EXIT;
            return;
        }

        // only open suppressed alarms and send as new message not update as this alarm has never been sent out
		// remember to update the cache info
        if( alarm->state == TA_Base_Core::AlarmSuppressed )
        {
			// Update the internal cache to AlarmOpen
            alarm->state = TA_Base_Core::AlarmOpen;

			// Send message and process the alarm for other rules
            doMessageSendingForUnsuppressedAlarm(alarm);
			RuleRepository::getInstance()->processAlarm(*alarm, TA_Base_App::AbstractRule::SUBMIT);
			//alarm->isChildOfAvalanche=false; //Remove code to avoid reset of this flag with avalache timeout expires.
        }

        FUNCTION_EXIT;
    }

    //
    // closeAlarm
    //
    void AlarmCache::closeAlarmInternal( TA_Base_Core::AlarmDetailCorbaDef* alarm , bool isOperatorTriggered, const std::string& p_sessionId, 
		const std::string& p_closeValue /*=""*/, unsigned long p_closeTime /*=0*/ )
    {
        FUNCTION_ENTRY( "closeAlarmInternal" );
        //ThreadGuard guard( m_cacheLock );

        //AlarmDetailCorbaDef* alarm = find(p_alarmId);

        if( alarm == NULL )
        {
            return; // Alarm does not exist
        }

        if( alarm->state == TA_Base_Core::AlarmOpen ||
            alarm->state == TA_Base_Core::AlarmSuppressed )
        {
			TA_Base_Core::DateTime curTime = time(NULL);
			//TD17851 - marc_bugfix Add  so that alarm would be erased at one location at a time
			std::string current_AlarmID(alarm->alarmID.in());
			if (!current_AlarmID.empty())
			{
				//push alarm ID to container
				m_CurrentAlarmsToBeClosed.push_back(current_AlarmID);
			}
			try  //add this try..catch. so the "current_AlarmID" can be popped up in any case. 
			{
				bool bNeedNotifyUpdate = (alarm->state==TA_Base_Core::AlarmOpen)?true:false;

				p_closeValue.empty()?alarm->alarmValue=CORBA::string_dup(""):alarm->alarmValue=CORBA::string_dup(p_closeValue.c_str());
				p_closeTime==0? alarm->closeTime = curTime: alarm->closeTime = p_closeTime;
				// update display time to close time
				alarm->displayTime.time = alarm->closeTime;
				alarm->displayTime.millitm = 0;
				alarm->state = TA_Base_Core::AlarmClosed;				

				// We update the alarm value here in order to get the correct alarm value
				// assigned to the alarm update so clients can get the correct value.
				// update the value depending whether it is a system alarm or datapoint
				updateAlarmValue(alarm);

				// no updates for suppressed alarms
				if (true == bNeedNotifyUpdate )
				{
					// Generate the update
					std::auto_ptr<AlarmUpdateCorbaDef> update = std::auto_ptr<AlarmUpdateCorbaDef>(new AlarmUpdateCorbaDef());
					update->messageTypeKey =  alarm->messageTypeKey ;
					update->messageContext = CORBA::string_dup( alarm->messageContext );
					update->assocEntityKey = alarm->assocEntityKey;
					update->locationKey = alarm->locationKey;
					update->alarmID = CORBA::string_dup(alarm->alarmID);
					update->alarmHasBeenClosed = true;
					update->alarmHasBeenOpened = false; 
					update->alarmHasBeenAcknowledged = false;
					update->commentsWereAdded = false;
					update->mmsStateHasChanged = false;
					update->mmsState=alarm->mmsState;
					update->decisionSupportStateHasChanged = false;
					update->decisionSupportState=alarm->decisionSupportState;
					update->alarmAcknowledgedBy = CORBA::string_dup(alarm->alarmAcknowledgedBy);
					update->ackTime = alarm->ackTime;
					update->closeTime = alarm->closeTime;
					p_closeValue.empty()?update->closeValue=CORBA::string_dup(""):update->closeValue=CORBA::string_dup(p_closeValue.c_str());
					update->session = CORBA::string_dup(p_sessionId.c_str());
					// Wenguang++ Alarm Message Size Reduction
					//update->alarmDescription = CORBA::string_dup( alarm->alarmDescription );
					update->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm->messageTypeKey,  std::string(alarm->alarmParameterList.in())).c_str());
					update->assetName = CORBA::string_dup( alarm->assetName );

					
					AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
					initAlarmAgentQueueItemCorbaDef(queueItem);
					queueItem->containsAlarmDetailCorbaDef = false;
					queueItem->containsAlarmUpdateCorbaDef = true;
					queueItem->containsAlarmDetailForPrintingCorbaDef = false;
					queueItem->isOperatorTriggered = isOperatorTriggered;
					queueItem->theUpdate = *update;
					//m_queue->push(queueItem);

				   // CL-21508 begin
					AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *alarm );
					if( !std::string(alarm->alarmAcknowledgedBy.in()).empty() && !isOperatorTriggered )
					{
						pNewWrapper->setIsSendCloseEvent(TA_Base_App::CLOSE_FOR_NOMALIZED);
					}		
	                //                 m_queue->push(queueItem);
					m_queue->push(  pNewWrapper );
					// CL-21508 end

					// We assign the close value from the alarmDetail here so our MFT's 
					// can receive the correct value that needs to be displayed
					update->closeValue = CORBA::string_dup(alarm->alarmValue.in());

					// Wenguang++ Alarm Message Size Reduction
					update->alarmDescription = CORBA::string_dup("");
						doMessageSendingForUpdatedAlarm(update.get(), alarm->entityName.in(), "", alarm->assetName.in(), true, p_closeValue);
				}
				

				RuleRepository::getInstance()->processAlarm(*alarm, TA_Base_App::AbstractRule::CLOSE);

				if( !std::string(alarm->alarmAcknowledgedBy.in()).empty() )
				{
					// If the alarm is closed and acknowledged we can remove
					// it from the cache (as well as any other maps it is stored in)

					erase(alarm);
				}
			}			 
			catch (...)
			{
				if (!m_CurrentAlarmsToBeClosed.empty())
				{
					//pop alarm id 
					m_CurrentAlarmsToBeClosed.pop_back();
				}
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "closeAlarm - by alarm type and entity key unknow error");
				throw;  //raise the exception again.
			}
			if (!m_CurrentAlarmsToBeClosed.empty())
			{
				//pop alarm id 
				m_CurrentAlarmsToBeClosed.pop_back();
			}
			
        }
	

        FUNCTION_EXIT;
    }


    //
    // closeAlarm
    //
	void AlarmCache::closeAlarm( const std::string& p_alarmId , bool isOperatorTriggered, const std::string& p_sessionId, 
		const std::string& p_closeValue /*=""*/, unsigned long p_closeTime /*=0*/ )
    {
        FUNCTION_ENTRY( "closeAlarm - by id" );
        ThreadGuard guard( m_cacheLock );		
        AlarmDetailCorbaDef* alarm = find(p_alarmId);
		closeAlarmInternal(alarm,isOperatorTriggered,p_sessionId,p_closeValue,p_closeTime);
		FUNCTION_EXIT;
	}


    //
    // closeAlarm
    //
    void AlarmCache::closeAlarm( unsigned long p_entityKey, unsigned long p_alarmType, bool isOperatorTriggered /*= false*/, 
		const std::string& p_closeValue /*=""*/, unsigned long p_closeTime /*=0*/)
    {
        FUNCTION_ENTRY( "closeAlarm - by alarm type and entity key" );

        ThreadGuard guard( m_cacheLock );

        AlarmVector alarms;
        find( p_alarmType, p_entityKey, alarms );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
             "Find returned %d alarms", alarms.size());

        for( unsigned int i = 0; i < alarms.size(); i++ )
        {
			  //std::string alarmID= (alarms[i])->alarmID.in();
			  closeAlarmInternal(alarms[i],isOperatorTriggered, "", p_closeValue, p_closeTime);
				  
//             if( (alarms[i])->state == TA_Base_Core::AlarmOpen ||
//                 (alarms[i])->state == TA_Base_Core::AlarmSuppressed )
//             {
// 				TA_Base_Core::DateTime curTime = time(NULL);
// 				//TD17851 - marc_bugfix Add  so that alarm would be erased at one location at a time
// 				std::string current_AlarmID(alarms[i]->alarmID.in());
// 				if (!current_AlarmID.empty())
// 				{
// 					m_CurrentAlarmsToBeClosed.push_back(current_AlarmID);
// 				}
// 				try  //add this try..catch. so the "current_AlarmID" can be popped up in any case
// 				{
// 					// no updates for suppressed alarms
// 					if ( (alarms[i])->state == TA_Base_Core::AlarmOpen )
// 					{
// 						AlarmUpdateCorbaDef* pTmpAlarmUpdate = new AlarmUpdateCorbaDef();
// 						std::auto_ptr<AlarmUpdateCorbaDef> alarmUpdate(pTmpAlarmUpdate);
// 
// 						alarmUpdate->messageTypeKey = (alarms[i])->messageTypeKey;
// 						alarmUpdate->messageContext = CORBA::string_dup( (alarms[i])->messageContext );
// 						alarmUpdate->assocEntityKey = (alarms[i])->assocEntityKey;
// 						alarmUpdate->locationKey = (alarms[i])->locationKey;
// 						alarmUpdate->alarmID = CORBA::string_dup((alarms[i])->alarmID);
// 						alarmUpdate->alarmHasBeenClosed = true;
// 						alarmUpdate->alarmHasBeenOpened = false;
// 						alarmUpdate->alarmHasBeenAcknowledged = false;
// 						alarmUpdate->commentsWereAdded = false;
// 						alarmUpdate->mmsStateHasChanged = false;
// 						alarmUpdate->mmsState=(alarms[i])->mmsState;
// 						alarmUpdate->decisionSupportStateHasChanged = false;
// 						alarmUpdate->decisionSupportState=(alarms[i])->decisionSupportState;
// 						alarmUpdate->alarmAcknowledgedBy = CORBA::string_dup((alarms[i])->alarmAcknowledgedBy);
// 						alarmUpdate->ackTime = (alarms[i])->ackTime;
// 						p_closeTime==0? alarmUpdate->closeTime = curTime: alarmUpdate->closeTime = p_closeTime;
// 						p_closeValue.empty()? alarmUpdate->closeValue = CORBA::string_dup(""): alarmUpdate->closeValue = CORBA::string_dup(p_closeValue.c_str());
// 						alarmUpdate->session = CORBA::string_dup("");
// 						// Wenguang++ Alarm Message Size Reduction
// 						//alarmUpdate->alarmDescription = CORBA::string_dup( (alarms[i])->alarmDescription );
// 						alarmUpdate->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( (alarms[i])->messageTypeKey,  std::string((alarms[i])->alarmParameterList.in())).c_str());
// 						alarmUpdate->assetName = CORBA::string_dup( (alarms[i])->assetName );
// 
// 						//cl-19819
// 						TA_Base_Core::IAlarmTypeData* alarmType = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmTypeRecord( alarms[i]->messageTypeKey); //get alarm type from cache instead of query db
// 						if (alarmType == NULL)
// 						{
// 							LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
// 							 "Can not find Alarm type");
// 							return;
// 						}
// 					//Tianxiang speed up processing alarms
// 						if( alarmType->isSystemAlarm())
// 						{
// 							alarmUpdate->closeValue = CORBA::string_dup(alarmType->getNormalValue().c_str());
// 						}
// 
// 
// 						AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
// 						queueItem->containsAlarmDetailCorbaDef = false;
// 						queueItem->containsAlarmUpdateCorbaDef = true;
// 						queueItem->containsAlarmDetailForPrintingCorbaDef = false;
// 						queueItem->isOperatorTriggered = isOperatorTriggered;
// 						queueItem->theUpdate = *(alarmUpdate.get());
// 						//m_queue->push(queueItem);
// 
// 						// update display time to close time & value
// 						alarms[i]->displayTime.time = alarmUpdate->closeTime;
// 						alarms[i]->displayTime.millitm = 0;
// 						alarms[i]->alarmValue = alarmUpdate->closeValue;
// 						// CL-21508 begin
// 						alarms[i]->state = TA_Base_Core::AlarmClosed;   //for write event					
//                     	AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *(alarms[i]) );
// 						//m_queue->push(queueItem);
//                     	m_queue->push(  pNewWrapper );
//                     	// CL-21508 end
// 
// 						// Wenguang++ Alarm Message Size Reduction
// 						alarmUpdate->alarmDescription = CORBA::string_dup("");
// 						doMessageSendingForUpdatedAlarm(alarmUpdate.get(), (alarms[i])->entityName.in(),"", (alarms[i])->assetName.in()); 
//                 }
// 
//                 (alarms[i])->state = TA_Base_Core::AlarmClosed;
// 				// update the value depending whether it is a system alarm or datapoint
// 				updateAlarmValue(alarms[i]);
// 
//                 RuleRepository::getInstance()->processAlarm(*(alarms[i]), TA_Base_App::AbstractRule::CLOSE);
// 
//                 if( !std::string((alarms[i])->alarmAcknowledgedBy.in()).empty() )
//                 {
//                     // If the alarm is closed and acknowledged, we can remove it from the
//                     // cache (as well as any other maps it is stored in)
// 
//                     erase( alarms[i] );
//                 }
// 				}
// 				catch(...)
// 				{
// 					if (!m_CurrentAlarmsToBeClosed.empty())
// 					{
// 						m_CurrentAlarmsToBeClosed.pop_back();
// 					}
// 					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "closeAlarm - by alarm type and entity key unknow error");
// 					throw;  //raise the exception again.
// 				}
// 				if (!m_CurrentAlarmsToBeClosed.empty())
// 				{
// 					//pop alarm id 
// 					m_CurrentAlarmsToBeClosed.pop_back();
// 				}
// 
//             }

        }

        FUNCTION_EXIT;
    }


	//
	// updateAlarmValue
    //
	void AlarmCache::updateAlarmValue(TA_Base_Core::AlarmDetailCorbaDef* alarm)
	{
		bool isClosed=false;
		bool isAck = false;
		bool isSystemAlarm=true;
		std::string newValue("");

		// check for system or non system alarm type
		//Tianxiang speed up processing alarms
		TA_Base_Core::IAlarmTypeData* alarmType = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmTypeRecord( alarm->messageTypeKey); //get alarm type from cache instead of query db
	
		if (alarmType == NULL)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
             "Can not find Alarm type");
			return;
		}
		
		try
		{
			isSystemAlarm = alarmType->isSystemAlarm();
		}
		catch ( const TA_Base_Core::DataException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DataException", "Failed to determine if Alarm was persistent" );
		}
		catch ( const TA_Base_Core::DatabaseException& /*ex */)
		{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
				"DatabaseException", "Failed to connect to database to load AlarmTypeData" );
		}
		catch(...)
		{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
             "Failed to check system alarm");
		}
		//Tianxiang speed up processing alarms
		std::string type = alarmType->getName();
		std::string alarmValue = alarmType->getAlarmValue();
		std::string normalValue = alarmType->getNormalValue();
		
		if (alarm->state == TA_Base_Core::AlarmClosed) isClosed = true;
		if ( !std::string(alarm->alarmAcknowledgedBy.in()).empty() ) isAck = true;

		if (isSystemAlarm && isClosed && !isAck) //alarm is normalized status
		{			 
			alarm->alarmValue = CORBA::string_dup(normalValue.c_str());
			return;
		}

		// TODO: Value submitted by all subsystem or datapoint should be the exact string
		// TODO: Alarm agent should not be doing all these hacking
		std::string oldValue(alarm->alarmValue.in());
		if (oldValue.empty() == true) // get it from the database
		{
			newValue = obtainValueFromParameter(alarmType->getKey(), type,alarm, isSystemAlarm, isClosed);
		}
		else if (isSystemAlarm) // system alarm
		{
			isClosed? newValue = normalValue: newValue = alarmValue;
		}
		else // check whether it is a boolean datapoint type
		{
			// Note: We remove some parts of the this code because of logic error.
			// std::string::find returns the index of first character of the first match
			// otherwise it returns std::string::npos. npos is a static member constant value with 
			// the greatest possible value for an element of type size_t. So in this case std::string::find will
			// always return a value greater than or equal to zero.
			//if (type.find("DpGeneral", 0) >= 0 || type.find("DpFalse", 0) >=0 || type.find("DpTrue", 0) >= 0)
				newValue = std::string( alarm->alarmValue.in() );
			//else
			//	newValue = type + " " + std::string( alarm->alarmValue.in() );
		}

		// put a dash line if the value is empty
		if (newValue.empty() == true) newValue = "-";

		alarm->alarmValue = CORBA::string_dup(newValue.c_str());
	}

    // updateAlarmComment
    //
    void AlarmCache::updateAlarmComment( const std::string& p_alarmId, const std::string& p_comment,
                                         const std::string& p_session )
    {
        FUNCTION_ENTRY( "updateAlarmComment" );

        ThreadGuard guard( m_cacheLock );

        AlarmDetailCorbaDef* alarm = find(p_alarmId);

        if( alarm == NULL )
        {
            return;
        }

        alarm->alarmComments = CORBA::string_dup(p_comment.c_str());

		std::auto_ptr<AlarmUpdateCorbaDef> update = std::auto_ptr<AlarmUpdateCorbaDef>(new AlarmUpdateCorbaDef());
        initAlarmUpdateCorbaDef(update.get());
		update->messageTypeKey =  alarm->messageTypeKey ;
		update->messageContext = CORBA::string_dup( alarm->messageContext );
		update->assocEntityKey = alarm->assocEntityKey;
		update->locationKey = alarm->locationKey;
        update->alarmID = CORBA::string_dup(alarm->alarmID);
        update->alarmHasBeenClosed = false;
        update->alarmHasBeenOpened = false;
        update->alarmHasBeenAcknowledged = false;
        update->commentsWereAdded = true;
        update->mmsStateHasChanged = false;
		update->mmsState=alarm->mmsState;   //alarm agent cannot start on monitor mode when control mode agent has the queue data.
        update->decisionSupportStateHasChanged = false;
		update->decisionSupportState=alarm->decisionSupportState;
        update->alarmAcknowledgedBy = CORBA::string_dup(alarm->alarmAcknowledgedBy);
        update->ackTime = alarm->ackTime;
        update->closeTime = alarm->closeTime;
        update->alarmComments = CORBA::string_dup(p_comment.c_str());
        update->session = CORBA::string_dup(p_session.c_str());
		// Wenguang++ Alarm Message Size Reduction
		//update->alarmDescription = CORBA::string_dup( alarm->alarmDescription );
		update->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm->messageTypeKey,  std::string(alarm->alarmParameterList.in())).c_str());
		update->assetName = CORBA::string_dup( alarm->assetName );

        AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
        initAlarmAgentQueueItemCorbaDef(queueItem);
        queueItem->containsAlarmDetailCorbaDef = false;
        queueItem->containsAlarmUpdateCorbaDef = true;
        queueItem->containsAlarmDetailForPrintingCorbaDef = false;
		queueItem->isOperatorTriggered = true;
        queueItem->theUpdate = *update;

        // CL-21508 begin
        AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *alarm );
//         m_queue->push(queueItem);
        m_queue->push(  pNewWrapper );
        // CL-21508 end

		// Wenguang++ Alarm Message Size Reduction
		update->alarmDescription = CORBA::string_dup("");
        doMessageSendingForUpdatedAlarm(update.get(), alarm->entityName.in(), p_session, alarm->assetName.in());

        FUNCTION_EXIT;
    }


    //
    // updateAlarmAcknowledgeStateNonReentrant
    //
    void AlarmCache::updateAlarmAcknowledgeStateNonReentrant( const std::string& p_alarmId, const std::string& p_acknowledgedBy,
                                                  const std::string& p_session, bool isOperatorTriggered /*= false*/ )
    {
        FUNCTION_ENTRY( "updateAlarmAcknowledgeStateNonReentrant" );

        ThreadGuard guard( m_cacheLock );
        updateAlarmAcknowledgeStateReentrant(p_alarmId, p_acknowledgedBy, p_session, isOperatorTriggered);

        FUNCTION_EXIT;
    }


    //
    // updateAlarmAcknowledgeStateReentrant
    //
    void AlarmCache::updateAlarmAcknowledgeStateReentrant( const std::string& p_alarmId, const std::string& p_acknowledgedBy,
                                                           const std::string& p_session, bool isOperatorTriggered /*= false*/  )
    {
        FUNCTION_ENTRY( "updateAlarmAcknowledgeStateReentrant" );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Acknowledging alarm with ID = %s", p_alarmId.c_str());

        AlarmDetailCorbaDef* alarm = find(p_alarmId);

        if( alarm == NULL )
        {
            return;
        }

		// TD #9163
		// check that this alarm has not already been acknowledged by someone else
		if( (alarm->alarmAcknowledgedBy.in() != NULL) && (strlen(alarm->alarmAcknowledgedBy.in()) > 0) )
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Alarm already acknowledged by %s", alarm->alarmAcknowledgedBy.in());
			//fix alarm memory, we found that alarms will not be deleted in the memory, though alarm has been closed and ack.
			//so we add the following code to solve this situation.
			if (alarm->state==TA_Base_Core::AlarmClosed)
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Alarm already closed by %s  alarm id=%s", alarm->alarmAcknowledgedBy.in(), alarm->alarmID.in());
				erase( alarm );
			}
			return;
		}

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
			"Acknowledging alarm with uuid %s, entitykey %u, type %u and asset %s",
			alarm->alarmID.in(),
			alarm->assocEntityKey,
			alarm->messageTypeKey,
			alarm->assetName.in());

        alarm->alarmAcknowledgedBy = CORBA::string_dup(p_acknowledgedBy.c_str());
        alarm->alarmAcknowledgedSession = CORBA::string_dup(p_session.c_str());
        alarm->ackTime = time(NULL);
		updateAlarmValue(alarm);

		std::auto_ptr<AlarmUpdateCorbaDef> update = std::auto_ptr<AlarmUpdateCorbaDef>(new AlarmUpdateCorbaDef());
        initAlarmUpdateCorbaDef(update.get());
		update->messageTypeKey =  alarm->messageTypeKey;
		update->messageContext = CORBA::string_dup( alarm->messageContext );
		update->assocEntityKey = alarm->assocEntityKey;
		update->locationKey = alarm->locationKey;
        update->alarmID = CORBA::string_dup(alarm->alarmID);
        update->alarmHasBeenClosed = false;
        update->alarmHasBeenOpened = false;
        update->alarmHasBeenAcknowledged = true;
        update->commentsWereAdded = false;
        update->mmsStateHasChanged = false;
		update->mmsState=alarm->mmsState;
        update->decisionSupportStateHasChanged = false;
		update->decisionSupportState=alarm->decisionSupportState;
        update->alarmAcknowledgedBy = CORBA::string_dup(p_acknowledgedBy.c_str());
        update->ackTime = time(NULL);
        update->closeTime = alarm->closeTime;
        update->session = CORBA::string_dup(p_session.c_str());
		// Wenguang++ Alarm Message Size Reduction
		//update->alarmDescription = CORBA::string_dup( alarm->alarmDescription );
		update->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm->messageTypeKey,  std::string(alarm->alarmParameterList.in())).c_str());
		update->assetName = CORBA::string_dup( alarm->assetName );

        AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
        initAlarmAgentQueueItemCorbaDef(queueItem);
        queueItem->containsAlarmDetailCorbaDef = false;
        queueItem->containsAlarmUpdateCorbaDef = true;
        queueItem->containsAlarmDetailForPrintingCorbaDef = false;
		queueItem->isOperatorTriggered = isOperatorTriggered;
        queueItem->theUpdate = *update;

		std::string c_AlarmID(alarm->alarmID.in());
		std::deque<std::string>::iterator alarmIDIterator = std::find(m_CurrentAlarmsToBeClosed.begin(), 
															m_CurrentAlarmsToBeClosed.end(), c_AlarmID);

        // CL-21508 begin
        AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *alarm );
		if (alarmIDIterator == m_CurrentAlarmsToBeClosed.end())
		{
	        if( alarm->state != TA_Base_Core::AlarmOpen && isOperatorTriggered != false)
		    {
				pNewWrapper->setIsSendCloseEvent(TA_Base_App::CLOSE_FOR_ACK);
			}
		}

		
//         m_queue->push(queueItem);
        m_queue->push(  pNewWrapper );
        // CL-21508 end

		// Wenguang++ Alarm Message Size Reduction
		update->alarmDescription = CORBA::string_dup("");
        doMessageSendingForUpdatedAlarm(update.get(), alarm->entityName.in(), p_acknowledgedBy, alarm->assetName.in());

        RuleRepository::getInstance()->processAlarm(*alarm, TA_Base_App::AbstractRule::ACK);

		

		//If Alarm is to be closed, no need to delete here, since calling function would erase alarm
		if (alarmIDIterator == m_CurrentAlarmsToBeClosed.end())
		{
	        if( alarm->state != TA_Base_Core::AlarmOpen )
		    {
			    // If the alarm is closed or suppressed, we can remove it from the
				// cache (as well as any other maps it is stored in)
				
				erase( alarm );
			}

		}
	
        FUNCTION_EXIT;
    }


    //
    // updateDecisionSupportState
    //
    void AlarmCache::updateDecisionSupportState( const std::string& p_alarmId, const TA_Base_Core::DecisionSupportStateType& p_state,
                                                 const std::string& p_session )
    {
        FUNCTION_ENTRY( "updateDecisionSupportState" );

        ThreadGuard guard( m_cacheLock );

        AlarmDetailCorbaDef* alarm = find(p_alarmId);

        if( alarm == NULL )
        {
            return;
        }

        alarm->decisionSupportState = p_state;

		std::auto_ptr<AlarmUpdateCorbaDef> update = std::auto_ptr<AlarmUpdateCorbaDef>(new AlarmUpdateCorbaDef());
        initAlarmUpdateCorbaDef(update.get());
		update->messageTypeKey = alarm->messageTypeKey;
		update->messageContext = CORBA::string_dup( alarm->messageContext );
		update->assocEntityKey = alarm->assocEntityKey;
		update->locationKey = alarm->locationKey;
        update->alarmID = CORBA::string_dup(alarm->alarmID);
        update->alarmHasBeenClosed = false;
        update->alarmHasBeenOpened = false;
        update->alarmHasBeenAcknowledged = false;
        update->commentsWereAdded = false;
        update->mmsStateHasChanged = false;
		update->mmsState=alarm->mmsState;
        update->decisionSupportStateHasChanged = true;
        update->alarmAcknowledgedBy = CORBA::string_dup(alarm->alarmAcknowledgedBy);
        update->ackTime = alarm->ackTime;
        update->closeTime = alarm->closeTime;
        update->alarmComments = alarm->alarmComments;
        update->session = CORBA::string_dup(p_session.c_str());
        update->decisionSupportState = p_state;
		// Wenguang++ Alarm Message Size Reduction
		//update->alarmDescription = CORBA::string_dup( alarm->alarmDescription );
		update->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm->messageTypeKey,  std::string(alarm->alarmParameterList.in())).c_str()) ;
		update->assetName = CORBA::string_dup( alarm->assetName );

        AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
        initAlarmAgentQueueItemCorbaDef(queueItem);
        queueItem->containsAlarmDetailCorbaDef = false;
        queueItem->containsAlarmUpdateCorbaDef = true;
        queueItem->containsAlarmDetailForPrintingCorbaDef = false;
		queueItem->isOperatorTriggered = false;
        queueItem->theUpdate = *update;

        // CL-21508 begin
        AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *alarm );
//         m_queue->push(queueItem);
        m_queue->push(  pNewWrapper );
        // CL-21508 end

		// Wenguang++ Alarm Message Size Reduction
		update->alarmDescription = CORBA::string_dup("");
        doMessageSendingForUpdatedAlarm(update.get(), alarm->entityName.in(), p_session, alarm->assetName.in());

        FUNCTION_EXIT;
    }


    //
    // updateMMSState
    //
    void AlarmCache::updateMMSState( const std::string& p_alarmId, const TA_Base_Core::MmsStateType& p_state )
    {
        FUNCTION_ENTRY( "updateMMSState" );

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Updating mms state of alarm %s to %d", p_alarmId.c_str(), p_state);

		if(( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR)) &&
		(p_state == TA_Base_Core::MMS_ALM_SENT))
		{
			//received from control agent after saving the MMS Service request in DB
			//may be alarm is acknowledged and normalised so the alarm is not in cache
			//remove the mms alarm from the AlarmMMSSubmitter quene based on Alarm ID
			AlarmMMSSubmitter::getInstance()->remove(p_alarmId);
		}

        ThreadGuard guard( m_cacheLock );

        AlarmDetailCorbaDef* alarm = find(p_alarmId);

        if( alarm == NULL )
        {
            return;
        }

		updateAlarmMMSState(alarm,p_state);
		FUNCTION_EXIT;
	}

	void AlarmCache::updateAlarmMMSStateToSend( AlarmDetailCorbaDef* alarm)
	{
		FUNCTION_ENTRY( "updateMMSStateToSend" );

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
			"Updating mms state of alarm %s to %d", alarm->alarmID.in(), TA_Base_Core::MMS_ALM_SENT);
		{
			ThreadGuard guard( m_cacheLock );

			//update mms state of the alarm object in cache
			AlarmDetailCorbaDef* pAlarm = find(alarm->alarmID.in());
			if(pAlarm != NULL)
			{
				//pAlarm->mmsState = TA_Base_Core::MMS_ALM_SENT;
				//alarm = pAlarm;
				updateAlarmMMSState(pAlarm,TA_Base_Core::MMS_ALM_SENT);	
				FUNCTION_EXIT;
				return;
			}
		}

		//alarm is already acknowldeged and normalised
		// update the mms state to notify the monitor agent.
		updateAlarmMMSState(alarm,TA_Base_Core::MMS_ALM_SENT);	
		FUNCTION_EXIT;
	}



	void AlarmCache::updateAlarmMMSState( AlarmDetailCorbaDef* alarm, const TA_Base_Core::MmsStateType& p_state )
	 {
		FUNCTION_ENTRY("updateAlarmMMSState");
		
		alarm->mmsState = p_state;

		std::auto_ptr<AlarmUpdateCorbaDef> update = std::auto_ptr<AlarmUpdateCorbaDef>(new AlarmUpdateCorbaDef());
        initAlarmUpdateCorbaDef(update.get());
		update->messageTypeKey = alarm->messageTypeKey;
		update->messageContext = CORBA::string_dup( alarm->messageContext );
		update->assocEntityKey = alarm->assocEntityKey;
		update->locationKey = alarm->locationKey;
        update->alarmID = CORBA::string_dup(alarm->alarmID);
        update->alarmHasBeenClosed = false;
        update->alarmHasBeenOpened = false;
        update->alarmHasBeenAcknowledged = false;
        update->commentsWereAdded = false;
        update->decisionSupportStateHasChanged = false;
		update->decisionSupportState=alarm->decisionSupportState;
        update->mmsStateHasChanged = true;
        update->alarmAcknowledgedBy = CORBA::string_dup(alarm->alarmAcknowledgedBy);
        update->ackTime = alarm->ackTime;
        update->closeTime = alarm->closeTime;
        update->alarmComments = alarm->alarmComments;
        update->session = CORBA::string_dup("");
        update->mmsState = p_state;
		// Wenguang++ Alarm Message Size Reduction
		//update->alarmDescription = CORBA::string_dup( alarm->alarmDescription );
		update->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarm->messageTypeKey,  std::string(alarm->alarmParameterList.in())).c_str());
		update->assetName = CORBA::string_dup( alarm->assetName );

        AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
        initAlarmAgentQueueItemCorbaDef(queueItem);
        queueItem->containsAlarmDetailCorbaDef = false;
        queueItem->containsAlarmUpdateCorbaDef = true;
        queueItem->containsAlarmDetailForPrintingCorbaDef = false;
		queueItem->isOperatorTriggered = false;
        queueItem->theUpdate = *update;

        // CL-21508 begin
        AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *alarm );
//         m_queue->push(queueItem);
        m_queue->push(  pNewWrapper );
        // CL-21508 end

		std::string p_session("");

		// Wenguang++ Alarm Message Size Reduction
		update->alarmDescription = CORBA::string_dup("");
		if(p_state == TA_Base_Core::MMS_ALM_SENT)
		{
			doMessageSendingForUpdatedAlarm(update.get(), alarm->entityName.in(), p_session, alarm->assetName.in(),false);
		}
		else
		{
			doMessageSendingForUpdatedAlarm(update.get(), alarm->entityName.in(), p_session, alarm->assetName.in());
		}

	/*	if(( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR)) &&
			(p_state == TA_Base_Core::MMS_ALM_SENT))
		{
			//received from control agent after saving the MMS Service request in DB
			//remove the mms alarm from the AlarmMMSSubmitter quene
			AlarmMMSSubmitter::getInstance()->remove(std::string(alarm->alarmID));
		}
	*/
        FUNCTION_EXIT;
	 }


    //
    // hasDataLoaded
    //
    bool AlarmCache::hasDataLoaded()
    {
        return m_dataLoaded;
    }


    //
    // loadAlarms
    //
    void AlarmCache::loadAlarms(bool bSkipDataLoading)
    {
        FUNCTION_ENTRY( "loadAlarms" );
		if (true == bSkipDataLoading)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skip Alarm loading from database" );
			m_dataLoaded = true;
			return;
		}

        if( m_dbLoader == 0 )
        {
			m_dataLoaded = false;
			m_dbLoader = new DatabaseLoader(this);
			m_dbLoader->start();
        }
        FUNCTION_EXIT;
    }
    
	void AlarmCache::unloadAlarms()
	{
		FUNCTION_ENTRY( "unloadAlarms" );
		ThreadGuard guard(m_cacheLock);
		// Delete the database loader
		if (m_dbLoader != 0)
		{
			m_dbLoader->terminateAndWait();
			delete m_dbLoader;
			m_dbLoader = 0;
		}
		// clean the db queue
		m_queue->clear();
		// delete all existing alarms
		for( AlarmIdToAlarmMap::iterator p = m_cache.begin(); p != m_cache.end(); p++ )
		{
			if( p->second != NULL )
			{
				delete p->second;
				p->second = 0;
			}
		}
		m_cache.clear();
		m_entityToAlarmMap.clear();

		m_dataLoaded = false;

		FUNCTION_EXIT;
	}

    //
    // loadFromDatabase
    //
    void AlarmCache::loadFromDatabase()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "loadFromDatabase entry" );

        ThreadGuard guard(m_cacheLock); // Lock on the whole method
        try
        {
            Timer timer;

            std::vector<IAlarmData*> alarms;

            std::stringstream location( RunParams::getInstance().get(RPARAM_LOCATIONKEY) );
            std::stringstream entityKey( RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ENTITY_KEY.c_str()) );
            unsigned long loc, eKey;
            location >> loc;
            entityKey >> eKey;
			unsigned long maxAlarmFromDataBase=0;

			unsigned int alarmCount = 0;			
            if( 0 != RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ENABLE_FEDERATION.c_str()).compare("true") )
            {
				// TD 7378 check that the number of alarms does not exceed the max limit
				alarmCount = AlarmAccessFactory::getInstance().getDBActiveAlarmCount();
				maxAlarmFromDataBase=alarmCount;
				if (alarmCount >= MAX_CACHED_ALARMS)
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Maximum alarm cache level reached, Agent stops load alarms");					
                    if (RunParams::getInstance().isSet("MaxAlarmFromDB"))
					{
						maxAlarmFromDataBase=atol(RunParams::getInstance().get("MaxAlarmFromDB").c_str());
					}								
					// raise an alarm to display to operator that alarm cannot be loaded
					//raiseExceededMaxAlarm("load alarms from DB",true);
					//TA_THROW( AlarmCacheCapacityExceededException("Maximum alarm cache level reached. Further alarm submissions will not be accepted until cache size reduced.") );					
				}
				if (maxAlarmFromDataBase>=MAX_CACHED_ALARMS || maxAlarmFromDataBase<=0)
				{
					maxAlarmFromDataBase=15000;
				}
				alarms = AlarmAccessFactory::getInstance().getActiveAlarms(maxAlarmFromDataBase);      
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
							 "load alarm from db count=%d", maxAlarmFromDataBase);
            }
            else
            {
				// TD 7378 check that the number of alarms does not exceed the max limit
				alarmCount = AlarmAccessFactory::getInstance().getDBActiveAlarmCountForLocation(loc);
				maxAlarmFromDataBase=alarmCount;
				if (alarmCount >= MAX_CACHED_ALARMS)
				{
					// raise an alarm to display to operator that alarm cannot be loaded
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Maximum alarm cache level reached, Agent stops load alarms");
					//raiseExceededMaxAlarm("load alarms from DB",true);
					//TA_THROW( AlarmCacheCapacityExceededException("Maximum alarm cache level reached. Further alarm submissions will not be accepted until cache size reduced.") );
					if (RunParams::getInstance().isSet("MaxAlarmFromDB"))
					{
						maxAlarmFromDataBase=atol(RunParams::getInstance().get("MaxAlarmFromDB").c_str());
					}					
				}
				if (maxAlarmFromDataBase>=MAX_CACHED_ALARMS || maxAlarmFromDataBase<=0)
				{
					maxAlarmFromDataBase=15000;
				}
				alarms = AlarmAccessFactory::getInstance().getActiveAlarmsForLocation(loc,maxAlarmFromDataBase);
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
							 "load alarm from db count=%d", maxAlarmFromDataBase);
				 
				 
            }
            unsigned int loadfromdb=alarms.size();
			if (alarmCount > 0)
			{
				for( unsigned int i = 0; i < alarms.size(); i++ )
				{
					try
					{
						AlarmDetailCorbaDef* alarm = alarmDataToAlarmMessage(alarms[i]);

						m_cache[alarm->alarmID.in()] = alarm;

						m_entityToAlarmMap.insert(std::pair<unsigned long, AlarmDetailCorbaDef*>
							(alarm->assocEntityKey, alarm));

						LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
							 "Cached alarm %s", alarm->alarmID.in());
					}
					catch( ... )
					{
						LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
							 "Caught an exception while trying to load alarm" );
					}
					delete alarms[i];
				}
			}

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                 "Loaded %li alarms in %f seconds", alarms.size(), timer.elapsedTime());

            alarms.clear();

            m_dataLoaded = true;

            m_alarmSender->sendAlarmsLoadedNotification(loc, eKey);
        }
        catch(const DatabaseException&)
        {
            m_dataLoaded = true;  //force the alarm agent start to work
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "loadFromDatabase exit" );
    }

	// TD 7378
	// raiseExceededMaxAlarm
	//
	void AlarmCache::raiseExceededMaxAlarm(std::string problem)
	{
		FUNCTION_ENTRY("raiseExceededMaxAlarm");

		// log the exception
 		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Alarm Exceeded max cached limit");
		//only raise the alarm when opening alarm in the database arrives max allowable number.
		//so the alarm will not be raised after alarm agent started, otherwise the alarm will be sent to the agent itself recurrence every 5 seconds.
// 		if (raise==true) 
// 		{			
// 			unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
// 			std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
// 
// 			TA_Base_Core::DescriptionParameters dp;
// 			dp.push_back(new TA_Base_Core::NameValuePair("Entity", entityName));
// 			dp.push_back(new TA_Base_Core::NameValuePair("Problem", problem));
// 
// 			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::AlarmAgentAlarm::AlarmAgentCacheMaxAlarm,
// 							atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ENTITY_KEY.c_str()).c_str()),
// 							atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ENTITY_TYPE_KEY.c_str()).c_str()),
// 							dp, entityName,locationKey,0,
// 							RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::ASSET_NAME.c_str()),
// 							TA_Base_Core::AlarmConstants::defaultTime );
//		}

		// set the database loaded to indicate that loading will not continue ie done
		if (m_dataLoaded==false) m_dataLoaded = true;
		
		FUNCTION_EXIT;
	}

	//
	// terminateDataLoading
	//
	void AlarmCache::terminateDataLoading()
	{
		FUNCTION_ENTRY("terminateDataLoading");
		
		m_dbLoader->terminate();

		FUNCTION_EXIT;
	}


    //
    // requestFromControllingAgent
    //
    void AlarmCache::requestFromControllingAgent()
    {
        FUNCTION_ENTRY( "requestFromControllingAgent" );

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
        {
			try
			{
				boost::shared_ptr<TA_Base_Core::AlarmSequence> alarmSeq;
				boost::shared_ptr<TA_Base_Core::AlarmAgentQueueItemSequence> queueSeq;

				// Stop the loader from calling this function again
				m_dbLoader->acquire(false);
				
				// TD 14082 control alarm agent corba callback
				AlarmCache::AlarmAgentNamedObject controlAlarmAgent;

			    const std::string alarmAgentType(TA_Base_Core::AlarmAgentEntityData::getStaticType());
				const unsigned long alarmAgentLocationKey = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );

				TA_Base_Core::CorbaNameList alarmAgentNames(
					TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(alarmAgentType, alarmAgentLocationKey,true));

				// Make sure there's exactly one Train Agent entity configured for this location.
				if (alarmAgentType.empty())
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There is no alarm agent entity type");
					m_dataLoaded = false;
					m_dbLoader->acquire(true);
					return;
				}
				else if (alarmAgentNames.size() > 1)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There is more than one alarm agent servant at location %li", alarmAgentLocationKey );
					m_dataLoaded = false;
					m_dbLoader->acquire(true);
					return;
				}    

				// Set the named objects' corba name so that the Train Agent can be resolved correctly.
				controlAlarmAgent.setCorbaName(alarmAgentNames[0]);
			    // Extend the default corba timeout.
			    controlAlarmAgent.setObjectTimeout(ALARM_AGENT_TIMEOUT);

				// TD 14082 do direct corba request and skip message request
				//CORBA::Any nothing;

				//m_stateSender->sendStateUpdateMessage(
				//    TA_Base_Core::AlarmStateUpdate::AlarmAgentStateRequest,
				//    0,
				//    RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
				//    nothing);

				TA_Base_Core::AlarmSequence * pTmpAlarmSeq = NULL;
				TA_Base_Core::AlarmAgentQueueItemSequence * pTmpQueueSeq = NULL;

				//xinsong ++ 
				if ( controlAlarmAgent.getAgentName().empty() == false)
				{
					CORBA_CALL_RETURN( pTmpAlarmSeq,
					                   (controlAlarmAgent),
					                   getAlarms,
					                   () );
					alarmSeq.reset(pTmpAlarmSeq);
					CORBA_CALL_RETURN( pTmpQueueSeq,
					                   (controlAlarmAgent),
					                   getQueueUpdates,
					                   () );
					queueSeq.reset(pTmpQueueSeq);
					receiveAlarmCacheBatchUpdate(*alarmSeq);					
					receiveAlarmQueueBatchUpdate(*queueSeq);

					m_dataLoaded = true;
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
						"Can't locate or initialise Control Alarm Agent. Try again later.");
					m_dataLoaded = false;
					m_dbLoader->acquire(true);
				}
			}
			catch(const CORBA::SystemException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
				m_dataLoaded = false;
				m_dbLoader->acquire(true);
			}
			catch(TA_Base_Core::ObjectResolutionException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the alarm agent");
				m_dataLoaded = false;
				m_dbLoader->acquire(true);
			}
			catch(TA_Base_Core::OperationModeException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't load Alarm Cache or Queue from Control Alarm Agent");
				m_dataLoaded = false;
				m_dbLoader->acquire(true);
			}
			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Can't load Alarm Cache or Queue from Control Alarm Agent.	Caught TransactiveException: %s", ex.what());
				m_dataLoaded = false;
				m_dbLoader->acquire(true);
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
					"Can't load Alarm Cache or Queue from Control Alarm Agent. Caught Corba Exception");
				m_dataLoaded = false;
				m_dbLoader->acquire(true);

			}
			catch (...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "Unknown exception caught: Can't load Alarm Cache or Queue from Control Alarm Agent. Caught General Exception" );
				m_dataLoaded = false;
				m_dbLoader->acquire(true);
			}

        }

        FUNCTION_EXIT;
    }


	TA_Base_Core::AlarmAgentQueueItemSequence* AlarmCache::getBatchQueueUpdate()
    {

        ThreadGuard guard(m_cacheLock);

         //No need to lock in this method, getAll and getCurrentQueue take care of that for us
		return m_queue->getCurrentQueue();

	}

	
    // TD 14082
    // sendBatchStateUpdates
    //
    void AlarmCache::sendBatchStateUpdates()
    {
        FUNCTION_ENTRY( "sendBatchStateUpdates" );

        /* Do nothing as this will not be used
		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
            //No need to lock in this method, getAll and getCurrentQueue take care of that for us

            TA_Base_Core::AlarmAgentQueueItemSequence* queueSeq = m_queue->getCurrentQueue();
			TA_Base_Core::AlarmSequence* alarmSeq = getAll();

            CORBA::Any queueData, alarmData;
            queueData <<= *queueSeq;
            alarmData <<= *alarmSeq;

			// TD 14082
            //m_stateSender->sendStateUpdateMessage(
            //    TA_Base_Core::AlarmStateUpdate::AlarmAgentBatchQueueUpdate,
            //    0,
            //    RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
            //    queueData);

            // m_stateSender->sendStateUpdateMessage(
            //    TA_Base_Core::AlarmStateUpdate::AlarmAgentBatchCacheUpdate,
            //    0,
            //    RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
            //    alarmData);

            delete queueSeq;
            delete alarmSeq;
        } */

        FUNCTION_EXIT;
    }


    //
    // receiveAlarmCacheBatchUpdate
    //
    void AlarmCache::receiveAlarmCacheBatchUpdate( const AlarmSequence& update)
    {
        FUNCTION_ENTRY( "receiveAlarmCacheBatchUpdate" );

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
        {

            ThreadGuard guard(m_cacheLock); // Lock on the whole method

            // Clear the current cache
            for( AlarmIdToAlarmMap::iterator p = m_cache.begin(); p != m_cache.end(); p++ )
            {
                if( p->second != NULL )
                {
                    delete p->second;
                    p->second = 0;
                }
            }
            m_cache.clear();
            m_entityToAlarmMap.clear();

            // Add the alarms to the cache
            for( unsigned int i = 0; i < update.length(); i++ )
            {
                AlarmDetailCorbaDef* alarm = new AlarmDetailCorbaDef();

                *alarm = update[i];

                 // TD 7534: Update the decision support state if required
                alarm->decisionSupportState = checkDecisionSupportStateType(alarm->decisionSupportState, alarm->assocEntityTypeKey,
                    alarm->assocEntityKey, alarm->messageTypeKey);

                m_cache[alarm->alarmID.in()] = alarm;

                m_entityToAlarmMap.insert(std::pair<unsigned long, AlarmDetailCorbaDef*>
                    (alarm->assocEntityKey, alarm));
            }

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                 "Received %d alarms from the controlling agent", update.length() );
        }

        FUNCTION_EXIT;
    }


    //
    // receiveAlarmQueueBatchUpdate
    //
    void AlarmCache::receiveAlarmQueueBatchUpdate( const AlarmAgentQueueItemSequence& update)
    {
        FUNCTION_ENTRY( "receiveAlarmQueueBatchUpdate" );

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
        {
            ThreadGuard guard(m_cacheLock);

            m_queue->clear();

            for( unsigned int i = 0; i < update.length(); i++ )
            {
                AlarmAgentQueueItemCorbaDef* item = new AlarmAgentQueueItemCorbaDef();

                *item = update[i];

                // CL-21508 begin
                AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( item, item->theMessage );
//                 m_queue->push(item);
                m_queue->push(  pNewWrapper );
                // CL-21508 end
            }
        }

        FUNCTION_EXIT;
    }


    //
    // removeQueueItem
    //
    void AlarmCache::removeQueueItem(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& queueItem)
    {
        FUNCTION_ENTRY( "removeQueueItem" );

        m_queue->removeQueueItem(queueItem);

        FUNCTION_EXIT;
    }


    //
    // processUpdate
    //
    void AlarmCache::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        //FUNCTION_ENTRY( "processUpdate" );
		LOG0(SourceInfo,DebugUtil::DebugDebug, "processUpdate in");
        unsigned long assocEntityTypeKey = 0;
        unsigned long assocEntityKey = 0;
        unsigned long assocAlarmTypeKey = 0;

        {
            ThreadGuard guard( m_assocLock ); 

            // A configuration update for Alarm Plan Associations has been received. Regardless of
            // whether it is a Create, Delete or Update event, we simply check whether there are 
            // alarm plan associations for the affected alarms and update them if necessary.

            std::map<unsigned long, TA_Base_Core::IAlarmPlanAssocData*>::iterator iter = m_alarmPlanAssociations.end(); 
            
            // Update local store of alarm plan associations
            switch(updateEvent.getModifications())
            {
                case TA_Base_Core::Create:
                {
                    // Add a new alarm plan object to our list
                    unsigned long key = updateEvent.getKey(); 
                    TA_Base_Core::IAlarmPlanAssocData* alarmPlan = new TA_Base_Core::AlarmPlanAssocData(updateEvent.getKey());
                    
                    iter = m_alarmPlanAssociations.find(updateEvent.getKey());
                    if(iter != m_alarmPlanAssociations.end())
                    {
                        return;
                    }
					//try to get data from DB;
                    assocEntityTypeKey = alarmPlan->getEntityTypeKey();
                    assocEntityKey = alarmPlan->getEntityKey();
                    assocAlarmTypeKey = alarmPlan->getAlarmType();
					
					//if get from DB successfully, put into map.
					m_alarmPlanAssociations[updateEvent.getKey()] = alarmPlan;


                    break;
                }
                case TA_Base_Core::Update:
                {
                    iter = m_alarmPlanAssociations.find(updateEvent.getKey());
                    if(iter == m_alarmPlanAssociations.end())
                    {
                        return;
                    }

                    iter->second->invalidate();

                    assocEntityTypeKey = iter->second->getEntityTypeKey();
                    assocEntityKey = iter->second->getEntityKey();
                    assocAlarmTypeKey = iter->second->getAlarmType();

                    break;
                }
                case TA_Base_Core::Delete:
                {
                    iter = m_alarmPlanAssociations.find(updateEvent.getKey());
                    if(iter == m_alarmPlanAssociations.end())
                    {
                        return;
                    }

                    assocEntityTypeKey = iter->second->getEntityTypeKey();
                    assocEntityKey = iter->second->getEntityKey();
                    assocAlarmTypeKey = iter->second->getAlarmType();

                    m_alarmPlanAssociations.erase(iter);

                    break;
                }
            }
        }

        // Update the alarms associated with this rule
        updatePlanStatusOfRelevantAlarms(assocEntityTypeKey, assocEntityKey, assocAlarmTypeKey);
        LOG0(SourceInfo,DebugUtil::DebugDebug, "processUpdate out");
        //FUNCTION_EXIT;
    }


    void AlarmCache::processAlarmRuleUpdate(AbstractRule& alarmRule, TA_Base_App::AbstractRule::ERuleState action)
    {
        FUNCTION_ENTRY( "processAlarmRuleUpdate" );
        
        // An update for Alarm Rules has been received. Regardless of
        // whether it is a Create, Delete or Update event, we simply check whether there are 
        // alarm plan associations for the affected alarms and update them if necessary.


        unsigned long assocEntityTypeKey = 0;
        unsigned long assocEntityKey = 0;
        unsigned long assocAlarmTypeKey = 0;
		std::string assocAlarmDescription("");
		std::string assocAlarmType(""); //20448++
        
        const std::type_info &alarmRuleTypeInfo = typeid(alarmRule);
        assocEntityTypeKey = alarmRule.getEntityType();
        assocEntityKey = alarmRule.getEntityKey();
		assocAlarmTypeKey = alarmRule.getAlarmTypeKey();
		//Tianxiang speed up processing alarms
		TA_Base_Core::IAlarmTypeData* alarmType = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmTypeRecord( assocAlarmTypeKey); //get alarm type from cache instead of query db
		if (alarmType == NULL)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
             "Can not find Alarm type");
			return;
		}

		//Tianxiang speed up processing alarms
		assocAlarmType = alarmType->getName();
        
        if (alarmRuleTypeInfo == typeid(PlanExecutionRule))
        {
			// Update the alarms associated with this rule
			updatePlanStatusOfRelevantAlarms(assocEntityTypeKey, assocEntityKey, assocAlarmTypeKey);
        }
        else if ( alarmRuleTypeInfo == typeid(SuppressionRule) )
		{
			// Yanrong++ TD19819
			std::string sessionId = "";
			SuppressionRule* suppressRule = dynamic_cast<SuppressionRule*> (&alarmRule);
			assocAlarmDescription = suppressRule->getAlarmDescription();
			try{
				TA_Base_Core::IAlarmRuleData* alarmRule = TA_Base_Core::AlarmRuleAccessFactory::getInstance().getAlarmRule(suppressRule->getKey());
				TA_Base_Core::AlarmRuleData* ruleData = dynamic_cast<TA_Base_Core::AlarmRuleData*> (alarmRule);
				sessionId = ruleData->getCreatedBySessionKey();
			}
			catch( const TA_Base_Core::DataException& )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                 "getAlarmRule from DB failed, pkey=%li (TD19819)",
				 suppressRule->getKey());
			}
			
			// Use alarm agent's session if there is no session from alarm rule
			if (sessionId.empty() == true) sessionId = RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str());

			/*
			switch (action)
			{
				case TA_Base_App::AbstractRule::RULE_CREATED:
					// suppress all existing alarms
					updateCacheForInhibitedAlarms(sessionId, assocEntityTypeKey, assocEntityKey, assocAlarmTypeKey, assocAlarmDescription);
					break;
				case TA_Base_App::AbstractRule::RULE_UPDATED:
					// suppress all existing alarms matching the current
					updateCacheForInhibitedAlarms(sessionId, assocEntityTypeKey, assocEntityKey, assocAlarmTypeKey, assocAlarmDescription);
					break;
				case TA_Base_App::AbstractRule::RULE_DELETED:
					break;
			}*/

			// process all the actions
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[XF] Alarm Inhibition: assocEntityKey[%lu], assocAlarmTypeKey[%lu], assocAlarmType[%s], assocAlarmDescription[%s]", 
											assocEntityKey,
											assocAlarmTypeKey,
											assocAlarmType.c_str(), 
											assocAlarmDescription.c_str());
			
			updateCacheForInhibitedAlarms(sessionId, assocEntityTypeKey, 
				assocEntityKey, assocAlarmTypeKey, 
				action, assocAlarmType, assocAlarmDescription); //20448++
			// ++Yanrong TD19819
		}
		else
        {
            // Not a plan rule, don't worry about it
            return;
        }

                
        FUNCTION_EXIT;
    }


    void AlarmCache::updatePlanStatusOfRelevantAlarms(unsigned long entityTypeKey, unsigned long entityKey, unsigned long alarmTypeKey)
    {
        // Update the alarms associated with this rule
        AlarmIdToAlarmMap::iterator cacheItr = m_cache.begin();
        for(; cacheItr != m_cache.end(); ++cacheItr)
        {
            if((cacheItr->second->assocEntityTypeKey == entityTypeKey && alarmTypeKey == alarmTypeKey) ||
                (cacheItr->second->assocEntityKey == entityKey && cacheItr->second->messageTypeKey == alarmTypeKey))
            {
                // Check if there are still any related associations for the alarm. Passing in NoRelatedPlan
                // will ensure that this will be checked, and updated if necessary.
                
                TA_Base_Core::DecisionSupportStateType oldDecisionSupportState = cacheItr->second->decisionSupportState;
                TA_Base_Core::DecisionSupportStateType newDecisionSupportState= checkDecisionSupportStateType(
                    TA_Base_Core::NoRelatedPlan, entityTypeKey, entityKey, alarmTypeKey);

                // If support state has changed, send an update
                if(oldDecisionSupportState != newDecisionSupportState)
                {
                    //cacheItr->second->decisionSupportState = newDecisionSupportState;
                    
                    TA_Base_Core::AlarmDetailCorbaDef* alarm = cacheItr->second;

                    // Session Id is not used for decision support update
                    updateDecisionSupportState(CORBA::string_dup(alarm->alarmID), newDecisionSupportState, "");   
                }
            }
        }
    }


	// Yanrong++ TD19819
	void AlarmCache::inhibitAlarm(TA_Base_Core::AlarmDetailCorbaDef* alarm,
		const std::string& sessionId)
	{
		FUNCTION_ENTRY( "inhibitAlarm" );
		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// Generate the update
			AlarmUpdateCorbaDef* update = new AlarmUpdateCorbaDef();
			
			update->messageTypeKey = alarm->messageTypeKey;
			update->assocEntityKey = alarm->assocEntityKey;
			update->locationKey = alarm->locationKey;
			update->ackTime = alarm->ackTime;

			update->messageContext = CORBA::string_dup(alarm->messageContext);
			update->alarmAcknowledgedBy = CORBA::string_dup(alarm->alarmAcknowledgedBy);
			update->alarmID = CORBA::string_dup(alarm->alarmID);
			update->assetName = CORBA::string_dup(alarm->assetName);
			update->session = CORBA::string_dup(sessionId.c_str());
			update->alarmDescription = CORBA::string_dup("");

			update->alarmHasBeenClosed = true;
			update->alarmHasBeenOpened = false; 
			update->alarmHasBeenAcknowledged = true;
			update->commentsWereAdded = false;
			update->mmsStateHasChanged = false;
			update->decisionSupportStateHasChanged = false;
			update->mmsState=alarm->mmsState;
			update->decisionSupportState=alarm->decisionSupportState;

			/*
			std::stringstream params;
			params << "messageTypeKey = " << update->messageTypeKey << "\n"
				<< "messageContext = " << update->messageContext.in() << "\n"
				<< "assocEntityKey = " << update->assocEntityKey << "\n"
				<< "locationKey = " << update->locationKey << "\n"
				<< "alarmID = " << update->alarmID.in() << "\n"
				<< "alarmAcknowledgedBy = " << update->alarmAcknowledgedBy.in() << "\n"
				<< "ackTime = " << update->ackTime << "\n"
				<< "assetName = " << update->assetName.in() << "\n"
				<< "session = " << update->session.in() << "\n"
				<< "alarmHasBeenClosed = " << update->alarmHasBeenClosed << "\n"
				<< "alarmHasBeenOpened = " << update->alarmHasBeenOpened << "\n"
				<< "alarmHasBeenAcknowledged = " << update->alarmHasBeenAcknowledged << "\n"
				<< "commentsWereAdded = " << update->commentsWereAdded << "\n"
				<< "mmsStateHasChanged = " << update->mmsStateHasChanged << "\n"
				<< "decisionSupportStateHasChanged = " << update->decisionSupportStateHasChanged << "\n"
				<< "alarmDescription = " << update->alarmDescription.in() << "\n"
				<< "messageSequenceNumber = " << update->messageSequenceNumber << "\n";

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD19819 inhibitAlarm sendAlarmUpdate, update params:\n%s",
				params.str().c_str());*/

			/*
			update->messageSequenceNumber = getNextMessageNumber();
			m_alarmSender->sendAlarmUpdate( *update );*/

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD19819 inhibitAlarm, alarmId=%s",
				alarm->alarmID.in());
			doMessageSendingForUpdatedAlarm(update, alarm->entityName.in(), sessionId, alarm->assetName.in());
			
			delete update;
		}

		FUNCTION_EXIT;
	}


	void AlarmCache::uninhibitAlarm(TA_Base_Core::AlarmDetailCorbaDef* alarm,
		const std::string& sessionId)
	{
		FUNCTION_ENTRY( "uninhibitAlarm" );

		/*std::stringstream params;
		params << "alarmId = " <<alarm->alarmID.in() << "\n"
			<< "messageContext = " << alarm->messageContext.in() << "\n"
			<< "messageTypeKey = " << alarm->messageTypeKey << "\n"
			<< "messageSequenceNumber = " << alarm->messageSequenceNumber << "\n"
			<< "alarmComments = " << alarm->alarmComments.in() << "\n"
			<< "assetName = " << alarm->assetName.in() << "\n"
			<< "assocEntityKey = " << alarm->assocEntityKey << "\n"
			<< "assocEntityTypeKey = " << alarm->assocEntityTypeKey << "\n"
			<< "entityName = " << alarm->entityName.in() << "\n"
			<< "locationKey = " << alarm->locationKey << "\n"
			<< "state = " << alarm->state << "\n";

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD19819 uninhibitAlarm sendAlarm, alarm params:\n%s",
			params.str().c_str());*/
	
		/*
		alarm->messageSequenceNumber = getNextMessageNumber();
		m_alarmSender->sendAlarm( *alarm ); */
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD19819 uninhibitAlarm, alarmId=%s",
			alarm->alarmID.in());
		doMessageSendingForNewAlarm(alarm);
		FUNCTION_EXIT;
	}
	// ++Yanrong TD19819

	
	//
	// updateCacheForInhibitedAlarms
	//
	// Wenguang TD18266
    void AlarmCache::updateCacheForInhibitedAlarms(std::string sessionId, unsigned long entityTypeKey,
		unsigned long entityKey, unsigned long alarmTypeKey,
		AbstractRule::ERuleState ruleState,
		const std::string& assocAlarmType  /*20448++*/,
		const std::string& alarmDescription /*= ""*/)
	{
		FUNCTION_ENTRY("updateCacheForInhibitedAlarms");

		// vector of alarms for close and ack
		std::vector<TA_Base_Core::AlarmDetailCorbaDef*> inhibitedAlarms;

		{
			ThreadGuard guard(m_cacheLock);
			// Update the alarms associated with this rule
			AlarmIdToAlarmMap::iterator cacheItr = m_cache.begin();
			for(; cacheItr != m_cache.end(); ++cacheItr)
			{
				// check the alarm type and entity/entityType match
				if((cacheItr->second->assocEntityTypeKey == entityTypeKey && cacheItr->second->messageTypeKey == alarmTypeKey) ||
					(cacheItr->second->assocEntityKey == entityKey && cacheItr->second->messageTypeKey == alarmTypeKey))
				{
					TA_Base_Core::AlarmDetailCorbaDef* alarm = cacheItr->second;

					// check the alarm description match if not empty
					// ie. if empty = suppress by type, otherwise suppress by description
					if (alarmDescription.empty() == false) // suppress by description
					{
						//cl-19819
						std::string cachedAlarmDescription = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription(alarm->messageTypeKey,alarm->alarmParameterList.in());//getExpandedDescriptionalarm->
						//std::string cachedAlarmDescription = alarm->alarmDescription.in();
						//cl-19819

						if (cachedAlarmDescription.compare(alarmDescription) == 0)
						{
							// it is a match, store for close or open
							inhibitedAlarms.push_back(alarm);
						}
					}
					else // default -> suppress by type
					{
						// it is a match, store for close or open
						inhibitedAlarms.push_back(alarm);
					}
				}
			}
		}

		if (inhibitedAlarms.size() > 0)
		{
			try
			{
				// Yanrong++ TD19819
				// obtain an helper to close and ack the alarm
				/*
				TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().operatorCloseAndAcknowledgeAlarms(inhibitedAlarms, sessionId.c_str() );
				*/

				//20448++
				struct timeb currentSystemTime;
				std::string assetname = inhibitedAlarms[0]->assetName.in();
				TA_Base_Core::DescriptionParameters descriptionParameters;		
				
				std::string eventMsgString ("") ;
				std::ostringstream tempEventMsgString;
				/* 
				event:30079		AlarmInhibitionSucceeded	[EventMessage] Alarm Inhibit [OnOff] - Successful
				- For Inhibition by Type:   AlarmType "[AlarmType]":  Alarm Inhibit [On/Off] - Successful
				- For Inhibition by Description:   AlarmDescription "[AlarmDescription]": Alarm Inhibit [On/Off] - Successful
				*/
				if (alarmDescription.empty() == false) // suppress by description
				{
					tempEventMsgString << "AlarmDescription \"" <<alarmDescription<<"\": "; 
				} 
				else
				{
					tempEventMsgString << "AlarmType \"" <<assocAlarmType<<"\": "; 
				}
				eventMsgString = tempEventMsgString.str();
				//++20448

				switch( ruleState )
				{
					case TA_Base_App::AbstractRule::RULE_CREATED:
					case TA_Base_App::AbstractRule::RULE_UPDATED:
						{
							LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "TD19819 ++++++ CREATE/UPDATE inhibited rules, size=%d", (int)inhibitedAlarms.size() );
							for( int i = 0; i < (int)inhibitedAlarms.size() ; ++i )
							{
								inhibitAlarm( inhibitedAlarms[i], sessionId );
							}
						//20448++
							descriptionParameters.clear();
							TA_Base_Core::NameValuePair pair1 ( "OnOff", "ON");
							descriptionParameters.push_back ( &pair1 );

							TA_Base_Core::NameValuePair pair2 ( "EventMessage", eventMsgString );
							descriptionParameters.push_back ( &pair2 );

							ftime ( &currentSystemTime );

							m_auditSender->sendAuditMessageWithAsset ( TA_Base_Core::AlarmAudit::AlarmInhibitionSucceeded,	// message type
								entityKey,	// entity key
								descriptionParameters,	// description
								"",			// details
								sessionId,	// session ID
								"",			// alarm ID
								"",			// incidentkey
								"",			// parent event ID
								assetname, // asset name
								NULL,			// filterable Data
							    currentSystemTime );
							//++20448
						}
						break;
					case TA_Base_App::AbstractRule::RULE_DELETED:
						{
							LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "TD19819 ------ DELETE inhibited rules, size=%d", (int)inhibitedAlarms.size() );
							for( int j = 0; j < (int)inhibitedAlarms.size(); ++j )
							{
								uninhibitAlarm( inhibitedAlarms[j], sessionId );
							}
						//20448++
							descriptionParameters.clear();
							TA_Base_Core::NameValuePair pair1 ( "OnOff", "OFF");
							descriptionParameters.push_back ( &pair1 );
							
							TA_Base_Core::NameValuePair pair2 ( "EventMessage", eventMsgString);
							descriptionParameters.push_back ( &pair2 );
							
							ftime ( &currentSystemTime );
							
							m_auditSender->sendAuditMessageWithAsset ( TA_Base_Core::AlarmAudit::AlarmInhibitionSucceeded,	// message type
								entityKey,	// entity key
								descriptionParameters,	// description
								"",			// details
								sessionId,	// session ID
								"",			// alarm ID
								"",			// incidentkey
								"",			// parent event ID
								assetname, // asset name
								NULL,			// filterable Data
								currentSystemTime );						
							//++20448
						}
						break;
				}
				// ++Yanrong TD19819

			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Could not close and ack alarms via AlarmModificationHelper");
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"(TD19819) can not find inhibited alarms in m_cache. ruleState=%d, entityKey=%li,alarmTypeKey=%li",
				ruleState, entityKey, alarmTypeKey);
		}

		FUNCTION_EXIT;
	}
 
    //
    // find
    //
    TA_Base_Core::AlarmDetailCorbaDef* AlarmCache::find( const std::string& p_alarmId )
    {
        FUNCTION_ENTRY( "find - 1" );

        AlarmIdToAlarmMap::const_iterator alarmItr = m_cache.find(p_alarmId);
        
		if( alarmItr != m_cache.end() )
        {
			FUNCTION_EXIT;
            return alarmItr->second;
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Can't find alarm id: %s in alarm cache", p_alarmId.c_str() );
        LOG( SourceInfo, DebugUtil::FunctionExit, "find - 1 (not found)" );
        return NULL;
    }


    //
    // find
    //
    void AlarmCache::find( unsigned long p_alarmType, unsigned long p_entityKey,
                               TA_Base_App::AlarmCache::AlarmVector& vec)
    {
        FUNCTION_ENTRY( "find - 2" );

        EntityToAlarmMap::const_iterator start = m_entityToAlarmMap.find(p_entityKey);
        EntityToAlarmMap::const_iterator end = m_entityToAlarmMap.upper_bound(p_entityKey);
        EntityToAlarmMap::const_iterator endOfMap = m_entityToAlarmMap.end();

        while( (start != end) && (start != endOfMap) )
        {
            AlarmDetailCorbaDef* alarm = start->second;

            TA_ASSERT((alarm != NULL), "Alarm cannot be NULL");
            TA_ASSERT((alarm->messageTypeKey != 0), "Message type key cannot be 0");

            unsigned long storedAlarm = alarm->messageTypeKey;

			std::stringstream logMsg;
			logMsg << "Alarm types to compare are : map - ";
			logMsg << storedAlarm;
			logMsg << " alarm - ";
			logMsg << p_alarmType;

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, logMsg.str().c_str());

            if( p_alarmType == storedAlarm )
            {
				LOG3(SourceInfo, DebugUtil::DebugDebug, "Found Alarm by entityKey=%d, alarmType=%d,  with ID=%s",p_entityKey, storedAlarm, alarm->alarmID.in());
                vec.push_back(alarm);
            }
            start++;
        }

        FUNCTION_EXIT;
    }


    //
    // erase
    //
    void AlarmCache::erase(TA_Base_Core::AlarmDetailCorbaDef* p_alarm)
    {
        FUNCTION_ENTRY( "erase" );

		ThreadGuard guard( m_cacheLock );

        AlarmIdToAlarmMap::iterator cacheItr = m_cache.find(p_alarm->alarmID.in());

        EntityToAlarmMap::iterator entityMapItr = m_entityToAlarmMap.find(p_alarm->assocEntityKey);
        EntityToAlarmMap::iterator end = m_entityToAlarmMap.upper_bound(p_alarm->assocEntityKey);
        EntityToAlarmMap::iterator endOfMap = m_entityToAlarmMap.end();
        bool foundInEntityMap = false;

        while( (entityMapItr != end) && (entityMapItr != endOfMap) && (foundInEntityMap == false))
        {
            if( 0 == std::string(p_alarm->alarmID.in()).compare(entityMapItr->second->alarmID.in()) )
            {
                foundInEntityMap = true;
        }
            else
        {
                entityMapItr++;
            }
        }

		LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "About to delete alarm with ID=%s, entitykey=%d, alarmType=%d", p_alarm->alarmID.in(), p_alarm->assocEntityKey, p_alarm->messageTypeKey);

        TA_ASSERT((cacheItr != m_cache.end()), "Alarm not found in cache");
        TA_ASSERT((true == foundInEntityMap), "Alarm not found in entity map");

        m_cache.erase(cacheItr);
        m_entityToAlarmMap.erase(entityMapItr);

        delete p_alarm;

		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Cache sizes after deleting alarm AlarmID map=%d, Entity map=%d", m_cache.size(), m_entityToAlarmMap.size());

        p_alarm = 0;

        FUNCTION_EXIT;
    }


    //
    // getNextMessageNumber
    //
    unsigned long AlarmCache::getNextMessageNumber()
    {
        ThreadGuard guard(m_messageNumLock);
        m_messageSequenceCount++;

        if( m_messageSequenceCount == ULONG_MAX )
        {
            m_messageSequenceCount = 1;
        }

        return m_messageSequenceCount;
    }


	//
	// getNextMessageNumber
	//
	unsigned long AlarmCache::getNextStateMessageNumber()
	{
		ThreadGuard guard(m_stateSeqNumLock);
		m_stateMessageSequenceCount++;

		if( m_stateMessageSequenceCount == ULONG_MAX )
		{
			m_stateMessageSequenceCount = 1;
		}

		return m_stateMessageSequenceCount;
	}

	//
	// doMessageSendingForUnsuppressedAlarm
	//
	void AlarmCache::doMessageSendingForUnsuppressedAlarm( TA_Base_Core::AlarmDetailCorbaDef* p_alarm )
	{
		FUNCTION_ENTRY("doMessageSendingForUnsuppressedAlarm");

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// Generate detail for submission as new alarm
            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair alarmDesc("alarmDescription",p_alarm->alarmDescription.in());
            dp.push_back(&alarmDesc);

			std::string session = RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str());

			p_alarm->messageSequenceNumber = getNextMessageNumber();

			// Send the alarm on the defined type
			m_alarmSender->sendAlarm( *p_alarm );


            // Generate the update for state update and database
			std::auto_ptr<AlarmUpdateCorbaDef> update = std::auto_ptr<AlarmUpdateCorbaDef>(new AlarmUpdateCorbaDef());
            initAlarmUpdateCorbaDef(update.get());
			update->messageTypeKey = p_alarm->messageTypeKey;
			update->messageContext = CORBA::string_dup( p_alarm->messageContext );
			update->assocEntityKey = p_alarm->assocEntityKey;
			update->locationKey = p_alarm->locationKey;
            update->alarmID = CORBA::string_dup(p_alarm->alarmID);
            update->alarmHasBeenClosed = false;
            update->alarmHasBeenOpened = true; 
            update->alarmHasBeenAcknowledged = false;
            update->commentsWereAdded = false;
            update->mmsStateHasChanged = false;
			update->mmsState=p_alarm->mmsState;
            update->decisionSupportStateHasChanged = false;
			update->decisionSupportState=p_alarm->decisionSupportState;
            update->alarmAcknowledgedBy = CORBA::string_dup(p_alarm->alarmAcknowledgedBy);
            update->ackTime = p_alarm->ackTime;
            update->closeTime = p_alarm->closeTime;
            update->session = CORBA::string_dup(session.c_str());
			//update->alarmDescription = CORBA::string_dup( p_alarm->alarmDescription );
			update->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( p_alarm->messageTypeKey,  std::string(p_alarm->alarmParameterList.in())).c_str());
			update->assetName = CORBA::string_dup( p_alarm->assetName );
            AlarmAgentQueueItemCorbaDef* queueItem = new AlarmAgentQueueItemCorbaDef();
            initAlarmAgentQueueItemCorbaDef(queueItem);
            queueItem->containsAlarmDetailCorbaDef = false;
            queueItem->containsAlarmUpdateCorbaDef = true;
            queueItem->containsAlarmDetailForPrintingCorbaDef = false;
			queueItem->isOperatorTriggered = false;
            queueItem->theUpdate = *update;

            // CL-21508 begin
            AlarmQueueWrapper* pNewWrapper = new AlarmQueueWrapper( queueItem, *p_alarm );
//             m_queue->push(queueItem);
            m_queue->push(  pNewWrapper );
            // CL-21508 end

			// Wenguang++ Alarm Message Size Reduction
			update->alarmDescription = CORBA::string_dup("");
            
			CORBA::Any updateData;
            updateData <<= *update;

			p_alarm->messageSequenceNumber =getNextStateMessageNumber();

			// Send state update
            m_stateSender->sendStateUpdateMessage(
                    TA_Base_Core::AlarmStateUpdate::AlarmStateUpdate,
                    0,
                    RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
                    updateData);
		}
		FUNCTION_EXIT;
	}

    //
    // doMessageSendingForNewAlarm
    //
    void AlarmCache::doMessageSendingForNewAlarm( TA_Base_Core::AlarmDetailCorbaDef* p_alarm )
    {
         FUNCTION_ENTRY( "doMessageSendingForNewAlarm" );

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
            CORBA::Any stateUpdateData;
            stateUpdateData <<= *p_alarm;

            //TA_Base_Core::DescriptionParameters dp;
            //TA_Base_Core::NameValuePair alarmDesc("alarmDescription",p_alarm->alarmDescription.in());
            //dp.push_back(&alarmDesc);

			std::string session = RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str());

			// Extract the time
			//timeb sourceTime = {0,0,-1,-1};
			//sourceTime.time = p_alarm->sourceTime.time;
			//sourceTime.millitm = p_alarm->sourceTime.millitm;

			// do not send the alarm if it is suppressed but keep it in the cache till the state changes to open or close
			if (p_alarm->state != TA_Base_Core::AlarmSuppressed)
			{
				p_alarm->messageSequenceNumber = getNextMessageNumber();

				// Send the alarm on the defined type
				m_alarmSender->sendAlarm( *p_alarm );

				// TD 13530
				//m_auditSender->sendAuditMessageWithAsset(
				//		TA_Base_Core::AlarmAudit::AlarmSubmitted,
				//		p_alarm->assocEntityKey, dp, "", session, p_alarm->alarmID.in(),
				//		"", "", p_alarm->assetName.in(), 0, sourceTime);
			}
			// but send state-update message to keep monitoring-agent updated.
			else // send suppressed alarm audit message to update the events
			{
				// TD 13530
				// m_auditSender->sendAuditMessageWithAsset(
				//		TA_Base_Core::AlarmAudit::AlarmSupressed,
				//		p_alarm->assocEntityKey, dp, "", session, p_alarm->alarmID.in(),
				//		"", "", p_alarm->assetName.in(), 0, sourceTime);
			}
            p_alarm->messageSequenceNumber =getNextStateMessageNumber();
            m_stateSender->sendStateUpdateMessage(
                TA_Base_Core::AlarmStateUpdate::AlarmStateUpdate,
                0,
                RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
                stateUpdateData);
        }

        FUNCTION_EXIT;
    }


    //
    // doMessageSendingForUpdatedAlarm
    //
    void AlarmCache::doMessageSendingForUpdatedAlarm( TA_Base_Core::AlarmUpdateCorbaDef* p_alarm,
                                                      const std::string& p_entityName,
                                                      const std::string& p_session,
                                                      const std::string& p_assetName,
													  bool bSendAuditUpdate/* = true*/,
													  const std::string& p_closeValue/* = ""*/)
    {
        FUNCTION_ENTRY( "doMessageSendingForUpdatedAlarm" );

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// no update info if the alarm is still suppressed ie suppressed alarms should not reach this pt
			

			if(bSendAuditUpdate)
			{
				// Send the update only if the alarm is in the store ie. not suppressed/inhibited
				p_alarm->messageSequenceNumber = getNextMessageNumber();
				m_alarmSender->sendAlarmUpdate( *p_alarm );
			}

            //TA_Base_Core::DescriptionParameters dp;
            //TA_Base_Core::NameValuePair alarmDesc("alarmDescription", p_alarmDesc);
            //dp.push_back(&alarmDesc);

			/* TD 13530 Audit messages to be sent by AlarmQueue after updating alarm table
			// Different Audit message depending on the update type
            if( p_alarm->alarmHasBeenAcknowledged )
            {
                m_auditSender->sendAuditMessageWithAsset(
                        TA_Base_Core::AlarmAudit::AlarmAcknowledged,
                        p_alarm->assocEntityKey, dp, "", p_alarm->session.in(), p_alarm->alarmID.in(), "", "", p_assetName);
            }
            else if( p_alarm->alarmHasBeenClosed )
            {
                m_auditSender->sendAuditMessageWithAsset(
                        TA_Base_Core::AlarmAudit::AlarmClosed,
                        p_alarm->assocEntityKey, dp, "", p_alarm->session.in(), p_alarm->alarmID.in(), "", "", p_assetName);
            }
            else if( p_alarm->commentsWereAdded )
            {
                m_auditSender->sendAuditMessageWithAsset(
                        TA_Base_Core::AlarmAudit::AlarmCommentsUpdated,
                        p_alarm->assocEntityKey, dp, "", p_alarm->session.in(), p_alarm->alarmID.in(), "", "", p_assetName);
            }
			else if( p_alarm->mmsStateHasChanged )
			{
				// send new mms state changed audit message or event to DB and update event viewer
                m_auditSender->sendAuditMessageWithAsset(
                        TA_Base_Core::AlarmAudit::AlarmMmsStateUpdated,
                        p_alarm->assocEntityKey, dp, "", p_alarm->session.in(), p_alarm->alarmID.in(), "", "", p_assetName);
			}
			else if( p_alarm->decisionSupportStateHasChanged )
			{
				// send new dss state changed audit message or event to DB and update event viewer
                m_auditSender->sendAuditMessageWithAsset(
                        TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
                        p_alarm->assocEntityKey, dp, "", p_alarm->session.in(), p_alarm->alarmID.in(), "", "", p_assetName);
			} */

			// We need to re-assign the alarm close value here so state update can get the 
			// unformatted close value of the alarm
			if (p_alarm->alarmHasBeenClosed == true)
			{	
				p_alarm->closeValue = CORBA::string_dup(p_closeValue.c_str());
			}

            CORBA::Any updateData;
            updateData <<= *p_alarm;

			// Send state update
			p_alarm->messageSequenceNumber =getNextStateMessageNumber();
            m_stateSender->sendStateUpdateMessage(
                    TA_Base_Core::AlarmStateUpdate::AlarmStateUpdate,
                    0,
                    RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
                    updateData);
        }

        FUNCTION_EXIT;
    }


    //
    // getCORBATimeout
    //
    unsigned long AlarmCache::getCORBATimeout()
    {
        FUNCTION_ENTRY( "getCORBATimeout" );

        // If the corba timeout has already been determined then return straight away.
        if (m_corbaTimeout != 0)
        {
            return m_corbaTimeout;
        }

        // Check if it is stored in RunParams.
        if (RunParams::getInstance().isSet(RPARAM_CORBATIMEOUT))
        {
            m_corbaTimeout = strtoul(RunParams::getInstance().get(RPARAM_CORBATIMEOUT).c_str(), NULL, 0);
            return m_corbaTimeout;
        }

        m_corbaTimeout = 120;

        FUNCTION_EXIT;

        return m_corbaTimeout;
    }


    //
    // alarmDataToAlarmMessage
    //
    TA_Base_Core::AlarmDetailCorbaDef* AlarmCache::alarmDataToAlarmMessage( IAlarmData* p_alarmData )
    {
        FUNCTION_ENTRY( "alarmDataToAlarmMessage" );

        AlarmDetailCorbaDef* alarmMsg = new AlarmDetailCorbaDef();

        alarmMsg->subsystemKey = p_alarmData->getSubsystemKeyOfAssociatedEntity();
        alarmMsg->locationKey = p_alarmData->getLocationKeyOfAssociatedEntity();
        // Wenguang++ Alarm Message Size Reduction
		// alarmMsg->entityName = CORBA::string_dup(p_alarmData->getNameOfAssociatedEntity().c_str());
		alarmMsg->entityName = CORBA::string_dup("");
        alarmMsg->assocEntityTypeKey = p_alarmData->getTypeOfAssociatedEntity();

        alarmMsg->sourceTime.time = p_alarmData->getSourceTime().time;
        alarmMsg->sourceTime.millitm = p_alarmData->getSourceTime().millitm;
        alarmMsg->transactiveTime.time = p_alarmData->getTransactiveTime().time;
        alarmMsg->transactiveTime.millitm = p_alarmData->getTransactiveTime().millitm;
        alarmMsg->ackTime = p_alarmData->getAckTime();
        alarmMsg->closeTime = p_alarmData->getCloseTime();
        alarmMsg->assocEntityKey = p_alarmData->getEntityKey();
        alarmMsg->alarmSeverity = (unsigned short)p_alarmData->getSeverity();
        // Wenguang TD18266
		alarmMsg->messageTypeKey = p_alarmData->getTypeKey();
        alarmMsg->messageContext = CORBA::string_dup(p_alarmData->getContext().c_str());
        alarmMsg->alarmID = CORBA::string_dup(p_alarmData->getKey().c_str());
        alarmMsg->alarmComments = CORBA::string_dup(p_alarmData->getComments().c_str());
        alarmMsg->assetName = CORBA::string_dup(p_alarmData->getAssetName().c_str());
		alarmMsg->avalancheHeadID = CORBA::string_dup(p_alarmData->getAVLHeadId().c_str());
		// Wenguang++ Alarm Message Size Reduction
        //alarmMsg->alarmDescription = CORBA::string_dup(p_alarmData->getDescription().c_str());
		alarmMsg->alarmDescription = CORBA::string_dup("");
        alarmMsg->alarmParameterList = CORBA::string_dup(p_alarmData->getParamList().c_str());
        alarmMsg->parentAlarmID = CORBA::string_dup("");
        alarmMsg->messageSequenceNumber = 0;
        switch ( p_alarmData->getState() )
        {
			case TA_Base_Core::IAlarmData::CLOSED:
				alarmMsg->state = TA_Base_Core::AlarmClosed;
				break;
			case TA_Base_Core::IAlarmData::OPEN:
				alarmMsg->state = TA_Base_Core::AlarmOpen;
				break;
			case TA_Base_Core::IAlarmData::SUPPRESSED:
				alarmMsg->state = TA_Base_Core::AlarmSuppressed;
				break;
			default:
				TA_ASSERT( false, "Unknown alarm state" );
        }

        switch ( p_alarmData->getMmsStateType() )
        {
			case TA_Base_Core::IAlarmData::TYPE_NONE:
				alarmMsg->mmsState = TA_Base_Core::MMS_NONE;
				break;
			case TA_Base_Core::IAlarmData::TYPE_SEMI:
				alarmMsg->mmsState = TA_Base_Core::MMS_SEMI;
				break;
			case TA_Base_Core::IAlarmData::TYPE_AUTO:
				alarmMsg->mmsState = TA_Base_Core::MMS_AUTO;
				break;
			case TA_Base_Core::IAlarmData::TYPE_JR_SENT:
				alarmMsg->mmsState = TA_Base_Core::MMS_JR_SENT;
				break;
			case TA_Base_Core::IAlarmData::TYPE_ALM_SENT:
				alarmMsg->mmsState = TA_Base_Core::MMS_ALM_SENT;
				break;
			case TA_Base_Core::IAlarmData::TYPE_INHIBIT:
				alarmMsg->mmsState = TA_Base_Core::MMS_INHIBIT;
				break;
			case TA_Base_Core::IAlarmData::TYPE_FAILED:
				alarmMsg->mmsState = TA_Base_Core::MMS_FAILED;
				break;
			default:
				TA_ASSERT( false, "Unknown alarm mms state type" );
		}

		switch ( p_alarmData->getAVLStatus() )
		{
			case TA_Base_Core::IAlarmData::HEAD_AVL:
				alarmMsg->isHeadOfAvalanche = true;
				alarmMsg->isChildOfAvalanche = false;
				break;
			case TA_Base_Core::IAlarmData::CHILD_AVL:
				alarmMsg->isHeadOfAvalanche = false;
				alarmMsg->isChildOfAvalanche = true;
				break;
			default:
				alarmMsg->isHeadOfAvalanche = false;
				alarmMsg->isChildOfAvalanche = false;
		}

		alarmMsg->isUniqueAlarm = p_alarmData->getIsUnique();

		// TD 10687 - getOperatorDelimList uses a monster query on the event table...
        // Until we have a better (and faster!) way to retrieve session override details
        // it should remain disabled.
        std::string tempAckedBy;
		tempAckedBy = p_alarmData->getOperatorNameForAcknowledgedBySessionKey();
		alarmMsg->alarmAcknowledgedBy = CORBA::string_dup(tempAckedBy.c_str());
		alarmMsg->alarmAcknowledgedSession = CORBA::string_dup(p_alarmData->getAcknowledgedBy().c_str());
		alarmMsg->decisionSupportState = convertToDecisionSupportStateType(p_alarmData->getPlanStatus());

        // alarmMsg->mmsState = TA_Base_Core::MMS_NONE;

		// initialise display time based on alarm state
		if (p_alarmData->getState() == TA_Base_Core::IAlarmData::CLOSED)
		{
			// display time is closed time
			alarmMsg->displayTime.time = alarmMsg->closeTime;
			alarmMsg->displayTime.millitm = 0;
		}
		else
		{
			// display is the source time
			alarmMsg->displayTime = alarmMsg->sourceTime;
		}
		alarmMsg->alarmValue = CORBA::string_dup(""); // leave it empty as updateAlarmValue will do checking
		updateAlarmValue(alarmMsg);

        FUNCTION_EXIT;

        return alarmMsg;
    }

	std::string AlarmCache::obtainValueFromParameter(unsigned long alarmTypeKey, std::string alarmType, TA_Base_Core::AlarmDetailCorbaDef* alarm, bool isSystemAlarm /*FALSE*/, bool isClosed /*FALSE*/)
	{
		std::string value("");
		//DTL-1719 -- remove unnecessary code and just retrieve alarm value from database
		TA_Base_Core::IAlarmTypeData* alarmTypeData =TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmTypeRecord(alarmTypeKey); //get alarm type from cache instead of query db
		if (alarmTypeData == NULL)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
				"Can not find Alarm type");
			return "default";
		}

		//DTL-1719 -- remove unnecessary code and just retrieve alarm value from database
		if (isClosed) 
			value = alarmTypeData->getNormalValue();
		else 
			value = alarmTypeData->getAlarmValue();

		// Check whether it is system alarm
		// If yes, check the configured alarm or normalised (ie closed) value in DB
		if (!isSystemAlarm)
		{
			TA_Base_Core::DescriptionParameters description;
			TA_Base_Core::gGetParametersFromString(std::string(alarm->alarmParameterList.in()), description);
			TA_Base_Core::ConstDescriptionParametersIterator valueParam;
			bool found=false;
			std::string OpenValue("");
			for (valueParam=description.begin(); valueParam!=description.end(); ++valueParam)
			{
				// Find "DataPointValue" if not closed
				// or "ClosedValue" if closed
				// However, if there is no ClosedValue but the alarm is closed, use back the DataPointValue
				if ( (*valueParam)->name.compare("DataPointValue") == 0 )
				{
					OpenValue = std::string( (*valueParam)->value.c_str() ); // memorise the open value for use
					if (isClosed==false) 
					{
						found = true;
						break; // break the cycle
					}
				}
				else if ( (*valueParam)->name.compare("ClosedValue") == 0 && isClosed==true )
				{
					if (std::string((*valueParam)->value.c_str()).compare("-")==0) found = false;
					else found = true;
					break;
				}
			}

			// TD 12785 Only display the value without the alarm type if there is parameter value
			// if not, use the alarm type description
			if(found==true && valueParam != description.end()) // case of value found for the appropriate alarm state
			{
				//DTL-1719 -- remove unnecessary code and just retrieve alarm value from database
				if (value.empty())
					value += (*valueParam)->value;
				else
					value += " " + (*valueParam)->value;

			}
			else if (isClosed && found==false && OpenValue.empty()==false) // case of closed value not listed but open value provided
			{
				//DTL-1719 -- remove unnecessary code and just retrieve alarm value from database
				if (value.empty())
					value += OpenValue;
				else
					value += " " + OpenValue;
			}

			// delete the description param after use
			for (int i = 0;i < static_cast<int>(description.size());i++)
			{
				delete description[i];
				description[i] = NULL;
			}
		}

		return value;
	}

    //
    // alarmAlreadyRaised
    //
    bool AlarmCache::alarmAlreadyRaised( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm )
    {
        FUNCTION_ENTRY( "alarmAlreadyRaised" );

        EntityToAlarmMap::iterator entityItr = m_entityToAlarmMap.find( p_alarm.assocEntityKey );

        if(entityItr != m_entityToAlarmMap.end() )
        {
            EntityToAlarmMap::iterator endOfMatches = m_entityToAlarmMap.upper_bound(p_alarm.assocEntityKey);
            do
            {
                TA_ASSERT((entityItr->second != NULL), "Alarm cannot be NULL");

                if( (p_alarm.messageTypeKey != NULL) && (entityItr->second != NULL) )
                {

                    AlarmDetailCorbaDef* alarm = entityItr->second;

                    TA_ASSERT((alarm != NULL), "Alarm cannot be NULL");
                    TA_ASSERT( alarm->messageTypeKey != 0, "Message type key cannot be 0");

					std::stringstream msg;
                    msg <<  "Message types are : alarm -  ";
					msg <<  p_alarm.messageTypeKey;
                    msg <<  " map - ";
					msg <<  alarm->messageTypeKey;

                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, msg.str().c_str());

                    if( p_alarm.messageTypeKey == alarm->messageTypeKey )
                    {
                        if(alarm->state == TA_Base_Core::AlarmOpen)
                        {
                            // TD15661 unique alarm should not be raised again if alarm is still open
                            // but had been acknowledged.
                            // if(std::string(alarm->alarmAcknowledgedBy.in()).empty())
                            {
                                FUNCTION_EXIT;
                                return true;
                            }
                        }
                    }
                }
                entityItr++;
            } while( entityItr != endOfMatches );
        }
        FUNCTION_EXIT;
        return false;
    }

    //
    // printCacheSize
    //
    void AlarmCache::printCacheSize()
    {
        FUNCTION_ENTRY( "printCacheSize" );

        ThreadGuard guard(m_cacheLock);

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
             "Cache sizes are cache - %d, entity map - %d",
             m_cache.size(), m_entityToAlarmMap.size());

        FUNCTION_EXIT;
    }


    //
    // truncateTo3Digits
    //
    unsigned int AlarmCache::truncateTo3Digits(unsigned int anInteger)
    {
        // Get the first 3 digits.
        std::ostringstream outStream;
        outStream << anInteger;
        std::string milliStr = outStream.str();
        if (milliStr.length() > 3)
        {
            milliStr = milliStr.substr(0,3);
        }

        // Convert it to an int.
        std::istringstream inStream(milliStr);
        unsigned int milliInt;
        inStream >> milliInt;
        return milliInt;
    }

    
    //
    // convertToDecisionSupportStateType
    //
    TA_Base_Core::DecisionSupportStateType AlarmCache::convertToDecisionSupportStateType(const std::string& stateTypeString)
    {
        FUNCTION_ENTRY( "convertToDecisionSupportStateType" );

        TA_Base_Core::DecisionSupportStateType decisionStateType; 

        if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::AUTO_PLAN_NOT_RUN) )
        {
            decisionStateType = TA_Base_Core::RelatedAutomaticPlanNotRun;
        }
		else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::SEMIAUTO_PLAN_NOT_RUN) )
		{
            decisionStateType = TA_Base_Core::RelatedSemiAutomaticPlanNotRun;
		}
        else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::PLAN_RUN) )
        {
            decisionStateType = TA_Base_Core::RelatedPlanHasBeenRun;
        }
        else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::PLAN_POSTPONED) )
        {
            decisionStateType = TA_Base_Core::RelatedPlanHasBeenPostponed;
        }
        else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::NO_ASSOC_PLAN) )
        {
            decisionStateType = TA_Base_Core::NoRelatedPlan;
        }
		else
		{
			TA_ASSERT(false, "Unknown DSS state type string submitted.");
		}

        FUNCTION_EXIT;
        return decisionStateType;
    }


    //
    // getDecisionSupportStateType
    //
	TA_Base_Core::DecisionSupportStateType AlarmCache::checkDecisionSupportStateType(TA_Base_Core::DecisionSupportStateType stateType, unsigned long entityTypeKey, unsigned long entityKey, unsigned long alarmTypeKey)
    {
        FUNCTION_ENTRY( "checkDecisionSupportStateType" );
        TA_Base_Core::DecisionSupportStateType decisionStateType;

        // When an alarm is submitted (first created), it's decision support type is set to No Related Plan. Here
        // we check if it has a related plan and reset the decision support type.
        if(stateType == TA_Base_Core::NoRelatedPlan)
        {
            PlanExecutionRule dummyRule(0, 0, 0, AbstractRule::SUBMIT, 0, "");
            // Determine if it is a Semi-Automatic plan
            if(alarmPlanAssociationExists(entityTypeKey, entityKey, alarmTypeKey))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "getDecisionSupportStateType - SemiAutomaticPlan");
                decisionStateType = TA_Base_Core::RelatedSemiAutomaticPlanNotRun;
            }
            // Determine if it is an automatic plan
            else if(RuleRepository::getInstance()->doAnyRulesOfTypeApplyToAlarm(typeid(dummyRule), entityTypeKey, entityKey, alarmTypeKey))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "getDecisionSupportStateType - AutomaticPlan");
                decisionStateType = TA_Base_Core::RelatedAutomaticPlanNotRun;
            }
			else
			{
				// There is not related plan
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "getDecisionSupportStateType - No Related Plan");
				decisionStateType = TA_Base_Core::NoRelatedPlan;
			}
        }
        else
        {
            decisionStateType = stateType;
        }
		
		FUNCTION_EXIT;
		return decisionStateType;
    }


    //
    // loadAlarmPlanAssociations
    //
    void AlarmCache::loadAlarmPlanAssociations()
    {
        std::vector<TA_Base_Core::IAlarmPlanAssocData*> alarmPlanAssocs = TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().getAllAlarmPlanAssociations();
        std::vector<TA_Base_Core::IAlarmPlanAssocData*>::iterator iter = alarmPlanAssocs.begin();
        for(; iter != alarmPlanAssocs.end(); ++iter)
        {
            m_alarmPlanAssociations[(*iter)->getKey()] = *iter;
        }
    }


    //
    // alarmPlanAssociationExists
    //
    bool AlarmCache::alarmPlanAssociationExists(unsigned long entityTypeKey, unsigned long entityKey, unsigned long alarmTypeKey)
    {
        FUNCTION_ENTRY( "alarmPlanAssociationExists" );

        ThreadGuard guard( m_assocLock ); 

        std::map<unsigned long, TA_Base_Core::IAlarmPlanAssocData*>::iterator iter = m_alarmPlanAssociations.begin();
        for(; iter != m_alarmPlanAssociations.end(); ++iter)
        {
            TA_Base_Core::IAlarmPlanAssocData* assocData = iter->second;

            if((assocData->getEntityTypeKey() == entityTypeKey && assocData->getAlarmType() == alarmTypeKey) ||
                (assocData->getEntityKey() == entityKey && assocData->getAlarmType() == alarmTypeKey))
            {
                FUNCTION_EXIT;
                return true;
            }
        }
        FUNCTION_EXIT;
        return false;
    }

    //
    // getOperatorNameFromSession
    //
    std::string AlarmCache::getOperatorNameFromSession(const std::string& p_session)
    {
        try
        {
            std::map<std::string, TA_Base_Core::IOperator*>::iterator p = m_operatorMap.find(p_session);

            if( p != m_operatorMap.end() )
            {
                return p->second->getName();
            }

            if( m_authLib != NULL )
            {
                TA_Base_Bus::SessionInfo s = m_authLib->getSessionInfo(p_session, RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()));
                TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(s.UserId, false);
                
				m_operatorMap.insert(std::pair<std::string,TA_Base_Core::IOperator*>(p_session, op));
               
                return op->getName();
	        }
        }
        catch( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Unable to retrieve session information, setting user ID to 0");
        }
		 
		return "Unknown";
    }

	/**
	 * isSessionOperator()
	 */
	bool AlarmCache::isSystemOperator(const std::string& p_sessionID)
	{
		if (p_sessionID.empty())
			return false;

		try
		{
			std::map<std::string, TA_Base_Core::IOperator*>::iterator p = m_operatorMap.find(p_sessionID);

			if( p != m_operatorMap.end() )
			{
				return p->second->isSystemOperator();
			}

			if( m_authLib != NULL )
			{
				TA_Base_Bus::SessionInfo s = m_authLib->getSessionInfo(p_sessionID, RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str()));
				TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(s.UserId, false);

				m_operatorMap.insert(std::pair<std::string,TA_Base_Core::IOperator*>(p_sessionID, op));

				return op->isSystemOperator();
			}
		}
		catch( ... )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Unable to retrieve session information, setting user ID to 0");
		}
		return false;
	}

	void AlarmCache::loadMMSAlarmFromDatabase()
	{
		FUNCTION_ENTRY("loadMMSAlarmFromDatabase");
		ThreadGuard guard(m_cacheLock);
		AlarmIdToAlarmMap::const_iterator p = m_cache.begin();
		AlarmIdToAlarmMap::const_iterator end = m_cache.end();

		for( p = m_cache.begin(); p!= end; p++ )
		{
			const TA_Base_Core::AlarmDetailCorbaDef* alarm = p->second;

			if(alarm->mmsState == TA_Base_Core::MMS_AUTO)
			{
				RuleRepository::getInstance()->processMMSAlarm(*alarm);
			}
		}

		FUNCTION_EXIT;
	}

	void AlarmCache::resetDataLoader()
	{
		FUNCTION_ENTRY("AlarmCache::resetDataLoader");
		if (0 != m_dbLoader)
		{
			m_dbLoader->terminateAndWait();

			delete m_dbLoader;
			m_dbLoader = 0;
		}

		FUNCTION_EXIT;
	}
};
