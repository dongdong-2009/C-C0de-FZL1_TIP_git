/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/IJobExecutionListener.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This interface should be implemented by any class that needs to receive
  * events about job instances starting and completing.
  *
  */


#ifndef JOB_EXECUTION_LISTENER_H
#define JOB_EXECUTION_LISTENER_H

#include <string>

namespace TA_Base_App
{


class IJobExecutionListener
{

public:
    /**
     *  Called when a job is about to start.
     *
     *  @param jobID          the ID of the job.
     *  @param minuteStarted  the minute at which the job was started, relative to the epoch.
     */
    virtual void startingJob( const std::string& jobID, unsigned long minuteStarted )   = 0;


    /**
     *  Called immediately after a job completes.
     *  If there was an error, the errorMessage parameter is not "".
     *
     *  @param jobID          the ID of the job.
     *  @param minuteStarted  the minute at which the job was started, relative to the epoch.
     */
    virtual void jobCompleted( const std::string& jobID, unsigned long minuteStarted )  = 0;


    /**
     *  Called immediately after a job instance is deleted.
     *  If there was an error, the errorMessage parameter is not "".
     *
     *  @param jobID          the ID of the job.
     */
    virtual void jobDeleted( const std::string& jobID )  = 0;
};


}; // TA_Base_App


#endif // JOB_EXECUTION_LISTENER_H
