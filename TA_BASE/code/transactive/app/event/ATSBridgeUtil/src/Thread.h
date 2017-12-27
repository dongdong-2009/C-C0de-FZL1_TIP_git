#pragma once
namespace TA_Base_Core
{
	class Thread
	{
	public:
		Thread();

		// terminateAndWait() MUST have been called, and the thread stopped, before 
		// the destructor is called.
		virtual ~Thread();

		// start() spawns a new thread which then executes run(), which is implemented
		// by a derived class.  The thread will finish when run() exits, but start() 
		// must not be called again until terminateAndWait() has been called.
		void start();

		// Calls terminate(), which is implemented by a derived class, then blocks until run()
		// has exited.  Can be called even if the thread isn't already running.
		void terminateAndWait();

		// Specify the priority of this Thread object, one of
		// 0=LOW_PRIORITY, 1=NORMAL (default), 2=HIGH
		void setPriority(int newPriority);

		// Returns the thread id of this Thread object. NOTE that this id can NOT be
		// compared to the id returned by getCurrentThreadId().  nothrow.
		unsigned int getId() const;

		// Returns the execution state of this Thread object.  nothrow.
		// - THREAD_STATE_RUNNING indicates the thread is running.
		// - THREAD_STATE_UNKNOWN indicates the thread is not running and can be started again.
		// - THREAD_STATE_TERMINATED is not used.
		// - THREAD_STATE_NEW occurs briefly during start()
		enum ThreadState_t
		{
			THREAD_STATE_NEW ,               
			THREAD_STATE_RUNNING ,     
			THREAD_STATE_TERMINATED , 
			THREAD_STATE_UNKNOWN                                     
		};
		ThreadState_t getCurrentState() const;

		// STATIC METHODS WHICH OPERATE ON THE CALLING THREAD

		// Use this method to sleep, rather than the platform-specific method
		// Sleep for *at least* the given number of milliseconds (depending on load
		// it could actually be longer of course)
		static void sleep(unsigned int milliseconds);

		// calculates an absolute time in seconds and nanoseconds, suitable for
		// use in timed_waits on condition variables, which is the current time
		// plus the given relative offset.
		static void get_time(unsigned long* abs_sec, unsigned long* abs_nsec,
			unsigned long rel_sec = 0, unsigned long rel_nsec=0);

		// Returns the thread id of the calling thread.  nothrow.
		static unsigned int getCurrentThreadId();

		// TO BE IMPLEMENTED BY DERIVED CLASSES:

		// run() is called by the new thread that is spawned when start() is called.  
		// NOTE: dont call run() directly, because it would then be called by the
		// calling thread rather than a newly-spawned thread.
		virtual void run() = 0;


		void intervalRun();

		// terminate() is called by terminateAndWait(), but it can also be called
		// directly (be sure the thread isn't running before starting it again though).  
		virtual void terminate() = 0;

		// An implementation of run() will usually contain a loop that continues to execute 
		// until some condition has been met (such as a boolean value being set to indicate 
		// that the thread should terminate).  A synchronisation object will be used to put 
		// the thread to sleep when there is no work to do.  For example:
		//  while (m_running)
		//  {
		//      m_semaphore.wait();
		//  }

		// terminate() should cause run() to exit, typically by signalling a synchronisation 
		// object and setting a boolean flag.
		// For example:
		//  {
		//      m_running = false;
		//      m_semaphore.post();
		//  }

	private:
		// Undetached omni_thread - terminateAndWait() uses omni_thread::join(), which can only
		// be called on undetached threads.  It is necessesary to call join() on the omni_thread
		// object in order to reclaim it's storage (join() calls delete this; on itself), which
		// is why terminateAndWait() HAS to be called after start() has been called.  
		 
		unsigned long m_hHandle;
		unsigned int  m_priority;
		ThreadState_t m_status;
	};
};
