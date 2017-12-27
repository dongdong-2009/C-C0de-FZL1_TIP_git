/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyRequestSender.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Broadcasts duty requests containing a unique id. The response is received via the
  *  DutyResponseSubject.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DutyRequestSender::DutyRequestSender() :
		TA_Base_Bus::SeObserver<TA_Base_Bus::DutyResponseSubjectImpl>(*TA_Base_Bus::DutyResponseSubject::instance()),
		//m_responseFlag(0),
		m_state(NULL),
		m_subsToApply(NULL),
		m_subsToCancel(NULL),
		m_commsSender(TA_Base_Core::MessagePublicationManager::getInstance().
			getCommsMessageSender(TA_Base_Core::DutyAgentBroadcastComms::Context))

	{
		FUNCTION_ENTRY("DutyRequestSender");
		FUNCTION_EXIT;
	}

	// called from the message queue thread
	void DutyRequestSender::update( DutyResponseSubjectImpl& responseSubject )
	{
		FUNCTION_ENTRY("update");

		if( m_state == NULL )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "There are no outstanding requests" );
			FUNCTION_EXIT;
			return;
		}

		// wait until all messages have been sent
		ACE_Guard<ACE_Thread_Mutex> guard(m_sendLock);

		std::string responseId;
		IDutyAgentRespCorbaDef::EDutyResponse response(IDutyAgentRespCorbaDef::DR_DENY);

		responseSubject.getResponse( responseId, response );

		processResponse( responseId, response );
		
		//m_responseFlag.release();
		m_responseFlag.signal();

		FUNCTION_EXIT;
	}

	// called from the agent thread
	void DutyRequestSender::send( const DutyPrimitive& session, DutyState& state, DASType::SubsystemMap& subsToApply, 
			DASType::SubsystemMap& subsToCancel )
	{
		FUNCTION_ENTRY("send");

		if( m_state != NULL )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "There are already outstanding responses" );
			FUNCTION_EXIT;
			return;
		}

		try
		{
			m_state        = &state;
			m_subsToApply  = &subsToApply;
			m_subsToCancel = &subsToCancel;

			DutyRequestConverter requestConverter(session);

			// TD19417++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TDACCESS: ========  Request session=%s =======",
				session.m_session.c_str());
			// ++TD19417
			state.convert(requestConverter);

			requestConverter.buildRequestMap(m_outstandingRequests, state);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "m_outstandingRequests.size : %d",m_outstandingRequests.size() );
			if( m_outstandingRequests.size() > 0 )
			{
				sendRequests();

				waitForResponses();
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TDACCESS: ++++++ Before call DefaultApplyConvertor");
			DefaultApplyConverter dtc(subsToApply);

			state.convert(dtc);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TDACCESS: ++++++ After call DefaultApplyConvertor");
		}
		catch(...)
		{
			m_state        = NULL;
			m_subsToApply  = NULL;
			m_subsToCancel = NULL;
			m_outstandingRequests.clear();
			throw;
		}

		m_state        = NULL;
		m_subsToApply  = NULL;
		m_subsToCancel = NULL;
		m_outstandingRequests.clear();

		FUNCTION_EXIT;
	}

	// called from the agent thread
	void DutyRequestSender::waitForResponses()
	{
		FUNCTION_ENTRY("waitForResponses");

		if( m_state == NULL )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "There are no outstanding requests" );
			FUNCTION_EXIT;
			return;
		}

		ACE_Time_Value timeout = DutyDbCache::instance()->getPopupTimeout();
		ACE_Time_Value currentTime = ACE_OS::gettimeofday(); 
		ACE_Time_Value waitUntil( currentTime + timeout );
		
		int waitResult = 0;
		bool continueWaiting = true;

		while( continueWaiting )
		{
			if( m_outstandingRequests.size() > 0 )
			{
				currentTime = ACE_OS::gettimeofday();
				if( waitUntil > currentTime )
				{
					// calculate the remaining time
					timeout = waitUntil - currentTime;
					//waitResult = m_responseFlag.acquire(waitUntil);
					waitResult = m_responseFlag.timedWait(timeout.msec());

					//if( waitResult == -1 || m_outstandingRequests.size() == 0 )
					if( waitResult == 0 || m_outstandingRequests.size() == 0 )
					{
						//if( waitResult == -1 )
						if( waitResult == 0 )
						{
							//LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "ACE_Thread_Semaphore timed acquire error: %d", ACE_OS::last_error() );
							LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "TA_Base_Core::Condition timed out" );
						}

						continueWaiting = false;
					}
				}
				else
				{
					continueWaiting = false;
				}
			}
			else
			{
				continueWaiting = false;
			}
			
		}			

		//if( waitResult == -1 )
		if( waitResult == 0 )
		{
			DutyRequestConverter::RequestMap::iterator it;

			for( it=m_outstandingRequests.begin(); it!=m_outstandingRequests.end(); it++ )
			{
				processTimeout( it->second );
			}

			m_outstandingRequests.clear();
		}

		TA_ASSERT( m_outstandingRequests.size() == 0, "There are still outstanding requests" );

		FUNCTION_EXIT;
	}

	// called from the message queue thread
	void DutyRequestSender::processResponse( const std::string& responseId,
		const	IDutyAgentRespCorbaDef::EDutyResponse& response )
	{
		FUNCTION_ENTRY("processResponse");

		TA_ASSERT( m_state != NULL      , "No requests have been sent" );
		TA_ASSERT( m_subsToApply != NULL, "No requests have been sent" );
		TA_ASSERT( m_subsToApply != NULL, "No requests have been sent" );


		DutyRequestConverter::RequestMap::iterator it;

		if( (it = m_outstandingRequests.find(responseId)) != m_outstandingRequests.end() )
		{
			switch( response )
			{
				case IDutyAgentRespCorbaDef::DR_ACCEPT:
				{
					processAccept( it->second );
				}
				break;

				case IDutyAgentRespCorbaDef::DR_DENY:
				{
					processDeny( it->second );
				}
				break;

				case IDutyAgentRespCorbaDef::DR_TIMEOUT:
				{
					processTimeout( it->second );
				}
				break;

				default:
				{
					// bad response
					processAccept( it->second );
				}
				break;
			}

			m_outstandingRequests.erase(it);
		}
		FUNCTION_EXIT;
	}

 	void DutyRequestSender::processAccept( DutyAgentTypeCorbaDef::DutyRequest& request )
 	{
 		FUNCTION_ENTRY("processAccept");
 
 		unsigned long regIdx = 0;
 		unsigned long subIdx = 0;
 		std::pair<DASType::SubsystemMap::iterator,bool> insertResult;
 		DASType::SubsystemSet emptySubSet;
 		
 		for( regIdx=0; regIdx<request.duty.subsystems.length(); regIdx++ )
 		{
 			insertResult = m_subsToApply->insert( DASType::SubsystemMap::value_type(
 					request.duty.subsystems[regIdx].region, emptySubSet) );
 
 			for( subIdx=0; subIdx<request.duty.subsystems[regIdx].subsystems.length(); subIdx++ )
 			{
 				insertResult.first->second.insert( request.duty.subsystems[regIdx].subsystems[subIdx] );
 			}
 		}
 		FUNCTION_EXIT;
 	}


	void DutyRequestSender::processDeny( DutyAgentTypeCorbaDef::DutyRequest& request )
	{
		FUNCTION_ENTRY("processDeny");

		unsigned long regIdx = 0;
		unsigned long subIdx = 0;
		std::pair<DASType::SubsystemMap::iterator,bool> insertResult;
		DASType::SubsystemSet emptySubSet;

		for( regIdx=0; regIdx<request.duty.subsystems.length(); regIdx++ )
		{
			insertResult = m_subsToCancel->insert( DASType::SubsystemMap::value_type(
					request.duty.subsystems[regIdx].region, emptySubSet) );

			for( subIdx=0; subIdx<request.duty.subsystems[regIdx].subsystems.length(); subIdx++ )
			{
				m_state->processDeny( request.duty.subsystems[regIdx].region,
					request.duty.subsystems[regIdx].subsystems[subIdx] );
				
				insertResult.first->second.insert( request.duty.subsystems[regIdx].subsystems[subIdx] );
			}
		}
		FUNCTION_EXIT;
	}

	void DutyRequestSender::processTimeout( DutyAgentTypeCorbaDef::DutyRequest& request )
	{
		FUNCTION_ENTRY("processTimeout");

		unsigned long regIdx = 0;
		unsigned long subIdx = 0;
		std::pair<DASType::SubsystemMap::iterator,bool> insertResult;
		DASType::SubsystemSet emptySubSet;

		for( regIdx=0; regIdx<request.duty.subsystems.length(); regIdx++ )
		{
			insertResult = m_subsToApply->insert( DASType::SubsystemMap::value_type(
					request.duty.subsystems[regIdx].region, emptySubSet) );

			for( subIdx=0; subIdx<request.duty.subsystems[regIdx].subsystems.length(); subIdx++ )
			{
				m_state->processTimeout( request.duty.subsystems[regIdx].region,
					request.duty.subsystems[regIdx].subsystems[subIdx] );

				insertResult.first->second.insert( request.duty.subsystems[regIdx].subsystems[subIdx] );
			}
		}
		FUNCTION_EXIT;
	}

	void DutyRequestSender::sendRequests()
	{
		FUNCTION_ENTRY("sendRequests");
	
		DutyRequestConverter::RequestMap::const_iterator it;

		ACE_Guard<ACE_Thread_Mutex> guard(m_sendLock);

		for( it=m_outstandingRequests.begin(); it!=m_outstandingRequests.end(); it++ )
		{
			CORBA::Any commsData;

			commsData <<= it->second;
		
			m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::DutyRequest, commsData, NULL );
		}

		FUNCTION_EXIT;
	}
}
