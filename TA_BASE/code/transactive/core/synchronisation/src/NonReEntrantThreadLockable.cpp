/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/NonReEntrantThreadLockable.cpp $
* @author:  Robert Young
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* Implementation of the IThreadLockable interface that does not allow the same thread
* to acquire the lock more than once (i.e. a thread cannot acquire lock again without 
* first releasing).
*
* This lock should not be acquired directly, but managed using a temporary object (i.e. 
* ThreadGuard) which ensures the lock is acquired during construction, and subsequently 
* released during destruction.
*
* Note, this class wrappers the omni_mutex class, which in turn wrappers the OS-specific 
* threading API. 
*
*/

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadLockableHolderVerifier.h"
#include "core/utilities/src/TAAssert.h"

#include <omnithread.h>

namespace TA_Base_Core
{
    //
    // Constructor
    //
    NonReEntrantThreadLockable::NonReEntrantThreadLockable() 
        : m_isLockCurrentlyAcquired( false ),
          m_lock( new omni_mutex ),
          m_threadLockableHolderVerifier( new ThreadIdWrapper )
    {
    }


    //
    // Destructor
    //
    NonReEntrantThreadLockable::~NonReEntrantThreadLockable()
    {
        delete m_lock;
        delete m_threadLockableHolderVerifier;
    }


    //
    // acquire
    //
    void NonReEntrantThreadLockable::acquire()
    {
        // (NonReEntrant) If the thread has currently acquired this lock already, client code should not be calling acquire
        // before first releasing.
        TA_ASSERT( false == ( m_isLockCurrentlyAcquired && m_threadLockableHolderVerifier->isCurrentThreadId() ) ,
                   "Thread attempted to acquire NonReEntrantThreadLockable on consecutive occasions without first releasing." );

        m_lock->acquire();

        // Thread has acquired lock so set as current lock holder and set flag indicating lock is acquired.
        m_threadLockableHolderVerifier->setCurrentThreadId();
        m_isLockCurrentlyAcquired = true;
    }


    //
    // release
    //
    void NonReEntrantThreadLockable::release()
    {
        TA_ASSERT( m_isLockCurrentlyAcquired, "Call made to release while lock not currently acquired." );

        // Reset flag indicating lock is acquired.
        m_isLockCurrentlyAcquired = false;

        m_lock->release();
    }
}
