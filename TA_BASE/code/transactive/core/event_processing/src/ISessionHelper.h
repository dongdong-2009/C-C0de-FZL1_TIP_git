/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_RealTimeEvent/TA_BASE/transactive/core/event_processing/src/ISessionHelper.h $
  * @author: 
  * @version: $Revision: # $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * Defines the interface used to retrieve 
  * Session Information from Authentication Library.
  */

#if !defined(_I_SESSION_HELPER_H_1234_)
#define _I_SESSION_HELPER_H_1234_

namespace TA_Base_Core
{
    class ISessionHelper
    {
    public:
        /** 
          * getSessionInfo
          *
          * Returns the Session Information based on
		  * Specified sesssion ID.
          *
          * @return
          */
        virtual void getSessionInfo(std::string sessionId,unsigned long & operatorKey, long& profileID, unsigned long& sessionLocationKey ) =0;		 
    };
};

#endif // !defined(_I_SESSION_HELPER_H_1234_)

