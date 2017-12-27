#ifndef READWRITETHREADLOCKABLE_H_INCLUDED
#define READWRITETHREADLOCKABLE_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/ReadWriteThreadLockable.h $
  * @author:  Andrew Del Carlo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/04/09 09:48:03 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Implementation of the IThreadLockable interface that allows multipule threads to acquire a
  * read lock. A count of read locks held is maintained, with the lock only being released 
  * when the equivalent number of calls to release() are made.
  *
  * It also has a non-rentrant write lock. 
  *
  * Read lock requests are given priority over write lock requests. 
  * ie: a write lock request will block until all read locks are released and no further 
  *     read locks are pending.
  *
  * This lock should not be acquired directly, but managed using a temporary object (i.e. 
  * ThreadGuard) which ensures the lock is acquired during construction, and subsequently 
  * released during destruction.
  *
  * Note, this class wrappers the omni_mutex class, which in turn wrappers the OS-specific 
  * threading API. 
  *
  * NOTE: Condition is made a friend class so it can access backupReEntrantAcquisitionState() and
  * restoreReEntrantAcquisitionState(). This is necessary to allow Condition to backup and
  * restore the state before and after calling wait() or timedWait() on it's omni_condition.
  * Such backup and restore is needed as the linked omni_mutex (wrappered by this class) is intrinisically
  * released and reacquired at these times by the omni_condition. By allowing Condition to wrapper
  * the omni_condition's linked omni_mutex with this class, it can offer re-entrant access of the
  * mutex to it's clients.
  */

#include "IThreadLockable.h"
#include "Semaphore.h"

//
// Forward declarations
//

class omni_mutex;
class ACE_RW_Mutex;

namespace TA_Base_Core
{
    // Forward declared struct so as not to bring in any dependencies here that are
    // needed to identify the current thread on various platforms.
	class ThreadIdWrapper;
}

namespace TA_Base_Core
{
    class ReadWriteThreadLockable : public IThreadLockable
	{
	public:

        // Make Condition friend class so it can access backupReEntrantAcquisitionState() and
        // restoreReEntrantAcquisitionState(). This is necessary to allow Condition to backup and
        // restore the state before and after calling wait() or timedWait() on it's omni_condition.
        // Such backup and restore is needed as linked omni_mutex (wrappered by this class) is intrinisically
        // released and reacquired at these times by the omni_condition. By allowing Condition to wrapper
        // the omni_condition's linked omni_mutex with this class, it can offer re-entrant access of the
        // mutex to it's clients.
        friend class Condition;

        /** 
          * Default Constructor
          */
		ReadWriteThreadLockable();


        /** 
          * Destructor
          */
		virtual ~ReadWriteThreadLockable();


        /** 
          * acquire
          *
          * Acquire the write lock. 
          *
          */
		virtual void acquire();


        /** 
          * acquire
          *
          * Acquire the read lock. 
          *
          */
		virtual void acquireRead();


        /** 
          * release
          *
          * Release the lock. 
          *
          * PreCondition: The lock is currently acquired.  
          */
		virtual void release();

		/** 
          * release
          *
          * Release the lock. 
          *
          * PreCondition: The lock is currently acquired.  
          */
		virtual void releaseRead();

	private:

        //
        // Disable the default copy and assignment constructors as shouldn't used.
        //
        ReadWriteThreadLockable( const ReadWriteThreadLockable& );
        ReadWriteThreadLockable& operator=( const ReadWriteThreadLockable& );


		// Struct used to store the identifier of the thread that has currently aquired
		// the lock and inform client code whether its thread is a match.
		ThreadIdWrapper* m_threadLockableHolderVerifier;
		// Indicates when someone has aquired the read lock and the number
        // of times it has been aquired.
		volatile unsigned int m_nReaders;
		Semaphore m_semReaders;
		Semaphore m_semWriters;

		volatile unsigned int m_numTimesThreadAcquiredWriteLock;
		
        ////////////////////////////////////////////////////////////////////////////////
        //// Private state and methods to allow friend class Condition to offer     ////
        //// its linked mutex to client code wrappered in re-entrant access. These  ////
        //// state and methods allow Condition to backup and restore the re-entrant ////
        //// lock acquisition state. Condition needs to backup just before calling  ////
        //// wait or timedWait on its wrappered omni_condition, and restore just    ////
        //// after the wait completes. This is because the associated mutex is      ////
        //// intrinisically released just before entering wait and intrinisically   ////
        //// re-acquired directly afterwards. The period inbetween allows           ////
        //// competing thread to acquire and release the lock and hence modify the  ////
        //// re-entrant acquisition state.                                          ////
        ////////////////////////////////////////////////////////////////////////////////


        /** 
          * backupReEntrantAcquisitionState
          *
          * Private method so as to allow friend class Condition to backup 
          * re-entrant acquisition state before intrinsically releasing its
          * linked mutex prior to entering wait.
          */
        void backupReEntrantAcquisitionState();


        /** 
          * restoreReEntrantAcquisitionState
          *
          * Private method so as to allow friend class Condition to restore the previous
          * re-entrant acquisition state after intrinsically re-acquiring its
          * linked mutex after completing wait.
          */
        void restoreReEntrantAcquisitionState();



        // Member used to backup the number of times the read lock has been aquired
        volatile unsigned int m_previousNumTimesAcquiredReadLock;
		
		// Member used to backup the identifier of the thread that has currently aquired
		// the lock.
		ThreadIdWrapper* m_previousThreadLockableHolderVerifier;


        ACE_RW_Mutex* m_aceRwMutex;
	};
}

#endif // READWRITETHREADLOCKABLE_H_INCLUDED

