#ifndef THREADGUARD_H_INCLUDED
#define THREADGUARD_H_INCLUDED
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/ThreadGuard.h $
  * @author:  B Fountain
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class acquires access to the given lock on construction, excluding
  * all other threads from access to code which is similarly locked. The lock
  * is released on destruction.
  * 
  * To make methods synchronised (ala Java), inherit from IThreadLockable implementation 
  * and instantiate a local variable of class ThreadGuard at the top of each method
  * that you want to synchronise, using (*this) to initialise the lock.
  *
  */

#include "core/synchronisation/src/IThreadLockable.h"

#if !defined (TA_THREADGUARD)

#define TA_NAMED_THREADGUARD(threadlockable, guardName) \
	LOG(SourceInfo, \
		TA_Base_Core::DebugUtil::GenericLog, \
		TA_Base_Core::DebugUtil::DebugTrace, \
		"ThreadGuard: Attempting to acquire guard %s for lock %s", \
		#guardName , #threadlockable ); \
        TA_Base_Core::ThreadGuard guardName(threadlockable, true);

#define TA_THREADGUARD_NAME(prefix, suffix) prefix ## suffix

#define TA_THREADGUARD(threadlockable) TA_NAMED_THREADGUARD(threadlockable, TA_THREADGUARD_NAME(threadlockable, ThreadGuard))

#endif

namespace TA_Base_Core
{
	class ThreadGuard
	{
	public:

        /** 
          * Constructor
          *
          * Constructs ThreadGuard object, automatically acquiring the 
          * IThreadLockable object passed to it.
          *
          * @param &IThreadLockable - The IThreadLockable this ThreadGuard
          *                           is to acquire/release    
          */
		ThreadGuard( IThreadLockable& lockable, bool log = false );

		
        /** 
          * Destructor
          *
          * Automatically releases the IThreadLockable this ThreadGuard was
          * constructed with.
          *
          */
        virtual ~ThreadGuard();


        /** 
          * acquire
          *
          * Acquire the lock.
          *
          */
		inline void acquire()
    	{
        	m_lockable.acquire();
    	}

        /** 
          * release
          *
          * Release the lock.
          * 
          */
		inline void release()
    	{
        	m_lockable.release();
    	}

	private:

        //
        // Disable the default copy and assignment constructors as shouldn't used.
        //
        ThreadGuard( const ThreadGuard& );
        ThreadGuard& operator=( const ThreadGuard& );


		IThreadLockable& m_lockable;    // Reference to IThreadLockable this object 
		bool m_log;
	};
} // namespace TA_Base_Core

#endif // THREADGUARD_H_INCLUDED

