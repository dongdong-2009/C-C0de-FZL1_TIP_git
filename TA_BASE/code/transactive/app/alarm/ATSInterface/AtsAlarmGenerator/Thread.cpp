#include "stdafx.h"
#include "Thread.h"
#include "process.h"
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>

namespace TA_Base_Core
{
	unsigned int __stdcall threadFunc(void* pParam )
	{
		Thread* pThis = static_cast<Thread*>(pParam);
		pThis->intervalRun();
		
		return 1;
	} 

	void Thread::intervalRun()
	{
		try
		{
			run();
		}		 
		catch (...)
		{
			MessageBox(NULL, "Exception occured in the Thread::run function", "Error", MB_OK|MB_ICONERROR);
		}
		
		m_status=Thread::THREAD_STATE_TERMINATED;

	}

	Thread::Thread(void):
	m_priority(0),m_hHandle(0),m_status(Thread::THREAD_STATE_UNKNOWN)
	{

	}

	Thread::~Thread(void)
	{

	}

	void Thread::start()
	{
	   if (m_hHandle!=0)
		{
		   return;
		}
		
		try
		{
			unsigned long hHandle = _beginthreadex
				(NULL,
				0,
				threadFunc, 
				(void*)this,
				0, 
				NULL);
			m_hHandle=hHandle;
			m_status=Thread::THREAD_STATE_RUNNING;
		}
		catch (...)
		{
			//
		}
	}


	void Thread::terminateAndWait()
	{
		if (m_hHandle != 0)
		{
			 
			terminate();	 
			//m_thread->join((void**)NULL);
			m_hHandle=0;	 
		}
	}


	void Thread::sleep(unsigned int milliSeconds)
	{
		 // The omni_thread sleep() takes two parameters - seconds and nanoseconds(!),
		 // but milliseconds are easier to manage and less prone to order-of-magnitude
		 // cockups
		::Sleep(milliSeconds);
	}


	void Thread::get_time(unsigned long* abs_sec, unsigned long* abs_nsec,
						  unsigned long  rel_sec, unsigned long rel_nsec)
	{
		 
	}


	void Thread::setPriority(int newPriority)
	{ 
		m_priority = newPriority;	 
	}


	unsigned int Thread::getId() const
	{
		 return m_hHandle;
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
		//ThreadState_t lvCurrentState = THREAD_STATE_UNKNOWN;
		//return lvCurrentState;
		return m_status;
	}
}
