/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File:  $
 * @author:  
 * @version: $Revision: # $
 *
 * Last modification: $DateTime: $
 * Last modified by:  $Author: $
 * 
 * This class used to retrive the session information
 * from authentication library. (Used by RealtimeEventHelper class in core/Event_processing project)
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined(_MSC_VER)


#include "SessionHelper.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

namespace TA_Base_Bus
{

	SessionHelper::SessionHelper()
	{
		FUNCTION_ENTRY("SessionHelper");		
		FUNCTION_EXIT;
	}

	
	SessionHelper::~SessionHelper()
	{
		FUNCTION_ENTRY("~SessionHelper");		
		FUNCTION_EXIT;
	}

	void SessionHelper::getSessionInfo(std::string sessionId,unsigned long & operatorKey, long & profileID, unsigned long& sessionLocationKey)
	{
		FUNCTION_ENTRY("getSessionInfo");	
		TA_Base_Bus::AuthenticationLibrary* m_authLib = new TA_Base_Bus::AuthenticationLibrary();
		TA_Base_Bus::SessionInfo sessionInfo = m_authLib->getSessionInfo(sessionId, sessionId);
		operatorKey = sessionInfo.UserId;
		profileID = sessionInfo.ProfileId[0];
		sessionLocationKey = sessionInfo.WorkstationId;			//console key
		FUNCTION_EXIT;
	}

}



