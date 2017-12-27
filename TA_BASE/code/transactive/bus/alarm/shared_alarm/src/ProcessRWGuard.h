 
#ifndef  ProcessRWGuard_H_INCLUDE
#define  ProcessRWGuard_H_INCLUDE

/** 
*
* @author:  Noel R. Tajanlangit
* @version: $Revision: #1 $
* 
* This class acquires access to the given lock on construction, excluding
* all other process from access to code which is similarly locked. The lock
* is released on destruction.
*
*/

#include "IProcessRWLockable.h"

namespace TA_Base_Core
{
	class ProcessRWGuard
	{
	public:
		
		/** 
		* Constructor
		*
		* Constructs ProcessRWGuard object, automatically acquiring the 
		* IProcessRWLockable object passed to it.
		*
		* @param &IProcessRWLockable - The IProcessRWLockable this ProcessRWGuard
		*                           is to acquire/release    
		* @log   &log - whether write log or not
		*/
		ProcessRWGuard(IProcessRWLockable& lockable, READ_WRITE_ACCESS rwAccess = WRITE_ACCESS, bool log = false );


		/** 
		* Constructor
		*
		* Constructs ProcessRWGuard object, automatically acquiring the 
		* IProcessRWLockable object passed to it.
		*
		* @param &IProcessRWLockable - The IProcessRWLockable this ProcessRWGuard
		*                           is to acquire/release    
		* @log   &log - whether write log or not
		*/
		ProcessRWGuard(IProcessRWLockable& lockable, bool log = false  );
		
		
        /** 
		* Destructor
		*
		* Automatically releases the IProcessRWLockable this ProcessRWGuard was
		* constructed with.
		*
		*/
        virtual ~ProcessRWGuard();
		
		
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

		bool acquire(int MillisecondTimout, READ_WRITE_ACCESS rwAccess = WRITE_ACCESS);

 		
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
		ProcessRWGuard();
		ProcessRWGuard& operator=( const ProcessRWGuard& );
		IProcessRWLockable& m_lockable;    // Reference to IProcessRWLockable this object 
		bool m_log;
		bool m_acquired;
	};
}

#endif // ProcessRWGuard_H_INCLUDE
