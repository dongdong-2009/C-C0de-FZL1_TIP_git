// GetActiveSessionsMO.cpp: implementation of the GetActiveSessionsMO class.
//
//////////////////////////////////////////////////////////////////////

#include "bus/security/authentication_library/src/GetActiveSessionsMO.h"
#include "bus/security/authentication_library/src/AuthenPeerTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	GetActiveSessionsMO::GetActiveSessionsMO(AuthenPeerTask * pAuthenPeerTask, const std::string & sessionId, ACE_Future<bool> & execResult)
		:AuthenPeerTaskMO(pAuthenPeerTask, sessionId,execResult)
	{
	}

	GetActiveSessionsMO::~GetActiveSessionsMO()
	{

	}

	void GetActiveSessionsMO::execute()
	{
		m_pAuthenPeerTask->getActiveSessionsImpl(m_sessionId);
	}
}