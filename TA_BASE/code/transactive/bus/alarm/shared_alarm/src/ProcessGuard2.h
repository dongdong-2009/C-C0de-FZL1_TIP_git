 
#ifndef  ProcessGuard2_H_INCLUDE
#define  ProcessGuard2_H_INCLUDE
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
 
#include "IProcessLockable2.h"

namespace TA_Base_Core
{
	class ProcessGuard2  
	{
	public:
		
		/** 
		* Constructor
		*
		* Constructs ProcessGuard2 object, automatically acquiring the 
		* IProcessLockable2 object passed to it.
		*
		* @param &IProcessLockable2 - The IProcessLockable2 this ProcessGuard2
		*                           is to acquire/release    
		* @log   &log - whether write log or not
		*/
		ProcessGuard2(IProcessLockable2& lockable, bool log = false );


		/** 
		* Constructor
		*
		* Constructs ProcessGuard2 object, automatically acquiring the 
		* IProcessLockable2 object passed to it.
		*
		* @param &IProcessLockable2 - The IProcessLockable2 this ProcessGuard2
		*                           is to acquire/release    
		* @log   &log - whether write log or not
		*/
		ProcessGuard2(IProcessLockable2& lockable, int enableTimeout, bool log = false  );
		
		
        /** 
		* Destructor
		*
		* Automatically releases the IProcessLockable2 this ProcessGuard2 was
		* constructed with.
		*
		*/
        virtual ~ProcessGuard2();
		
		
        /** 
		* acquire
		*
		* Acquire the lock.
		*
		*/
		inline void acquire()
		{
			m_lockable.acquire();
			m_acquired=true;
		}


		inline bool acquire(int MillisecondTimout)
		{
			bool r=m_lockable.acquire(MillisecondTimout);
			m_acquired=true;
			return r;
		}
		
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
		ProcessGuard2();
		ProcessGuard2& operator=( const ProcessGuard2& );
		IProcessLockable2& m_lockable;    // Reference to IProcessLockable2 this object 
		bool m_log;
		bool m_acquired;

	};
}

#endif // ProcessGuard2_H_INCLUDE
