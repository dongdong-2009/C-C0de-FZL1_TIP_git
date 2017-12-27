 
#ifndef  PROCESSGUARD_H_INCLUDE
#define  PROCESSGUARD_H_INCLUDE
/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $
* 
* This class acquires access to the given lock on construction, excluding
* all other process from access to code which is similarly locked. The lock
* is released on destruction.
*
*/
 
#include "IProcessLockable.h"

namespace TA_Base_Core
{
	class ProcessGuard  
	{
	public:
		
		/** 
		* Constructor
		*
		* Constructs ProcessGuard object, automatically acquiring the 
		* IProcessLockable object passed to it.
		*
		* @param &IProcessLockable - The IProcessLockable this ProcessGuard
		*                           is to acquire/release    
		* @log   &log - whether write log or not
		*/
		ProcessGuard(IProcessLockable& lockable, bool log = false );
		
		
        /** 
		* Destructor
		*
		* Automatically releases the IProcessLockable this ProcessGuard was
		* constructed with.
		*
		*/
        virtual ~ProcessGuard();
		
		
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
		ProcessGuard();
		ProcessGuard& operator=( const ProcessGuard& );
		IProcessLockable& m_lockable;    // Reference to IProcessLockable this object 
		bool m_log;

	};
}

#endif // PROCESSGUARD_H_INCLUDE
