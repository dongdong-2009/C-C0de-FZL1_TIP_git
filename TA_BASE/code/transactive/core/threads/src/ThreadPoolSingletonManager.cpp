/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/ThreadPoolSingletonManager.cpp $
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


#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolManager.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

namespace TA_Base_Core
{
	//
	// static declaration
	//

	unsigned int								    ThreadPoolSingletonManager::m_refCounter = 0;
	ThreadPoolSingletonManager *				    ThreadPoolSingletonManager::m_singletonManager = 0;
	ReEntrantThreadLockable							ThreadPoolSingletonManager::m_lock;

	ThreadPoolSingletonManager::ThreadPoolSingletonManager()
		: m_threadPoolManager (NULL)
	{
		// create ThreadedWorker objects, based on the runtime parameter
		// "ThreadedWorkers". Use the default number if not defined
		int maxNumber = 30;
		if ( true == RunParams::getInstance().isSet ( "ThreadedWorkers" ) )
		{
			maxNumber = atoi ( RunParams::getInstance().get( "ThreadedWorkers" ).c_str() );
		}

		m_threadPoolManager = new ThreadPoolManager(maxNumber, maxNumber);

		TA_ASSERT (m_threadPoolManager != NULL, "Thread Pool Manager not created");
	}


	ThreadPoolSingletonManager::~ThreadPoolSingletonManager()
	{
		delete m_threadPoolManager;
		m_threadPoolManager = NULL;
	}

	ThreadPoolSingletonManager * ThreadPoolSingletonManager::getInstance()
	{
		// prevent multiple threads attempting to delete/create
		// simultaneously
		TA_THREADGUARD( m_lock );

		// if the singleton has not been created yet
		if ( 0 == m_singletonManager )
		{
			// create one and only one instance
			m_singletonManager = new ThreadPoolSingletonManager();
		}

		// increment reference counter and return the single instance of
		// ThreadPoolSingletonManager
		m_refCounter++;
		return m_singletonManager;
	}


	void ThreadPoolSingletonManager::removeInstance ( ThreadPoolSingletonManager * instance )
	{
		// prevent multiple threads attempting to delete/create
		// simultaneously
		TA_THREADGUARD( m_lock );

		if ( 0 != instance )
		{
			// decrement reference counter
			m_refCounter--;

			// if not referenced anywhere
			if ( 0 == m_refCounter )
			{
				// destroy the only instance
				delete m_singletonManager;
				m_singletonManager = 0;
			}
		}
		// else do nothing
	}

	void ThreadPoolSingletonManager::queueWorkItem ( IWorkItemPtr workItem )
	{
		m_threadPoolManager->queueWorkItem(workItem);
	}
	
	void ThreadPoolSingletonManager::removeWorkItem ( IWorkItemPtr workItem )
	{
		m_threadPoolManager->removeWorkItem(workItem);
	}


    unsigned long ThreadPoolSingletonManager::getOriginalSize()
    {
        return m_threadPoolManager->getOriginalSize();
    }
    
    
    unsigned long ThreadPoolSingletonManager::getNumberOfThreads()
    {
        return m_threadPoolManager->getNumberOfThreads();
    }


    unsigned long ThreadPoolSingletonManager::getUpperLimit()
    {
        return m_threadPoolManager->getUpperLimit();
    }


    void ThreadPoolSingletonManager::setUpperLimit(unsigned long upperLimit)
    {
        m_threadPoolManager->setUpperLimit(upperLimit);
    }

}
