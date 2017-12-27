/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/DeadSessionCheckerThread.h $
 * @author:   Anita Lee
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class provides the C++ interface that represents the CORBA interface of the Authentication
 * Agent. Agent and library will use this interface, while CORBA translation will take place
 * elsewhere.
 */

#if !defined(DEAD_SESSION_CHECKER_THREAD_H)
#define DEAD_SESSION_CHECKER_THREAD_H

#include "core/threads/src/Thread.h"

#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"

#include "app/security/authentication_agent/src/SessionRegister.h"
#include "app/security/authentication_agent/src/AuthenticationMessageReceiver.h"

namespace TA_Base_App
{
	class Authentication;
}

namespace TA_Base_App
{
    class DeadSessionCheckerThread : public TA_Base_Core::Thread
    {

    public:

        DeadSessionCheckerThread(bool isControl);

        virtual ~DeadSessionCheckerThread() {};

	    virtual void run();

        virtual void terminate();

        void setSessionCheckerFrequency(unsigned long frequency)
        {
            m_frequency = frequency;
        }

        void setTimeOutPeriod(time_t timeOutPeriod)
        {
            m_timeout = timeOutPeriod;
        }

        void setControl()
        {
			setEndTimeForBadSession();
            m_isControl = true;
        }

        void setMonitor()
        {
            m_isControl = false;
        }
        
		void setAuthenticator( Authentication* authenticator );

    private:
		void setEndTimeForBadSession();
    private:

        Authentication* m_authenticator;
        unsigned long m_frequency;
        time_t m_timeout;
        bool m_terminated;
        bool m_isControl;
		//if one session has no active time, it should be ended after a time limit 
		time_t m_endTimeForBadSession;

        static const std::string DISABLE_SESSION_TIMEOUT;

    };
} // end namespace TA_Base_App

#endif // !defined(DEAD_SESSION_CHECKER_THREAD_H)
