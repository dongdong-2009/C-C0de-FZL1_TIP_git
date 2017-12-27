#ifndef THREADLOCKABLEHOLDERVERIFIER_H_INCLUDED
#define THREADLOCKABLEHOLDERVERIFIER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/ThreadLockableHolderVerifier.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Platform-independant helper used to maintain the identity of a thread, and to 
  * determine whether the currently executing thread has the same id.
  *
  */

#if defined ( SOLARIS ) || defined ( LINUX )
#include <pthread.h>
#else
#include <windows.h>
#endif  // defined ( SOLARIS ) || defined ( LINUX )

namespace TA_Base_Core
{
	class ThreadIdWrapper
	{
    public: 
        ThreadIdWrapper() :
            m_threadId(0)
        {
        }

        /** 
          * setCurrentThreadId
          *
          * Set the id to the current thread id.
          *
          */
		void setCurrentThreadId()
		{
			#if defined ( SOLARIS ) || defined ( LINUX )
			m_threadId = pthread_self();
			#else
			m_threadId = GetCurrentThreadId();
			#endif  // defined ( SOLARIS ) || defined ( LINUX )
			return;
		}


        /** 
          * isCurrentThreadId
          *
          * returns true if the id is the same as the current thread's id.
          *
          *
          * @return bool
          */
		bool isCurrentThreadId()
		{
			#if defined ( SOLARIS ) || defined ( LINUX )
			return ( 0 != pthread_equal( pthread_self(), m_threadId ) ); 
			#else
			return ( GetCurrentThreadId() == m_threadId );
			#endif  // defined ( SOLARIS ) || defined ( LINUX )
			return false;
		}
        

        unsigned int getCurrentThreadId()
        {
            return (unsigned int) m_threadId;
        }

    private:
		#if defined ( SOLARIS ) || defined ( LINUX )
		pthread_t m_threadId;
		#else
		DWORD m_threadId; 
		#endif  // defined ( SOLARIS ) || defined ( LINUX )
	};
}

#endif // THREADLOCKABLEHOLDERVERIFIER_H_INCLUDED
