/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/SchedulingManagerException.cpp $
  * @author San Teo
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * Scheduling Manager. It is derived from TransactiveException.
  * This exception class will encompass all errors that
  * can possibly occur.
  */

#include "core\exceptions\src\SchedulingManagerException.h"
#include "core\utilities\src\TAAssert.h"
#include "core\utilities\src\DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    //////////////////////////////////////////////////////////////////////
    // Constants
    //////////////////////////////////////////////////////////////////////

     const std::string SchedulingManagerException::SCHEDULING_AGENT_FAILURE_STRING = "Unable to communicate with the Scheduling Agent";
     const std::string SchedulingManagerException::RIGHTS_AGENT_FAILURE_STRING = "Unable to communicate with the Rights Agent";
     const std::string SchedulingManagerException::ACCESS_DENIED_STRING = "Operator has insufficient privilege to perform action";
     const std::string SchedulingManagerException::DATABASE_ERROR_STRING = "Unable to contact the database or encountered some kind of db error";
     const std::string SchedulingManagerException::DATA_ERROR_STRING = "Scheduling Manager or Agent is not configured correctly in the database";
     const std::string SchedulingManagerException::JOB_NOT_EXIST_STRING = "The Scheduling Agent does not recognise the job";
     const std::string SchedulingManagerException::JOB_EXIST_STRING = "The job is identical as an existing one";
     const std::string SchedulingManagerException::JOB_IN_PAST_STRING = "The job has been scheduled to run in the past";
     const std::string SchedulingManagerException::NO_JOB_PARAMETER_STRING = "Job specific details are not specified";
     const std::string SchedulingManagerException::MISSING_JOB_PARAMETER_STRING = "One or more job parameters are missing";
     const std::string SchedulingManagerException::INVALID_JOB_PARAMETER_STRING = "One or more job parameters are invalid";
     const std::string SchedulingManagerException::BAD_SCHEDULE_STRING = "One or more schedule fields are invalid";
     const std::string SchedulingManagerException::NO_DAY_OF_WEEK_STRING = "Day of week is not specified";
     const std::string SchedulingManagerException::UNKNOWN_EXCEPTION_STRING = "Some unknown exception has occurred";
     const std::string SchedulingManagerException::TOO_MANY_JOBS_STRING = "Too many jobs have been scheduled to start on the same time";
     const std::string SchedulingManagerException::SESSION_PROFILES_NOT_MATCH_JOB_PROFILE_STRING = "The session profiles do not include one that is associated with the job";
	 //TD16352++
	 const std::string SchedulingManagerException::EDIT_LOCK_STRING = "Unable to Edit/Delete Job due to edit lock";
	 //++TD16352


    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////


    SchedulingManagerException::SchedulingManagerException(const ESchedulingManagerFail failType) 
        : m_failType(failType),
          TransactiveException()
    {
        logException();
    }	

    SchedulingManagerException::SchedulingManagerException(const std::string& what, const ESchedulingManagerFail failType)
        : m_failType(failType),
          TransactiveException(what)
    {
        logException();
    }	



    SchedulingManagerException::~SchedulingManagerException()
    {

    }

    //////////////////////////////////////////////////////////////////////
    // Member Functions
    //////////////////////////////////////////////////////////////////////

    void SchedulingManagerException::logException()
    {
        switch(m_failType)
        {
            case ( SCHEDULING_AGENT_FAILURE ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",SCHEDULING_AGENT_FAILURE_STRING.c_str() );
                break;

            case ( RIGHTS_AGENT_FAILURE ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",RIGHTS_AGENT_FAILURE_STRING.c_str() );
                break;

            case ( ACCESS_DENIED ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",ACCESS_DENIED_STRING.c_str() );
                break;

            case ( DATABASE_ERROR ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",DATABASE_ERROR_STRING.c_str() );
                break;

            case ( DATA_ERROR ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",DATA_ERROR_STRING.c_str() );
                break;

            case ( JOB_NOT_EXIST ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",JOB_NOT_EXIST_STRING.c_str() );
                break;

            case ( JOB_IN_PAST ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",JOB_IN_PAST_STRING.c_str() );
                break;

            case ( JOB_EXIST ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",JOB_EXIST_STRING.c_str() );
                break;

            case ( NO_JOB_PARAMETER ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",NO_JOB_PARAMETER_STRING.c_str() );
                break;

            case ( INVALID_JOB_PARAMETER ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",INVALID_JOB_PARAMETER_STRING.c_str() );
                break;

            case ( MISSING_JOB_PARAMETER ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",MISSING_JOB_PARAMETER_STRING.c_str() );
                break;

            case ( BAD_SCHEDULE ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",BAD_SCHEDULE_STRING.c_str() );
                break;

            case ( NO_DAY_OF_WEEK ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",NO_DAY_OF_WEEK_STRING.c_str() );
                break;

            case ( TOO_MANY_JOBS ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",TOO_MANY_JOBS_STRING.c_str() );
                break;

            case ( SESSION_PROFILES_NOT_MATCH_JOB_PROFILE ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",SESSION_PROFILES_NOT_MATCH_JOB_PROFILE_STRING.c_str() );
                break;
			//TD16352++
			case ( EDIT_LOCK ):
				LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",EDIT_LOCK_STRING.c_str() );
                break;
			//++TD16352

            case ( UNKNOWN_EXCEPTION ):
                LOG( SourceInfo, DebugUtil::ExceptionDetails, "SchedulingManagerException",UNKNOWN_EXCEPTION_STRING.c_str() );
                break;

            default:
                TA_ASSERT(false,"Unknown Scheduling Manager Exception Type");
        }
    }

} // End TA_Base_Core
