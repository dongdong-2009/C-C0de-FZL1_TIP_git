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
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

// CL 15035 ++
#ifndef CL15035_TRACE
	#define CL15035_TRACE(x,y) LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, (x), (y))
#endif
// ++ CL 15035
#include <time.h>

#include "app/alarm/alarm_agent/src/AlarmQueue.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
// CL-21508
#include "app/alarm/alarm_agent/src/AlarmQueueWrapper.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/AlarmStateUpdate_MessageTypes.h"
#include "core/message/types/AlarmUpdate_MessageTypes.h"
#include "core/message/types/AlarmAudit_MessageTypes.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"

#include "core/data_access_interface/src/AlarmAccessFactory.h"
#include "core/data_access_interface/src/AlarmData.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DbUniqueConstraintViolated.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/Timer.h"

using TA_Base_Core::AlarmMessageCorbaDef;
using TA_Base_Core::AlarmDetailCorbaDef;
using TA_Base_Core::AlarmUpdateCorbaDef;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::Timer;
using TA_Base_Core::IAlarmData;
using TA_Base_Core::AlarmAccessFactory;
using TA_Base_Core::MessagePublicationManager;
using TA_Base_Core::AlarmAgentQueueItemCorbaDef;

namespace TA_Base_App
{

    //
    // Constructor
    //
    AlarmQueue::AlarmQueue()
    : TransactionQueue()
    {
        m_stateSender = MessagePublicationManager::getInstance().getStateUpdateMessageSender(
                            TA_Base_Core::AlarmStateUpdate::Context);

		// TD 13530 audit sender initialisation
	    m_auditSender = MessagePublicationManager::getInstance().getAuditMessageSender(
                            TA_Base_Core::AlarmAudit::Context);

        m_alarmsToTest = atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::NUMBER_OF_ALARMS_TO_TEST.c_str()).c_str());

        m_queueUpdateBatchSize = m_alarmsToTest;

        m_queueItemsProcessed = new TA_Base_Core::AlarmAgentQueueItemSequence();
    }


    //
    // Destructor
    //
    AlarmQueue::~AlarmQueue()
    {
        clear();

        if( m_stateSender != NULL )
        {
            delete m_stateSender;
            m_stateSender = 0;
        }
        
		if( m_auditSender != NULL )
        {
            delete m_auditSender;
            m_auditSender = 0;
        }

        if( m_queueItemsProcessed != NULL )
        {
            delete m_queueItemsProcessed;
            m_queueItemsProcessed = NULL;
        }
    }


    //
    // run
    //
    void AlarmQueue::run()
    {
        AlarmQueueWrapper* wrapper = NULL;
		 

        while( true )
        {
            m_queueSemaphore.wait();

            if( m_terminated )
            {
                break;
            }
            MoveToWriteQueue();
            if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {  
				std::string msgID = "";  //for log only
                try
                {
					while( (!isEmptyWriteQueue()) && (0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL)) && !m_terminated )
					{
						wrapper = NULL;						
						wrapper = popTopWriteQueue();
						//get the id in case we got an exception for logging
						msgID = wrapper->getAlarmAgentQueueItem()->theMessage.alarmID.in();

						handleNewQueueItem(wrapper);
						delete wrapper;
						wrapper=NULL;

						LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
							"Queue size is %d", size());
					}
				}
				catch( const TA_Base_Core::DbUniqueConstraintViolated& ex ) // bad news
				{
					LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DbUniqueConstraintViolated",
						ex.what() );

					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
						"Assuming previous Control agent has written alarm (%s), deleting from queue",
						// CL-21508
						msgID.c_str() );

					delete wrapper;
					wrapper=NULL;


				}
				catch( const TA_Base_Core::DatabaseException& ex ) // anything else is worth another go
				{
					LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException",
						ex.what() );

					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
						"Caught a database exception. Keep on trying" );
					//push back to top
					pushTopWriteQueue(wrapper);
				}                    
				catch( const TA_Base_Core::DataException& )
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
						"Caught a data exception. Deleting queue item alarm id = %s",msgID.c_str());
					delete wrapper;
					wrapper=NULL;
				}
				catch( ... )
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
						"Caught an unknown exception. Deleting queue item. Alarm id = %s",msgID.c_str());
					delete wrapper;
					wrapper=NULL;
				}
            }
        }
    }


    //
    // terminate
    //
    void AlarmQueue::terminate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "terminate");

        ThreadGuard guard(m_lock);

        m_terminated = true;

        m_queueSemaphore.post();
    }


    //
    // handleNewQueueItem
    //
    // CL-21508 begin
//     void AlarmQueue::handleNewQueueItem( TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item )
    void AlarmQueue::handleNewQueueItem( AlarmQueueWrapper* pWrapper )
    // CL-21508 end
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "handleNewQueueItem");

        // CL-21508
        TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item = pWrapper->getAlarmAgentQueueItem();

        if( p_item->containsAlarmUpdateCorbaDef )
        {
            updateAlarm( p_item->theUpdate );

			// TD 13530 Send audit message after successful handling of the queue item ie. update DB
			// TD 15011 Send auto close or ack message if it is not operator triggered
            // CL-21508 begin
// 			sendUpdateAuditMessage( p_item->theUpdate, p_item->isOperatorTriggered );
			// Don't send Audit message if the MMS State of the alarm is MMS_ALM_SENT
			// MMS_ALM_SENT state update is used by Control Alarm Agent to Monitor Alarm agent 
			// to remove alarm from AlarmMMSSubmitter Quene alone
			if(!(p_item->theUpdate.mmsStateHasChanged ) ||
				(p_item->theUpdate.mmsStateHasChanged && 
					p_item->theUpdate.mmsState != TA_Base_Core::MMS_ALM_SENT))
			{
				sendUpdateAuditMessage( *pWrapper, p_item->isOperatorTriggered );
			}
			sendStateUpdates(p_item);
            // CL-21508 end
        }
        else
        {
            if( p_item->containsAlarmDetailCorbaDef )
            {
                //if( safeToWrite(p_item->theMessage) )
                //{
				try
				{
					insertNewAlarm( p_item->theMessage );
				}
				catch(...)
				{
					// fail to insert the alarm to database
					// just return without send Audit and StateUpdate messages

					return;
				}

				// TD 13530 Send audit message after successful handling of the queue item ie. inserting DB
				// NOTE: Currently all new alarm submit message are system generated so there is no operator triggered
				// NOTE: If in future, there is need to differentiate system or operator generated alarm,
				// NOTE: add a function for sendOperatorInsertAuditMessage, as above
                // CL-21508 begin
//                 sendInsertAuditMessage( p_item->theMessage );
                sendInsertAuditMessage( *pWrapper );
                sendStateUpdates(p_item);
                // CL-21508 end
                //}
            }
        }
    }

	//
	// TD 13530 This is to send the audit message after submitting an alarm (ie. inserting DB)
	//
    // CL-21508 begin
    void AlarmQueue::sendInsertAuditMessage( AlarmQueueWrapper& refWrapper )
	{
        FUNCTION_ENTRY("sendInsertAuditMessage");

        const AlarmDetailCorbaDef& p_alarm = refWrapper.getAlarmAgentQueueItem()->theMessage;

		try
		{
			std::string session = RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::SESSION.c_str());

			// Create the alarm description name value pair
			TA_Base_Core::DescriptionParameters dp; 
			TA_Base_Core::NameValuePair alarmDesc("alarmDescription",p_alarm.alarmDescription.in() );
			dp.push_back(&alarmDesc);

			// Get the source time
			timeb sourceTime = {p_alarm.sourceTime.time,p_alarm.sourceTime.millitm,-1,-1};
			timeb closeTime = {p_alarm.closeTime, 0, -1, -1};

			// Determine the alarm state to send the right audit message
			switch (p_alarm.state)
			{
				case TA_Base_Core::AlarmClosed: // use close time
					// Send close alarm audit message
//                     m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmClosed,
//                         p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 0, closeTime);
                    m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmClosed,
                            p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 
                            refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                            refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                            refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                            refWrapper.getAlarmStatus(), refWrapper.getOMFlag(),0, closeTime );
					break;

				case TA_Base_Core::AlarmOpen: // use source time
					// Send audit message for new Alarm
//                     m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmSubmitted,
//                         p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 0, sourceTime);
                    m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmSubmitted,
                            p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 
                            refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                            refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                            refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                            refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, sourceTime );
					break;

				case TA_Base_Core::AlarmSuppressed: // use the source time
					// Send audit message for suppressed Alarm
//                     m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmSupressed,
//                         p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 0, sourceTime);
                    m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmSupressed,
                            p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(),
                            refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                            refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                            refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                            refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, sourceTime );
					break;
            
				default:
					TA_ASSERT( false, "Unknown alarm state" );
			}

			// Determine whether the alarm is related to avalanche suppression
			if (p_alarm.isHeadOfAvalanche)
			{
				// Send audit message for Avalanche head
//                 m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmIsAvalancheHead,
// 						p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 0, sourceTime);
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmIsAvalancheHead,
                        p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(),
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, sourceTime);
				
				// TD13487++
				std::string strHeadId(p_alarm.alarmID);
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
					"Sent Avalanche Head Audit message. Head id: %s", strHeadId.c_str() );
			}
			else if (p_alarm.isChildOfAvalanche)
			{
				// TD 15035 Don't send audit message for avalanche child
				// Send audit message for Avalanche child
				// m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmIsAvalancheChild,
				//		p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 0, sourceTime);
				
				// CL 15035 ++
				// Send audit message for Avalanche child
				std::string strMsgID;
				strMsgID = \
// 				m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmIsAvalancheChild,
// 						p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(), 0, sourceTime);
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmIsAvalancheChild,
						p_alarm.assocEntityKey, dp, "", session, p_alarm.alarmID.in(), "", "", p_alarm.assetName.in(),
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, sourceTime);

				CL15035_TRACE( "[sendingAlarmAvalancheChild-Msg-1] AlarmIsAvalancheChild Messsage ID:%s", strMsgID.c_str() );
				// ++ CL15035

				// TD13487++
				std::string strAlarmId(p_alarm.alarmID);
				std::string strHeadId(p_alarm.avalancheHeadID);
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
					"Received Avalanche Child. Child id: %s, Head id: %s", strAlarmId.c_str(), strHeadId.c_str() );
			}
		}
		catch (...)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				"Exception caught while sending insert audit message for alarm id: %s", std::string(p_alarm.alarmID).c_str() );
			// throw database exception error to retry poping the message
			throw TA_Base_Core::DatabaseException("SendAuditMessage");
		}

		FUNCTION_EXIT;
	}
    // CL-21508 end

	//
	// TD 13530 This is to send the audit message after handling (ie. updating DB) for an alarm
	//
    // CL-21508 begin
    void AlarmQueue::sendUpdateAuditMessage( AlarmQueueWrapper& refWrapper, bool isOperatorTriggered /*= false*/ )
    {
		FUNCTION_ENTRY("sendUpdateAuditMessage");

        const AlarmUpdateCorbaDef& p_update = refWrapper.getAlarmAgentQueueItem()->theUpdate;

		try
		{
			// Create the alarm description name value pair
			TA_Base_Core::DescriptionParameters dp;
			TA_Base_Core::NameValuePair alarmDesc("alarmDescription",p_update.alarmDescription.in() );
			dp.push_back(&alarmDesc);

			// Different Audit message depending on the update type
			if( p_update.alarmHasBeenAcknowledged )
			{
				timeb ackTime = {p_update.ackTime, 0, -1, -1};
				if (isOperatorTriggered)
				{
//                     m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmAcknowledged,
//                         p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 0, ackTime );
                    m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmAcknowledged,
                        p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, ackTime );
				}
				else
				{
// 					m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AutoAlarmAcknowledged,
// 							p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 0, ackTime );
                    m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AutoAlarmAcknowledged,
                        p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, ackTime );
				}
			}
			else if( p_update.alarmHasBeenClosed )
			{
				timeb closeTime = {p_update.closeTime, 0, -1, -1};
				if (isOperatorTriggered)
				{
// 					m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmClosed,
// 							p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 0, closeTime );
                    m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmClosed,
                        p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, closeTime );
				}
				else
				{
// 					m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AutoAlarmClosed,
// 							p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 0, closeTime );
                    m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AutoAlarmClosed,
                        p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag(), 0, closeTime );
				}
			}
			else if( p_update.commentsWereAdded )
			{
				// log current time
// 				m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmCommentsUpdated,
// 						p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in() );
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmCommentsUpdated,
                    p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                    refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                    refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                    refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                    refWrapper.getAlarmStatus(),refWrapper.getOMFlag() );
			}
			else if( p_update.mmsStateHasChanged )
			{
				// log current time
				// send new mms state changed audit message or event to DB and update event viewer
// 				m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmMmsStateUpdated,
// 						p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in() );
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmMmsStateUpdated,
                        p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag());
			}
			else if( p_update.decisionSupportStateHasChanged )
			{
				// CL 15087 (1) ++
				std::ostringstream strOut;
				TA_Base_Core::NameValuePair nvp0("Value", "");
				TA_Base_Core::NameValuePair nvp("alarmDescription", "");
				switch( p_update.decisionSupportState )
				{
				case TA_Base_Core::RelatedPlanHasBeenPostponed:
					strOut << "DSS Postpone for " << p_update.alarmDescription.in()
						<< "";
					dp.clear();
					nvp0.value = "SUCCESSFUL";
					nvp.value = strOut.str().c_str();
					dp.push_back(&nvp0);
					dp.push_back(&nvp);
					break;
				case TA_Base_Core::RelatedPlanHasBeenRun:
					// just output to log
					/*
					strOut << "DSS Run for " << p_update.alarmDescription.in()
						<< "";
					dp.clear();
					nvp0.value = "ACTIVE";
					nvp.value = strOut.str().c_str();
					dp.push_back(&nvp);
					*/
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
						"DSS Run for Alarm:<%s> Value:<%s>", 
						p_update.alarmDescription.in(),  "ACTIVE");
					break;
				case TA_Base_Core::RelatedSemiAutomaticPlanNotRun:
					break;
				case TA_Base_Core::RelatedAutomaticPlanNotRun:
					break;
				case TA_Base_Core::NoRelatedPlan:
					break;
				default:
					//should not happen
					strOut << "Unknown State Update flag for Alarm:<" << p_update.alarmDescription.in()
						<< ">";
					dp.clear();
					nvp.value = strOut.str().c_str();
					dp.push_back(&nvp);
				}
				// ++ CL 15087 (1)
				// log current time
				// send new dss state changed audit message or event to DB and update event viewer
// 				m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
// 						p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in() );
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
                        p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag() );
			}
			else if( p_update.alarmHasBeenOpened )
			{
				// log current time
				// this is only for avalanche child which is reopen
				// send message for opening a suppressed/re-open alarm
// 				m_auditSender->sendAuditMessageWithAsset( TA_Base_Core::AlarmAudit::AlarmSubmitted,
// 						p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in() );
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmSubmitted,
                        p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
                        refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
                        refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
                        refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
                        refWrapper.getAlarmStatus(),refWrapper.getOMFlag() );
			}
			else
			{
				// TD13487++
				// Log an Error message for this unknown update message
				std::string strAlarmId(p_update.alarmID);
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
					"An unknown update state has been received for alarm id: %s", strAlarmId.c_str() );
			}
			if(refWrapper.getIsSendCloseEvent()==TA_Base_App::CLOSE_FOR_NOMALIZED)
			{
				timeb closeTime = {p_update.closeTime, 0, -1, -1};
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmClosed,
	                p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
	                refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
	                refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
	                refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
	                refWrapper.getAlarmStatus(),refWrapper.getOMFlag(),0, closeTime);
			}
			else if(refWrapper.getIsSendCloseEvent()==TA_Base_App::CLOSE_FOR_ACK)
			{
				timeb ackTime = {p_update.ackTime, 0, -1, -1};
                m_auditSender->sendAuditMessageEx( TA_Base_Core::AlarmAudit::AlarmClosed,
	                p_update.assocEntityKey, dp, "", p_update.session.in(), p_update.alarmID.in(), "", "", p_update.assetName.in(), 
	                refWrapper.getAlarmDescritpion(), refWrapper.getEventLelOrSeverityKey(), refWrapper.getAlarmValue(), 
	                refWrapper.getAlarmTypeKey(), refWrapper.getAlarmComments(), refWrapper.getAlarmMMSState(), 
	                refWrapper.getAlarmDSSState(), refWrapper.getAlarmAVLstate(), refWrapper.getAlarmACKedBy(), 
	                refWrapper.getAlarmStatus(),refWrapper.getOMFlag(),0,ackTime );
			}
		}

		catch (...)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				"Exception caught while sending update audit message for alarm id: %s", std::string(p_update.alarmID).c_str() );
			// throw database exception error to retry poping the message
			throw TA_Base_Core::DatabaseException("SendUpdateAuditMessage");
		}

		FUNCTION_EXIT;
	}

    //
    // insertNewAlarm
    //
    void AlarmQueue::insertNewAlarm( const AlarmDetailCorbaDef& p_alarm )
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Inserting new alarm with id %s into the database", std::string(p_alarm.alarmID).c_str());


        IAlarmData * pTmpNewAlarm = AlarmAccessFactory::getInstance().createAlarm();
		std::auto_ptr<IAlarmData> newAlarm(pTmpNewAlarm);
		
            timeb sourceTime = {p_alarm.sourceTime.time,p_alarm.sourceTime.millitm,-1,-1};
            timeb transactiveTime = {p_alarm.transactiveTime.time,p_alarm.transactiveTime.millitm,-1,-1};

            newAlarm->setSourceTime(sourceTime);
			// Wenguang++ Alarm Message Size Reduction
            //newAlarm->setDescription(std::string(p_alarm.alarmDescription));
            newAlarm->setParamList(std::string(p_alarm.alarmParameterList));
            newAlarm->setEntityKey(p_alarm.assocEntityKey);
            newAlarm->setTransactiveTime(transactiveTime);
            newAlarm->setSeverity(p_alarm.alarmSeverity);
            newAlarm->setTypeKey(p_alarm.messageTypeKey);
            newAlarm->setContext(std::string(p_alarm.messageContext));
            newAlarm->addComment(std::string(p_alarm.alarmComments));
            newAlarm->setAssetName(std::string(p_alarm.assetName));
            newAlarm->setPlanStatus("");
			newAlarm->setAVLHeadId(std::string(p_alarm.avalancheHeadID));
            newAlarm->setAckTime(p_alarm.ackTime);
            newAlarm->setCloseTime(p_alarm.closeTime);
            // CL-21508
//             switch (p_alarm.state)
//             {
//                 case TA_Base_Core::AlarmClosed:
//                     newAlarm->setState(TA_Base_Core::IAlarmData::CLOSED);
//                     break;
//                 case TA_Base_Core::AlarmOpen:
//                     newAlarm->setState(TA_Base_Core::IAlarmData::OPEN);
//                     break;
//                 case TA_Base_Core::AlarmSuppressed:
//                     newAlarm->setState(TA_Base_Core::IAlarmData::SUPPRESSED);
//                     break;
//                 default:
//                     TA_ASSERT( false, "Unknown alarm state" );
//             }
			newAlarm->setState(AlarmQueueWrapper::convertAlarmStatus(p_alarm));
            // CL-21508
			newAlarm->setMmsStateType( AlarmQueueWrapper::convertMmsStateType( p_alarm.mmsState) );
			newAlarm->setPlanStatus( AlarmQueueWrapper::convertPlanState(p_alarm.decisionSupportState) );
			newAlarm->setAVLStatus( AlarmQueueWrapper::convertAVLStatus(p_alarm) );
            newAlarm->setIsUnique( p_alarm.isUniqueAlarm );

            newAlarm->applyChanges(std::string(p_alarm.alarmID));
    }


    //
    // updateAlarm
    //
    void AlarmQueue::updateAlarm( const AlarmUpdateCorbaDef& p_update )
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
             "Updating alarm with id %s in the database", std::string(p_update.alarmID).c_str());

        IAlarmData* pTmpAlarm  = AlarmAccessFactory::getInstance().getAlarm(std::string(p_update.alarmID));
		std::auto_ptr<IAlarmData> alarm(pTmpAlarm);

            // rather then an ever deeping nest of if-else-if...
            // we use a dummy while loop and break out
            while (1)
            {

                if( p_update.alarmHasBeenAcknowledged )
                {
                    alarm->setAcknowledgedBy(std::string(p_update.session));
					alarm->setOperatorName(std::string(p_update.alarmAcknowledgedBy));					 
                    alarm->setAckTime(p_update.ackTime);
                    break;
                }
                if( p_update.alarmHasBeenClosed )
                {
                    alarm->setState(TA_Base_Core::IAlarmData::CLOSED);
					// update the close time
                    alarm->setCloseTime(p_update.closeTime);
					// update the param list by appending the new close value (if any)
					std::string closeValue(p_update.closeValue.in());
					if (closeValue.empty()==false)
					{
						std::string paramList = alarm->getParamList();
						paramList += "ClosedValue:" + closeValue;
						paramList += ",";
						alarm->setParamList(paramList);
					}
                    break;
                }
                if( p_update.commentsWereAdded )
                {
                    alarm->addComment(std::string(p_update.alarmComments));
                    break;
                }
                if( p_update.decisionSupportStateHasChanged )
                {
                    alarm->setPlanStatus( AlarmQueueWrapper::convertPlanState(p_update.decisionSupportState) );
                    break;
                }
                if( p_update.alarmHasBeenOpened )
                {
                    alarm->setState(TA_Base_Core::IAlarmData::OPEN);
                    break;
                }
                if( p_update.mmsStateHasChanged )
                {
					alarm->setMmsStateType( AlarmQueueWrapper::convertMmsStateType(p_update.mmsState) );
					break; // break from the while loop
				}// end of if
				
				TA_ASSERT(false, "Unhandled alarm update");
            
			}// end of while loop

            alarm->applyChanges();
    }


    //
    // removeQueueItem
    //
    void AlarmQueue::removeQueueItem(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item)
    {
        if(0 != RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                 "Received a removeQueueItem request while in control mode. Ignoring." );
            return;
        }

        remove(p_item);
   
        AlarmCache::getInstance()->printCacheSize();
    }
    // CL-21508 end


    // CL-21508 end




    //
    // sendStateUpdates
    //
    void AlarmQueue::sendStateUpdates(TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item)
    {
        unsigned int len = m_queueItemsProcessed->length();
        len++;

        m_queueItemsProcessed->length(len);
        (*m_queueItemsProcessed)[len - 1] = *p_item;

        if( len >= m_queueUpdateBatchSize )
        {
            CORBA::Any stateUpdate;
            stateUpdate <<= *m_queueItemsProcessed;

            m_stateSender->sendStateUpdateMessage(
                TA_Base_Core::AlarmStateUpdate::QueueItemRemoved,
                0,
                RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(), stateUpdate);

            // CL-21508
            delete m_queueItemsProcessed;
            m_queueItemsProcessed = NULL;
            m_queueItemsProcessed = new TA_Base_Core::AlarmAgentQueueItemSequence();
        }

        // delete p_item; // delete the item last incase error happened above //using auto_ptr to handle deletion
    }


    //
    // safeToWrite
    //
    bool AlarmQueue::safeToWrite( const AlarmDetailCorbaDef& p_alarm )
    {
        if( m_alarmsToTest > 0 )
        {
            try
            {
                m_alarmsToTest--;
                IAlarmData* alarm = AlarmAccessFactory::getInstance().getAlarm(std::string(p_alarm.alarmID));
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                     "The alarm with ID %s already exists, not processing.", std::string(p_alarm.alarmID).c_str());
                // TD14341 fix memory leak problem
                delete alarm;
                return false;
            }
            catch( ... )
            {
                // Alarm unique, safe to write
                return true;
            }
        }
        return true;
    }

	
};
