// SpecialRightLibrary.h: interface for the SpecialRightLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPECIALRIGHTLIBRARY_H__17DF2234_206A_4C8F_A9DC_A20C6F659D06__INCLUDED_)
#define AFX_SPECIALRIGHTLIBRARY_H__17DF2234_206A_4C8F_A9DC_A20C6F659D06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

#include "core/naming/src/NamedObject.h"

#include "bus/security/security_utilities/src/SecurityUtilitiesInternal.h"
#include "bus/security/rights_library/src/RightsLibraryInternal.h"
//#include "core/message/IDL/src/CommsMessageCorbaDef.h"
//#include "core/message/src/CommsMessageSender.h"
//#include "core/message/src/MessageSubscriptionManager.h"

#include "bus/security/security_utilities/src/SeObserver.h"
#include "bus/security/security_utilities/src/SeSubject.h"
#include "bus/security/security_utilities/src/SessionUpdateSubject.h"
#include "bus/security/security_utilities/src/ISessionUpdateObserver.h"
#include "bus/security/rights_agent/idl/src/IRightsAgentCorbaDef.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/rights_library/src/RightsCache.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"


namespace TA_Base_Bus
{		
	class SpecialRightLibrary : public RightsLibrary	
	{
	public:
		SpecialRightLibrary(std::string& entityName); 
		virtual ~SpecialRightLibrary();
		
		virtual bool isActionPermittedOnResource(
			const std::string& sessionId, unsigned long resourceKey,
			unsigned long actionKey, std::string& reasonString,
			EDecisionModule& decisionModule );
		
		virtual bool isActionPermittedOnResourceAndLocSub(
			const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
			unsigned long actionKey, std::string& reasonString,
			EDecisionModule& decisionModule );
		
		virtual void resetCache();
		
	private:
		SpecialRightLibrary( const SpecialRightLibrary& );
		SpecialRightLibrary& operator=( const SpecialRightLibrary& );
		
        /**
		* isSuperProfile
		*
		* Returns true if the specified session is using the super profile.
		*
		* @param sessionId The ID of the session to check the profiles for.
		*
		* @return bool True if the specified session has the super profile associated with it.
		*/
        bool isSuperProfile( const std::string& sessionId );
		
        /**
		* isSuperSession
		*
		* Returns true if the specified session is the super session.
		*
		* @param sessionId The ID of the session to check.
		*
		* @return bool True if the specified session is the super session.
		*/
        bool isSuperSession( const std::string& sessionId );
		
		typedef TA_Base_Core::NamedObject<IRightsAgentCorbaDef,
										  IRightsAgentCorbaDef_ptr,
										  IRightsAgentCorbaDef_var> RightsAgentType;
		
		RightsAgentType* m_rightsAgent;
		SessionUpdateSubject m_sessionUpdates;
		RightsCache m_cache;
		DutyCommsSubscriber m_subscriber;
		TA_Base_Core::NonReEntrantThreadLockable m_cacheLock;
        unsigned long m_superProfileKey;
        AuthenticationLibrary m_authLibrary;
    };
}
#endif // !defined(AFX_SPECIALRIGHTLIBRARY_H__17DF2234_206A_4C8F_A9DC_A20C6F659D06__INCLUDED_)
