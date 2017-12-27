 
#ifndef NonReEntrantProcessRWLockable_H_Include
#define NonReEntrantProcessRWLockable_H_Include

/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $ 
*
* Implementation of the IProcessRWLockable interface that does not allow the same process
* to acquire the lock more than once (i.e. a process cannot acquire lock again without 
* first releasing).
*
* This lock should not be acquired directly, but managed using a temporary object (i.e. 
* ProcessGuard) which ensures the lock is acquired during construction, and subsequently 
* released during destruction.
*
* Note, this class only wrappers the win32-specific API,currently.
*       we will provide OS independent implementation in the future.
*
*/

#include <string>
#include "IProcessRWLockable.h"

//#include "ace\RW_Process_Mutex.h"

#ifdef __WIN32__
   #include "Windows.h"
#endif	

namespace TA_Base_Core
{
	//zhiqiang CL-21011 class NonReEntrantProcessRWLockable : IProcessRWLockable
	class NonReEntrantProcessRWLockable : public IProcessRWLockable
	{
	public:
		NonReEntrantProcessRWLockable(std::string name);
		virtual ~NonReEntrantProcessRWLockable();

		/** 
		* acquire
		*
		* Acquire the lock.
		*
		* PreCondition: The lock is not already acquired by the current process.
		*/
        virtual void acquire(READ_WRITE_ACCESS rwAccess = WRITE_ACCESS);
        
        virtual bool acquire(int MillisecondTimeout, READ_WRITE_ACCESS rwAccess = WRITE_ACCESS);
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
		NonReEntrantProcessRWLockable( const NonReEntrantProcessRWLockable& );
        NonReEntrantProcessRWLockable& operator=( const NonReEntrantProcessRWLockable& );

		volatile bool m_isLockCurrentlyAcquired;
		

        // The underlying omni_mutex this class is wrapping.
	#ifdef __WIN32__
		//ACE_RW_Process_Mutex* m_aceRW_ProcLock;
		HANDLE m_hNamedMutex;
		READ_WRITE_ACCESS m_rwAccess;
	#endif	
	};
}

#endif // NonReEntrantProcessRWLockable_H_Include
