/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/core/naming/src/NamingHeartbeatMonitor.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/02/27 19:31:54 $
  * Last modified by:  $Author: avteam $
  *
  * This is a workaround and should be removed once naming issue had been fixed.
  */

#if !defined(AFX_NAMINGHEARTBEATMONITOR_H__F1C8772E_71CC_492F_8EBB_60E17D9DA97A__INCLUDED_)
#define AFX_NAMINGHEARTBEATMONITOR_H__F1C8772E_71CC_492F_8EBB_60E17D9DA97A__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <time.h>
#include <string>
#include "core/naming/IDL/src/BindingRequestOriginatorCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/corba/src/ServantBase.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_Base_Core
{
    class BindingRequestMessageSender;

    class NamingHeartbeatMonitor :
            public virtual POA_TA_Base_Core::BindingRequestOriginatorCorbaDef,
            public virtual TA_Base_Core::Thread,
		    public virtual TA_Base_Core::ServantBase
    {
    public:
	    /**
	    * constructor
	    *
	    */
        NamingHeartbeatMonitor(std::string agentName);

	    /**
	    * desctructor
	    *
	    */
        virtual ~NamingHeartbeatMonitor();

        /** 
          * run
          *
          * The starting point for the thread. Periodically sends 
          * heartbeats to all peers and System Controllers monitoring 
          * the group we are in (if any).
          *
          */
   		virtual void run();

        /** 
          * terminate
          *
          * Terminates the thread.
          *
          */
        virtual void terminate();

		/**
		* doOperationModeChange
		*
		* if the agent become Control
		*/
		void doOperationModeChange(bool toControl);

        void sendNamingHeartbeat();

        /**
		* callback
		*
	    * this method is called by a binding request listener to deliver the requested
        * object reference
		*
		* @param	requestedObject	the resolved object. MUST NOT be NULL.
		* @param	timeToLive	The length of time the object will remain valid
		*				 in the cache (default 600 secs)
        */
		virtual void callback( NamedObjectRepositoryCorbaDef_ptr requestedObject, CORBA::ULong timeToLive );

    private:
        NamingHeartbeatMonitor();
        NamingHeartbeatMonitor(const NamingHeartbeatMonitor& theInstance);
        NamingHeartbeatMonitor & operator= ( const NamingHeartbeatMonitor & rhs );

		// Sender used to send binding request messages to object repositories.
		std::auto_ptr<BindingRequestMessageSender> m_bindingRequestSender;

        /**
          * The last heartbeat received.
          */
        time_t m_lastHeartbeat;

        TA_Base_Core::ReEntrantThreadLockable m_lock;

        // Block thread execution until callback received or timeout expires.
        Condition m_condition;

        /**
          * The interval, in seconds, between heartbeats.
          */
        long m_heartbeatInterval;

        long m_resolveTimeoutMs;

        /**
          * True if the thread is running, and false otherwise.
          */
        bool volatile m_isRunning;

		/**
		* This is true if the agent is in control mode
		*/
		bool volatile m_inControlMode;

        std::string m_agentName;

        static const unsigned long DEFAULT_HEARTBEAT_INTERVAL;
        static const unsigned long DEFAULT_HEARTBEAT_FAILLED_COUNT;

    public:
        static const std::string PARAM_NAMING_HEARTBEAT_INTERVAL;
    };

}; // TA_Base_Core

#endif // !defined(AFX_NAMINGHEARTBEATMONITOR_H__F1C8772E_71CC_492F_8EBB_60E17D9DA97A__INCLUDED_)
