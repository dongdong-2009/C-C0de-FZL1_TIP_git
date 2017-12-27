/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class maintains information about a peer
  * System Controller (or System Controller that is
  * observing a group we are in). It contains a pointer to
  * an IHeartbeatReceiverCorbaDef that it uses to send
  * heartbeats.
  *
  */

#if !defined(PeerSCInfo_42202991_5F53_4ef2_AA2C_B37EB632C6FD__INCLUDED_)
#define PeerSCInfo_42202991_5F53_4ef2_AA2C_B37EB632C6FD__INCLUDED_

#include <string>
#include "core/process_management/IDL/src/IHeartbeatReceiverCorbaDef.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_App
{

    class PeerSCInfo : public TA_Base_Core::Thread
    {

    public:

        /**
          * Constructor
          *
          * @param myHostname           The hostname we are running on.
          * @param peerHostname         The hostname of the System Controller
          *                             we wish to send heartbeats to.
          * @param heartbeatSendPeriod  The time, in milliseconds, between
          *                             heartbeats.
          */
        PeerSCInfo(const std::string& myHostname, const std::string& peerHostname, int heartbeatSendPeriod, int heartbeatSendNumber);

        /**
          * Destructor
          */
	    virtual ~PeerSCInfo();

        /**
          * run
          *
          * Starts this thread, which continually sends a heartbeat
          * to the System Controller.
          */
        virtual void run();

        /**
          * terminate
          *
          * Terminates the thread.
          */
        virtual void terminate();


    private:
        PeerSCInfo(const PeerSCInfo& thePeerSCInfo);
   		PeerSCInfo& operator=(const PeerSCInfo&);

        /**
          * resolvePeer
          *
          * Attempts to resolve the remote System Controller's
          * SystemControllerMonitor. Called in the constructor
          * and if the reference is null.
          */
        void resolvePeer();

        /** 
          * beatHeart
          *
          * Sends a heartbeat to the System Controller represented 
          * by this PeerSCInfo object.
		  *
          */
        void beatHeart();

        /**
          * The name of our host (ie the name of the host that is
          * sending the heartbeats).
          */
        const std::string m_myHostname;

        /**
          * The name of the peer that we are sending heartbeats to.
          */
        const std::string m_peerHostname;

        /**
          * True if we have resolved the peer System Controller
          * successfully, and false otherwise.
          */
        bool m_isPeerResolved;

        /**
          * The time, in milliseconds, between heartbeats.
          */
        unsigned long m_heartbeatSendPeriod;
        unsigned long m_heartbeatSendNumber;

		/**
		  * The time, in milliseconds, the first heartbeat was sent.
		  */
		// yanrong++: CL-21195, 2010-01-22
		//unsigned long m_firstHeartbeatTime;
		//double m_firstHeartbeatTime;
		// ++yanrong

		/**
		  * The number of the last heartbeat sent.
		  */
		// yanrong++: CL-21195, 2010-01-22
		//unsigned long m_heartbeatCount;
		// ++yanrong

        /**
          * A pointer to an IHeartbeatReceiverCorbaDef object that
          * is used to send the heartbeat.
          */
        TA_Base_Core::IHeartbeatReceiverCorbaDef_var m_systemControllerMonitor;

        /**
          * True if the thread is running, and false otherwise.
          */
        bool volatile m_isRunning;

		/**
          * The port to listen on.
          */
		int m_port;

    };

} // namespace TA_Base_App

#endif // !defined(PeerSCInfo_42202991_5F53_4ef2_AA2C_B37EB632C6FD__INCLUDED_)
