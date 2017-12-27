/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/AdminSession.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * AdminSession contains information about the administration 
  * session.
  *
  */

#include "StdAfx.h"
#include "AdminClient.h"
#include "AdminSession.h"

namespace TA_Base_App
{

    AdminSession::AdminSession(const std::string& userName, const std::string& userPassword,
                               unsigned long sessionId)
    {
        m_userName     = userName;
        m_userPassword = userPassword;
        time(&m_loginTime);
        m_logoutTime = 0;
        m_sessionID = sessionId;
    }

    AdminSession::~AdminSession()
    {
    }

    std::string AdminSession::getUserName()
    {
        return m_userName;
    }

    void AdminSession::setLogoutTime(time_t logoutTime)
    {
        m_logoutTime = logoutTime;
    }

    time_t AdminSession::getLoginTime()
    {
        return m_loginTime;
    }

    time_t AdminSession::getLogoutTime()
    {
        return m_logoutTime;
    }

    std::string AdminSession::getUserPassword()
    {
        return m_userPassword;
    }

    unsigned long AdminSession::getSessionId()
    {
        return m_sessionID;
    }

} // namespace TA_Base_App


