/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobInstanceExecutionException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  * Thrown when a job instance cannot be executed, or fails during execution.
  *
  */


#ifndef JOB_INSTANCE_EXECUTION_EXCEPTION_H
#define JOB_INSTANCE_EXECUTION_EXCEPTION_H

#include "SchedulingException.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"

namespace TA_Base_App
{

class JobInstanceExecutionException: public SchedulingException
{

public:

    JobInstanceExecutionException(
        const std::string & message)
        :
        SchedulingException(message)
    {
    }

    JobInstanceExecutionException(const JobInstanceExecutionException & rhs)
        :
        SchedulingException(rhs)
    {
    }

    virtual ~JobInstanceExecutionException() throw () {}

    virtual const char *what() const throw ()
    {
        return SchedulingException::what();
    }
private:
	JobInstanceExecutionException();

};


}; // TA_Base_App

#endif // JOB_INSTANCE_EXECUTION_EXCEPTION_H
