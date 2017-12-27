/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobThread.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * Runs an instance of a job.
  *
  */


#ifndef JOB_THREAD_H
#define JOB_THREAD_H


#include "core/threads/src/Thread.h"


namespace TA_Base_App
{

// Forward declaration
class IJobInstance;

class JobThread:  public TA_Base_Core::Thread
{

public:

    /**
     *  Constructor
     */
    JobThread(const IJobInstance* jobInstance);

    /**
     *  Destructor.
     */
	virtual ~JobThread();

    void markAsDeleted( const bool bDeleted = true );

    bool isMarkDeleted();

private:

    /**
     *  Called by the start method.
     */
    void run();

    /**
     *  This method does nothing, as the run method will only return
     *  when IJobInstance's execute method returns.
     */
    void terminate();

    JobThread();
    JobThread(const JobThread&);
    const JobThread& operator=(const JobThread&);

    IJobInstance* m_jobInstance;
    bool m_bDeleted;
};


}; // TA_Base_App

#endif // JOB_THREAD_H
