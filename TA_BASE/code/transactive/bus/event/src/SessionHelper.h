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

#ifndef SESSION_HELPER_123456_H
#define SESSION_HELPER_123456_H

#include <string>
#include "core/event_processing/src/ISessionHelper.h"

namespace TA_Base_Bus
{
	class SessionHelper : public TA_Base_Core::ISessionHelper
	{
	public :

		SessionHelper();

		~SessionHelper();
		/** 
          * getSessionInfo
          *
          * Returns the Session Information based on
		  * Specified sesssion ID.
          *
          * @return
          */
		void getSessionInfo(std::string sessionId,unsigned long & operatorKey, long& profileID, unsigned long& sessionLocationKey );
	
	};
};

#endif //SESSION_HELPER_123456_H