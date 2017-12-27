/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/WriteReadThreadLockable.cpp $
  * @author:  Andrew Del Carlo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
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

#include "core/synchronisation/src/WriteReadThreadLockable.h"
#include "core/synchronisation/src/ThreadLockableHolderVerifier.h"
#include "core/utilities/src/TAAssert.h"

#include <omnithread.h>

namespace TA_Base_Core
{

#if 0

    //
    // Default Constructor
    //
    WriteReadThreadLockable::WriteReadThreadLockable() 
        : m_writeLockPending( new omni_mutex )
    {
		
    }


    //
    // Destructor
    //
    WriteReadThreadLockable::~WriteReadThreadLockable()
    {
		delete m_writeLockPending;
    }


    //
    // acquire
    //
    void WriteReadThreadLockable::acquireRead()
    {
        //wait till its safe to start reading
		
		//writer preference
		m_writeLockPending->acquire();
		m_writeLockPending->release();
		
		ReadWriteThreadLockable::acquireRead();
   }


    //
    // acquire
    //
    void WriteReadThreadLockable::acquire()
    {
		//writers need preference, achieved by blocking new readers
		m_writeLockPending->acquire();

		ReadWriteThreadLockable::acquire();
        
		m_writeLockPending->release();
    }

#endif

}