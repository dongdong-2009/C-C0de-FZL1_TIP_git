/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/DeadSessionCheckerThread.cpp $
 * @author:   Anita Lee
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class checks for dead sessions. It periodically loops through the session register, 
 * checks the timestamp and determines whether the session should be removed or not. If 
 * the Authentication Agent hasn't been notified by the Control Station that a session is
 * still alive within the timeout period, the session will be removed.
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)   // Decorated name too long
#endif // defined(_MSC_VER)

#include <time.h>

#include "app/security/authentication_agent/src/DeadSessionCheckerThread.h"
#include "app/security/authentication_agent/src/Authentication.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"


using TA_Base_Bus::SessionInfo;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    
    const std::string DeadSessionCheckerThread::DISABLE_SESSION_TIMEOUT = "DisableSessionTimeout";

	const long CHECK_TIME_FOR_BAD_SESSION = 5*60;

    DeadSessionCheckerThread::DeadSessionCheckerThread(bool isControl) 
        : m_authenticator(NULL), 
          m_terminated(false),
          m_frequency(0),
          m_timeout(0),
          m_isControl(isControl)
    {
    }


	void DeadSessionCheckerThread::setAuthenticator( Authentication* authenticator )
	{
		m_authenticator = authenticator;
	}

    void DeadSessionCheckerThread::run()
    {
        FUNCTION_ENTRY("run");

        TA_ASSERT(m_frequency > 0, "SessionCheckerFrequency value is invalid");
        TA_ASSERT(m_timeout > 0, "TimeOutPeriod value is invalid");
		TA_ASSERT(m_authenticator != NULL, "Authenticator has not been set");

        const int TIME_IN_MILLISECS = 1000;

		//initialize for current time, to avoid 
        time_t lastActiveTime = 0;
		time_t startTime=0, endTime=0;
		std::vector<TA_Base_Bus::SessionInfo> sessionRegister;

        while(!m_terminated)
        {
			time(&startTime);
			if ( m_isControl )
			{
				sessionRegister.clear();
				m_authenticator->getSessionRegister().getRegister(sessionRegister);
				
				// ly++ for CL-20784, date:20090420
				m_authenticator->getSessionRegister().appendGhostSession( sessionRegister );
				// ++ly

				std::vector<TA_Base_Bus::SessionInfo>::iterator iter = sessionRegister.begin();
				for(; iter != sessionRegister.end() && !m_terminated; ++iter, lastActiveTime = 0)
				{

//					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DeadSessionCheckerThread, check session<%s> , userid=%d, workstationId=%d", 
//						(iter->SessionId).c_str(), iter->UserId, iter->WorkstationId);

					if( !m_authenticator->getLastSessionActiveTime(iter->SessionId,lastActiveTime,m_endTimeForBadSession) )
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "DeadSessionCheckerThread, session<%s> could not get last session activeTime, userid=%d, workstationId=%d", 
							(iter->SessionId).c_str(), iter->UserId, iter->WorkstationId);
						
						//continue;
					}

					time_t timeNow;
					time(&timeNow);

					if((timeNow - lastActiveTime > m_timeout) && 
						!TA_Base_Core::RunParams::getInstance().isSet(DISABLE_SESSION_TIMEOUT.c_str()) // Check the run-param to ensure that session timeout has not been disabled
						) // Only remove session if this is the Control Agent
					{
						// Remove the session - it's no longer active
						LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "DeadSessionCheckerThread, session<%s> will be removed, userid=%d, workstationId=%d, timeNow=%lu, lastActiveTime=%lu, m_timeout=%lu", 
							(iter->SessionId).c_str(), iter->UserId, iter->WorkstationId
							, timeNow, lastActiveTime, m_timeout);
						m_authenticator->endSession(iter->SessionId);
					}
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
							"DeadSessionCheckerThread, session<%s> will be active, userid=%d, workstationId=%d, timeNow=%lu, lastActiveTime=%lu, m_timeout=%lu", 
							(iter->SessionId).c_str(), iter->UserId, iter->WorkstationId
							, timeNow, lastActiveTime, m_timeout);
					}
				}
			}

            time(&endTime);

            time_t sleepTime = m_frequency - (endTime - startTime);

            for(int i = 0; i < sleepTime && !m_terminated; ++i)
            {
                sleep(TIME_IN_MILLISECS);
            }
        }

        FUNCTION_EXIT;
    }

    void DeadSessionCheckerThread::terminate()
    {
        FUNCTION_ENTRY("terminate");

        m_terminated = true;

        FUNCTION_EXIT;
    }

	void DeadSessionCheckerThread::setEndTimeForBadSession()
	{
		time(&m_endTimeForBadSession);
		//if one session has not ended before last login, it means it does not have entry in sessionRegister 
		//it will only exist CHECK_TIME_FOR_BAD_SESSION seconds
		m_endTimeForBadSession =  m_endTimeForBadSession - m_timeout + CHECK_TIME_FOR_BAD_SESSION;
	}
}
