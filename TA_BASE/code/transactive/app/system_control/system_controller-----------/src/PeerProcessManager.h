/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/PeerProcessManager.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class allows peers to register for updates
  * on this System Controller's process data. The
  * System Controller will call notify() everytime
  * process information changes. This adds the process
  * update to a queue and is sent to each peer by this
  * class.
  *
  */

#if !defined(PeerProcessManager_27BF4BE6_54FE_4053_9521_710BFDAE17DE__INCLUDED_)
#define PeerProcessManager_27BF4BE6_54FE_4053_9521_710BFDAE17DE__INCLUDED_

#include "core/process_management/IDL/src/IProcessSubjectCorbaDef.h"
#include "core/process_management/IDL/src/IProcessObserverCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"
#include "core/corba/src/ServantBase.h"
#include "core/corba/src/CorbaUtil.h"

#include <map>
#include <queue>

namespace TA_Base_App
{

    class PeerProcessManager : public virtual POA_TA_Base_Core::IProcessSubjectCorbaDef,
                               public virtual TA_Base_Core::ServantBase,
							   public virtual TA_Base_Core::Thread
    {

    public:

        /**
          * Constructor
          */
	    PeerProcessManager();

        /**
          * Destructor
          */
	    virtual ~PeerProcessManager();

        /** 
          * registerProcessObserver
          *
		  * Called by a System Controller that wishes to observe
		  * a peer. Also allows the peer to observe it.
		  *
		  * @param host      The hostname of the System Controller registering.
		  * @param observer  An object reference that the peer can call back
		  *					 on to send process updates to the System Controller
		  *					 that is registering.
		  *
		  * @return An object that the registering System Controller can use 
		  *			to send process updates to us.
		  * 
		  */
		virtual TA_Base_Core::IProcessObserverCorbaDef_ptr registerProcessObserver(
			const char* host, TA_Base_Core::IProcessObserverCorbaDef_ptr observer);

		/**
		  * deregisterProcessObserver
		  *
		  * Called by an observer that no longer wishes to
		  * observe this System Controller.
		  *
		  * @param host  The hostname of the System Controller deregistering.
		  */
        virtual void deregisterProcessObserver(const char* host);
        
		/**
		  * registerPeer
		  *
		  * Called to register a peer. Adds the peer to the map of observers
		  * and sends it all of our process information.
		  *
		  * @param host      The hostname of the System Controller registering.
		  * @param observer  An object reference that the peer can call back
		  *					 on to send process updates to the System Controller
		  *					 that is registering.
		  *
		  */
		virtual void registerPeer(std::string host, 
									TA_Base_Core::IProcessObserverCorbaDef_ptr observer);

        /** 
          * notify
          *
          * Notifys all observers of this System Controller that process
          * data has been updated. Called by this System Controller
          * everytime process data changes.
          *
          * @param processData The process data that has been updated.
          *
          */
        virtual void notify(const TA_Base_Core::ProcessData& processData);

        /** 
          * run
          *
          * The starting point of this thread. Sends process updates
		  * from a queue to peers.
          *
          */
        void run();

        /**
          * terminate
          *
          * Terminates the thread.
          *
          */
        void terminate();

        TA_Base_Core::IProcessObserverCorbaDef_var getObserver(const std::string& host);

    private:
  	    PeerProcessManager( const PeerProcessManager& thePeerProcessManager);
        PeerProcessManager& operator=(const PeerProcessManager&);

        /** 
          * sendProcessUpdate
          *
          * Sends a process update to a peer.
          *
          * @param processData  The struct containing information about the process.
          */
	    void sendProcessUpdate(const TA_Base_Core::ProcessData& processData);


        /**
          * A map of hostnames to observers that wish to receive process data
          * updates from this System Controller.
          */
        std::map<std::string, TA_Base_Core::IProcessObserverCorbaDef_ptr> m_observers;

		/**
		  * Signalled whenever a new process update needs to be sent.
		  */
		TA_Base_Core::Semaphore m_semaphore;

		/**
		  * True if the thread has been told to terminate.
		  */
		bool m_isTerminated;

		/**
		  * A queue of process updates still to be sent.
		  */
		std::queue<TA_Base_Core::ProcessData> m_processDataQueue;

   		TA_Base_Core::ReEntrantThreadLockable m_lock;
    };

} // namespace TA_Base_App

#endif // !defined(PeerProcessManager_27BF4BE6_54FE_4053_9521_710BFDAE17DE__INCLUDED_)
