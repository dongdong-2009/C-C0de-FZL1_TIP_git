/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/ThreadPoolSingletonManager.h $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * ThreadPoolSingletonManager provides a single interface for scheduling of asynchronous operations,
  * which are represented by instances of IWorkItem interface.  For the current design, ThreadPoolSingletonManager
  * creates and manages ten (10) threaded workers to execute the registered asynchronous operations.
  *
  */

#ifndef THREAD_POOL_SINGLETON_MANAGER_H
#define THREAD_POOL_SINGLETON_MANAGER_H

#include <vector>

#include "core/threads/src/IThreadPoolWorkerCallback.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/threads/src/IWorkItem.h"

namespace TA_Base_Core
{
	class ThreadPoolManager;

	class ThreadPoolSingletonManager	
	{
	public:

		/**
		*
		* getInstance
		*
		* Get an instance of ThreadPoolSingletonManager.  Caller is responsible for destroying
		* the instance by calling removeInstance(), passing in the instance created
		* by this method.
		*
		*/
		static ThreadPoolSingletonManager * getInstance();


		/**
		*
		* removeInstance
		*
		* Remove an instance of ThreadPoolSingletonManager that has been created by the getInstance()
		*
		* @param	instance	The instance of the ThreadPoolSingletonManager to be destroyed
		*
		*/
		static void removeInstance ( ThreadPoolSingletonManager * instance );


		//
		// Operations specific to ThreadPoolSingletonManager
		//


		/**
		*
		* queueWorkItem
		*
		* Queue the specified IWorkItem for processing by a thread in the thread pool
		* and return immediately
		*
		* @param	workItem	The work item to be processed by a thread
		*
		*/
		void queueWorkItem ( IWorkItemPtr workItem );


		/**
		*
		* removeWorkItem
		*
		* Remove the specified IWorkItem so that it is no longer processed by a thread
		* in the thread pool.  This will block until the specified IWorkItem is successfully
		* removed from the ThreadPool.
		*
		* @param	workItem	The work item to be processed by a thread
		*
		*/
		void removeWorkItem ( IWorkItemPtr workItem );


        /** 
          * getOriginalSize
          *
          * @return the original pool size
          */
        unsigned long getOriginalSize();
        
        /** 
          * getNumberOfThreads
          *
          * @return the current thread pool size
          */
        unsigned long getNumberOfThreads();


        /** 
          * getUpperLimit
          *
          * @return the maximum size the pool can be
          */
        unsigned long getUpperLimit();


        /** 
          * setUpperLimit
          *
          * @return the maximum size the pool can be
          */
        void setUpperLimit(unsigned long upperLimit);

	private:

		virtual ~ThreadPoolSingletonManager();
		ThreadPoolSingletonManager();
		ThreadPoolSingletonManager ( const ThreadPoolSingletonManager & theInstance );
		ThreadPoolSingletonManager & operator= ( const ThreadPoolSingletonManager & rhs );

	protected:

		static ThreadPoolSingletonManager *					m_singletonManager;
		static ReEntrantThreadLockable						m_lock;
		static unsigned int									m_refCounter;

		ThreadPoolManager*									m_threadPoolManager;
	};
};

#endif
