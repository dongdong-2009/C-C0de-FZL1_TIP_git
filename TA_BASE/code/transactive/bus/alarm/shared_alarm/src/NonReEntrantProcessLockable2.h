 
#ifndef NonReEntrantProcessLockable2_H_Include
#define NonReEntrantProcessLockable2_H_Include

/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $ 
*
* Implementation of the IProcessLockable2 interface that does not allow the same process
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
#include "IProcessLockable2.h"

#ifdef __WIN32__
   #include "Windows.h"
#endif	

namespace TA_Base_Core
{
	//zhiqiang CL-21011 class NonReEntrantProcessLockable2 : IProcessLockable2
	class NonReEntrantProcessLockable2 : public IProcessLockable2
	{
	public:
		NonReEntrantProcessLockable2(std::string name);
		virtual ~NonReEntrantProcessLockable2();

		/** 
		* acquire
		*
		* Acquire the lock.
		*
		* PreCondition: The lock is not already acquired by the current process.
		*/
        virtual void acquire();
        
        virtual bool acquire(int MillisecondTimeout);
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
		NonReEntrantProcessLockable2( const NonReEntrantProcessLockable2& );
        NonReEntrantProcessLockable2& operator=( const NonReEntrantProcessLockable2& );

		volatile bool m_isLockCurrentlyAcquired;
		

        // The underlying omni_mutex this class is wrapping.
	#ifdef __WIN32__
        HANDLE m_Mutexlock;
	#endif	

	};
}

#endif // NonReEntrantProcessLockable2_H_Include
