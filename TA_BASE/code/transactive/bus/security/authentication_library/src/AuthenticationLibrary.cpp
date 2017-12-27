/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/authentication_library/src/AuthenticationLibrary.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// AuthenticationLibrary.cpp: implementation of the AuthenticationLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined(_MSC_VER)

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/AuthenPeerTaskManager.h"

#include "bus/security/authentication_library/src/KeepSessionAliveThreadManager.h"

#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/corba/src/CorbaUtil.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/CodeConverter.h"

#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/IOperatorPassword.h"
#include "core/data_access_interface/src/OperatorPasswordAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

//CL-21875
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/OperatorPasswordAccessFactory.h"
#include "core/data_access_interface/src/IOperatorPassword.h"

#include <openssl/sha.h>

using TA_Base_Core::IDatabase;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using TA_Base_Core::DebugUtil;   

#define CATCH_AUTHENTICATION_EXCEPTION() \
	catch(TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& ex) \
    { \
	    LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", ex.reason.in()); \
	    TA_THROW( TA_Base_Core::AuthenticationAgentException(ex.reason.in()) ); \
    } \
	catch(TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& ex) \
    { \
	    LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationSecurityException", ex.description.in()); \
	    throwSecurityException( ex.description.in(), ex.reason ); \
    } \
	catch(CORBA::Exception& ex) \
    { \
	    LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); \
	    TA_THROW( TA_Base_Core::AuthenticationAgentException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()) ); \
    } \
	catch(TA_Base_Core::ObjectResolutionException& ex) \
    { \
	    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what()); \
	    throw; \
    } \
	catch(...) \
    { \
	    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown"); \
	    TA_THROW( TA_Base_Core::AuthenticationAgentException("Unknown Exception thrown while attempting to set up session") ); \
    }

namespace TA_Base_Bus
{

	AuthenticationLibrary::AuthenticationLibrary(bool isMgr /*=false*/)
		:m_pAuthenPeerTaskManager(NULL)
	{
		FUNCTION_ENTRY("AuthenticationLibrary");
		TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "Location Key not set" );
		//in order to create the KeepSessionAliveThreadManager as far as early
		int locationId = KeepSessionAliveThreadManager::getInstance().getLocalLocationId();
		FUNCTION_EXIT;
	}


	AuthenticationLibrary::~AuthenticationLibrary()
	{
		FUNCTION_ENTRY("~AuthenticationLibrary");
		if ( NULL != m_pAuthenPeerTaskManager )
		{
			delete m_pAuthenPeerTaskManager;
			m_pAuthenPeerTaskManager = NULL;
		}
		KeepSessionAliveThreadManager::releaseInstance();
		FUNCTION_EXIT;
	}


	std::string AuthenticationLibrary::requestSession(unsigned long userId, 
		unsigned long profileId, 
		unsigned long locationId, 
		unsigned long workstationId, 
		const std::string & password,
		bool displayOnly)
	{
		FUNCTION_ENTRY("requestSession");

		CORBA::String_var sessionId;
		std::string newSessionId = "";
		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		// As the CORBA interface throws CORBA exceptions, it is wrappered in a try/catch block so the
		// exceptions can be modified to be C++ exceptions.

		try
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[TD15901] Requesting session for user: %lu, profile: %lu, location:%lu, workstation:%lu",userId, profileId, locationId, workstationId);
			locationId = getActualLocationKey(locationId);
			// TD12427: Before requesting a session make sure an equivalent one does not already exist. Note
			// that only operator ID, profile ID, and workstation ID are checked as that is all the
			// authentication agent checks when it looks for dead sessions.

			// Create the session info object containing the information to search for.
			SessionInfo session;
			session.SessionId = "";
			session.UserId = userId;
			session.OperatorId.push_back( userId );
			session.ProfileId.push_back( profileId );
			session.LocationId = locationId; //no need for find findEquivalentSessionNotUsingSessionId
			session.WorkstationId = workstationId;
			session.isDisplayOnly=displayOnly;
			// Check with the keep alive manager to see if it is keeping an equivalent session alive.
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[2011-11-08] indEquivalentSessionNotUsingSessionId");
			std::string existingSession = findEquivalentSessionNotUsingSessionId( session );
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[2011-11-08] indEquivalentSessionNotUsingSessionId Done");
			if ( !existingSession.empty() )
			{
				// An equivalent session was found so don't request another one. Instead return the ID for the
				// existing session.

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "when request session at location %d, using exist session<%s>, not requesting a new one.",
					locationId, existingSession.c_str() );

				// ensure session is added to local cache
				session.SessionId = existingSession;
				session.LocationId = locationId;
				EnsureSpecificCondition( session.SessionId.size() != 0 , "sessionId should not be empty" );

				KeepSessionAliveThreadManager::getInstance().startKeepingSessionAlive( session );
				std::auto_ptr<TA_Base_Core::IOperatorPassword> theOperatorPassword(TA_Base_Core::OperatorPasswordAccessFactory::getInstance().getOperatorPassword(userId,false,true));
				theOperatorPassword->applyChanges();
				return existingSession;
				/*[2011-11-16]
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "found old session at location %d, ending exist session<%s>.",
				locationId, existingSession.c_str() );
				endSession(existingSession);*/


			}

			if ( !KeepSessionAliveThreadManager::getInstance().isLocalLocationId(locationId) )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Requesting session for different location, userid=%lu, request loction id=%d",
					userId, locationId);
			}
			// An equivalent session was not found, so new one is required.

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[2011-11-08] indEquivalentSessionNotUsingSessionId");
			std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(userId, false));
			std::string origin_Password = iOperator->getName();
			std::string encrypted_Password;
			origin_Password +="/";
			if(password.length() == 0)
			origin_Password += "agentOperator";
			else
			origin_Password += password;
			int rtnCode= GetHash(origin_Password,encrypted_Password);

			if(rtnCode != 0 )

			{

				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "failed to get hash!");
				TA_THROW(TA_Base_Core::AuthenticationSecurityException("Failed to encrypt password",
					TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[2011-11-08] AuthenticationLibrary requestsession");
			AuthenticationAgentNamedObj & authAgent = 
				KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(locationId);
			CORBA_CALL_RETURN( sessionId,
				authAgent,
				requestSession,
				( userId,
				profileId,
				locationId,
				workstationId,
				encrypted_Password.c_str(),
				displayOnly ) );
			/*[2011-11-04]
			CORBA_CALL(	authAgent,
			beginDisplayonly,
			(displayOnly)
			);*/
			
			std::string new_SessionId(sessionId.in());
			newSessionId = new_SessionId;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15901] New session '%s' requested.", new_SessionId.c_str());
				//sessionId.in() );

			// Add the new information to the summary.
			//session.SessionId = sessionId.in();
			session.SessionId = new_SessionId.c_str(); //the session id will be accessed in heartbeat threads, string COW semantics
			session.LocationId = locationId;
			//xinsong++ add
			session.WorkstationId = workstationId;
			session.OperatorId.push_back(userId);
			session.ProfileId.push_back(profileId);
			EnsureSpecificCondition( session.SessionId.size() != 0 , "sessionId should not be empty" );

			// Make sure the session is kept alive.
			KeepSessionAliveThreadManager::getInstance().startKeepingSessionAlive( session );
		}
		CATCH_AUTHENTICATION_EXCEPTION();
		FUNCTION_EXIT;

		//return sessionId.in();
		return newSessionId;
	}


	void AuthenticationLibrary::changePassword(const std::string & sessionId, const std::string & oldPassword, 
		const std::string & newPassword)
	{
		FUNCTION_ENTRY("changePassword");
		EnsureSpecificCondition( sessionId.size() != 0 , "sessionId should not be empty" );


		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		// As the CORBA interface throws CORBA exceptions, it is wrappered in a try/catch block so the
		// exceptions can be modified to be C++ exceptions.
		try
		{
			TA_Base_Bus::SessionInfo session = getSessionInfo(sessionId,sessionId);
			std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(session.OperatorId[0], false));

			std::string origin_Password = iOperator->getName();
			std::string encrypted_Password_old;
			origin_Password +="/";
			origin_Password += oldPassword;
			int rtnCode= GetHash(origin_Password,encrypted_Password_old);

			if(rtnCode != 0 )
			{

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"failed to get hash!");
				TA_THROW(TA_Base_Core::AuthenticationSecurityException("Failed to encrypt password",
					TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
			}


			origin_Password = iOperator->getName();
			std::string encrypted_Password_new;
			origin_Password +="/";
			origin_Password += newPassword;
			rtnCode= GetHash(origin_Password,encrypted_Password_new);
			if(rtnCode != 0 )
			{

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"failed to get hash!");
				TA_THROW(TA_Base_Core::AuthenticationSecurityException("Failed to encrypt password",
					TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
			}

			AuthenticationAgentNamedObj & authAgent = 
				KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);

			CORBA_CALL( authAgent,
				changePassword,
				( sessionId.c_str(), encrypted_Password_old.c_str(), encrypted_Password_new.c_str() ) );
		}
		CATCH_AUTHENTICATION_EXCEPTION();

		FUNCTION_EXIT;
	}


	void AuthenticationLibrary::beginOverride(const std::string & sessionId, 
		unsigned long userId, 
		unsigned long profileId, 
		unsigned long locationId, 
		const std::string & password)
	{
		FUNCTION_ENTRY("beginOverride");
		EnsureSpecificCondition( sessionId.size() != 0 , "sessionId should not be empty" );

		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		// As the CORBA interface throws CORBA exceptions, it is wrappered in a try/catch block so the
		// exceptions can be modified to be C++ exceptions.
		try
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15901] Begin override '%s' requested.",
				sessionId.c_str() );
			locationId = getActualLocationKey(locationId);
			if ( !KeepSessionAliveThreadManager::getInstance().isLocalLocationId(locationId) )
			{
				unsigned localLocationId = KeepSessionAliveThreadManager::getInstance().getLocalLocationId();
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "begin override session, different from local location: userid=%lu, local location Id=%d, request location id=%d, sessionId=%s",
					userId, localLocationId, locationId, sessionId.c_str());
			}

			std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(userId, false));
			std::string origin_Password = iOperator->getName();
			std::string encrypted_Password;
			origin_Password +="/";
			origin_Password += password;
			int rtnCode= GetHash(origin_Password,encrypted_Password);

			if(rtnCode != 0 )
			{

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"failed to get hash!");
				TA_THROW(TA_Base_Core::AuthenticationSecurityException("Failed to encrypt password",
					TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
			}


			// Call the method
			AuthenticationAgentNamedObj & authAgent = 
				KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);

			CORBA_CALL( authAgent,
				beginOverride,
				( sessionId.c_str(), userId, profileId, locationId, encrypted_Password.c_str() ) );
			updateOverrideInfoToDb(sessionId,userId,profileId);
		}
		CATCH_AUTHENTICATION_EXCEPTION();


		FUNCTION_EXIT;
	}


	void AuthenticationLibrary::endOverride(const std::string & sessionId)
	{
		FUNCTION_ENTRY("endOverride");
		EnsureSpecificCondition( sessionId.size() != 0 , "sessionId should not be empty" );

		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		// As the CORBA interface throws CORBA exceptions, it is wrappered in a try/catch block so the
		// exceptions can be modified to be C++ exceptions.
		try
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15901] End override '%s' requested.",
				sessionId.c_str() );

			// Call the method
			AuthenticationAgentNamedObj & authAgent = 
				KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);			

			CORBA_CALL( authAgent,
				endOverride,
				( sessionId.c_str() ) );
		}
		CATCH_AUTHENTICATION_EXCEPTION();

		FUNCTION_EXIT;
	}


	std::string AuthenticationLibrary::changeProfile(const std::string & sessionId, 
		unsigned long profileId, 
		unsigned long locationId, 
		const std::string & password,
		bool displayOnly)
	{
		FUNCTION_ENTRY("changeProfile");
		EnsureSpecificCondition( sessionId.size() != 0 , "sessionId should not be empty" );

		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		try
		{
			locationId = getActualLocationKey(locationId);
			// get the details of the current session as they could be lost before the changeProfile call returns
			SessionInfo oldSession;
			std::string newSessionIdStr("");
			if (! KeepSessionAliveThreadManager::getInstance().canFindSessionWithId( sessionId, oldSession ) )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "could not find sessionID <%s> when changing profile",
					sessionId.c_str() );
				TA_THROW( TA_Base_Core::AuthenticationAgentException("session lost when changing profile") )
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Change profile '%s' requested.",
				sessionId.c_str() );

			//TD18707 xinsong++
			//if it is different between old location and new location
 			if ( ! KeepSessionAliveThreadManager::getInstance().isSameLocationId(sessionId, locationId) )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "change Profile, different location, location id=%d, sessionId=%s",
					locationId, sessionId.c_str());
				//first create new session, then end old session 
				try
				{
					newSessionIdStr = requestSession(oldSession.OperatorId[0], profileId, locationId, oldSession.WorkstationId, password,displayOnly);
				}
				catch(...)
				{
					throw;
				}
				//do not throw exception when ending session
				try
				{
					endSession( sessionId );
				}
				catch(...)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Change profile '%s' requested, ending session failed",
						sessionId.c_str() );
				}
			}
			else
			{
				//encrypt password
				std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(oldSession.OperatorId[0], false));
				std::string origin_Password = iOperator->getName();
				std::string encrypted_Password;
				origin_Password +="/";
				origin_Password += password;
				int rtnCode= GetHash(origin_Password,encrypted_Password);

				if(rtnCode != 0 )
			 {

				 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"failed to get hash!");
				 TA_THROW(TA_Base_Core::AuthenticationSecurityException("Failed to encrypt password",
					 TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
			 }

				CORBA::String_var sessionIdVar;
				AuthenticationAgentNamedObj & authAgent = 
					KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);
				// Call the method
				CORBA_CALL_RETURN( sessionIdVar,
					authAgent,
					changeProfile,
					( sessionId.c_str(),
					profileId,
					locationId,
					encrypted_Password.c_str(),
					displayOnly) );
				/*
				CORBA_CALL(	authAgent,
				beginDisplayonly,
				(displayOnly)
				);*/

				newSessionIdStr = sessionIdVar.in();
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"[TD15901] Change profile, new session '%s' requested.",
				newSessionIdStr.c_str() );

			SessionInfo newSession;
			// Create a summary of all known information for the new session. The remaining information
			// will be extracted from the old session before it is removed from the keep alive thread.
			newSession.UserId = oldSession.OperatorId[0];
			newSession.SessionId = newSessionIdStr;
			newSession.ProfileId.push_back( profileId );
			//xxs add
			newSession.OperatorId.push_back(oldSession.OperatorId[0]);
			newSession.WorkstationId = oldSession.WorkstationId;
			newSession.LocationId = locationId;
			newSession.isDisplayOnly = displayOnly;
			// Make sure the new session is kept alive
			// the old session will be deleted by KeepSessionAliveThread
			KeepSessionAliveThreadManager::getInstance().stopKeepingSessionAlive(sessionId);
			KeepSessionAliveThreadManager::getInstance().startKeepingSessionAlive( newSession );
			FUNCTION_EXIT;
			return newSessionIdStr;
		}
		CATCH_AUTHENTICATION_EXCEPTION();

		FUNCTION_EXIT;
		return "";
	}


	void AuthenticationLibrary::getProfiles(const std::string & sessionId, std::vector<unsigned long> & profileVector)
	{
		FUNCTION_ENTRY("getProfiles");
		EnsureSpecificCondition( sessionId.size() != 0 , "sessionId should not be empty" );


		profileVector.clear();

		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		// As the CORBA interface throws CORBA exceptions, it is wrappered in a try/catch block so the
		// exceptions can be modified to be C++ exceptions.
		try
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15901] Get profiles '%s' requested.",
				sessionId.c_str() );

			// Call the method
			TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence_var profiles;
			AuthenticationAgentNamedObj & authAgent = 
				KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);

			CORBA_CALL_RETURN( profiles, authAgent, getProfiles, ( sessionId.c_str() ) );

			for (unsigned int i = 0;i < profiles->length();i++)
			{
				profileVector.push_back( profiles[i] );
			}
		}
		CATCH_AUTHENTICATION_EXCEPTION();

		FUNCTION_EXIT;
	}

	std::vector<unsigned long> AuthenticationLibrary::getProfiles(const std::string & sessionId )
	{
		std::vector<unsigned long> vtProfiles;
		getProfiles( sessionId, vtProfiles );
		return vtProfiles;
	}

	// This method is slightly different to all the other methods. As well as wrappering a call to the 
	// AuthenticationAgent, if that call generates any exception, this method will then attempt to retrieve the requested
	// session information from the database.
	SessionInfo AuthenticationLibrary::getSessionInfo(const std::string & sessionId, const std::string & requestingSessionId)
	{
		if ( TA_Base_Core::SessionAccessFactory::isSupperSessionId( sessionId ) )
		{
			return getSessionInfoFromDb( sessionId );
		}
		EnsureSpecificCondition( sessionId.size() != 0 , "sessionId should not be empty" );

		try
		{

			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef_var session;
			try
			{
				// Call the method
				AuthenticationAgentNamedObj & authAgent = 
					KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);			
				CORBA_CALL_RETURN( session,
					authAgent,
					getSessionInfo,
					( sessionId.c_str(), requestingSessionId.c_str() ) );
			}
			CATCH_AUTHENTICATION_EXCEPTION();
			return convertCORBASessionToCSession(session);
		}
		catch(...)
		{
			// Now try to get this info from the database
			return getSessionInfoFromDb(sessionId);
		}
	}

	SessionInfo AuthenticationLibrary::getSessionInfoFromDb( const std::string & sessionId )
	{
		// Wrap the lot in a try-catch - database code can throw exceptions
		try
		{
			std::auto_ptr<TA_Base_Core::ISession> session( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId) );

			// Create the session object to put the information in
			SessionInfo sessionInfo;

			// Copy the details into the session object
			sessionInfo.UserId = session->getOperatorKey();
			sessionInfo.OperatorId.push_back( session->getOperatorKey() );
			sessionInfo.ProfileId.push_back(session->getProfileKey());
			sessionInfo.SessionId = sessionId;
			sessionInfo.LocationId = session->getLocationKey();
			sessionInfo.WorkstationId = session->getConsoleKey();
			sessionInfo.isDisplayOnly = session->getDisplayMode();
			return sessionInfo;
		}
		catch(TA_Base_Core::DatabaseException& ex)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException",
				"Database exception thrown while retrieving session information from the database, with the following error: "
				+ std::string(ex.what()));

			std::stringstream exMsg;
			exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}
		catch(TA_Base_Core::DataException& ex)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException",
				"Data exception thrown while retrieving session information from the database, with the following error: "
				+ std::string(ex.what()));

			std::stringstream exMsg;
			exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}
		catch(TA_Base_Core::DataConfigurationException& ex)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "DataConfigurationException",
				"Data Configuration exception thrown while retrieving session information from the database, with the following error: "
				+ std::string(ex.what()));

			std::stringstream exMsg;
			exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}
		catch(...)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "...",
				"Unknown exception thrown while retrieving session information from the database");

			std::stringstream exMsg;
			exMsg << "An unknown error occurred while loading session information for session " << sessionId << " from the database.";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}
	}


	void AuthenticationLibrary::endSession(const std::string & sessionId)
	{
		FUNCTION_ENTRY("endSession");
		EnsureSpecificCondition( sessionId.size() != 0 , "sessionid should not be empty" );

		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		// As the CORBA interface throws CORBA exceptions, it is wrappered in a try/catch block so the
		// exceptions can be modified to be C++ exceptions.
		try
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15901] End session '%s' requested.",
				sessionId.c_str() );

			// Stop keeping the old session alive.
			KeepSessionAliveThreadManager::getInstance().stopKeepingSessionAlive( sessionId );

			// Call the method
			AuthenticationAgentNamedObj & authAgent = 
				KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);
			CORBA_CALL( authAgent,
				endSession,
				( sessionId.c_str() ) );
		}
		CATCH_AUTHENTICATION_EXCEPTION();

		FUNCTION_EXIT;
	}

	void AuthenticationLibrary::getActiveSessions(const std::string & sessionId, 
		std::vector<SessionInfo> & vtSessionInfo)
	{
		getAuthenPeerTaskManager()->getActiveSessions(sessionId, vtSessionInfo);
	}


	void AuthenticationLibrary::getActiveSessionsForOneLocation( const std::string & noUseSessionId, AuthenticationAgentNamedObj & authAgent, 
		std::vector<SessionInfo> & sessionInfoVector)
	{
		FUNCTION_ENTRY("getActiveSessionsForOneLocation");

		sessionInfoVector.clear();

		// The only thing that this library call does is convert the CORBA interface to a C++ interface. 
		// As the CORBA interface throws CORBA exceptions, it is wrappered in a try/catch block so the
		// exceptions can be modified to be C++ exceptions.
		try
		{
			//			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TD15901] Get active sessions '%s' requested.",
			//                    sessionId.c_str() );

			// Call the method
			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence_var sessions; 

			CORBA_CALL_RETURN( sessions,
				authAgent,
				getActiveSessions,
				( noUseSessionId.c_str() ) );

			for (unsigned int i = 0;i < sessions->length();i++)
			{
				sessionInfoVector.push_back( convertCORBASessionToCSession( sessions[i] ) );
			}
		}
		CATCH_AUTHENTICATION_EXCEPTION();

		FUNCTION_EXIT;
	}

	std::vector<SessionInfo> AuthenticationLibrary::getActiveSessions(const std::string & sessionId)
	{
		std::vector<SessionInfo> vtSessionInfo;
		getActiveSessions( sessionId, vtSessionInfo );
		return vtSessionInfo;
	}



	SessionInfo AuthenticationLibrary::convertCORBASessionToCSession(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& corbaSession)
	{
		SessionInfo sessionInfo;

		sessionInfo.SessionId = corbaSession.SessionId;
		sessionInfo.UserId = corbaSession.UserId;
		sessionInfo.LocationId = corbaSession.LocationId;
		sessionInfo.WorkstationId = corbaSession.WorkstationId;
		sessionInfo.isDisplayOnly = corbaSession.isDisplayOnly;
		unsigned int i;
		for ( i=0; i<corbaSession.OperatorId.length(); i++ )
		{
			sessionInfo.OperatorId.push_back( corbaSession.OperatorId[i] );
		}
		for ( i=0; i<corbaSession.ProfileId.length(); i++ )
		{
			sessionInfo.ProfileId.push_back( corbaSession.ProfileId[i] );
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[2011-12-19]AuthenticationLibrary::convertCORBASessionToCSession %d",sessionInfo.isDisplayOnly);
		return sessionInfo;
	}

	TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef AuthenticationLibrary::convertCSessionToCORBASession(const SessionInfo& cSession)
	{
		IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession;

		corbaSession.SessionId     = CORBA::string_dup(cSession.SessionId.c_str());
		corbaSession.UserId        = cSession.UserId;
		corbaSession.LocationId    = cSession.LocationId;
		corbaSession.WorkstationId = cSession.WorkstationId;
		corbaSession.isDisplayOnly = cSession.isDisplayOnly;
		unsigned int i;
		corbaSession.OperatorId.length( cSession.OperatorId.size() );
		for ( i=0; i<cSession.OperatorId.size(); i++ )
		{
			corbaSession.OperatorId[i] = cSession.OperatorId[i];
		}
		corbaSession.ProfileId.length( cSession.ProfileId.size() );
		for( i=0; i<cSession.ProfileId.size(); i++ )
		{
			corbaSession.ProfileId[i] = cSession.ProfileId[i];
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[2011-12-19]AuthenticationLibrary::convertCSessionToCORBASession %d",corbaSession.isDisplayOnly);
		return(corbaSession);
	}

	void AuthenticationLibrary::throwSecurityException(const std::string & description,
		TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason reason)
	{
		// If we get a security exception, it could be one of a large number of reasons for it.
		// The corresponding C++ exception has the same number of types, but (of course) you can't
		// just go from one to the other, so we need this switch statement.
		switch (reason)
		{
		case TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_USER:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::INVALID_USER) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_PASSWORD:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_PROFILE:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::INVALID_PROFILE) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_LOCATION:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::INVALID_LOCATION) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_WORKSTATION:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::INVALID_WORKSTATION) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::PROFILE_NOT_PERMITTED:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::PROFILE_NOT_PERMITTED) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::LOCATION_NOT_PERMITTED:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::LOCATION_NOT_PERMITTED) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_SESSION:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::INVALID_SESSION) );
			break;


		case TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_PASSWORD_LENGTH:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD_LENGTH) );
			break; 

		case TA_Base_Bus::IAuthenticationAgentCorbaDef::USER_LOCKED:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::USER_LOCKED) );
			break; 

		case TA_Base_Bus::IAuthenticationAgentCorbaDef::PASSWORD_USEDBEFORE:
			TA_THROW(TA_Base_Core::AuthenticationSecurityException(description.c_str(),
				TA_Base_Core::AuthenticationSecurityException::PASSWORD_USEDBEFORE) );
			break; 

		default:
			TA_THROW(TA_Base_Core::TransactiveException(description.c_str()));
			break;
		}
	}

	// TD12427: Before requesting a session make sure an equivalent one does not already exist. Note
	// that only operator ID, profile ID, and workstation ID are checked as that is all the
	// authentication agent checks when it looks for dead sessions.
	std::string AuthenticationLibrary::findEquivalentSessionNotUsingSessionId(const TA_Base_Bus::SessionInfo& sessionToMatch)
	{
		std::string tmpSessionId("no use sessionid");

		std::vector<SessionInfo> activeSessions;

		AuthenticationAgentNamedObj & authAgent = 
			KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionToMatch.LocationId);

		getActiveSessionsForOneLocation(tmpSessionId, authAgent, activeSessions);
		std::vector<SessionInfo>::iterator iter = activeSessions.begin();
		for(; iter != activeSessions.end(); ++iter)
		{
			TA_Assert( sessionToMatch.SessionId.empty() );
			//just compare the base OpearatorID
			//if ( ( !sessionToMatch.OperatorId.empty() ) && ( ! isVectorEqual(sessionToMatch.OperatorId, iter->OperatorId ) ) )
			if ( ( !sessionToMatch.OperatorId.empty() ) && ( sessionToMatch.OperatorId[0] != iter->OperatorId[0] ) )
			{
				continue;
			}
			//compare base profileID
			if ( ( !sessionToMatch.ProfileId.empty() ) && ( sessionToMatch.ProfileId[0] != iter->ProfileId[0] ) )
			{
				//Profile IDs do not match.";
				continue;
			}
			if ( ( 0 != sessionToMatch.WorkstationId ) && ( sessionToMatch.WorkstationId != iter->WorkstationId ) )
			{
				//Workstation ID does not match
				continue;
			}
			return iter->SessionId;
		}

		return "";
	}


	bool AuthenticationLibrary::isVectorEqual(const std::vector<unsigned long> & vtFirst, const std::vector<unsigned long> & vtSecond )
	{
		if ( vtFirst.size() != vtSecond.size() )
		{
			return false; 
		}
		for( size_t i=0; i<vtFirst.size(); i++)
		{
			if ( vtFirst[i] != vtSecond[i] )
			{
				return false;
			}
		}
		return true;
	}

	void AuthenticationLibrary::EnsureSpecificCondition(bool fCondition, char * msgStr )
	{
		//TA_ASSERT( session.SessionId.size() != 0 , "sessionId should not be empty" );
		if ( ! fCondition )
		{
			TA_THROW( TA_Base_Core::AuthenticationAgentException(msgStr) );
		}
	}

	AuthenPeerTaskManager * AuthenticationLibrary::getAuthenPeerTaskManager()
	{
		if ( NULL == m_pAuthenPeerTaskManager )
		{
			m_pAuthenPeerTaskManager = new AuthenPeerTaskManager(this);
		}
		return m_pAuthenPeerTaskManager;
	}

	void AuthenticationLibrary::dumpSessionInfo( std::vector<SessionInfo> & vtSessionInfo )
	{
		for( size_t i=0; i<vtSessionInfo.size(); i++)
		{
			SessionInfo & sessionInfo = vtSessionInfo[i];
			std::ostringstream ostr;
			ostr << "locationId=" <<sessionInfo.LocationId << ", ";
			ostr << "UserId=" <<sessionInfo.UserId << ", ";
			ostr << "sessionId=" << sessionInfo.SessionId;

			LOG1(SourceInfo, DebugUtil::DebugInfo, "dump session details: %s", ostr.str().c_str());
		}
	}

	//CL-21875
	//begin ovverrided successfully, write the override operator and profile to database
	void AuthenticationLibrary::updateOverrideInfoToDb( const std::string & sessionId, unsigned long userOverrideId, unsigned long profileOverrideId )
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "AuthenticationLibrary::updateOverrideInfoToDb");

		std::auto_ptr<TA_Base_Core::ISession> theSession( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId));
		theSession->updateOverrideInfoToDb(userOverrideId,profileOverrideId);

		LOG( SourceInfo, DebugUtil::FunctionExit, "AuthenticationLibrary::updateOverrideInfoToDb");
	}
	//end override successfully, erase the override operator and profile from database
	void AuthenticationLibrary::updateOverrideInfoToDb( const std::string & sessionId)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "AuthenticationLibrary::updateOverrideInfoToDb");

		std::auto_ptr<TA_Base_Core::ISession> theSession( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId));
		theSession->updateOverrideInfoToDb();

		LOG( SourceInfo, DebugUtil::FunctionExit, "AuthenticationLibrary::updateOverrideInfoToDb");
	}

	int AuthenticationLibrary::GetHash(const std::string& data, std::string& results)
	{
        //unsigned char *origin = new unsigned char[data.length()];
        //strcpy((char*)origin,data.data());
		//SHA1(origin, strlen(data.data()), obuf);
        unsigned char obuf[SHA_DIGEST_LENGTH] = { 0 };
        std::string dataUTF8 = TA_Base_Core::CodeConverter::toUTF8(data);
		unsigned char* origin = (unsigned char*) dataUTF8.c_str();

		SHA1(origin, dataUTF8.size(), obuf);

		std::stringstream stmCon;
		stmCon << std::hex;
		for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
			stmCon << std::setw(2) <<std::setfill('0') << (int) (obuf[i]);
		}
		std::string outputString = stmCon.str();
		for(unsigned int i=0;i<outputString.length();i++)
		{
			outputString[i] = toupper(outputString[i]);
		}
		results = outputString;

		return 0;
	}

	unsigned long	AuthenticationLibrary::getActualLocationKey(unsigned long sourceLocationKey)
	{
		unsigned long rtnLocationKey = sourceLocationKey;
		if(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_STATION_DEPLOY_KEY))	
		{
			unsigned long tmpLocationKey = atol(TA_Base_Core::RunParams::getInstance().get(RPARAM_STATION_DEPLOY_KEY).c_str());
			rtnLocationKey = tmpLocationKey;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "change location key from %lu to %lu",sourceLocationKey, tmpLocationKey);
		}
		return rtnLocationKey;
	}

}

