/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_PreSiteRelease/CODE_TA_BASE_4669_T01271300/transactive/core/threads/src/NamingThreadPoolSingleton.h $
  * @author:	HoaVu
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2008/10/20 14:46:29 $
  * Last modified by : $Author: avteam $
  *
  *
  * NamingThreadPoolSingleton provides a single interface for scheduling of asynchronous operations,
  * which are represented by instances of IWorkItem interface.  For the current design, NamingThreadPoolSingleton
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

	class NamingThreadPoolSingleton	
	{
	public:

		/**
		*
		* getInstance
		*
		* Get an instance of NamingThreadPoolSingleton.  Caller is responsible for destroying
		* the instance by calling removeInstance(), passing in the instance created
		* by this method.
		*
		*/
		static NamingThreadPoolSingleton* getInstance();


		/**
		*
		* removeInstance
		*
		* Remove an instance of NamingThreadPoolSingleton that has been created by the getInstance()
		*
		* @param	instance	The instance of the NamingThreadPoolSingleton to be destroyed
		*
		*/
		static void removeInstance( NamingThreadPoolSingleton* instance );


		//
		// Operations specific to NamingThreadPoolSingleton
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


        void cleanUp();

	private:

		virtual ~NamingThreadPoolSingleton();
		NamingThreadPoolSingleton();
		NamingThreadPoolSingleton ( const NamingThreadPoolSingleton & theInstance );
		NamingThreadPoolSingleton & operator= ( const NamingThreadPoolSingleton & rhs );

	protected:

		static NamingThreadPoolSingleton *					m_singletonManager;
		static ReEntrantThreadLockable						m_lock;
		static unsigned int									m_refCounter;

		ThreadPoolManager*									m_threadPoolManager;
	};
};

#endif
