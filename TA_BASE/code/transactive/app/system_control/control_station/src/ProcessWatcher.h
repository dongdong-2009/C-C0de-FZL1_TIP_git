/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ProcessWatcher.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This will monitor the list of running applications to see when they are terminated.
  * If an application is no longer running this will alert the ProcessManager.
  * 
  */


#if !defined(ProcessWatcher_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)
#define ProcessWatcher_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

class ProcessManager;


class ProcessWatcher : public TA_Base_Core::Thread
{
public:

    /**
    * Constructor
    */
    ProcessWatcher( ProcessManager& processManager );


    /**
    * Destructor
    */
    virtual ~ProcessWatcher();

    
    /**
    * run
    *
	* This will block until terminate() is called.
    * NOTE: do not call this method directly. It needs to be public so that it can be called from
    * the thread-spawning function
	*/
	void run();


	/**
    * terminate
    *
	* The terminate() method will cause run() to return. 
    * NOTE: probably not a good idea to call this method directly - call terminateAndWait() instead
	*/
	void terminate();


private:
   
    // Made private as they are not needed
	ProcessWatcher( const ProcessWatcher& theProcessWatcher);
	ProcessWatcher& operator=(const ProcessWatcher& pm);


    /**
    * isTerminating
    *
    * This should be called to determine if we are terminating. run() should use
    * this to determine if it should exit it's loop. This uses the m_isTerminating
    * member variable. This method should be used instead of accessing the variable
    * directly so it can be thread locked correctly.
    *
    * @return true indicates the thread should terminate
    *         false indicates the thread can keep running
    */
    bool isTerminating();


    /**
    * setIsTerminating
    *
    * This should be called by terminate to indicate that the run() method should
    * return as soon as possible. This uses the m_isTerminating
    * member variable. This method should be used instead of accessing the variable
    * directly so it can be thread locked correctly.
    *
    * @param bool - This indicates if we should terminate. true indicates the run()
    *               method should return as soon as possible.
    */
    void setIsTerminating(bool isTerminating);


    // The process manager to callback if we find an application has terminated
    ProcessManager& m_processManager;

    // true if this thread is terminating
    bool m_isTerminating;

    // protects the m_isTerminating
    TA_Base_Core::NonReEntrantThreadLockable m_terminateLock;
};

#endif // !defined(ProcessWatcher_708AAFEC_CF18_4ef6_9324_E0E24833A44E__INCLUDED_)
