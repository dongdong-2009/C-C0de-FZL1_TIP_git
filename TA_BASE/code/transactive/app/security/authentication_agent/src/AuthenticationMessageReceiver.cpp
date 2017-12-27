/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/AuthenticationMessageReceiver.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class is the listener for the incoming distributed update messages.
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <time.h>

#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"

#include "app/security/authentication_agent/src/AuthenticationMessageReceiver.h"
#include "app/security/authentication_agent/src/Authentication.h"
#include "bus/security/authentication_library/src/SessionInfo.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/AuthenticationAgentStateUpdateBroadcast_MessageTypes.h"

#include "core/exceptions/src/AuthenticationSecurityException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/NameValuePair.h"

using TA_Base_Bus::Authentication::BASE_OPERATOR;
using TA_Base_Bus::Authentication::BASE_PROFILE;

using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::AuthenticationSecurityException;
using TA_Base_Bus::SessionInfo;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{

    //
    // Constructor
    //

    AuthenticationMessageReceiver::AuthenticationMessageReceiver(SessionRegister& reg) :
		m_sessionRegister(reg),
		m_stateSender( TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(
			TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::Context)),
		m_shutdown(false)

    {
        FUNCTION_ENTRY("AuthenticationMessageReceiver::AuthenticationMessageReceiver() constructor");
        FUNCTION_EXIT;
    }
    
	AuthenticationMessageReceiver::~AuthenticationMessageReceiver()
	{
		shutdownReceiver();
	}

    // 
    // receiveSpecialisedMessage
    //

    void AuthenticationMessageReceiver::receiveSpecialisedMessage(
        const TA_Base_Core::CommsMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");

        if( 0 == std::string(message.messageTypeKey).compare(TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedUpdate.getTypeKey()) )
        {
            //TD15069 azenitha++
            if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {
                LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                    "[TD15069] Expected receiving of AuthenticationAgentDistributedUpdate (System is in Control Mode)");
                //continue processing                
            }
            else
            {
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                    "[TD15069] Unexpected receiving of AuthenticationAgentDistributedUpdate (System is in Monitor Mode)");
                return;
            }
            //TD15069 ++azenitha

            // This is a message that a session has been created, modified, or ended at another Authentication Agent

            // There are two types of message possible - start or end session. Both, however, are transported the same way.
        
            // Now pull out the actual object in the message
            TA_Base_Bus::SessionInfoCorbaDef* session;
            if (0 != ( message.messageState >>= session ) )
            {
                LOG_GENERIC (SourceInfo, DebugUtil::DebugDebug, "Received AuthenticationAgentDistributedUpdate for session '%s'",
                    session->SessionId.in() );

                processSession(session);
            }
            else
            {
                // Bad - but can't do anything to prevent it, so just log.
                LOG_GENERIC (SourceInfo, DebugUtil::DebugWarn, "Failed attempt to extract distributed state update from any.");
            }
        }
        else if ( 0 == std::string(message.messageTypeKey).compare(TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedBatchStateUpdate.getTypeKey()) )
        {
            //TD15069 azenitha++
            if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {
                LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                    "[TD15069] Expected receiving of AuthenticationAgentDistributedBatchStateUpdate (System is in Control Mode)");
                //continue processing                
            }
            else
            {
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                    "[TD15069] Unexpected receiving of AuthenticationAgentDistributedBatchStateUpdate (System is in Monitor Mode)");
                return;
            }
            //TD15069 ++azenitha

            // This message is delivered if any of the agents request it.

            // Now pull out the actual object in the message
            TA_Base_Bus::AuthentcationBatchUpdateCorbaDef* batchUpdate;
            if ( ( message.messageState >>= batchUpdate) != 0 )
            {
                LOG_GENERIC (SourceInfo, DebugUtil::DebugDebug, "Received AuthenticationAgentDistributedBatchStateUpdate with %d updates.",
                    batchUpdate->SessionRegister.length() );

                // The update doesn't contain any useful information in the database sequence, so only need to look at the 
                // register sequence.
                for (unsigned int i = 0;i < batchUpdate->SessionRegister.length();i++)
                {
                    // Now process the update.
                    processSession( &batchUpdate->SessionRegister[i] );
                }
            }
            else
            {
                // Bad - but can't do anything to prevent it, so just log.
                LOG_GENERIC (SourceInfo, DebugUtil::DebugWarn, "Failed attempt to extract distributed batch state update from any.");
            }
        }
        else if ( 0 == std::string(message.messageTypeKey).compare(TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedStateUpdateRequest.getTypeKey()) )
        {
            //TD15069 azenitha++
            if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {
                LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                    "[TD15069] Expected receiving of AuthenticationAgentDistributedStateUpdateRequest (System is in Control Mode)");
                //continue processing                
            }
            else
            {
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                    "[TD15069] Unexpected receiving of AuthenticationAgentDistributedStateUpdateRequest (System is in Monitor Mode)");
                return;
            }
            //TD15069 ++azenitha

            LOG_GENERIC (SourceInfo, DebugUtil::DebugDebug, "Received AuthenticationAgentDistributedStateUpdateRequest, sending batch state update." );

            // This message doesn't contain any data that needs to be extract.

            // Send a batchstateupdate message
            sendBatchStateUpdate();
        }
        else
        {
            // Unrecognised message type
            LOG_GENERIC (SourceInfo, DebugUtil::DebugWarn, "Received unexpected message (Message Receiver).");
        }

        FUNCTION_EXIT;
        
    }

    // 
    // shutdownReceiver
    //

    void AuthenticationMessageReceiver::shutdownReceiver()
    {
        FUNCTION_ENTRY("shutdownReceiver");
		if( !m_shutdown )
		{
			MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
			m_shutdown = true;
		}
        FUNCTION_EXIT;
    }

    // 
    // startReceiver
    //

    void AuthenticationMessageReceiver::startReceiver()
    {
        FUNCTION_ENTRY("startReceiver");
        TA_Base_Core::FilterData filterData;

        // agentName info 
        TA_Base_Core::NameValuePair nvp(Authentication::AGENT_ID_STRING, 
            TA_Base_Core::NameValuePair::NOT_EQUALS, // Not equals equality check
            Authentication::AGENT_ID);

        filterData.push_back(&nvp);

        // Register the subscriber with the MessageSubscribtionManager

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage
				(TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedUpdate, this, &filterData);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                "[TD15069] No need for subscription to AuthenticationAgentDistributedUpdate (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage
				(TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedBatchStateUpdate, this, &filterData);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                "[TD15069] No need for subscription to AuthenticationAgentDistributedBatchStateUpdate (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha


        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage
				(TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedStateUpdateRequest, this, &filterData);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                "[TD15069] No need for subscription to AuthenticationAgentDistributedStateUpdateRequest (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        FUNCTION_EXIT;
    }

    void AuthenticationMessageReceiver::processSession(TA_Base_Bus::SessionInfoCorbaDef* session)
    {
        FUNCTION_ENTRY("processSession");

        // The session will either be a new session, an old session, or a profile update.
        if ( session->updateType == TA_Base_Bus::TA_Security::SessionStart )
        {
            // A new session has been created in the system.

            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Processing SessionStart for session '%s' (U:%d, W:%d, L:%d).",
                session->SessionId.in(), session->UserId, session->WorkstationId, session->LocationId );

            // Get the session ID
            std::string sessionId = session->SessionId.in();

            // Only add the session if it is not already in the register.
            if ( ! m_sessionRegister.isSessionInRegister( sessionId ) )
            {
                // The addSession method takes a SessionInfo object, so consturct one
                SessionInfo sessionInfo;

                sessionInfo.UserId = session->UserId;

                sessionInfo.SessionId = sessionId;
                sessionInfo.LocationId = session->LocationId;
                sessionInfo.WorkstationId = session->WorkstationId;

                unsigned int i;

                sessionInfo.OperatorId.push_back(session->UserId);
                for ( i=0; i<session->additionalOperators.length(); i++ )
                {
                    sessionInfo.OperatorId.push_back( session->additionalOperators[i] );
                }
                sessionInfo.ProfileId.push_back(session->ProfileId);
                for ( i=0; i<session->additionalProfiles.length(); i++ )
                {
                    sessionInfo.ProfileId.push_back( session->additionalProfiles[i] );
                }

                try
                {
                    m_sessionRegister.addSession(sessionInfo,true);

					//TD15069 azenitha++
					LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"[TD15069] m_sessionRegister.addSession() called (%s)", sessionId.c_str());
					//TD15069 azenitha++
                }
                catch ( AuthenticationSecurityException& )
                {
                    // Just in case the session was added while the object was being constructed - do nothing.
                }
            }
            else
            {
                // Due to the way the synchronisation messages are handled attempting to add a session to the
                // register when it already exists is not an error condition. Instead just log a debug message
                // stating what was attempted.
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Session '%s' already exists in the register, ignore it.",
                    sessionId.c_str() );
            }
        }
        else if ( session->updateType == TA_Base_Bus::TA_Security::ProfileUpdate )
        {
            // The list of operators and profiles for the session have changed, most likely as a result
            // of an override.

            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Processing ProfileUpdate for session '%s' (U:%d, W:%d, L:%d).",
                session->SessionId.in(), session->UserId, session->WorkstationId, session->LocationId );

            // Get the session ID
            std::string sessionId = session->SessionId.in();

            // Make sure the session is in the register before updating.
            if ( m_sessionRegister.isSessionInRegister( sessionId ) )
            {
                try
                {
                    m_sessionRegister.clearOverrides( sessionId );

					//TD15069 azenitha++
					LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"[TD15069] m_sessionRegister.clearOverrides() called (%s)", sessionId.c_str());
					//TD15069 azenitha++

                    // Number of operators must match the number of profiles.
                    TA_ASSERT( session->additionalOperators.length()==session->additionalProfiles.length(), "Operator <-> Profile mismatch" );

                    for ( unsigned int i=0; i<session->additionalProfiles.length(); i++ )
                    {
                        m_sessionRegister.addOverride(sessionId,session->additionalOperators[i],session->additionalProfiles[i]);

						//TD15069 azenitha++
						LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
							"[TD15069] m_sessionRegister.addOverride() called (%s)", sessionId.c_str());
						//TD15069 azenitha++
                    }
                }
                catch ( AuthenticationSecurityException& )
                {
                    // If the session is not in the register, can't do anything about it.
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Received a ProfileUpdate for session '%s', which is not in the register.",
                    sessionId.c_str() );
            }
        }
        else if ( session->updateType == TA_Base_Bus::TA_Security::SessionEnd )
        {
            // An existing session has been removed from the system.

            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Processing SessionEnd for session '%s' (U:%d, W:%d, L:%d).",
                session->SessionId.in(), session->UserId, session->WorkstationId, session->LocationId );

            // Make sure the session is in the register before ending.
            if ( m_sessionRegister.isSessionInRegister( session->SessionId.in() ) )
            {
                // HACK until we can find the real problem this is a workaround...
                // make sure it's not current active session

				//TD15069 azenitha++
				if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
                {
					time_t lastActiveTime;
					if ( m_sessionRegister.getLastSessionActiveTime( session->SessionId.in(), lastActiveTime ))
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Processing SessionEnd for session '%s' but this session is in SessionActiveTime!", session->SessionId.in());
						time_t timeNow;
						time(&timeNow);
						unsigned long timeout = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSION_TIMEOUT_PERIOD).c_str(), NULL, 10);

						if ( timeNow - lastActiveTime < timeout)
						{
							// hmmm. someone thinks our current session has gone away
							// we know better so we better tell them
							sendBatchStateUpdate();

							FUNCTION_EXIT;
							return;
						}
					}
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "[TD15069] Unexpected sending of state update message (System is in Monitor Mode)");
				}
				//TD15069 ++azenitha

				try
				{
					m_sessionRegister.removeSession( session->SessionId.in() );

					//TD15069 azenitha++
					LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"[TD15069] m_sessionRegister.removeSession() called (%s)", session->SessionId.in());
					//TD15069 azenitha++
				}
				catch ( AuthenticationSecurityException& )
				{
					// The session was not on the map. Can't do anything about it.
				}
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Received a SessionEnd for session '%s', which is not in the register.",
                    session->SessionId.in() );
            }
        }
        FUNCTION_EXIT;
    }


    void AuthenticationMessageReceiver::sendBatchStateUpdate()
    {
        FUNCTION_ENTRY("sendBatchStateUpdate");
        // Create an any into which the sequence can be placed
        CORBA::Any stateUpdate;

        // Create the batch update. It contains two sequences.
        TA_Base_Bus::AuthentcationBatchUpdateCorbaDef batchUpdate;

        // The first is the database queue. For these updates, it is not needed.
        batchUpdate.DatabaseQueue.length(0);

        // Grab the session register
        std::vector<SessionInfo> reg = m_sessionRegister.getRegister();

        // And bundle it into the other sequence - setting the length first
        batchUpdate.SessionRegister.length(reg.size());

        // Now loop through the vector and add items to the sequence for each one.
        for (unsigned int i = 0;i < reg.size();i++)
        {
            // Ensure there is at least one operator and profile for this session
            TA_ASSERT(reg[i].OperatorId.size() > 0, "The OperatorId vector was empty");
            TA_ASSERT(reg[i].ProfileId.size() > 0,"The ProfileId vector was empty");

            // Currently the batch update contains only sessions that should be added to the requesting
            // agents register if they don't already exist. I.e. All updates are of type 'SessionStart'.
            batchUpdate.SessionRegister[i].updateType = TA_Base_Bus::TA_Security::SessionStart;

            batchUpdate.SessionRegister[i].SessionId = CORBA::string_dup(reg[i].SessionId.c_str());
            batchUpdate.SessionRegister[i].WorkstationId = reg[i].WorkstationId;
            batchUpdate.SessionRegister[i].LocationId = reg[i].LocationId;
            batchUpdate.SessionRegister[i].ProfileId = reg[i].ProfileId[BASE_PROFILE];
            batchUpdate.SessionRegister[i].UserId = reg[i].OperatorId[BASE_OPERATOR];

            unsigned int j;

            batchUpdate.SessionRegister[i].additionalOperators.length( reg[i].OperatorId.size()-1 );
            for ( j=1; j<reg[i].OperatorId.size(); j++ )
            {
                batchUpdate.SessionRegister[i].additionalOperators[j-1] = reg[i].OperatorId[j];
            }

            batchUpdate.SessionRegister[i].additionalProfiles.length( reg[i].ProfileId.size()-1 );
            for ( j=1; j<reg[i].ProfileId.size(); j++ )
            {
                batchUpdate.SessionRegister[i].additionalProfiles[j-1] = reg[i].ProfileId[j];
            }
        }

        // Place the sequence into the state update
        stateUpdate <<= batchUpdate;
        
        TA_Base_Core::FilterData filterData;

        // agentName info 
        TA_Base_Core::NameValuePair nvp(Authentication::AGENT_ID_STRING,
            Authentication::AGENT_ID);

        filterData.push_back(&nvp);

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// Send the message
			m_stateSender->sendBroadcastCommsMessage(
				TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedBatchStateUpdate,
				stateUpdate,
				&filterData);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "[TD15069] Unexpected sending of AuthenticationAgentDistributedBatchStateUpdate (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        FUNCTION_EXIT;
    }
}
