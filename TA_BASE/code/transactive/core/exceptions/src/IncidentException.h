/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/IncidentException.h $
  * @author San Teo
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * Incident Manager. It is derived from TransactiveException.
  * This exception class will encompass all Incident Manager errors that
  * can possibly occur.
  */


#if !defined(AFX_IncidentException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_IncidentException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core\exceptions\src\TransactiveException.h"

namespace TA_Base_Core
{
    class IncidentException : public TransactiveException  
    {
    public:

        //
        // Used to specify how the incdent has failed. The application can use this
        // to choose which error message to display to the user.
        // Details are not important to the operator so all details are logged and operator
        // can be given a simple error dialog.
        //
        enum EFailType
        {
            ACCESS_DENIED,          // Session has insufficient rights to perform action
            DATABASE_ERROR,         // Unable to use the database or error in database
            DATA_ERROR,             // Error in the database configuration
            INCIDENT_EXIST,         // An incident is already assocaited with this alarm
            ALARM_NOT_EXIST,        // The associated alarm does not exist
            PLAN_NOT_EXIST,         // The associated plan does not exist
            ASSET_NOT_EXIST,        // The associated asset does not exist
            LOCATION_NOT_EXIST,     // The associated location does not exist
            INCIDENT_NOT_EXIST,     // Incident does not exist
            GENERAL_ERROR           // Unknown exception is thrown
        };

        /**
          * IncidentException (Constructor)
          * 
          * This constructs a IncidentException.
          *
          * @param EFailTypeed - This indicates what sort of failure occurred. This allows the code
          *                            catching the exception to decide how it wants to deal with the error
          *                            and what it wants to display to the user
          *
          */
        IncidentException(const EFailType failType) throw();

         /**
          * IncidentException (Constructor)
          * 
          * This constructs a IncidentException.
          *
          * @param string& - This contains as much information as possible about the error that occurred
          *                  additional information about the error.
          * @param EFailTypeed - This indicates what sort of failure occurred. This allows the code
          *                            catching the exception to decide how it wants to deal with the error
          *                            and what it wants to display to the user
          *
          */
        IncidentException(const std::string& what, const EFailType failType) throw();


        virtual ~IncidentException() throw();

        /**
          * getFailType
          * 
          * This returns the type of failure that occurred in Incident. The value
          * returned should allow the application to determine an appropriate course of
          * action. The application should always retrieve this so it can determine
          * what to do next.
          *
          * @return EFailType This indicates the type of failure that occurred.
          *
          */
        EFailType getFailType() const { return m_failType; };


    private:
        static const std::string ACCESS_DENIED_STRING;
        static const std::string DATABASE_ERROR_STRING;
        static const std::string DATA_ERROR_STRING;
        static const std::string INCIDENT_EXIST_STRING;
        static const std::string ALARM_NOT_EXIST_STRING;
        static const std::string PLAN_NOT_EXIST_STRING;
        static const std::string ASSET_NOT_EXIST_STRING;
        static const std::string LOCATION_NOT_EXIST_STRING;
        static const std::string INCIDENT_NOT_EXIST_STRING;
        static const std::string GENERAL_ERROR_STRING;

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
        EFailType   m_failType;
    };
}

#endif // !defined(AFX_IncidentException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
