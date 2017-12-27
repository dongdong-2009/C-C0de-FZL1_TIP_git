 
#ifndef NonReEntrantProcessLockable_H_Include
#define NonReEntrantProcessLockable_H_Include

/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $ 
*
* Implementation of the IProcessLockable interface that does not allow the same process
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
#include "IProcessLockable.h"

#ifdef __WIN32__
   #include "Windows.h"
#endif	

namespace TA_Base_Core
{
	//zhiqiang CL-21011 class NonReEntrantProcessLockable : IProcessLockable
	class NonReEntrantProcessLockable : public IProcessLockable
	{
	public:
		NonReEntrantProcessLockable(std::string name);
		virtual ~NonReEntrantProcessLockable();

		/** 
		* acquire
		*
		* Acquire the lock.
		*
		* PreCondition: The lock is not already acquired by the current process.
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
		NonReEntrantProcessLockable( const NonReEntrantProcessLockable& );
        NonReEntrantProcessLockable& operator=( const NonReEntrantProcessLockable& );

		volatile bool m_isLockCurrentlyAcquired;
		

        // The underlying omni_mutex this class is wrapping.
	#ifdef __WIN32__
        HANDLE m_Mutexlock;
	#endif	

	};
}

#endif // NonReEntrantProcessLockable_H_Include
