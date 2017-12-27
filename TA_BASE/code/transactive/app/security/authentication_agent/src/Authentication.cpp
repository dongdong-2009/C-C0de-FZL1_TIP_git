/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/Authentication.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IAuthentication interface. It will service all incomming CORBA requests.
 */

#if defined (_MSC_VER)
#pragma warning(disable:4786) // identifier was truncated to 'number' characters in the debug information
#endif // defined (_MSC_VER)

#include <ctime>
#include <algorithm>
#include <iomanip>
#include <iostream>

#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"

#include "app/security/authentication_agent/src/SessionRegister.h"
#include "app/security/authentication_agent/src/AuthenticationMessageReceiver.h"
#include "app/security/authentication_agent/src/Authentication.h"
#include "app/security/authentication_agent/src/AuthenticationAgentImpl.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "core/uuid/src/TAuuid.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RightsAgentEntityData.h" // yanrong
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "core/message/IDL/src/GenericMessageCorbaDef.h"
#include "core/message/types/AuthenticationAgentStateUpdateBroadcast_MessageTypes.h"
#include "core/message/types/AuthenticationAgentStateUpdate_MessageTypes.h"
#include "core/message/types/AuthenticationAgentAudit_MessageTypes.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/exceptions/src/RightsException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/uuid/src/TAuuid.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/database/src/DBException.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h" // CL-20822++
#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"

#include <time.h>
using TA_Base_Bus::Authentication::BASE_OPERATOR;
using TA_Base_Bus::Authentication::BASE_PROFILE;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_Bus;
using TA_Base_Core::AuthenticationAgentException;

using TA_Base_Core::AuthenticationSecurityException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataConfigurationException;

namespace
{
    const unsigned short DUTY_AGENT_CALL_TIMEOUT( 120 );
	const unsigned short RIGHTS_AGENT_CALL_TIMEOUT( 120 ); //yanrong
	// response time delay for corba call to DutyAgent, equivalent to RemoateDutyPeer::TIME_OF_FLIGHT
	const unsigned short RESPONSE_TIME( 10 );	// CL-20822++
};

namespace TA_Base_App
{
	
	const std::string Authentication::AUDIT_MSG_CONSOLE_NAME ("consolename" );
	const std::string Authentication::AUDIT_MSG_OPERATOR_ID  ("OperatorID"  );
	const std::string Authentication::AUDIT_MSG_PROFILE_ID   ("ProfileID"   );
	const std::string Authentication::AUDIT_MSG_SUPERVISOR_ID("SupervisorID");

    const std::string Authentication::AGENT_NAME = "AuthAgent";
    const std::string Authentication::AGENT_ID = TA_Base_Core::TA_Uuid::generateUUID().c_str();
    const std::string Authentication::AGENT_ID_STRING = "AgentId";
    const int Authentication::MIN_PASSWORD_LENGTH = 8;
    const int Authentication::MAX_PASSWORD_LENGTH = 50;

    Authentication::Authentication(SessionQueue& queue, SessionQueueProcessor& queueProcessor, 
                                    bool isControl, unsigned long entityKey) :
		m_sessionQueue(queue),
        m_sessionQueueProcessor(queueProcessor),
		m_sessionRegister(isControl,entityKey),
		//m_messageReceiver(m_sessionRegister),
		m_deadSessionCheckerThread(isControl),
		m_entityKey(entityKey),
		m_localStateSender(TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
			TA_Base_Core::AuthenticationAgentStateUpdate::Context)),
		m_bcastStateSender(TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(
			TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::Context)),
		m_auditSender(TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
			TA_Base_Core::AuthenticationAgentAudit::Context)),
		
		m_configSender(TA_Base_Core::MessagePublicationManager::getInstance().getConfigUpdateMessageSender(
			TA_Base_Core::ConfigUpdate::ConfigOperator))
    {
        FUNCTION_ENTRY("Authentication::Authentication() constructor");
		m_deadSessionCheckerThread.setAuthenticator(this);

        TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "Location Key not set" );
        m_localLocationId = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );
        FUNCTION_EXIT;
    }


    Authentication::~Authentication()
    {
        FUNCTION_ENTRY("destructor");
        try
        {
			m_deadSessionCheckerThread.terminateAndWait();
        }
        catch(...)
        {
            // Catch to ensure nothing leaks from ~.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "An exception was prevented from leaking out of the destructor");
        }
        FUNCTION_EXIT;
    }

	void Authentication::checkWorkstationExist(unsigned long workstationId )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"[2011-11-08] Authentication::checkWorkstationExist");
        try
        {
            TA_Base_Core::ConsoleAccessFactory::getInstance().getConsole(workstationId,false);
        }
        catch(TA_Base_Core::DataException& ex)
        {
            // If this is of type NO_VALUE, throw a security exception, otherwise throw an agent exception
            if (TA_Base_Core::DataException::NO_VALUE == ex.getFailType())
            {
                TA_THROW(TA_Base_Core::AuthenticationSecurityException("The workstation is not known to the system",
                TA_Base_Core::AuthenticationSecurityException::INVALID_WORKSTATION) );
            }
            else
            {
                TA_THROW(TA_Base_Core::AuthenticationAgentException("An error occurred while checking the workstation") );
            }
        }
        catch(TA_Base_Core::DatabaseException&)
        {            
            TA_THROW(TA_Base_Core::AuthenticationAgentException("An error occurred while checking the workstation") );
        }
        catch(TA_Base_Core::DataConfigurationException&)
        {
            TA_THROW(TA_Base_Core::AuthenticationAgentException("An error occurred while checking the workstation") );
        }
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"[2011-11-08] Authentication::checkWorkstationExist => Leaving.....");
	}

    std::string Authentication::requestSession(unsigned long userId, unsigned long profileId, unsigned long locationId, 
		unsigned long workstationId, const std::string & password,bool displayOnly)
    {
        FUNCTION_ENTRY("requestSession");

		TA_Base_Bus::GenericAgent::ensureControlMode();

		//TD18707 xinsong++
		//the locationId should be same as the agent id
		ensureIsLocalLocationId( locationId );

        // Scope in some classes that are needed in this method.
        using TA_Base_Core::SessionAccessFactory;
        using TA_Base_Core::ISession;
        using TA_Base_Core::DataConfigurationException;
        using TA_Base_Core::AuthenticationAgentException;
        using TA_Base_Core::AuthenticationSecurityException;
        using TA_Base_Bus::SessionInfo;

        // Retrieve the details for the specified user.
        UserCache::UserDetails user = m_userCache.getUserDetails(userId);

        // Need to make three security checks - the user must be able to use the specified profile, the profile must 
        // be able to be used in the specified location, and the users password must be correct. 
        // This method will thrown an exception if the session is not valid therefore it is sufficient to simply not catch it.
        std::string profileName;
        try
        {
            profileName = checkUserSecurity(user,profileId,locationId,password);
        }
        catch (AuthenticationSecurityException&)
        {
			//TD15069 azenitha++
			if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
			{
				sendSessionStateAuditMessage(" ",getUserName(userId), getWorkstationName(workstationId),
					getProfileName(profileId), TA_Base_Core::AuthenticationAgentAudit::AuthenticationRequestDenied);
				throw;
			}
			else
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
					"Unexpected sending of AuthenticationRequestDenied (System is in Monitor Mode)");
				throw;
			}
			//TD15069 ++azenitha
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"[CL-20784] New Session request, user:%lu, profile:%lu, location:%lu, workstation:%lu, security check pass",
			userId, profileId, locationId, workstationId);
        // Need to ensure there are no sessions in the register for the same user/profile/workstation combination.
        try
        {
            endOldDeadSessions(user,profileId,workstationId);
        }
        catch(TA_Base_Core::TransactiveException& ex)
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "An error occurred while removing a old dead session. New session will not be created");

            throw ex;
        }

        // Check the workstation exists - do this by asking for it from the console access factory. If it does
        // not exist, it will throw a database exception type and we can throw a security exception.
		checkWorkstationExist(workstationId);

        // At this point, the session is valid, and any "dead" sessions have been removed.
		// Time to create the new session.
        std::auto_ptr<ISession> theSession( SessionAccessFactory::getInstance().createSession( userId,
			workstationId, profileId, m_localLocationId, m_entityKey,displayOnly ) );

        // Need to convert the ISession into a SessionInfo that is added to the register
        SessionInfo session;

        session.UserId = theSession->getOperatorKey();

        session.SessionId = theSession->getSessionId();
        session.OperatorId.push_back(theSession->getOperatorKey());
        session.ProfileId.push_back(theSession->getProfileKey());
        session.WorkstationId = theSession->getConsoleKey();
        session.LocationId = locationId;
		session.isDisplayOnly = displayOnly;

        // Add it to the session register
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Authentication::requestSession, addSession: %s", session.SessionId.c_str() );
        m_sessionRegister.addSession(session, true);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Authentication::requestSession, addSession Done");

		try
		{
			/*[2011-11-04]
			// yanrong: notify the rights agent to update session cache
			CORBA_CALL( m_rightsAgent,
			sessionUpdate,
			( session.SessionId.c_str() ) );*/


			IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
				AuthenticationLibrary::convertCSessionToCORBASession(session);
			CORBA_CALL( m_rightsAgent,
				sessionUpdate,
				( corbaSession,false) );

		}
		catch( TA_Base_Core::OperationModeException& )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught OperationModeException when call sessionUpdate to RightsAgent.");
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught Unknown Exception when call sessionUpdate to RightsAgent.");
		}



        try
        {
			// Notify the duty agent
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-20822] authAllocateDuty call start" );
			CORBA_CALL( m_dutyAgent,
			            authAllocateDuty,
			            ( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session) ) );
        }
        catch(TA_Base_Core::TransactiveException& ex)
        {
			// CL-20822 free duty even allocateDuty action failed!
			try
			{
				// set corba call timeout to 1 second
				// we do not care about Duty Free result
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"[CL-20822] authFreeDuty when authAllocateDuty failed!!!" );
				omniORB::setClientThreadCallTimeout( 1 * 1000 );
				CORBA_CALL( m_dutyAgent,
					authFreeDuty,
					( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session) ) );
			}
			catch(TA_Base_Core::TransactiveException& ex)
			{
				// Any problem here will cause the request to fail...so need to remove the session from the register,
				// then throw an exception.
				std::string message("An error occurred while deallocating duty in requestSession. Session duties may not be reallocated. Error is: ");
				message += ex.what();
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,message.c_str());
				
			}
			catch(CORBA::Exception& ex)
			{
				// Any problem here will cause the request to fail...so need to remove the session from the register,
				// then throw an exception.
				std::string message("An error occurred while deallocating duty in requestSession. Session duties may not be reallocated. Error is: ");
				message += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,message.c_str());
			}
			catch(...)
			{
				// If the duty agent cannot be notified, throw an exception, and do not end the session.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
					"An error occurred while deallocating duty in requestSession. Session duties may not be reallocated.");
			}
			// also increase RemoteDutyPeer queue size and corba call delay
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-20822] authFreeDuty finished!" );

            // Any problem here will cause the request to fail...so need to remove the session from the register,
            // then throw an exception.
            m_sessionRegister.removeSession(session.SessionId);

            std::string message("An error occurred while allocating duty: ");
            message += ex.what();

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-20822] authAllocateDuty call end" );
            TA_THROW(AuthenticationAgentException(message.c_str()));
            
        }
        catch(CORBA::Exception& ex)
        {
			// CL-20822 free duty even allocateDuty action failed!
			try
			{
				// set corba call timeout to 1 second
				// we do not care about Duty Free result
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"[CL-20822] authFreeDuty when authAllocateDuty failed!!!" );
				omniORB::setClientThreadCallTimeout( 1 * 1000 );
				CORBA_CALL( m_dutyAgent,
					authFreeDuty,
					( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session) ) );
			}
			catch(TA_Base_Core::TransactiveException& ex)
			{
				// Any problem here will cause the request to fail...so need to remove the session from the register,
				// then throw an exception.
				std::string message("An error occurred while deallocating duty in requestSession. Session duties may not be reallocated. Error is: ");
				message += ex.what();
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,message.c_str());
				
			}
			catch(CORBA::Exception& ex)
			{
				// Any problem here will cause the request to fail...so need to remove the session from the register,
				// then throw an exception.
				std::string message("An error occurred while deallocating duty in requestSession. Session duties may not be reallocated. Error is: ");
				message += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,message.c_str());
			}
			catch(...)
			{
				// If the duty agent cannot be notified, throw an exception, and do not end the session.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
					"An error occurred while deallocating duty in requestSession. Session duties may not be reallocated.");
			}
			// also increase RemoteDutyPeer queue size and corba call delay
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-20822] authFreeDuty finished!" );
			
            // Any problem here will cause the request to fail...so need to remove the session from the register,
            // then throw an exception.
            m_sessionRegister.removeSession(session.SessionId);

            std::string message("An error occurred while allocating duty: ");
            message += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-20822] authAllocateDuty call end" );
            TA_THROW(AuthenticationAgentException(message.c_str()));
        }
        catch(...)
        {
            // Any problem here will cause the request to fail...so need to remove the session from the register,
            // then throw an exception.
            m_sessionRegister.removeSession(session.SessionId);

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-20822] authAllocateDuty call end" );
            TA_THROW(AuthenticationAgentException("An error occurred while allocating duty"));
        }
		//TD15901++ 
		//Write the session to DB first, timeout will erase this session from the list if duty alloc is not successful
		// Attempt to write the session to the database synchronously. If this fails, it will be added to
		// the monitored queue and written at a later stage. The SessionQueueProcessor will take responsibility 
		// for the life-time of the session object, so do not need to delete it in here.
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"write Session[%s] to db",theSession->getSessionId().c_str());
		m_sessionQueueProcessor.writeNewSession(theSession.release());
		//++TD15901
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"[CL-20822] authAllocateDuty call end" );



        // Get the name of the profile to use in sending the messages

        // Send the session events
        sendStateUpdateMessage(session, SessionStart);
        sendSessionNotification(session, SessionStart);

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			sendSessionStateAuditMessage(session.SessionId,"", getWorkstationName(workstationId),profileName,
				TA_Base_Core::AuthenticationAgentAudit::SessionStarted);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "Unexpected sending of request Session (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        // Now need to return the SessionId. 
        FUNCTION_EXIT;
        return session.SessionId;
    }

    void Authentication::endSession(const std::string & sessionId)
    {
        FUNCTION_ENTRY("endSession");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Authentication::endSession start");
		TA_Base_Bus::GenericAgent::ensureControlMode();

        // Get the information on this session first, so we can send a message later
        TA_Base_Bus::SessionInfo session;

        // Ending a session is entirely handled by the SessionRegister
        try
        {
			// ly++: remove those session after call DutyAgent and RightsAgent as they may want to retrieve
			// those session info from sessionRegister
			if ( m_sessionRegister.getGhostSessionInfo( sessionId, session ) )
			{
				//m_sessionRegister.removeGhostSession( sessionId );
			}
			else
			{
				session = m_sessionRegister.getSession(sessionId);
				m_sessionRegister.removeSession(sessionId);
				LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"removeSession <%s> from session register", sessionId.c_str());
			}
			// ++ly
        }
        catch(...)
        {
            // If there is an error removing the session from the register, then this session has already 
            // been ended, so just return
            return;
        }

		// ly++ for CL-20784
		// If a ghost session hold other stations duty and that station DutyAgent do not restart,
		//	ghost session will hold that station's duty, so we should call those DutyAgent to release
		//	those session's duty
		// If DutyAgent can not find this session in its duty map cache, it will not raise exception
		// ++ly
        try
        {
			// Notify the duty agent
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"[2011-11-09] Authentication::authFreeDuty");
			CORBA_CALL( m_dutyAgent,
			            authFreeDuty,
			            ( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session) ) );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"[2011-11-09] Authentication::authFreeDuty Done");
        }
        catch(TA_Base_Core::TransactiveException& ex)
        {
            // Any problem here will cause the request to fail...so need to remove the session from the register,
            // then throw an exception.
            std::string message("An error occurred while deallocating duty. Session duties may not be reallocated. Error is: ");
            message += ex.what();

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,message.c_str());
            
        }
        catch(CORBA::Exception& ex)
        {
            // Any problem here will cause the request to fail...so need to remove the session from the register,
            // then throw an exception.
            std::string message("An error occurred while deallocating duty. Session duties may not be reallocated. Error is: ");
            message += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,message.c_str());
        }
        catch(...)
        {
            // If the duty agent cannot be notified, throw an exception, and do not end the session.
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "An error occurred while deallocating duty. Session duties may not be reallocated.");
        }

		try
		{
			// yanrong: notify the rights agent to update session cache
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Authentication::endSession, CORBA_CALL sessionUpdate start");
			IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
				AuthenticationLibrary::convertCSessionToCORBASession(session);
			CORBA_CALL( m_rightsAgent,
				sessionUpdate,
				( corbaSession,true) );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Authentication::endSession, CORBA_CALL sessionUpdate done");
			
		}
		catch( TA_Base_Core::OperationModeException& )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught OperationModeException when call sessionUpdate to RightsAgent.");
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught Unknown Exception when call sessionUpdate to RightsAgent.");
		}
        
		// ly++ CL-20784
		if ( m_sessionRegister.isGhostSession( sessionId ) )
		{
			m_sessionRegister.removeSessionActiveTime( sessionId );
			m_sessionRegister.removeGhostSession( sessionId );
		}
		// ++ly

		TA_Base_Core::ISession* endedSession = TA_Base_Core::SessionAccessFactory::getInstance().createEndedSession(sessionId);

		m_sessionQueue.push(endedSession);

        // Send a message to the distributed agents to remove this session
        sendSessionNotification(session,SessionEnd);
        sendStateUpdateMessage(session, SessionEnd);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Authentication::endSession, sendStateUpdateMessage Done");
        // TD11984: If the session being ended has multiple profiles associated with it then log an
        // override removal event to make logging consistent.
        if ( 1 < session.ProfileId.size() )
        {
            //TD15069 azenitha++
            if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {
				sendOverrideAuditMessage( session.SessionId,"","","",TA_Base_Core::AuthenticationAgentAudit::OverrideEnded );
            }
            else
            {
                LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                    "[TD15069] Unexpected sending of OverrideEnded (System is in Monitor Mode)");
            }
            //TD15069 ++azenitha
        }

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// And send an event to signify this
			sendSessionStateAuditMessage(session.SessionId,"", getWorkstationName(session.WorkstationId),
				getProfileName(session.ProfileId[BASE_PROFILE]),TA_Base_Core::AuthenticationAgentAudit::SessionEnded);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "[TD15069] Unexpected sending of SessionEnded (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Authentication::endSession Done");

        FUNCTION_EXIT;
    }


    void Authentication::changePassword(const std::string & sessionId, const std::string & oldPassword, const std::string & newPassword)
    {
        FUNCTION_ENTRY("changePassword");

		TA_Base_Bus::GenericAgent::ensureControlMode();

        // First need to retrieve the session details from the session register
        TA_Base_Bus::SessionInfo session = m_sessionRegister.getSession(sessionId);

        // Then need to get the user details for the user of this session, in order to get at their current password.
        UserCache::UserDetails user = m_userCache.getUserDetails(session.OperatorId[BASE_OPERATOR]);

        // Check the password - throw an exception if it does not match
		std::string dbPasswd = TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(user.userId, false)->getPassword();
		if (0 != dbPasswd.compare(user.password) )
		{
			user.password = dbPasswd;
		}
        if (0 != dbPasswd.compare(oldPassword) )
        {
            TA_THROW(TA_Base_Core::AuthenticationSecurityException("The password does not match",
                TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
        }

        // Check the length of the password - throw an exception if it is not within the pre-defined boundaries
        int length = newPassword.length();
        if (MIN_PASSWORD_LENGTH > length || MAX_PASSWORD_LENGTH < length)
        {
            TA_THROW(TA_Base_Core::AuthenticationSecurityException("The new password is either too short or too long",
                TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD_LENGTH) );
        }

		try{
        // And call change password on the user cache with the user ID.
			m_userCache.changeUserPassword(session.OperatorId[BASE_OPERATOR],newPassword);

		}
		catch(DataException&)
		{
			TA_THROW(AuthenticationAgentException("A database error occured while attempting to change password") );
		}
		catch(DatabaseException&)
		{
			TA_THROW(AuthenticationAgentException("A database error occured while attempting to change password") );
		}
		catch(TA_Base_Core::AuthenticationSecurityException&)
		{
			TA_THROW(TA_Base_Core::AuthenticationSecurityException("The new password has been used before",
				TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
		}
		catch(...)
		{
			TA_THROW(AuthenticationAgentException("An unknown error occured while attempting to change password") );
		}
        // Call sendChangePasswordAuditMessage 
        sendChangePasswordAuditMessage( sessionId );

        FUNCTION_EXIT;
    }


    void Authentication::beginOverride(const std::string & sessionId,unsigned long userId,unsigned long profileId,
        unsigned long locationId, const std::string & password)
    {
        FUNCTION_ENTRY("beginOverride");

		TA_Base_Bus::GenericAgent::ensureControlMode();

        // Retrieve the details for the session that is being overridden
        TA_Base_Bus::SessionInfo session = m_sessionRegister.getSession(sessionId);

        // And retrieve the details for the overriding user.
        UserCache::UserDetails user = m_userCache.getUserDetails(userId);

        // Need to make three security checks - the user must be able to use the specified profile, the profile must 
        // be able to be used in the specified location, and the users password must be correct. 
        // This method will thrown an exception if the session is not valid therefore it is sufficient to simply not catch it.
        std::string profileName;
        try
        {
            profileName = checkUserSecurity(user,profileId,locationId,password);
        }
        catch (TA_Base_Core::AuthenticationSecurityException&)
        {
            //TD15069 azenitha++
            if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {
				sendOverrideAuditMessage(sessionId,user.userName,"",
					getProfileName(profileId),TA_Base_Core::AuthenticationAgentAudit::OverrideDenied);
				throw;
            }
            else
            {
                LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                    "Unexpected sending of OverrideDenied (System is in Monitor Mode)");
                throw;
            }
            //TD15069 ++azenitha
        }

        // Need to ensure there are no sessions in the register for the same user/profile/workstation combination.
        endOldDeadSessions(user,profileId,session.WorkstationId);

        // Okay, now that we get here, need to add the new profile to the existing session
        m_sessionRegister.addOverride(sessionId,user.userId,profileId);

        try
        {
			TA_Base_Bus::SessionInfo newSession = m_sessionRegister.getSession(sessionId);

			// yanrong: notify the rights agent to update session cache
			IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
				AuthenticationLibrary::convertCSessionToCORBASession(newSession);
			//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "CORBA CALL BEGIN m_RightsAgent profiles in [ %lu, %lu ]",newSession.ProfileId[0],newSession.ProfileId[newSession.ProfileId.size()]);
			CORBA_CALL( m_rightsAgent,
				sessionUpdate,
				( corbaSession,false ) );
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "CORBA CALL End m_RightsAgent");
		}
		catch( TA_Base_Core::OperationModeException& )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught OperationModeException when call sessionUpdate to RightsAgent.");
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught Unknown Exception when call sessionUpdate to RightsAgent.");
		}

		try
		{
			TA_Base_Bus::SessionInfo newSession = m_sessionRegister.getSession(sessionId);

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Sending %lu profiles to the Duty Agent", newSession.ProfileId.size() );

			// Notify the duty agent
			CORBA_CALL( m_dutyAgent,
				authReallocateDuty,
				( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session),TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(newSession) ) );
		}
		catch(...)
		{
			// As duty allocation failed, need to undo any changes, and throw an exception
			m_sessionRegister.clearOverrides(session.SessionId);

			//TD15069 azenitha++
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"[TD15069] m_sessionRegister.clearOverrides() called (%s)", sessionId.c_str());
			//TD15069 azenitha++

			TA_THROW(TA_Base_Core::AuthenticationAgentException("An error occurred while reallocating duty. Override was not initiated."));
		}

		// Send a message to the distribed agents to add the profile
		sendStateUpdateMessage(m_sessionRegister.getSession(sessionId), BeginOverride);
		sendSessionNotification(m_sessionRegister.getSession(sessionId),BeginOverride);

		//TD15069 azenitha++

		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
		{
			sendSessionStateAuditMessage(session.SessionId,user.userName, "",profileName,
				TA_Base_Core::AuthenticationAgentAudit::OverrideStarted);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
				"Unexpected sending of request Session (System is in Monitor Mode)");
		}
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// And send an audit message about the override
			sendOverrideAuditMessage(sessionId,user.userName,getUserName(session.OperatorId[BASE_OPERATOR]),
				profileName, TA_Base_Core::AuthenticationAgentAudit::OverrideStarted);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "[TD15069] Unexpected sending of OverrideStarted (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        FUNCTION_EXIT;
    }


    void Authentication::endOverride(const std::string & sessionId)
    {
        FUNCTION_ENTRY("endOverride");
		
		TA_Base_Bus::GenericAgent::ensureControlMode();

        // Get the session deatils
        TA_Base_Bus::SessionInfo session = m_sessionRegister.getSession(sessionId);

        // Ending an override is handled by the sessionRegister
        m_sessionRegister.clearOverrides(sessionId);

        try
        {
			TA_Base_Bus::SessionInfo newSession1 = m_sessionRegister.getSession(sessionId);


			// yanrong: notify the rights agent to update session cache
			IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
				AuthenticationLibrary::convertCSessionToCORBASession(newSession1);
			CORBA_CALL( m_rightsAgent,
				sessionUpdate,
				( corbaSession,false ) );

		}
		catch( TA_Base_Core::OperationModeException& )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught OperationModeException when call sessionUpdate to RightsAgent.");
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught Unknown Exception when call sessionUpdate to RightsAgent.");
		}


		try
		{

			TA_Base_Bus::SessionInfo newSession = m_sessionRegister.getSession(sessionId);
			// Notify the duty agent
			CORBA_CALL( m_dutyAgent,
				authReallocateDuty,
				( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session),TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(newSession) ) );
		}
		catch(...)
		{
			// Put the profiles back...
			TA_ASSERT( session.OperatorId.size()==session.ProfileId.size(), "Operator <-> Profile mismatch!" );
			for (unsigned int i = 1;i < session.ProfileId.size();i++)
			{
				m_sessionRegister.addOverride(session.SessionId,session.OperatorId[i],session.ProfileId[i]);
			}
			// As the deallocation of duty was not successful, throw an exception and fail the end of the override.
			TA_THROW(TA_Base_Core::AuthenticationAgentException("An error occurred while reallocating duty. Override was not ended"));
		}


		// Send a message to the distribed agents to reset the profile

		sendStateUpdateMessage(m_sessionRegister.getSession(sessionId), EndOverride);
		sendSessionNotification(m_sessionRegister.getSession(sessionId),EndOverride);

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// And send an audit message about the override
			sendOverrideAuditMessage(sessionId,"","","",TA_Base_Core::AuthenticationAgentAudit::OverrideEnded);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "[TD15069] Unexpected sending of OverrideEnded (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        FUNCTION_EXIT;
    }


    std::string Authentication::changeProfile(const std::string & sessionId,unsigned long profileId,
		unsigned long locationId, const std::string & password,bool displayOnly)
    {
        FUNCTION_ENTRY("changeProfile");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,"Authentication::changeProfile start");
		TA_Base_Bus::GenericAgent::ensureControlMode();

		//TD18707 xinsong++
		//the locationId should be same as the agent id
		ensureIsLocalLocationId( locationId );

        // Scope in some classes that are needed in this method.
        using TA_Base_Core::SessionAccessFactory;
        using TA_Base_Core::ISession;
        using TA_Base_Bus::SessionInfo;

        // Retrieve the information about the current session
        SessionInfo session = m_sessionRegister.getSession(sessionId);

        // Retrieve the details for the user.
        UserCache::UserDetails user = m_userCache.getUserDetails(session.OperatorId[BASE_OPERATOR]);

        // Check to ensure the user is not attempting to log into the profile they're already in
        TA_ASSERT(session.ProfileId.size() > 0, "The profile vector is empty. It should never have less than one entry");
        

        // Need to make three security checks - the user must be able to use the specified profile, the profile must 
        // be able to be used in the specified location, and the users password must be correct. 
        // This method will thrown an exception if the session is not valid therefore it is sufficient to simply not catch it.
        std::string profileName;
        try
        {
            profileName = checkUserSecurity(user,profileId,locationId,password);
        }
        catch (TA_Base_Core::AuthenticationSecurityException&)
        {
			//TD15069 azenitha++
			if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
			{
				sendSessionStateAuditMessage(" ",getUserName(session.OperatorId[BASE_OPERATOR]), getWorkstationName(session.WorkstationId),
					getProfileName(profileId), TA_Base_Core::AuthenticationAgentAudit::AuthenticationRequestDenied);
				throw;
			}
			else
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
					"[TD15069] Unexpected sending of AuthenticationRequestDenied (System is in Monitor Mode)");
				throw;
			}
        }

		
        SessionInfo newSession;
		bool displayOnlyChanged = false;
		if (profileId == session.ProfileId[TA_Base_Bus::Authentication::BASE_PROFILE] && locationId == session.LocationId )
		{
			// The user is actually logged into this profile already. Throw an AuthenticationSecurityException of type
			// INVALID_PROFILE
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,"[2011-12-28] SessionAccessFactory::getInstance().createSession");
			std::auto_ptr<ISession> theSession(SessionAccessFactory::getInstance().createSession(sessionId,
				session.OperatorId[BASE_OPERATOR],	
				session.WorkstationId,profileId,m_localLocationId,m_entityKey,displayOnly ));
			newSession.UserId = theSession->getOperatorKey();

			newSession.SessionId = theSession->getSessionId();
			newSession.OperatorId.push_back(theSession->getOperatorKey());
			newSession.ProfileId.push_back(theSession->getProfileKey());
			newSession.WorkstationId = theSession->getConsoleKey();
			newSession.LocationId = locationId;
			newSession.isDisplayOnly = displayOnly;
			// Add it to the session register
			m_sessionRegister.removeSession(sessionId);
			m_sessionRegister.addSession(newSession, true);
			if(session.isDisplayOnly != displayOnly)
				displayOnlyChanged = true;
			theSession->setDisplayOnly(displayOnly);

		}
		else
		{
			// Need to ensure there are no sessions in the register for the same user/profile/workstation combination.
			endOldDeadSessions(user,profileId,session.WorkstationId);

			
			// At this point, the session is valid, and any "dead" sessions have been removed. Time to create the new session.
			std::auto_ptr<ISession> theSession(SessionAccessFactory::getInstance().createSession(session.OperatorId[BASE_OPERATOR],
				session.WorkstationId,profileId,m_localLocationId,m_entityKey,displayOnly ));

			newSession.UserId = theSession->getOperatorKey();

			newSession.SessionId = theSession->getSessionId();
			newSession.OperatorId.push_back(theSession->getOperatorKey());
			newSession.ProfileId.push_back(theSession->getProfileKey());
			newSession.WorkstationId = theSession->getConsoleKey();
			newSession.LocationId = locationId;
			newSession.isDisplayOnly = displayOnly;
			// Add it to the session register
			m_sessionRegister.addSession(newSession, true);
			m_sessionQueueProcessor.writeNewSession(theSession.release());
		}
	
		try
		{
			// yanrong: notify the rights agent to update session cache
			IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
				AuthenticationLibrary::convertCSessionToCORBASession(newSession);
			CORBA_CALL( m_rightsAgent,
				sessionUpdate,
				( corbaSession,false ) );

			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,"[2011-12-28] rights agent sessionupdate done");

		}
		catch( TA_Base_Core::OperationModeException& )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught OperationModeException when call sessionUpdate to RightsAgent.");
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught Unknown Exception when call sessionUpdate to RightsAgent.");
		}


		try
		{
			// Notify the duty agent
			CORBA_CALL( m_dutyAgent,
				authAllocateDuty,
				( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(newSession) ) );
		}
		catch(...)
		{
			// Reallocation failed, so need to NOT end the last session, and throw an exception to fail this new session.
			// Therefore need to remove the session from the register
			m_sessionRegister.removeSession(newSession.SessionId);

			TA_THROW(TA_Base_Core::AuthenticationAgentException("Error allocating new duty. Profile was not changed."));
		}
        // But before finishing, end the old session
		//TD18707 xinsong++
		//if the endsession failed, do not throw exception, because it can be detected by keeplive thread 
		try
		{
			if (profileId != session.ProfileId[TA_Base_Bus::Authentication::BASE_PROFILE] || locationId != session.LocationId)
			{
				endSession(sessionId);
				LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"endSession called (%s) during change profile.", sessionId.c_str());
				//[2013-08-12] In case of change profile, duty is not available until the endsession(sessionId) is called,
				//therefore, authAllocateDuty again to allocate available duty
				try
				{
					// Notify the duty agent
					CORBA_CALL( m_dutyAgent,
						authAllocateDuty,
						( TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(newSession) ) );
				}
				catch(...)
				{
					// Reallocation failed, so need to NOT end the last session, and throw an exception to fail this new session.
					// Therefore need to remove the session from the register
					m_sessionRegister.removeSession(newSession.SessionId);

					TA_THROW(TA_Base_Core::AuthenticationAgentException("Error allocating new duty. Profile was not changed."));
				}
			}
		}
		catch(...)
		{
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
				"endSession failed during change profile, sessionID=%s", sessionId.c_str());
		}

        // Send the session events
        sendStateUpdateMessage(newSession, SessionStart);   // New session starting
        //sendStateUpdateMessage(session, SessionEnd);        // Old session ending //TD15901 //endSession already sends this one, no need to send here
        sendSessionNotification(newSession,SessionStart);
		//sendSessionNotification(session,SessionEnd); //TD15069 //TD15901 //endSession already sends this one, no need to send here
		
        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			sendSessionStateAuditMessage(newSession.SessionId,"", getWorkstationName(newSession.WorkstationId),
				profileName, TA_Base_Core::AuthenticationAgentAudit::SessionStarted);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "[TD15069] Unexpected sending of SessionStarted (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        // Now need to return the  new SessionId. 
        FUNCTION_EXIT;
        return newSession.SessionId;
    }


    void Authentication::getSessionInfo(const std::string & sessionId, const std::string & requestingSessionId, TA_Base_Bus::SessionInfo & sessionInfo )
    {
        // NOTE: The requestingSessionId parameter is not used in the current implementation. It is provided 
        // in case the security requirements of a system require checking the session that is requesting the 
        // session information (at the moment that is not a requirement). This will allow applicaitons to continue
        // to use the same API and to easiliy vary the funciton of this class.
        FUNCTION_ENTRY("getSessionInfo");

		TA_Base_Bus::GenericAgent::ensureControlMode();

        FUNCTION_EXIT;
        sessionInfo = m_sessionRegister.getSession(sessionId);         
    }


    void Authentication::getProfiles(const std::string & sessionId, std::vector<unsigned long> & vtProfileId )
    {
        FUNCTION_ENTRY("getProfiles");

		TA_Base_Bus::GenericAgent::ensureControlMode();

		vtProfileId.clear();

        // Attempt to find the session in the session register
        try
        {
            // Request the session
            TA_Base_Bus::SessionInfo session = m_sessionRegister.getSession(sessionId);

            // Copy the profiles out of the session into the new vector
            vtProfileId = session.ProfileId;
        }
        catch (TA_Base_Core::AuthenticationSecurityException&)
        {
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,
				"session <%s> hasn't profiles", sessionId.c_str());

            // The session was not in the register. Need to return an empty vector, so
            // just do nothing here.
        }
		catch(TA_Base_Core::AuthenticationAgentException&)
		{
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,
				"AuthenticationAgentException, session <%s> hasn't profiles", sessionId.c_str());
		}
		catch(...)
		{
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,
				"Unknown Exception, session <%s> hasn't profiles", sessionId.c_str());
		}

        FUNCTION_EXIT;
    }

    void Authentication::registerUpdate(const TA_Base_Bus::SessionInfo & session)
    {
        FUNCTION_ENTRY("registerUpdate");
        
        try
        {
			if(m_sessionRegister.isSessionInRegister(session.SessionId))
				m_sessionRegister.removeSession(session.SessionId);
            m_sessionRegister.addSession(session,true);

			//TD15069 azenitha++
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"session Register addSession() called (%s)", session.SessionId.c_str());
			//TD15069 azenitha++
        }
        catch(TA_Base_Core::AuthenticationAgentException&)
        {
            // This will be thrown if the session is already on the register. At this point, don't
            // want to deal with that occurance, except to ignore it.
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"session Register addSession() failed (%s)", session.SessionId.c_str());
        }

        FUNCTION_EXIT;
    }


    void Authentication::getActiveSessions(const std::string & sessionId_nouse, std::vector<TA_Base_Bus::SessionInfo> & vtSessionInfo )
    {
        FUNCTION_ENTRY("getActiveSessions");
		
		TA_Base_Bus::GenericAgent::ensureControlMode();

        // TD 11840 - this doesn't need to be access controlled.
        FUNCTION_EXIT;

        m_sessionRegister.getRegister( vtSessionInfo );
    }


    void Authentication::keepSessionAlive(time_t timestamp, const char* sessionId)
    {

		TA_Base_Bus::GenericAgent::ensureControlMode();

        time_t serverTimestamp;
        time(&serverTimestamp);

		LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"Authentication::keepSessionAlive, sessionId=%s", sessionId);
		// ly++ CL-20784
		// If someone want to active ghost session, clean it and ready to add to register session list
		if ( m_sessionRegister.isGhostSession( sessionId ) )
		{
			LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-2784] Ghost Session active, keepSessionAlive called for session:%s, remove ghost session activeTime", sessionId);
			m_sessionRegister.removeSessionActiveTime( sessionId );
			/*
			m_sessionRegister.removeGhostSession( sessionId );
			*/
			m_sessionRegister.moveGhostSessionToRegister(sessionId);
		}
        m_sessionRegister.updateSessionActiveTime(serverTimestamp, sessionId);
    }


    bool Authentication::getLastSessionActiveTime(const std::string& sessionId, time_t& activeTime, time_t agentStartUpTime)
    {
        return m_sessionRegister.getLastSessionActiveTime(sessionId, activeTime, agentStartUpTime);
    }

    void Authentication::setControl()
    {
        m_deadSessionCheckerThread.setControl();
    }


    void Authentication::setMonitor()
    {
        m_deadSessionCheckerThread.setMonitor();
    }


    void Authentication::updateSessionTime(const TA_Base_Bus::SessionInfo & session, bool isRemove)
    {
        if ( !isRemove )
        {
                       m_sessionRegister.addSession(session, true);

			//TD15069 azenitha++
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"m_sessionRegister.addSession() called (%s)", session.SessionId.c_str());
			//TD15069 azenitha++
        }
        else
        {
            m_sessionRegister.removeSession( session.SessionId );


			//TD15069 azenitha++
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"m_sessionRegister.removeSessionActiveTime() called (%s)", session.SessionId.c_str());
			//TD15069 azenitha++
        }
    }

    
    // TD14332 before agent change to control, update all sessionActiveTime to current time
    // in order for deadSessionChecker not mistakenly remove any active sessions.
    void Authentication::updateAllSessionsActiveTime()
    {
        m_sessionRegister.updateAllSessionsActiveTime();

		//TD15069 azenitha++
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"m_sessionRegister.updateAllSessionsActiveTime() called");
		//TD15069 azenitha++
    }


    void Authentication::startObjects()
    {
        //m_messageReceiver.startReceiver();

        TA_Base_Core::CorbaNameList dutyAgentName;
		TA_Base_Core::CorbaNameList rightsAgentName; //yanrong
        try
        {
			TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "RPARAM_LOCATIONKEY unset" );

			// Get the entity name based on the locationKey
			unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );

			std::string agentType = TA_Base_Core::DutyAgentEntityData::getStaticType();
			std::string rightsAgentType = TA_Base_Core::RightsAgentEntityData::getStaticType(); //yanrong

			// this will only return a single entry
			dutyAgentName = 
				TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( agentType, locationKey,true );

			// yanrong
			rightsAgentName = 
				TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( rightsAgentType, locationKey, true );

			TA_ASSERT(dutyAgentName.size() == 1, "corbaNames.size() != 1");
			TA_ASSERT(rightsAgentName.size() == 1, "corbaNames.size() != 1");
        }
        catch(...)
        {
            // Any problem here is a problem with the database
            TA_THROW(TA_Base_Core::AuthenticationAgentException
                ("Error retrieving database information for the Duty Agent."));
        }

        try
        {
            // Set the agent name - this is the name of the agent to contact.
            m_dutyAgent.setCorbaName( dutyAgentName[0] );
			m_rightsAgent.setCorbaName( rightsAgentName[0] ); // yanrong

            // TD11023: Increase the CORBA timeout to try and avoid inconsistent situations.
			// 			m_dutyAgent.setObjectTimeout( DUTY_AGENT_CALL_TIMEOUT );
			// 			m_rightsAgent.setObjectTimeout( RIGHTS_AGENT_CALL_TIMEOUT ); // yanrong
			
			// ly++ for CL-20822
			// calculate DutyAgent CorbaCall Timeout for AuthentcationAgent
			// Corba timeout set to [ RemoteDutyPeer_timeout * 2 + self_response_time ]
			// * 2 for CORBA Macro retry delay
			// need to restart AuthenticationAgent when ControlStation.AccessControlPopupTimeout changed
			std::auto_ptr<TA_Base_Core::ControlStation> cs( dynamic_cast<TA_Base_Core::ControlStation*>
				( TA_Base_Core::EntityAccessFactory::getInstance().getEntity( "ControlStation") ) );
			unsigned long ulDutyAgentTimeOut = cs->getAccessControlPopupTimeout();
			unsigned short usTimeout = ( (unsigned short)ulDutyAgentTimeOut + RESPONSE_TIME ) * 2 + RESPONSE_TIME;	// + 10 for response
			m_dutyAgent.setObjectTimeout( usTimeout );
			m_rightsAgent.setObjectTimeout( usTimeout );
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                "[CL-20822] Set DutyAgent corba call timeout to %u seconds", usTimeout );
			// ++ly

			//resolve before any dutyagent call function
			//m_dutyAgent
        }
        catch(...)
        {
            // Any problem here is a problem contacting the agent
            TA_THROW(TA_Base_Core::AuthenticationAgentException("Error connecting with the Duty Agent"));
        }

		// LY++ for CL-20784, start DeadSessionCheckerThread move behind as it may use m_dutyAgent and m_rightsAgent
		unsigned long frequency = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSION_CHECKING_FREQUENCY).c_str(), NULL, 10);
        unsigned long timeout = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSION_TIMEOUT_PERIOD).c_str(), NULL, 10);
        m_deadSessionCheckerThread.setSessionCheckerFrequency(frequency);
        m_deadSessionCheckerThread.setTimeOutPeriod(timeout);
		
        try
        {
            m_deadSessionCheckerThread.start();
        }
        catch(...)
        {
            TA_THROW(TA_Base_Core::AuthenticationAgentException
                ("Error running the dead session checker thread."));
        }

    }


    std::string Authentication::checkUserSecurity(UserCache::UserDetails& user,unsigned long profileId, 
        unsigned long locationId,const std::string& password)
    {
        FUNCTION_ENTRY("checkUserSecurity");
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, 
			"Authentication::checkUserSecurity start. ");
        // Bring in the exception to be used
        using TA_Base_Core::AuthenticationSecurityException;

        // There are three sections to security checking:
        // 1) Check that the password provided matches the users password
        // 2) Check that the profile requested is in the list of valid profiles for the user
        // 3) Check that the location to be logged into is in the list of valid locations for the profile
		std::string dbPasswd = TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(user.userId, false)->getPassword();
		if(!TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(user.userId,false)->isSystemOperator())
		{
			if (0 != dbPasswd.compare(user.password) )
			{
				user.password = dbPasswd;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Update password stored in UserCache");
			}
			if( 0 != dbPasswd.compare(password) )
			{
            TA_THROW(AuthenticationSecurityException("The password does not match",
                AuthenticationSecurityException::INVALID_PASSWORD) );
			}

		}

        // Check the profile/location. It is a map on profile, so search on profile first.
        std::map<unsigned long,UserCache::ProfileLocationDetails >::iterator iter (user.profileLocationMap.find(profileId));

        if (iter == user.profileLocationMap.end() )
        {
            // The profile is not in the map, so throw an excetpion.
            TA_THROW(AuthenticationSecurityException("The profile is not valid for the user",
                AuthenticationSecurityException::PROFILE_NOT_PERMITTED) );
        }
        else
        {
            // The profile is valid, so only need to check the location. 

            // If the location vector is empty, then the profile does not have associated locations, so don't need to do the
            // comparrison.
            if (iter->second.associatedLocations.size() == 0)
            {
                return iter->second.profileName;
            }
            else
            {
                // Location is a vector, so do that by looping through the vector checking the log-in location against all valid locations.
                for (unsigned int i = 0; i < iter->second.associatedLocations.size();i++)
                {
                    if (locationId == iter->second.associatedLocations[i].locationId)
                    {
                        // The location is valid. No more checks needed, so just return.
                        return iter->second.profileName;
                    }
                }

                // If we run off the end of the loop, the location was not found, so need to throw an exception
                TA_THROW(AuthenticationSecurityException("The location is not valid for the profile",
                    AuthenticationSecurityException::LOCATION_NOT_PERMITTED) );
            }
        }

        FUNCTION_EXIT;       
	    
    }

    void Authentication::endOldDeadSessions(UserCache::UserDetails& user,unsigned long profileId,
        unsigned long workstationId)
    {
        FUNCTION_ENTRY("endOldDeadSessions");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Authentication::endOldDeadSessions start.");
        std::vector<TA_Base_Bus::SessionInfo> sessions;
		m_sessionRegister.getRegister(sessions);
		// ly++ CL-20784
		m_sessionRegister.appendGhostSession(sessions);

        // Now loop through each session to determine if one already exists using the user/profile/workstation
        // combination of the new session
        for (unsigned int i = 0;i < sessions.size();i++)
        {
            // As we are referring to the first profile here, need to assert that the vector is greater
            // than 1 in size. It should be - it is an "impossible" situation for it to be empty
            TA_ASSERT(sessions[i].ProfileId.size() > 0, "The profile vector is emtpy.");
            if (user.userId == sessions[i].OperatorId[BASE_OPERATOR]
                && profileId == sessions[i].ProfileId[BASE_PROFILE] 
                && workstationId == sessions[i].WorkstationId)
            {
                // This session is the "same" as the new one being created, so need to end it
				LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
							"endSession() called (%s) in endOldDeadSessions", sessions[i].SessionId.c_str());
                endSession(sessions[i].SessionId);
            }
        }

        FUNCTION_EXIT;
    }

    void Authentication::sendSessionNotification(const TA_Base_Bus::SessionInfo & session, EUpdateType updateType)
    {
        FUNCTION_ENTRY("sendSessionNotification");

        // Create a TA_Base_Bus::SessionInfoCorbaDef object that will carry the information across the wire
        TA_Base_Bus::SessionInfoCorbaDef sessionInfo;

        // Need to use session.ProfileId[BASE_PROFILE], so ensure it exists
        TA_ASSERT(session.ProfileId.size() > 0, "The profile vector was empty.");
        
        sessionInfo.SessionId = CORBA::string_dup(session.SessionId.c_str());
        sessionInfo.WorkstationId = session.WorkstationId;
        sessionInfo.ProfileId = session.ProfileId[BASE_PROFILE];
        sessionInfo.UserId = session.OperatorId[BASE_OPERATOR];
        sessionInfo.LocationId = session.LocationId;

        sessionInfo.additionalOperators.length(0);
        sessionInfo.additionalProfiles.length(0);

        // Initialise i up here, because the compiler will throw an error if we try to initialise it in the loop headers.
        unsigned int i;

		std::string strUpdateType = "unknown";

        switch (updateType)
        {
        case SessionStart:
			strUpdateType = "Session Start"; 
            sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionStart;
            sessionInfo.additionalProfiles.length( session.ProfileId.size()-1 );
            for ( i=1; i<session.ProfileId.size(); i++ )
            {
                sessionInfo.additionalProfiles[i-1] = session.ProfileId[i];
            }
            sessionInfo.additionalOperators.length( session.OperatorId.size()-1 );
            for ( i=1; i<session.OperatorId.size(); i++ )
            {
                sessionInfo.additionalOperators[i-1] = session.OperatorId[i];
            }
            break;

        case SessionEnd:
			strUpdateType = "Session End"; 
            sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionEnd;            
            break;

        case ProfileUpdate:
			strUpdateType = "Profile Update"; 
			sessionInfo.updateType = TA_Base_Bus::TA_Security::ProfileUpdate;
            sessionInfo.additionalProfiles.length( session.ProfileId.size()-1 );
            for ( i=1; i<session.ProfileId.size(); i++ )
            {
                sessionInfo.additionalProfiles[i-1] = session.ProfileId[i];
            }
            sessionInfo.additionalOperators.length( session.OperatorId.size()-1 );
            for ( i=1; i<session.OperatorId.size(); i++ )
            {
                sessionInfo.additionalOperators[i-1] = session.OperatorId[i];
            }
            break;

		case BeginOverride:
			strUpdateType = "Begin Override"; 
			sessionInfo.updateType = TA_Base_Bus::TA_Security::BeginOverride;
			sessionInfo.additionalProfiles.length( session.ProfileId.size()-1 );
			for ( i=1; i<session.ProfileId.size(); i++ )
			{
				sessionInfo.additionalProfiles[i-1] = session.ProfileId[i];
			}
			sessionInfo.additionalOperators.length( session.OperatorId.size()-1 );
			for ( i=1; i<session.OperatorId.size(); i++ )
			{
				sessionInfo.additionalOperators[i-1] = session.OperatorId[i];
			}
			break;
		case EndOverride:
			strUpdateType = "End Override"; 
			sessionInfo.updateType = TA_Base_Bus::TA_Security::EndOverride;            
			break;
		default:
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "Unknown AuthenticationAgentDistributedUpdate Message Type, TypeId=%d", updateType);
			break;
        }

        // Create an any into which the session object can be placed
        CORBA::Any stateUpdate;

        // Inject the sessionInfo object into the any.
        stateUpdate <<= sessionInfo;
        
        TA_Base_Core::FilterData filterData;

        // agentName info 
        TA_Base_Core::NameValuePair nvp(AGENT_ID_STRING, // <== Don't use "AgentName" as the name, as it is already used
	        AGENT_ID); // The actual entity name of the agent

        filterData.push_back(&nvp);

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
                "[TD15069] sending of AuthenticationAgentDistributedUpdate, messageType=%s", strUpdateType.c_str());

			// Send the message
			m_bcastStateSender->sendBroadcastCommsMessage(
						TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedUpdate,
					stateUpdate,
					&filterData);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "[TD15069] Unexpected sending of AuthenticationAgentDistributedUpdate (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        FUNCTION_EXIT;
    }

    void Authentication::sendStateUpdateMessage(const TA_Base_Bus::SessionInfo & session, EUpdateType updateType)
    {
        FUNCTION_ENTRY("sendStateUpdateMessage");

        // Create an any into which the session object can be placed
        CORBA::Any stateUpdate;

        // Create a TA_Base_Bus::SessionInfoCorbaDef object that will carry the information across the wire
        TA_Base_Bus::SessionInfoCorbaDef sessionInfo;
        
        sessionInfo.SessionId = session.SessionId.c_str();
        sessionInfo.WorkstationId = session.WorkstationId;
        sessionInfo.UserId = session.OperatorId[BASE_OPERATOR];
        sessionInfo.LocationId = session.LocationId;
        sessionInfo.ProfileId = session.ProfileId[BASE_PROFILE];

        if (updateType == TA_Base_Bus::TA_Security::SessionStart)
        {
            sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionStart;

            unsigned long i;

            sessionInfo.additionalProfiles.length( session.ProfileId.size()-1 );
            for ( i=1; i<session.ProfileId.size(); i++ )
            {
                sessionInfo.additionalProfiles[i-1] = session.ProfileId[i];
            }
            sessionInfo.additionalOperators.length( session.OperatorId.size()-1 );
            for ( i=1; i<session.OperatorId.size(); i++ )
            {
                sessionInfo.additionalOperators[i-1] = session.OperatorId[i];
            }
        }
		else if (updateType == TA_Base_Bus::TA_Security::SessionEnd)
		{
			sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionEnd;
		}

		else if (updateType == TA_Base_Bus::TA_Security::BeginOverride)
		{
			sessionInfo.updateType = TA_Base_Bus::TA_Security::BeginOverride;

			unsigned long i;

			sessionInfo.additionalProfiles.length( session.ProfileId.size()-1 );
			for ( i=1; i<session.ProfileId.size(); i++ )
			{
				sessionInfo.additionalProfiles[i-1] = session.ProfileId[i];
			}
			sessionInfo.additionalOperators.length( session.OperatorId.size()-1 );
			for ( i=1; i<session.OperatorId.size(); i++ )
			{
				sessionInfo.additionalOperators[i-1] = session.OperatorId[i];
			}
		}
		else if (updateType == TA_Base_Bus::TA_Security::EndOverride)
		{
			sessionInfo.updateType = TA_Base_Bus::TA_Security::EndOverride;

		}
        else
        {
            sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionEnd;
        }
        // Place the sequence into the state update
        stateUpdate <<= sessionInfo;

        
        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			// Send the message
			m_localStateSender->sendStateUpdateMessage(
						TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentSessionTimeStateUpdate,
						m_entityKey,
						RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(),
						stateUpdate);
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "Unexpected sending of AuthenticationAgentSessionTimeStateUpdate (System is in Monitor Mode)");
        }
        //TD15069 ++azenitha

        FUNCTION_EXIT;
    }

    void Authentication::sendSessionStateAuditMessage(const std::string& sessionId,const std::string& operatorName, 
        const std::string& consoleName,const std::string& profileName, const TA_Base_Core::MessageType& messageType)
    {
	    // Create the parameter vector
        TA_Base_Core::DescriptionParameters dp;

        TA_Base_Core::NameValuePair consoleNamePair (AUDIT_MSG_CONSOLE_NAME, consoleName );
	    TA_Base_Core::NameValuePair profileNamePair (AUDIT_MSG_PROFILE_ID  , profileName );
        TA_Base_Core::NameValuePair operatorNamePair(AUDIT_MSG_OPERATOR_ID , operatorName);

		dp.push_back(&consoleNamePair);
		dp.push_back(&profileNamePair);

		if( operatorName.size() > 0 )
		{		
			dp.push_back(&operatorNamePair);
		}

        m_auditSender->sendAuditMessage(messageType,m_entityKey,dp,"N/A",sessionId,"","","");
    }

    void Authentication::sendChangePasswordAuditMessage(const std::string& sessionId)
    {
        // Create the parameter vector
        TA_Base_Core::DescriptionParameters dp;

        //TD15069 azenitha++
        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
			m_auditSender->sendAuditMessage(
				TA_Base_Core::AuthenticationAgentAudit::PasswordChanged,m_entityKey,dp,"N/A",sessionId,"","","");
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "Unexpected sending of PasswordChanged (System is in Monitor Mode)");
			TA_THROW(TA_Base_Core::AuthenticationAgentException
				("Unexpected sending of PasswordChanged (System is in Monitor Mode)"));;
        }
        //TD15069 ++azenitha
    }
    

    void Authentication::sendOverrideAuditMessage(const std::string& sessionId,const std::string& supervisorName,
        const std::string& operatorName, const std::string& profileName, const TA_Base_Core::MessageType& messageType)
    {
        // Create the parameter vector
        TA_Base_Core::DescriptionParameters dp;
		TA_Base_Core::NameValuePair supervisorNamePair(AUDIT_MSG_SUPERVISOR_ID, supervisorName);
		TA_Base_Core::NameValuePair profileNamePair   (AUDIT_MSG_PROFILE_ID   , profileName   );
		TA_Base_Core::NameValuePair operatorNamePair  (AUDIT_MSG_OPERATOR_ID  , operatorName  );

		if( supervisorName.size() > 0 )
		{
			dp.push_back(&supervisorNamePair);
		}

		if( profileName.size() > 0 )
		{
			dp.push_back(&profileNamePair);
		}

		if( operatorName.size() > 0 )
		{
			dp.push_back(&operatorNamePair);
		}

        m_auditSender->sendAuditMessage(messageType,m_entityKey,dp,"N/A",sessionId,"","","");
    }

    std::string Authentication::getWorkstationName(unsigned long workstationId)
    {
        // wrap this in a try-catch block in the event of a database error
        try
        {
            std::auto_ptr<TA_Base_Core::IConsole> console( TA_Base_Core::ConsoleAccessFactory::getInstance().getConsole(workstationId,false) );
            return console->getName();
        }
        catch (...)
        {
            // Catch ... because any error will be unrecoverable.
            return "Unknown";
        }
    }

    std::string Authentication::getProfileName(unsigned long profileId)
    {
        // wrap this in a try-catch block in the event of a database error
        try
        {
            std::auto_ptr<TA_Base_Core::IProfile> profile( TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(profileId,false) );
            return profile->getName();
        }
        catch (...)
        {
            // Catch ... because any error will be unrecoverable.
            return "Unknown";
        }        
    }

    std::string Authentication::getUserName(unsigned long userId)
    {
        // wrap this in a try-catch block in the event of a database error
        try
        {
            std::auto_ptr<TA_Base_Core::IOperator> user( TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(userId,false) );
            return user->getName();
        }
        catch (...)
        {
            // Catch ... because any error will be unrecoverable.
            return "Unknown";
        }        
    }

	//TD18707 xinsong++
	//the locationId should be same as the agent id
	void Authentication::ensureIsLocalLocationId( unsigned long locationId )
	{
		if ( locationId == 0 )
		{
			return;
		}

		std::ostringstream ostr;
		ostr << "requset session: error for location id, the local location id=" << m_localLocationId <<", the request location id=" << locationId;
		TA_ASSERT( m_localLocationId == locationId, ostr.str().c_str());
	}

	void Authentication::beginDisplayonly(bool display)
	{
		isDisplayOnlyMode = display;
	}


	void Authentication::sendChangeProfileCommMessage(const TA_Base_Bus::SessionInfo & session,EUpdateType updateType)
	{
		FUNCTION_ENTRY("sendChangeProfileCommMessage");

		// Create a TA_Base_Bus::SessionInfoCorbaDef object that will carry the information across the wire
		TA_Base_Bus::SessionInfoCorbaDef sessionInfo;

		// Need to use session.ProfileId[BASE_PROFILE], so ensure it exists
		TA_ASSERT(session.ProfileId.size() > 0, "The profile vector was empty.");

		sessionInfo.SessionId = CORBA::string_dup(session.SessionId.c_str());
		sessionInfo.WorkstationId = session.WorkstationId;
		sessionInfo.ProfileId = session.ProfileId[BASE_PROFILE];
		sessionInfo.UserId = session.OperatorId[BASE_OPERATOR];
		sessionInfo.LocationId = session.LocationId;
		sessionInfo.isDisplayOnly = session.isDisplayOnly;

		sessionInfo.additionalOperators.length(0);
		sessionInfo.additionalProfiles.length(0);

		// Initialise i up here, because the compiler will throw an error if we try to initialise it in the loop headers.
		unsigned int i;

		std::string strUpdateType = "unknown";

		switch (updateType)
		{
		case SessionStart:
			strUpdateType = "Session Start"; 
			sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionStart;
			sessionInfo.additionalProfiles.length( session.ProfileId.size()-1 );
			for ( i=1; i<session.ProfileId.size(); i++ )
			{
				sessionInfo.additionalProfiles[i-1] = session.ProfileId[i];
			}
			sessionInfo.additionalOperators.length( session.OperatorId.size()-1 );
			for ( i=1; i<session.OperatorId.size(); i++ )
			{
				sessionInfo.additionalOperators[i-1] = session.OperatorId[i];
			}
			break;

		case SessionEnd:
			strUpdateType = "Session End"; 
			sessionInfo.updateType = TA_Base_Bus::TA_Security::SessionEnd;            
			break;

		case BeginOverride:
			strUpdateType = "Begin Override"; 
			sessionInfo.updateType = TA_Base_Bus::TA_Security::BeginOverride;
			sessionInfo.additionalProfiles.length( session.ProfileId.size()-1 );
			for ( i=1; i<session.ProfileId.size(); i++ )
			{
				sessionInfo.additionalProfiles[i-1] = session.ProfileId[i];
			}
			sessionInfo.additionalOperators.length( session.OperatorId.size()-1 );
			for ( i=1; i<session.OperatorId.size(); i++ )
			{
				sessionInfo.additionalOperators[i-1] = session.OperatorId[i];
			}
			break;
		case EndOverride:
			strUpdateType = "End Override"; 
			sessionInfo.updateType = TA_Base_Bus::TA_Security::EndOverride;            
			break;
		default:
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
				"Unknown AuthenticationAgentDistributedUpdate Message Type, TypeId=%d", updateType);
			break;
		}

		// Create an any into which the session object can be placed
		CORBA::Any stateUpdate;

		// Inject the sessionInfo object into the any.
		stateUpdate <<= sessionInfo;

		TA_Base_Core::FilterData filterData;

		// agentName info 
		TA_Base_Core::NameValuePair nvp(AGENT_ID_STRING, // <== Don't use "AgentName" as the name, as it is already used
			AGENT_ID); // The actual entity name of the agent

		filterData.push_back(&nvp);

		//TD15069 azenitha++
		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"[TD15069] sending of AuthenticationAgentDistributedUpdate, messageType=%s", strUpdateType.c_str());
			std::string agentType = TA_Base_Core::RightsAgentEntityData::getStaticType();
			unsigned long rightsAgentEntityKey = ULONG_MAX;
			try
			{

				rightsAgentEntityKey = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(agentType,sessionInfo.LocationId)[0]->getKey();

			}
			catch( const TA_Base_Core::TransactiveException& ex )
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", ex.what() );

				FUNCTION_EXIT;
				TA_THROW( TA_Base_Core::RightsException(ex.what()) );
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Unhandled" );

				FUNCTION_EXIT;
				TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
			}
			std::ostringstream msg;
			msg << "Failed to find agent of type " << agentType << " at location " << sessionInfo.LocationId;
			if( rightsAgentEntityKey == ULONG_MAX)
			{
				FUNCTION_EXIT;
				TA_THROW( TA_Base_Core::RightsException(msg.str()) );
			}
			// Send the message
			m_bcastStateSender->sendCommsMessage(
				TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedUpdate,
				sessionInfo.WorkstationId,
				stateUpdate,
				rightsAgentEntityKey,
				sessionInfo.LocationId,
				&filterData);


		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
				"[TD15069] Unexpected sending of AuthenticationAgentDistributedUpdate (System is in Monitor Mode)");
		}
		//TD15069 ++azenitha

		FUNCTION_EXIT;
	}
}
