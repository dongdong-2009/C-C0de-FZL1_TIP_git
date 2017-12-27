// GetActiveSessionsMO.h: interface for the GetActiveSessionsMO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETACTIVESESSIONSMO_H__F42E5B4C_939F_4892_87D9_29FEC8A9F96D__INCLUDED_)
#define AFX_GETACTIVESESSIONSMO_H__F42E5B4C_939F_4892_87D9_29FEC8A9F96D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/security/authentication_library/src/AuthenConst.h"

#include "bus/security/authentication_library/src/AuthenPeerTaskMO.h"

namespace TA_Base_Bus
{
	class GetActiveSessionsMO : public AuthenPeerTaskMO  
	{
	public:
		GetActiveSessionsMO(AuthenPeerTask * pAuthenPeerTask, const std::string & sessionId, ACE_Future<bool> & execResult);
		virtual ~GetActiveSessionsMO();
	protected:
		virtual void execute();
		
	private:
		GetActiveSessionsMO& operator=( const GetActiveSessionsMO& );
		GetActiveSessionsMO( const GetActiveSessionsMO& );
	};
}

#endif // !defined(AFX_GETACTIVESESSIONSMO_H__F42E5B4C_939F_4892_87D9_29FEC8A9F96D__INCLUDED_)
