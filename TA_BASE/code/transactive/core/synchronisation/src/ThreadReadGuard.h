#ifndef THREADREADGUARD_H_INCLUDED
#define THREADREADGUARD_H_INCLUDED
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/ThreadReadGuard.h $
  * @author:  Andrew Del Carlo
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

#if !defined (TA_THREADREADGUARD)
#define TA_THREADREADGUARD(threadlockable) \
	LOG(SourceInfo, \
		TA_Base_Core::DebugUtil::GenericLog, \
		TA_Base_Core::DebugUtil::DebugTrace, \
		"ThreadReadGuard: Attempting to acquire %s", \
		#threadlockable); \
	TA_Base_Core::ThreadReadGuard threadReadGuard(threadlockable, true);		
#endif

namespace TA_Base_Core
{
	class ThreadReadGuard
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
		ThreadReadGuard( IThreadLockable& lockable, bool log = false );

		
        /** 
          * Destructor
          *
          * Automatically releases the IThreadLockable this ThreadGuard was
          * constructed with.
          *
          */
        virtual ~ThreadReadGuard();


        /** 
          * acquire
          *
          * Acquire the lock.
          *
          */
		inline void acquireRead()
    	{
        	m_lockable.acquireRead();
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
        ThreadReadGuard( const ThreadReadGuard& );
        ThreadReadGuard& operator=( const ThreadReadGuard& );


		IThreadLockable& m_lockable;    // Reference to IThreadLockable this object
		bool m_log;
	};
} // namespace TA_Base_Core

#endif // THREADREADGUARD_H_INCLUDED

