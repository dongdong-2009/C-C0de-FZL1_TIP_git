// SessionHelper.cpp: implementation of the SessionHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "security_simulator.h"
#include "SessionHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define CATCH_AUTH_EXCEPTION()  \
catch(TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& ) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException", "");  \
} \
catch(TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException", ex.reason.in() ); \
} \
catch(CORBA::Exception& ex) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str() ); \
} \
catch(TA_Base_Core::ObjectResolutionException&) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", "");  \
} \
catch(...) \
{ \
	LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");  \
}

namespace
{
	const std::string AUTHENTICATION_NAME = "AuthenticationAgent";
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SessionHelper::SessionHelper( std::vector< std::string >& agentLocation )
{
	for( std::vector< std::string >::iterator it = agentLocation.begin();
		 it != agentLocation.end(); ++it )
	{
		AuthenticationAgentNamedObj* authAgent = new AuthenticationAgentNamedObj;
		authAgent->setEntityName( *it + AUTHENTICATION_NAME );
		m_agentMap.insert( AuthAgentMap::value_type(*it, authAgent) );
	}	
}

SessionHelper::~SessionHelper()
{
	for (AuthAgentMapIter it = m_agentMap.begin(); it != m_agentMap.end(); ++it)
	{
		delete it->second;
	}
}

bool SessionHelper::checkSessionInfo()
{
	getSessionInfo();
	
	bool result = true;

	AgentSessionMap::iterator iter = m_result.begin();
	AgentSessionMap::iterator tempIter = m_result.begin();
	for (++iter ; iter != m_result.end(); ++iter)
	{
		if( isTwoSessionSetTheSame(tempIter->second, iter->second) == false )
		{
			result = false;
			break;
		}
	}

	logAllSessionInfo();

	return result;
}

void SessionHelper::getSessionInfo()
{
	AuthAgentMapIter it = m_agentMap.begin();
	for (; it != m_agentMap.end(); ++it)
	{
		std::vector<TA_Base_Bus::SessionInfo> sessionVector;
		
		try
        {
			AuthenticationAgentNamedObj* agent = it->second;
            // Call the method
            TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence_var sessions = (*agent)->getActiveSessions( "" );
			
            for (unsigned int i = 0;i < sessions->length();i++)
            {
                sessionVector.push_back( convertCORBASessionToCSession( sessions[i] ) );
            }
        }
		CATCH_AUTH_EXCEPTION()
			
		m_result.insert( AgentSessionMap::value_type( it->first, sessionVector ) );
	}
}

TA_Base_Bus::SessionInfo SessionHelper::convertCORBASessionToCSession( 
	const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& corbaSession)
{
	TA_Base_Bus::SessionInfo sessionInfo;
	
	sessionInfo.SessionId = corbaSession.SessionId;
	sessionInfo.UserId = corbaSession.UserId;
	sessionInfo.LocationId = corbaSession.LocationId;
	sessionInfo.WorkstationId = corbaSession.WorkstationId;
	
	unsigned int i;
	for ( i=0; i<corbaSession.OperatorId.length(); i++ )
	{
		sessionInfo.OperatorId.push_back( corbaSession.OperatorId[i] );
	}
	for ( i=0; i<corbaSession.ProfileId.length(); i++ )
	{
		sessionInfo.ProfileId.push_back( corbaSession.ProfileId[i] );
	}
	
	return sessionInfo;
}

bool SessionHelper::isTwoSessionSetTheSame( std::vector< TA_Base_Bus::SessionInfo >& objOne, 
										   std::vector< TA_Base_Bus::SessionInfo >& objTwo)
{
	bool ret = true;

	if (objOne.size() != objTwo.size())
	{
		ret = false;
	}
	else
	{
		for(std::vector< TA_Base_Bus::SessionInfo >::iterator iter1 = objOne.begin(); 
			iter1 != objOne.end(); ++iter1)
		{
			bool isMatchOne = false;
			for(std::vector< TA_Base_Bus::SessionInfo >::iterator iter2 = objTwo.begin(); 
				iter2 != objTwo.end(); ++iter2)
			{
				if( isTwoSessionInfoTheSame( *iter1, *iter2 ) )
				{
					isMatchOne = true;
					break;
				}
			}
			
			if(!isMatchOne)
			{
				ret = false;
				break;
			}
		}
	}

	return ret;
}

bool SessionHelper::isTwoSessionInfoTheSame( TA_Base_Bus::SessionInfo& objOne, 
											TA_Base_Bus::SessionInfo& objTwo)
{
	bool ret = true;

	if ( objOne.LocationId != objTwo.LocationId || 
		 objOne.SessionId != objTwo.SessionId || 
		 objOne.WorkstationId != objTwo.WorkstationId ||
		 objOne.ProfileId.size() != objTwo.ProfileId.size() ||
		 objOne.OperatorId.size() != objTwo.OperatorId.size() )
	{
		ret = false;
	}
	else
	{
		for( std::vector< unsigned long >::iterator it1 = objOne.ProfileId.begin();
			 it1 != objOne.ProfileId.end(); ++it1)
		{
			bool isMatchOne = false;
			for ( std::vector< unsigned long >::iterator it2 = objOne.ProfileId.begin();
				  it2 != objOne.ProfileId.end(); ++it2) 
			{
				if( *it1 == *it2  )
				{
				  isMatchOne = true;
				  break;
				}
			}
			if (!isMatchOne) 
			{
				ret = false;
				break;
			}
		}

		if (ret)
		{
			for( std::vector< unsigned long >::iterator iter1 = objOne.OperatorId.begin();
			iter1 != objOne.OperatorId.end(); ++it1)
			{
				bool isMatchOne = false;
				for ( std::vector< unsigned long >::iterator iter2 = objOne.OperatorId.begin();
				iter2 != objOne.OperatorId.end(); ++iter2) 
				{
					if( *iter1 == *iter2 )
					{
						isMatchOne = true;
						break;
					}
				}
				if (!isMatchOne) 
				{
					ret = false;
					break;
				}
			}
		}
	}
	return ret;
}

void SessionHelper::logAllSessionInfo()
{
	m_resultString = "";
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "log all test info:");
	AgentSessionMap::iterator agetnIter = m_result.begin();
	for (; agetnIter != m_result.end(); ++agetnIter)
	{
		std::stringstream logInfo;
		logInfo << agetnIter->first << ": " << "\n";

		for ( std::vector< TA_Base_Bus::SessionInfo >::iterator sessionInfoIt = agetnIter->second.begin(); 
			  sessionInfoIt != agetnIter->second.end(); ++sessionInfoIt)
		{
			logInfo << "SessionId: " <<sessionInfoIt->SessionId << " "
					<< "LocationId: " <<sessionInfoIt->LocationId << " "
					<< "WorkstationId: " <<sessionInfoIt->WorkstationId << " ";

			logInfo << "OperatorId: ";
			for ( std::vector< unsigned long >::iterator operatorIter = sessionInfoIt->OperatorId.begin();
				  operatorIter != sessionInfoIt->OperatorId.end(); ++operatorIter)
			{
				logInfo << *operatorIter << " ";
			}

			logInfo << "ProfileId: ";
			for ( std::vector< unsigned long >::iterator profileIter = sessionInfoIt->ProfileId.begin();
				  profileIter != sessionInfoIt->ProfileId.end(); ++profileIter)
			{
				logInfo << *profileIter << "\n";
			}
		}
		logInfo << "\n";

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, " %s", logInfo.str().c_str());

		m_resultString = logInfo.str().c_str();
	}

	FILE *fp = fopen("SessionInfo.Log", "at");
	if(fp != NULL)
	{
		fprintf(fp, " %s \n", m_resultString.c_str());
		fclose(fp);
	}

}