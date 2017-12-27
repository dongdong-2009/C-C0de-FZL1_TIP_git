/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/PopupMessageSender.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Listens to FrontEndStateChangeSubject and sends messages for displaying to affected operators.
  *
  */

#include "DutyAgent.h"
#include "PopupMessageSender.h"
#include "HandoverPopupConverter.h"
#include "DegradedPopupConverter.h"

namespace TA_Base_App
{
	
	PopupMessageSender::PopupMessageSender() :
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::FrontEndStateChangeSubjectImpl>(*TA_Base_Bus::FrontEndStateChangeSubject::instance()),
		TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() ),
		m_commsSender(TA_Base_Core::MessagePublicationManager::getInstance().
			getCommsMessageSender(TA_Base_Core::DutyAgentBroadcastComms::Context))
	{
		FUNCTION_ENTRY("PopupMessageSender");
		FUNCTION_EXIT;
	}
	
		void PopupMessageSender::update( TA_Base_Bus::FrontEndStateChangeSubjectImpl& frontEndStateChange, const TA_Base_Bus::DutyState & state)
	{
		FUNCTION_ENTRY("update");

		//TA_Base_Bus::DutyState* state = frontEndStateChange.getStateUpdate();

		// TD19075 ++
		std::string strAction = state.getInitiatingAction();
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "[PopupMessageSender::update] Action:%s", strAction.c_str());
		// ++ TD19075 
		if( (DutyAction::REQUEST_SUBSYSTEM_DUTY_TRANSFER == state.getInitiatingAction()) ||
			(DutyAction::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER == state.getInitiatingAction()) ||
            (DutyAction::REQUEST_REGION_DUTY_TRANSFER == state.getInitiatingAction()) )
		{
			HandoverPopupConverter hpc;

			state.convert(hpc);

			HandoverPopupConverter::PopupData popupData;
			HandoverPopupConverter::PopupData::const_iterator popupIt;

			hpc.buildPopupData(popupData);

			for( popupIt=popupData.begin(); popupIt!=popupData.end(); popupIt++ )
			{
				CORBA::Any commsData;
				commsData <<= *popupIt;

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Sending DutyChangedNotification" );
				m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::DutyChangedNotification, commsData, NULL );
			}

			FUNCTION_EXIT;
			return;
		}

			
		if( (DutyAction::AUTOMATIC_DELEGATION == state.getInitiatingAction()) ||
			(DutyAction::SYSTEMWIDE_DELEGATION  == state.getInitiatingAction()) ||
			(DutyAction::SUBSYSTEM_DEGRADED  == state.getInitiatingAction()) )
		{
			DegradedPopupConverter dpc;

			state.convert(dpc);

			DegradedPopupConverter::PopupData popupData;
			DegradedPopupConverter::PopupData::const_iterator popupIt;

			dpc.buildPopupData(popupData);

			// TD19075 ++
			// Only for Degrade mode: SUBSYSTEM_DEGRADED
			// all entity who deal with the DutyAgentBroadcastComms message shoud be change
			// as I know, ControlStation and security_utilities handle this message
			// all those three message is concern with degrade mode
//			if (DutyAction::SUBSYSTEM_DEGRADED == strAction)
			if(true)
			{
				try
				{
					if (popupData.size() > 0)
					{
						TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopupSequence dutySeq;
						dutySeq.length(popupData.size());
						int nIndex = 0;
						for(popupIt=popupData.begin(); popupIt!=popupData.end(); popupIt++, ++nIndex )
						{
							dutySeq[nIndex] = *popupIt;
						}
						CORBA::Any commsData;
						commsData <<= dutySeq;
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "[PopupMessageSender::update] Start Sending DutyPopupSequence Message!");
						m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::DutyChangedNotification, commsData, NULL );
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "[PopupMessageSender::update] Send DutyPopupSequence Message Successful!");
					}
				}
				catch (...)
				{
					LOG(SourceInfo, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "[PopupMessageSender::update] Send DutyPopupSequence ERROR");
				}
				// this part is used to acknowledge Security_Utitlties.DutyUpdateSubject::receiveSpecialisedMessage
				// that function use targetSession to do something, so we will not modify that function
				TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup popDataEmpty;
				for( popupIt=popupData.begin(); popupIt!=popupData.end(); popupIt++ )
				{
					CORBA::Any commsData;
					popDataEmpty.targetSession = popupIt->targetSession;
					popDataEmpty.denied.length(0);
					popDataEmpty.lost.length(0);
					popDataEmpty.gained.length(0);
					commsData <<= popDataEmpty;
					
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Sending DutyChangedNotification" );
					m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::DutyChangedNotification, commsData, NULL );
				}
			}
			else// never happen
			{
			// ++ TD19075
			for( popupIt=popupData.begin(); popupIt!=popupData.end(); popupIt++ )
			{
				CORBA::Any commsData;
				commsData <<= *popupIt;

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Sending DutyChangedNotification" );
				m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::DutyChangedNotification, commsData, NULL );
			}
			// TD19075 ++
			}
			// ++ TD19075

			FUNCTION_EXIT;
			return;
		}


		FUNCTION_EXIT;
	}

	void PopupMessageSender::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
	{
		FUNCTION_ENTRY("update");

		FrontEndStateParent::detach();
		AgentTerminateParent::detach();

		FUNCTION_EXIT;
	}
}
