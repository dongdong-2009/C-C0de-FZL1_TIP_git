// ThreadGuard.h: interface for the ThreadGuard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSGUARD_H__B9A0F016_5DB4_4602_BC97_4B91F1D47280__INCLUDED_)
#define AFX_PROCESSGUARD_H__B9A0F016_5DB4_4602_BC97_4B91F1D47280__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IThreadLockable.h"

namespace TA_Base_Bus
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
		ThreadGuard(IThreadLockable& lockable, bool log = false );
		
		
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
		ThreadGuard();
		ThreadGuard& operator=( const ThreadGuard& );
		IThreadLockable& m_lockable;    // Reference to IThreadLockable this object 
		bool m_log;

	};
}

#endif // !defined(AFX_PROCESSGUARD_H__B9A0F016_5DB4_4602_BC97_4B91F1D47280__INCLUDED_)
