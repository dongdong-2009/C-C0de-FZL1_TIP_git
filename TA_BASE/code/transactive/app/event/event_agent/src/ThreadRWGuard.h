/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/ThreadRWGuard.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/02/06 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __THREAD_RW_GUARD_H_INCLUDED__
#define __THREAD_RW_GUARD_H_INCLUDED__

#include "app/event/event_agent/src/IReadWriteThreadLockable.h"

namespace TA_Base_App
{
	class ThreadRWGuard
	{
	public:
		
		/** 
		* Constructor
		*
		* Constructs ThreadRWGuard object, automatically acquiring the 
		* IReadWriteThreadLockable object passed to it.
		*
		* @param &IReadWriteThreadLockable - The IReadWriteThreadLockable this ThreadRWGuard
		*                           is to acquire/release    
		* @log   &log - whether write log or not
		*/
		ThreadRWGuard(IReadWriteThreadLockable& lockable, READ_WRITE_ACCESS rwAccess = WRITE_ACCESS, bool log = false );


		/** 
		* Constructor
		*
		* Constructs ThreadRWGuard object, automatically acquiring the 
		* IProcessRWLockable object passed to it.
		*
		* @param &IProcessRWLockable - The IProcessRWLockable this ThreadRWGuard
		*                           is to acquire/release    
		* @log   &log - whether write log or not
		*/
		ThreadRWGuard(IReadWriteThreadLockable& lockable, bool log = false  );
		
		
        /** 
		* Destructor
		*
		* Automatically releases the IReadWriteThreadLockable this ThreadRWGuard was
		* constructed with.
		*
		*/
        virtual ~ThreadRWGuard();
		
		
        /** 
		* acquire
		*
		* Acquire the lock.
		*
		*/
		inline void acquire(READ_WRITE_ACCESS rwAccess = WRITE_ACCESS)
		{
			m_lockable.acquire(rwAccess);
			m_acquired=true;
			
 		}

		bool acquire(int MillisecondTimeout, READ_WRITE_ACCESS rwAccess = WRITE_ACCESS);

 		
        /** 
		* release
		*
		* Release the lock.
		* 
		*/
		inline void release()
		{
			if (m_acquired)
			{
				m_lockable.release();
				m_acquired=false;
			}
    	}
	private:
		//
        // Disable the default copy and assignment constructors as shouldn't used.
        //
		ThreadRWGuard();
		ThreadRWGuard& operator=( const ThreadRWGuard& );
		IReadWriteThreadLockable& m_lockable;    // Reference to IProcessRWLockable this object 
		bool m_log;
		bool m_acquired;
	};

} //namespace TA_Base_App

#endif //__THREAD_RW_GUARD_H_INCLUDED__