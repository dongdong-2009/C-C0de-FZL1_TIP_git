/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/GenericGUIException.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * GenericGUI. It is derived from TransactiveException.
  * This exception class will encompass all GenericGUI errors that
  * can possibly occur.
  */


#if !defined(AFX_GenericGUIException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_GenericGUIException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core\exceptions\src\TransactiveException.h"

namespace TA_Base_Core
{
    class GenericGUIException : public TransactiveException  
    {
    public:

        //
        // Used to specify how GenericGUI has failed. The application can use this
        // to choose which error message to display to the user.
        // Details are not important to the operator so all details are logged and operator
        // can be given a simple error dialog.
        //
        enum EGenericGUIFail
        {
            ENTITY_CONFIGURATION_INVALID,             // The entity was in the database but it's configuration was incorrect
            ENTITY_DOES_NOT_EXIST,                    // The entity specified on the command line does not exist in the database
            ENTITY_NOT_UNIQUE,                        // The entity specified was not unique in the database
            CONTROL_ENTITY_CONFIGURATION_INVALID,     // The entity to control was in the database but its configuration was incorrect
            CONTROL_ENTITY_DOES_NOT_EXIST,            // The entity to control was on the command line but does not exist in the database
            CONTROL_ENTITY_NOT_UNIQUE,                // The entity to control was not unique in the database
            COMMAND_LINE_INVALID,                     // The command line passed was invalid
            NO_DATABASE_CONNECTION,                   // Could not connect to the database and retrieve entity details
            COMMUNICATION_ERROR_WITH_CONTROL_STATION, // Could not communicate with the Control Station in some way.
            INITIALISATION_ERRORS,                    // This indicates some sort of initialisation error has occurred. Probably with CorbaUtil.
            ACCESS_DENIED,                            // The operator does not have the rights to run the application
            UNKNOWN_ACCESS_RIGHTS,                    // Could not determine the rights of the operator, could not resolve the rights agent or any other exception thrown by the rights library
            NO_ACTIVE_SESSION                         // Operator tries to run an application without having logged in
        };

     /**
       * GenericGUIException (Constructor)
       * 
       * This constructs a GenericGUIException.
       *
       * @param EGenericGUIFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       *
       */
        GenericGUIException(const EGenericGUIFail failType) throw();

      /**
       * GenericGUIException (Constructor)
       * 
       * This constructs a GenericGUIException.
       *
       * @param string& - This contains as much information as possible about the error that occurred. This can be any
       *                  additional information about the error.
       * @param EGenericGUIFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       *
       */
        GenericGUIException(const std::string& what, const EGenericGUIFail failType) throw();


        virtual ~GenericGUIException() throw();

     /**
       * getFailType
       * 
       * This returns the type of failure that occurred in GenericGUI. The value
       * returned should allow the application to determine an appropriate course of
       * action. The application should always retrieve this so it can determine
       * what to do next.
       *
       * @return EGenericGUIFail This indicates the type of failure that occurred.
       *
       */
        EGenericGUIFail getFailType() const { return m_failType; };


    private:

        static const std::string ENTITY_CONFIGURATION_INVALID_STRING;
        static const std::string ENTITY_DOES_NOT_EXIST_STRING;
        static const std::string ENTITY_NOT_UNIQUE_STRING;
        static const std::string CONTROL_ENTITY_CONFIGURATION_INVALID_STRING;
        static const std::string CONTROL_ENTITY_DOES_NOT_EXIST_STRING;
        static const std::string CONTROL_ENTITY_NOT_UNIQUE_STRING;
        static const std::string COMMAND_LINE_INVALID_STRING;
        static const std::string NO_DATABASE_CONNECTION_STRING;
        static const std::string COMMUNICATION_ERROR_WITH_CONTROL_STATION_STRING;
        static const std::string INITIALISATION_ERRORS_STRING;
        static const std::string ACCESS_DENIED_STRING;
        static const std::string UNKNOWN_ACCESS_RIGHTS_STRING;
        static const std::string NO_ACTIVE_SESSION_STRING;

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
        EGenericGUIFail   m_failType;
    };
}

#endif // !defined(AFX_GenericGUIException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
