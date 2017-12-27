/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/AdminSession.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AdminSession contains information about the administration 
  * session.
  *
  */

#if !defined(AFX_ADMINSESSION_H__E15D0514_32FF_11D5_B142_0050BAB0C7E8__INCLUDED_)
#define AFX_ADMINSESSION_H__E15D0514_32FF_11D5_B142_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>

namespace TA_Base_App
{

    class AdminSession  
    {
    public:

        /**
          * Constructor
          */
        AdminSession(const std::string& userName, const std::string& userPassword,
                     unsigned long sessionId);

        /**
          * Destructor
          */
        virtual ~AdminSession();
    
        /**
          * getUserName
          * 
          * @return A string containing the current user's name.
          */
        std::string getUserName();

        /**
          * getUserPassword
          * 
          * @return A string containing the current user's password.
          */
        std::string getUserPassword();

        /**
          * getSessionId
          * 
          * @return The current Session ID.
          */
        unsigned long getSessionId();

        /**
          * getLogoutTime
          * 
          * @return The logout time.
          */
        time_t getLogoutTime();

        /**
          * getLoginTime
          * 
          * @return The login time.
          */
        time_t getLoginTime();

        /**
          * setLogoutTime
          * 
          * @param The logout time
          */
        void setLogoutTime(time_t logoutTime);
    
    private:

        /**
          * The username for this session.
          */
        std::string m_userName;

        /**
          * The password for this session.
          */
        std::string m_userPassword;

        /**
          * The login time.
          */
        time_t m_loginTime;

        /**
          * The logout time.
          */
        time_t m_logoutTime;

        /**
          * The session ID.
          */
        unsigned long m_sessionID;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ADMINSESSION_H__E15D0514_32FF_11D5_B142_0050BAB0C7E8__INCLUDED_)
