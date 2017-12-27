// AuthenPeerTaskManager.h: interface for the AuthenPeerTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTHENPEERTASKMANAGER_H__82B17DA0_C8A5_4395_9C45_3B79B8AA8E89__INCLUDED_)
#define AFX_AUTHENPEERTASKMANAGER_H__82B17DA0_C8A5_4395_9C45_3B79B8AA8E89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/security/authentication_library/src/AuthenPeerTask.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class AuthenticationLibrary;
	
	class AuthenPeerTaskManager  
	{
	public:
		AuthenPeerTaskManager( AuthenticationLibrary * );
		virtual ~AuthenPeerTaskManager();
		
		void getActiveSessions(const std::string & sessionId, std::vector<SessionInfo> & vtSessionInfo);
		void waitOnAllTaskFinish();
	private:

		typedef std::map<unsigned long, AuthenPeerTask*> AuthenTaskMap;
		AuthenTaskMap m_authenTaskMap;

        // The lock for protecting access to the sessions list.
        TA_Base_Core::ReEntrantThreadLockable m_sessionGetLock;


	};

}

#endif // !defined(AFX_AUTHENPEERTASKMANAGER_H__82B17DA0_C8A5_4395_9C45_3B79B8AA8E89__INCLUDED_)
