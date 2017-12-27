/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/ReadWriteThreadLockable.cpp $
  * @author:  Andrew Del Carlo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/04/09 09:48:03 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Implementation of the IThreadLockable interface that allows the same thread to acquire the
  * lock multiple times (i.e. consecutively). A count of consecutive acquisitions by the same
  * thread is maintained, with the lock only being released when the equivalent number of calls
  * to release() are made.
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

#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "ace/RW_Mutex.h"
#include "core/synchronisation/src/ThreadLockableHolderVerifier.h"
#include "core/utilities/src/TAAssert.h"
#include <omnithread.h>


namespace TA_Base_Core
{
    //
    // Default Constructor
    //
    ReadWriteThreadLockable::ReadWriteThreadLockable() 
        : m_nReaders( 0 ),
		  m_semReaders( 1 ),// start count at one as first call to wait should not block
		  m_semWriters( 1 ),
		  m_numTimesThreadAcquiredWriteLock( 0 ),
		  m_threadLockableHolderVerifier( new ThreadIdWrapper ),
          m_aceRwMutex( new ACE_RW_Mutex )
    {
    }


    //
    // Destructor
    //
    ReadWriteThreadLockable::~ReadWriteThreadLockable()
    {
		 delete m_threadLockableHolderVerifier;
         delete m_aceRwMutex;
    }


    //
    // acquire
    //
    void ReadWriteThreadLockable::acquireRead()
    {
        {
            m_aceRwMutex->acquire_read();
            return;
        }

        //wait till its safe to start reading
        m_semReaders.wait();
        
        if (m_nReaders == 0) //if we are the first thread to gain reader access
        {
            // P( semWriters )
			//wait for writing to finish
            m_semWriters.wait();
        }
	
		m_nReaders++;
	        
		// V( semReaders )
        m_semReaders.post();
		
   }


    //
    // acquire
    //
    void ReadWriteThreadLockable::acquire()
    {
        {
            m_aceRwMutex->acquire_write();
            return;
        }

        // (ReEntrant, Non-rentrant suck, theyre unpredictable)
		// If the thread has currently acquired this lock already, increment the count and 
        // return without waiting on the sem (it will never be posted)
        if ( ( 0 < m_numTimesThreadAcquiredWriteLock ) && ( m_threadLockableHolderVerifier->isCurrentThreadId() ) )
        {
            m_numTimesThreadAcquiredWriteLock++;
	        return;
        }

		m_semWriters.wait();

        // Thread has acquired lock so set as current lock holder and set count to 1.
        m_threadLockableHolderVerifier->setCurrentThreadId();
        m_numTimesThreadAcquiredWriteLock = 1;

    }


	void ReadWriteThreadLockable::releaseRead()
	{
        {
            m_aceRwMutex->release();
            return;
        }

		// P( semReaders )
		m_semReaders.wait();
        
        TA_ASSERT(m_nReaders != 0, "release read called when no readers have the lock");
		
		m_nReaders--;


        if (m_nReaders == 0)
        {
            // V( semWriters )
            m_semWriters.post();
        }
        // V( semReaders )
        m_semReaders.post();
	}
	
	//
    // release
    //
    void ReadWriteThreadLockable::release()
    {
        {
            m_aceRwMutex->release();
            return;
        }

		// V( semWriters )
		TA_ASSERT(m_nReaders <= 0, "write release called when readers have the lock (Readers should call releaseRead");
		
		TA_ASSERT( ( 0 != m_numTimesThreadAcquiredWriteLock ), "Call made to release while lock not currently acquired." );

        // Decrement the count.
        m_numTimesThreadAcquiredWriteLock--;

        // If the count is now back to 0, release the mutex.
        if ( 0 == m_numTimesThreadAcquiredWriteLock )
        {
			m_semWriters.post();
		}
    }

    ////////////////////////////////////////////////////////////////////////////////
    //// Private methods to allow friend class Condition to offer its           ////
    //// linked mutex to client code wrappered in re-entrant access. These      ////
    //// methods allow Condition to backup and restore the re-entrant           ////
    //// lock acquisition state. Condition needs to backup just before calling  ////
    //// wait or timedWait on its wrappered omni_condition, and restore just    ////
    //// after the wait completes. This is because the associated mutex is      ////
    //// intrinisically released just before entering wait and intrinisically   ////
    //// re-acquired directly afterwards. The period inbetween allows           ////
    //// competing thread to acquire and release the lock and hence modify the  ////
    //// re-entrant acquisition state.                                          ////
    ////////////////////////////////////////////////////////////////////////////////


    //
    // backupReadWriteAcquisitionState
    //
    void ReadWriteThreadLockable::backupReEntrantAcquisitionState()
    {
        *m_previousThreadLockableHolderVerifier = *m_threadLockableHolderVerifier;
		m_previousNumTimesAcquiredReadLock = m_nReaders;
    }


    //
    // restoreReEntrantAcquisitionState
    //
    void ReadWriteThreadLockable::restoreReEntrantAcquisitionState()
    {
        *m_threadLockableHolderVerifier = *m_previousThreadLockableHolderVerifier;
        m_nReaders = m_previousNumTimesAcquiredReadLock;
    }
}