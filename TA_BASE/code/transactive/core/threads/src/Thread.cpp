
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/Thread.cpp $
  * @author:  B Fountain
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Platform independent wrapper which allows us to manage a single thread. This
  * is an abstract class - extend it to specify run() and terminate() methods
  *
  */

#include <stdio.h>
#include <omnithread.h>

#ifdef WIN32
#include <windows.h>
#elif defined ( SOLARIS ) || defined ( LINUX )
#include <pthread.h>
#else
#error Unsupported platform!
#endif // WIN32

#include "Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"
#include "core/utilities/src/DebugSETranslator.h"

// To allow runThread() to catch CORBA::Exceptions for debugging. 
#include "core/corba/src/CorbaUtil.h"

namespace TA_Base_Core
{

    // Copy from CorbaUtil::exceptionToString to reduce library dependency
    static std::string sExceptionToString(const CORBA::Exception& ex)
    {
        // get the minor code and append. This is proprietary
        try
        {
            // Attemp to cast to a systemException
            const CORBA::SystemException* sysEx = CORBA::SystemException::_downcast( &ex );

            // Must check the the possibility of a NULL sysEx which can lead to 
            // core dumps runtime.   
            if ( NULL == sysEx )
            {
                // We will throw this to be handled by the catch (...) below.
                throw 0;
            }

            // Extract the result string
            const char* buf = sysEx->NP_minorString();
            if ( buf != NULL )
            {
                return buf;
            }
        }
        catch ( ... )
        {
            // This is not actually proprietory at all - it should work for any ORB.
            // However the string you get is pretty terse - the proprietory exception
            // message methods (which omniORB doesn't seem to have) are usually much
            // more verbosecatch (CORBA::Exception& ex)
            try
            {
                CORBA::Any any;
                any <<= ex;
                CORBA::TypeCode_var tc = any.type();
                return tc->name();
            }
            catch ( ... )
            {
                return "Unable to extract string from exception";
            }
        }

        // Should never get here
        return "Unable to extract string from exception";
    }


	// runThread()
	//
	// This is a C-style function which is called from the thread object. It
	// expects its parameter to be a pointer to a Thread object, which it
	// will run
	//
	static void* runThread(void* ptr)
	{
		TA_ASSERT(ptr != NULL, "Null pointer passed to runThread()");

		Thread* myThread = (Thread*) ptr;

        // terminateAndWait() can't set m_thread back to NULL until after this method finishes,
        // so m_thread MUST be valid here.
        //TA_Assert(myThread->m_thread != NULL);

        gInitStructuredExceptionHandler();

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "runThread(): %s about to call run()", gPtrToStr(myThread).c_str() );

        myThread->run();

        try
        {
		    //myThread->run();
        }
        catch (WinStructuredException& e)
        {
			std::ostringstream stm;
            stm << "runThread(): WinStructuredException exceptionCode == " << std::hex << e.m_exceptionCode;
			LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), stm.str().c_str() );

            DebugUtil::getInstance().logStackTrace(gContextFromExceptionInfo(e.m_exceptionInfo));

            TA_Assert(0);
        }
        catch (std::exception& e)
        {
			std::string msg( "runThread(): " );
			msg.append( e.what() );
			LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
            TA_Assert(0);
        }
        catch (CORBA::Exception& e)
        {
			std::string msg( "runThread(): " );
            // msg.append( CorbaUtil::getInstance().exceptionToString(e) );
			msg.append( sExceptionToString( e ) );
			LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
            TA_Assert(0);
        }
        catch (...)
        {
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", 
                "runThread(): Unknown exception" );
            TA_Assert(0);
        }

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "runThread(): %s finished calling run()", gPtrToStr(myThread).c_str() );

        // Sets the thread state to STATE_TERMINATED
		// TODO LPP: Not sure if this should be called.  Do we want to expose the STATE_TERMINATED state?
        //myThread->exit();

		return NULL;
    }


	Thread::Thread() : 
        m_thread((omni_thread*)NULL),
        m_priority(omni_thread::PRIORITY_NORMAL)
	{
	}

	Thread::~Thread()
	{
	    // terminateAndWait() should have been called already, otherwise m_thread will leak
		if (m_thread != NULL)
		{
    		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            	"Thread::~Thread(): %s destructor called without calling terminateAndWait first", 
                gPtrToStr(this).c_str());

            // TODO LPP: Fix clients that are not calling terminateAndWait() and reenable this assertion.
            //TA_Assert(0);
		}
	}


	void Thread::start()
	{
        // start() must not be called if the thread is already running.
        // terminateAndWait() must be called in between successive calls to start().
		if (m_thread != NULL)
		{
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Thread::start(): %s is already running.", gPtrToStr(this).c_str() );

            // TODO LPP: Fix clients that are calling start() multiple times and reenable this assertion.
            //TA_Assert(0);
    		return;
		}
	
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Thread::start(): %s about to start.", gPtrToStr(this).c_str() );

        // THREAD_STATE_NEW only occurs during the following two calls
    	m_thread = new omni_thread(TA_Base_Core::runThread, (void*) this, (omni_thread::priority_t)m_priority);
	    m_thread->start();

        LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Thread::start(): %s has been started successfully. m_thread=%s", 
            gPtrToStr(this).c_str(), gPtrToStr(m_thread).c_str() );
	}


	void Thread::terminateAndWait()
	{
        if (m_thread != NULL)
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "Thread::terminateAndWait(): %s about to call terminate()", gPtrToStr(this).c_str() );

            terminate();

            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "Thread::terminateAndWait(): %s about to wait.", gPtrToStr(this).c_str() );
            // TODO LPP: The thread executing run() must NOT be the same thread calling terminateAndWait().
			// However, this is difficult to check at the moment, as we don't keep track of the thread's id.

            // join() doesn't return until run() has completed.  It deletes the omni_thread object.
            // Note that join() doesn't modify the thread state.
            m_thread->join((void**)NULL);

            // m_thread is no longer valid.  
            // NOTE: It is impossible to make Thread completely threadsafe.  omni_thread::join()
            // deletes the m_thread object, but we can't lock access to m_thread before calling
            // join(), as this is a blocking call.  So we have to lock it after - but there is a brief
            // period of time where another method can use m_thread even though it has been deleted.
        	m_thread = (omni_thread*)NULL;

            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "Thread::terminateAndWait(): %s has stopped.", gPtrToStr(this).c_str() );
        }
    }


    void Thread::sleep(unsigned int milliSeconds)
    {
        // The omni_thread sleep() takes two parameters - seconds and nanoseconds(!),
        // but milliseconds are easier to manage and less prone to order-of-magnitude
        // cockups
        omni_thread::sleep(milliSeconds / 1000, (milliSeconds % 1000) * 1000000);
    }


    void Thread::get_time(unsigned long* abs_sec, unsigned long* abs_nsec,
			              unsigned long  rel_sec, unsigned long rel_nsec)
    {
        omni_thread::get_time(abs_sec, abs_nsec, rel_sec, rel_nsec );
    }


    void Thread::setPriority(int newPriority)
    {
        TA_ASSERT((newPriority >= 0)&&(newPriority < 3), "Invalid priority level");      

        m_priority = newPriority;
        if (m_thread != NULL)
        {
            // NOTE: omni_thread::set_priority() seems to require the thread to be running when the
            // priority is changed.
            TA_Assert(m_thread->state() == omni_thread::STATE_RUNNING);

            m_thread->set_priority((omni_thread::priority_t)m_priority);
        }
    }


    unsigned int Thread::getId() const
    {
        return (m_thread != NULL) ? m_thread->id() : static_cast< unsigned int >( -1 );
    }


	unsigned int Thread::getCurrentThreadId()
	{
#ifdef WIN32
		return ::GetCurrentThreadId();
#elif defined ( SOLARIS ) || defined ( LINUX )
		return pthread_self();
#else
#error Unsupported Platform!
#endif // WIN32
	}


    Thread::ThreadState_t Thread::getCurrentState() const
    {
        ThreadState_t lvCurrentState = THREAD_STATE_UNKNOWN;
        if( 0 != m_thread )
        {
            // NOTE: Can't use standard C++ cast's to make this work.
            lvCurrentState = ( TA_Base_Core::Thread::ThreadState_t )m_thread->state();
        }
        return lvCurrentState;
    }
}



