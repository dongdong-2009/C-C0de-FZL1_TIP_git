/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_PreSiteRelease/CODE_TA_BASE_4669_T01271300/transactive/core/threads/src/NamingThreadPoolSingleton.cpp $
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


#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolManager.h"

#include "core/naming/src/NamingThreadPoolSingleton.h"

namespace TA_Base_Core
{
    //
    // static declaration
    //

    unsigned int                                    NamingThreadPoolSingleton::m_refCounter = 0;
    NamingThreadPoolSingleton *                     NamingThreadPoolSingleton::m_singletonManager = 0;
    ReEntrantThreadLockable                         NamingThreadPoolSingleton::m_lock;

    NamingThreadPoolSingleton::NamingThreadPoolSingleton()
        : m_threadPoolManager (NULL)
    {
        // create ThreadedWorker objects, based on the runtime parameter
        // "ThreadedWorkers". Use the default number if not defined
        int maxNumber = 40;
        int initialNumber = 10;

        if ( true == RunParams::getInstance().isSet ( "ThreadedWorkers" ) )
        {
            maxNumber = atoi ( RunParams::getInstance().get( "ThreadedWorkers" ).c_str() );
        }

        if ( true == RunParams::getInstance().isSet ( "InitialThreadedWorkers" ) )
        {
            initialNumber = atoi ( RunParams::getInstance().get( "InitialThreadedWorkers" ).c_str() );
        }

        m_threadPoolManager = new ThreadPoolManager(initialNumber, maxNumber);

        TA_ASSERT (m_threadPoolManager != NULL, "Thread Pool Manager not created");

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "NamingThreadPoolSingleton()" );
    }


    NamingThreadPoolSingleton::~NamingThreadPoolSingleton()
    {
        delete m_threadPoolManager;
        m_threadPoolManager = NULL;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~NamingThreadPoolSingleton()" );
    }


    NamingThreadPoolSingleton * NamingThreadPoolSingleton::getInstance()
    {
        // prevent multiple threads attempting to delete/create
        // simultaneously
        TA_THREADGUARD( m_lock );

        // if the singleton has not been created yet
        if ( 0 == m_singletonManager )
        {
            // create one and only one instance
            m_singletonManager = new NamingThreadPoolSingleton();
        }

        // increment reference counter and return the single instance of
        // NamingThreadPoolSingleton
        m_refCounter++;
        return m_singletonManager;
    }


    void NamingThreadPoolSingleton::removeInstance ( NamingThreadPoolSingleton* instance )
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


    void NamingThreadPoolSingleton::queueWorkItem ( IWorkItemPtr workItem )
    {
        m_threadPoolManager->queueWorkItem(workItem);
    }
    

    void NamingThreadPoolSingleton::removeWorkItem ( IWorkItemPtr workItem )
    {
        m_threadPoolManager->removeWorkItem(workItem);
    }


    unsigned long NamingThreadPoolSingleton::getOriginalSize()
    {
        return m_threadPoolManager->getOriginalSize();
    }
    
    
    unsigned long NamingThreadPoolSingleton::getNumberOfThreads()
    {
        return m_threadPoolManager->getNumberOfThreads();
    }


    unsigned long NamingThreadPoolSingleton::getUpperLimit()
    {
        return m_threadPoolManager->getUpperLimit();
    }


    void NamingThreadPoolSingleton::setUpperLimit(unsigned long upperLimit)
    {
        m_threadPoolManager->setUpperLimit(upperLimit);
    }


    void NamingThreadPoolSingleton::cleanUp()
    {
        m_threadPoolManager->cleanUp();
    }
}
