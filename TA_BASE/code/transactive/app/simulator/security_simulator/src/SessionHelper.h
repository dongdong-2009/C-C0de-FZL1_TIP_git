// SessionHelper.h: interface for the SessionHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONHELPER_H__DD5E8325_C6B1_4EA1_9EAC_D35772C40F17__INCLUDED_)
#define AFX_SESSIONHELPER_H__DD5E8325_C6B1_4EA1_9EAC_D35772C40F17__INCLUDED_

#include <map>

#include "core/naming/src/NamedObject.h"

#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "bus/security/authentication_library/src/SessionInfo.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SessionHelper  
{
	typedef TA_Base_Core::NamedObject<TA_Base_Bus::IAuthenticationAgentCorbaDef,
		TA_Base_Bus::IAuthenticationAgentCorbaDef_ptr, 
		TA_Base_Bus::IAuthenticationAgentCorbaDef_var>							AuthenticationAgentNamedObj;
	typedef std::map< std::string, AuthenticationAgentNamedObj* >				AuthAgentMap;
	typedef AuthAgentMap::iterator												AuthAgentMapIter;

	typedef std::map< std::string, std::vector< TA_Base_Bus::SessionInfo > >	AgentSessionMap;

public:

	SessionHelper( std::vector< std::string >& agentLocation );
	virtual ~SessionHelper();

	bool checkSessionInfo();

	TA_Base_Bus::SessionInfo convertCORBASessionToCSession( 
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& corbaSession);

	const std::string& getResultString() { return m_resultString; };

	void logAllSessionInfo();

	void getSessionInfo();

private:

	bool isTwoSessionSetTheSame( std::vector< TA_Base_Bus::SessionInfo >& objOne, 
		std::vector< TA_Base_Bus::SessionInfo >& objTwo);
	bool isTwoSessionInfoTheSame( TA_Base_Bus::SessionInfo& objOne, TA_Base_Bus::SessionInfo& objTwo);

	AuthAgentMap	m_agentMap;
	AgentSessionMap m_result;

	std::string		m_resultString;
	

};

#endif // !defined(AFX_SESSIONHELPER_H__DD5E8325_C6B1_4EA1_9EAC_D35772C40F17__INCLUDED_)
