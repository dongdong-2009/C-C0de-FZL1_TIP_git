// AuthenPeerTaskMO.h: interface for the AuthenPeerTaskMO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTHENPEERTASKMO_H__DA93E3C7_47A3_4EB7_85C6_8DC4F430E699__INCLUDED_)
#define AFX_AUTHENPEERTASKMO_H__DA93E3C7_47A3_4EB7_85C6_8DC4F430E699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/security/authentication_library/src/AuthenConst.h"

namespace TA_Base_Bus
{
	class AuthenPeerTask;

	class AuthenPeerTaskMO : public ACE_Method_Request
	{
	// Public methods
	public:
		AuthenPeerTaskMO( AuthenPeerTask * pAuthenPeerTask, const std::string & sessionId, ACE_Future<bool> & execResult );
		virtual int call();

	protected:
		virtual void execute() = 0;

	private:
		AuthenPeerTaskMO& operator=( const AuthenPeerTaskMO& );
		AuthenPeerTaskMO( const AuthenPeerTaskMO& );

	protected:
		ACE_Future<bool> m_execResult;
		AuthenPeerTask * m_pAuthenPeerTask;
		std::string m_sessionId;

	}; 
}

#endif // !defined(AFX_AUTHENPEERTASKMO_H__DA93E3C7_47A3_4EB7_85C6_8DC4F430E699__INCLUDED_)
