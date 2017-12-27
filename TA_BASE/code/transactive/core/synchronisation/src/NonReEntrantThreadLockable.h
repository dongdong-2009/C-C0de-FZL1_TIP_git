#ifndef NONREENTRANTTHREADLOCKABLE_H_INCLUDED
#define NONREENTRANTTHREADLOCKABLE_H_INCLUDED

/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/NonReEntrantThreadLockable.h $
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

#include "IThreadLockable.h"

// Forward declarations
class omni_mutex;
namespace TA_Base_Core
{
    // Forward declared struct so as not to bring in any dependencies here that are
    // needed to identify the current thread on various platforms.
    class ThreadIdWrapper;
}

namespace TA_Base_Core
{
    class NonReEntrantThreadLockable : public IThreadLockable
    {
    public:
        
        /** 
          * Constructor
          *
          */
        NonReEntrantThreadLockable();
        
        
        /** 
          * Destructor
          *
          */
        virtual ~NonReEntrantThreadLockable();
        
        
        /** 
          * acquire
          *
          * Acquire the lock.
          *
          * PreCondition: The lock is not already acquired by the current thread.
          */
        virtual void acquire();
        
        
        /** 
          * release
          *
          * Release the lock.
          *
          * PreCondition: The lock is currently acquired.  
          */
        virtual void release();
        
    private:

        //
        // Disable the default copy and assignment constructors as shouldn't used.
        //
        NonReEntrantThreadLockable( const NonReEntrantThreadLockable& );
        NonReEntrantThreadLockable& operator=( const NonReEntrantThreadLockable& );

        // Indicates when some thread has currently aquired this lock.
        volatile bool m_isLockCurrentlyAcquired;

        // The underlying omni_mutex this class is wrapping.
        omni_mutex* m_lock;
        
        // Struct used to store the identifier of the thread that has currently aquired
        // the lock and inform client code whether its thread is a match.
        ThreadIdWrapper* m_threadLockableHolderVerifier;
    };
}

#endif // NONREENTRANTTHREADLOCKABLE_H_INCLUDED

