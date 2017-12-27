// AuthenPeerTask.h: interface for the AuthenPeerTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AuthenPeerTask_H__37486E42_09B0_4A54_9EB2_0C30EB77F15C__INCLUDED_)
#define AFX_AuthenPeerTask_H__37486E42_09B0_4A54_9EB2_0C30EB77F15C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/security/authentication_library/src/AuthenConst.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

namespace TA_Base_Bus
{

	class AuthenPeerTask : public ACE_Task<ACE_MT_SYNCH>  
	{
	public:
		AuthenPeerTask(AuthenticationAgentNamedObj * pAuthAgent, AuthenticationLibrary * m_pAuthLib, unsigned long locationId );
		virtual ~AuthenPeerTask();
	public:
		int open(void*);
		int close(u_long flags = 0 );
		int svc(void);
		ACE_Future<bool> getActiveSessions( const std::string & noUseSessionId );
	public:
		void getActiveSessionsImpl( const std::string & noUseSessionId );
		std::vector<SessionInfo> & getResultSession()
		{
			//no need to guard
			return m_vtActiveSessionInfo;
		}
	private:
		AuthenticationAgentNamedObj * m_pAuthAgent;
		AuthenticationLibrary * m_pAuthLib;
		unsigned long m_locationId;
		//collect result for asynchronous getactivecollection 
		std::vector<SessionInfo> m_vtActiveSessionInfo;

		ACE_Activation_Queue m_activationQueue;

		static const ACE_Time_Value m_msgQueueTimeout;
		static const ACE_Time_Value m_activationQueueTimeout;
		static const unsigned long THREAD_COUNT;


	};
}

#endif // !defined(AFX_AuthenPeerTask_H__37486E42_09B0_4A54_9EB2_0C30EB77F15C__INCLUDED_)
