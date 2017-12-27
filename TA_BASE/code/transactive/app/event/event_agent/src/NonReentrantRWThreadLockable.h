/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/NonReentrantRWThreadLocable.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/02/06 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __NONREENTRANT_THREAD_LOCKABLE_H_INCLUDED__
#define __NONREENTRANT_THREAD_LOCKABLE_H_INCLUDED__

#include "app/event/event_agent/src/IReadWriteThreadLockable.h"
#include <boost/thread/shared_mutex.hpp>

namespace TA_Base_App
{
	typedef boost::shared_mutex T_SharedLock;

	class NonReentrantRWThreadLocable
		: public IReadWriteThreadLockable
	{
	public:
		NonReentrantRWThreadLocable(){};
		virtual ~NonReentrantRWThreadLocable(){};

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
		NonReentrantRWThreadLocable( const NonReentrantRWThreadLocable& );
        NonReentrantRWThreadLocable& operator=( const NonReentrantRWThreadLocable& );

		//Boost shared lock
		T_SharedLock m_sharedLock;
		READ_WRITE_ACCESS m_rwAccess;
	};


} //TA_Base_App

#endif //__NONREENTRANT_THREAD_LOCKABLE_H_INCLUDED__

