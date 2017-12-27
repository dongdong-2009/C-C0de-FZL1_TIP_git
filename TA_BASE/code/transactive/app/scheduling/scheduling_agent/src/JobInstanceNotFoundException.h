/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobInstanceNotFoundException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  * Thrown when a job instance cannot be found.
  *
  */


#ifndef JOB_INSTANCE_NOT_FOUND_EXCEPTION_H
#define JOB_INSTANCE_NOT_FOUND_EXCEPTION_H

#include "SchedulingException.h"

namespace TA_Base_App
{

class JobInstanceNotFoundException: public SchedulingException
{

public:

    JobInstanceNotFoundException( const std::string & message )
        :
        SchedulingException(message)
    {
    }


    virtual ~JobInstanceNotFoundException() throw () {};

    virtual const char *what() const throw ()
    {
        return SchedulingException::what();
    }

private:
	JobInstanceNotFoundException();

};


}; // TA_Base_App

#endif // JOB_INSTANCE_NOT_FOUND_EXCEPTION_H
