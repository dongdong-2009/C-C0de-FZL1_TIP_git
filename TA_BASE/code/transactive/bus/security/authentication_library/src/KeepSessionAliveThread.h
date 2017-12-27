/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/authentication_library/src/KeepSessionAliveThread.h $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(KEEPSESSIONALIVETHREAD_H)
#define KEEPSESSIONALIVETHREAD_H

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <list>
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"

namespace TA_Base_Bus
{
    typedef TA_Base_Core::NamedObject<TA_Base_Bus::IAuthenticationAgentCorbaDef,
            TA_Base_Bus::IAuthenticationAgentCorbaDef_ptr,
            TA_Base_Bus::IAuthenticationAgentCorbaDef_var> AuthenticationAgentNamedObj;


    class KeepSessionAliveThread : public virtual TA_Base_Core::Thread
    {
    public:

        /**
          * Constructor
          */
        KeepSessionAliveThread();


        /**
          * Destructor
          */
        ~KeepSessionAliveThread();


        /**
          * run
          */
        void run();


        /**
          * terminate
          */
        void terminate();


        /**
          * setHeartBeat
          *
          */
        void setHeartbeat( const unsigned long& heartbeat );


        /**
          * addSession
          *
          */
        void addSession( const SessionInfo& session );


        /**
          * removeSession
          *
          */
        void removeSession( const std::string& session );


        /**
          * removeAllSessions
          *
          */
        void removeAllSessions();


        /**
          * findMatchingSession
          *
          * Finds the first session that matches the specified details.
          *
          * @param sessionToMatch The session to match.
          * @param sessionFound The details of the matching session.
          *
          * @return bool True if a match was found.
          */
        bool canFindMatchingSession( const std::string & sessionId, SessionInfo& sessionFound);

    private:

        // Hidden methods.
        KeepSessionAliveThread( const KeepSessionAliveThread& );
        KeepSessionAliveThread& operator=(const KeepSessionAliveThread& );

        // Structures.
        struct KeepAliveInfo // The struct for storing keep alive status information.
        {
            unsigned long errorCount;
            SessionInfo sessionInfo;
        };

        // Type definitions.
        typedef std::list<KeepAliveInfo> KeepAliveList;	

        typedef enum EHeartbeatResult
        {
            E_HB_SUCCESSFUL,    // The heartbeat was successfully sent.
            E_HB_FAILED,        // The heartbeat was not sent.
            E_HB_FAILED_IGNORE, // The heartbeat was not sent, but the failure should be ignored.
            E_HB_FAILED_REMOVE  // The heartbeat was not sent and should never be tried again.
        };


        /**
          * findMatchingSession
          *
          * Finds the first session that matches the specified details.
          *
          * @param sessionToMatch The session to match.
          *
          * @return KeepAliveList::iterator The iterator pointing to the element.
          */
        KeepAliveList::iterator findSessionWithId( const std::string & sessionId );


        /**
          * sendHeartbeatForSession
          *
          * Sends the heartbeat for the specified session.
          *
          * @param sessionId The ID of the session to send the heartbeat for.
          *
          * @return EHeartbeatResult The result of the heartbeat attempt.
          */
        EHeartbeatResult sendHeartbeatForSession( const std::string& sessionId );

		bool doHeartBeat(KeepAliveList::iterator & keepAliveIt , const std::string & sessionId );

		void removeDeadSession( KeepAliveList & keepAliveList );
		void outputDebugInfomation( unsigned long & runtimes , const char * debugInfo, unsigned long baseValue = 1000, unsigned long largestValue=100000000);
	private:

        // Used to specify when the keep alive thread should be terminated.
        volatile bool m_terminated;

        // The condition used to wait for the heart beat interval.
        TA_Base_Core::Condition m_condition;

        // The number of seconds to wait between sending keep alive requests.
        unsigned long m_heartbeat;

        // The lock for protecting access to the sessions list.
        TA_Base_Core::ReEntrantThreadLockable m_keepAliveListLock;

        // The list used to store the information for sessions to keep alive.
        KeepAliveList m_keepAliveList;
		
		// Make sure run is called before terminate
		bool m_runStarted;
    };
}

#endif // !defined(KEEPSESSIONALIVETHREAD_H)
