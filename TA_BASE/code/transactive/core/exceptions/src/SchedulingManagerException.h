/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/SchedulingManagerException.h $
  * @author San Teo
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * Scheduling Manager. It is derived from TransactiveException.
  * This exception class will encompass all Scheduling Manager errors that
  * can possibly occur.
  */


#if !defined(AFX_SchedulingManagerException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_SchedulingManagerException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core\exceptions\src\TransactiveException.h"

namespace TA_Base_Core
{
    class SchedulingManagerException : public TransactiveException  
    {
    public:

        //
        // Used to specify how GenericGUI has failed. The application can use this
        // to choose which error message to display to the user.
        // Details are not important to the operator so all details are logged and operator
        // can be given a simple error dialog.
        //
        enum ESchedulingManagerFail
        {
            SCHEDULING_AGENT_FAILURE, // Couldn't contact the Scheduling Agent agent, OR it is in transitional mode
            RIGHTS_AGENT_FAILURE,   // Couldn't contact the Rights Agent
            ACCESS_DENIED,          // Session has insufficient rights to perform action
            DATABASE_ERROR,         // Unable to use the database or error in database
            DATA_ERROR,             // Error in the database configuration
            JOB_NOT_EXIST,          // Job does not exist
            JOB_EXIST,              // Job already exists
            JOB_IN_PAST,            // Schedule is set to run in the past
            NO_JOB_PARAMETER,       // The runparam JobInfo contain invalid value
            MISSING_JOB_PARAMETER,  // One or more of the job parameters are missing
            INVALID_JOB_PARAMETER,  // One or more of the job parameters are invalid
            BAD_SCHEDULE,           // Schedule is invalid
            NO_DAY_OF_WEEK,         // No day of week is specified
            TOO_MANY_JOBS,          // Too many jobs are scheduled to start on the same minute
            SESSION_PROFILES_NOT_MATCH_JOB_PROFILE, // The session profiles do not include one that is associated with the job
			//TD16352++
			EDIT_LOCK,				// Someone else is modifying cannot proceed
			//++TD16352
            UNKNOWN_EXCEPTION       // Unknown exception is thrown
        };

        /**
          * SchedulingManagerException (Constructor)
          * 
          * This constructs a SchedulingManagerException.
          *
          * @param ESchedulingManagerFailed - This indicates what sort of failure occurred. This allows the code
          *                            catching the exception to decide how it wants to deal with the error
          *                            and what it wants to display to the user
          *
          */
        SchedulingManagerException(const ESchedulingManagerFail failType) throw();

         /**
          * SchedulingManagerException (Constructor)
          * 
          * This constructs a SchedulingManagerException.
          *
          * @param string& - This contains as much information as possible about the error that occurred. This can be any
          *                  additional information about the error.
          * @param ESchedulingManagerFailed - This indicates what sort of failure occurred. This allows the code
          *                            catching the exception to decide how it wants to deal with the error
          *                            and what it wants to display to the user
          *
          */
        SchedulingManagerException(const std::string& what, const ESchedulingManagerFail failType) throw();


        virtual ~SchedulingManagerException() throw();

        /**
          * getFailType
          * 
          * This returns the type of failure that occurred in SchedulingManager. The value
          * returned should allow the application to determine an appropriate course of
          * action. The application should always retrieve this so it can determine
          * what to do next.
          *
          * @return ESchedulingManagerFail This indicates the type of failure that occurred.
          *
          */
        ESchedulingManagerFail getFailType() const { return m_failType; };


    private:

        static const std::string SCHEDULING_AGENT_FAILURE_STRING;
        static const std::string RIGHTS_AGENT_FAILURE_STRING;
        static const std::string ACCESS_DENIED_STRING;
        static const std::string DATABASE_ERROR_STRING;
        static const std::string DATA_ERROR_STRING;
        static const std::string JOB_NOT_EXIST_STRING;
        static const std::string JOB_EXIST_STRING;
        static const std::string JOB_IN_PAST_STRING;
        static const std::string NO_JOB_PARAMETER_STRING;
        static const std::string INVALID_JOB_PARAMETER_STRING;
        static const std::string MISSING_JOB_PARAMETER_STRING;
        static const std::string BAD_SCHEDULE_STRING;
        static const std::string NO_DAY_OF_WEEK_STRING;
        static const std::string TOO_MANY_JOBS_STRING;
        static const std::string UNKNOWN_EXCEPTION_STRING;
        static const std::string SESSION_PROFILES_NOT_MATCH_JOB_PROFILE_STRING;
		//TD16352++
		static const std::string EDIT_LOCK_STRING;
		//++TD16352

          /** 
            * logException
            *
            * This class must log the exception details that are specific to it. ie Everything except the
            * string message passed in. The logging is put in this method to keep it separate.
            *
            */
		void logException();

        //
        // Member Variables
        //

        // This is a set failure type so the application can deal with it appropriately
        ESchedulingManagerFail   m_failType;
    };
}

#endif // !defined(AFX_SchedulingManagerException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
