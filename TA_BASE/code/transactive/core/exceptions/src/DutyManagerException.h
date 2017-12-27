/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DutyManagerException.h $
  * @author Anita Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * DutyManager. It is derived from TransactiveException.
  * This exception class will encompass all DutyManager errors that
  * can possibly occur.
  */


#if !defined DUTY_MANAGER_EXCEPTION
#define DUTY_MANAGER_EXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core\exceptions\src\TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
    class DutyManagerException : public TransactiveException  
    {
    public:

        //
        // Used to specify how DutyManager has failed. The application can use this
        // to choose which error message to display to the user.
        // Details are not important to the operator so all details are logged and operator
        // can be given a simple error dialog.
        //
        enum EDutyManagerFail
        {
            NO_DATABASE_CONNECTION,                 // Could not connect to the database and retrieve entity details
            INVALID_CONFIGURATION,                  // Data is not configured correctly    
            CANNOT_CONTACT_AUTHENTICATION_AGENT,    // Unable to contact the Authentication Agent
            CANNOT_CONTACT_DUTY_AGENT,              // Unable to contact the Duty Agent
            ACCESS_DENIED,                          // The operator does not have the rights to run the application
            UNKNOWN_ACCESS_RIGHTS,                  // Could not determine the rights of the operator, could not resolve the rights agent or any other exception thrown by the rights library
            NO_SESSION_INFO,                        // The Authentication Agent could not retrieve the session information
            CANNOT_GET_DUTIES,                      // The Duty Agent could not get the duties                       
            CANNOT_GET_LOCATIONS,                   // Unable to retrieve takeable locations from the Duty Agent
            CANNOT_GET_SESSIONS,                    // Unable to get active sessions from the Authentication Agent
            CANNOT_UPDATE_SUBSYSTEM_DUTIES,         // The GUI is unable to update subsystem duties
            CANNOT_UPDATE_LOCATION_DUTIES,          // The GUI is unable to update the location duties 
            UNKNOWN_EXCEPTION                       // Some unknown exception has occurred
        };

     /**
       * DutyManagerException (Constructor)
       * 
       * This constructs a DutyManagerException.
       *
       * @param EDutyManagerFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       *
       */
        DutyManagerException(const EDutyManagerFail failType) throw()
        : m_failType(failType),
          TransactiveException()
        {
        }


      /**
       * DutyManagerException (Constructor)
       * 
       * This constructs a DutyManagerException.
       *
       * @param EDutyManagerFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       * @param string& - This contains as much information as possible about the error that occurred. This can be any
       *                  additional information about the error.
       *
       */
        DutyManagerException(const EDutyManagerFail failType, const std::string& what) throw()
        : m_failType(failType),
          TransactiveException(what)
        {
        }


        virtual ~DutyManagerException() throw() { }

     /**
       * getFailType
       * 
       * This returns the type of failure that occurred in DutyManager. The value
       * returned should allow the application to determine an appropriate course of
       * action. The application should always retrieve this so it can determine
       * what to do next.
       *
       * @return EDutyManagerFail This indicates the type of failure that occurred.
       *
       */
        EDutyManagerFail getFailType() const { return m_failType; };

	private:
		DutyManagerException() throw();

        //
        // Member Variables
        //

        // This is a set failure type so the application can deal with it appropriately
        EDutyManagerFail   m_failType;
    };
}

#endif // DUTY_MANAGER_EXCEPTION
