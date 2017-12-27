/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/MessageResourceException.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * GenericGUI. It is derived from TransactiveException.
  * This exception class will encompass all GenericGUI errors that
  * can possibly occur.
  */


#if !defined(AFX_MessageResourceException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_MessageResourceException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core\exceptions\src\TransactiveException.h"

namespace TA_Base_Core
{
    class MessageResourceException : public TransactiveException  
    {
    public:

        //
        // Used to specify how GenericGUI has failed. The application can use this
        // to choose which error message to display to the user.
        // Details are not important to the operator so all details are logged and operator
        // can be given a simple error dialog.
        //
        enum EMessageResourceError
        {
            DLL_NOT_FOUND,             // The TA_MessageResource DLL could not be found
            FORMATTING_ERROR,          // The message text contained formatting errors
            LANGUAGE_ERROR,            // The system language setting does not match the DLL
            RESOURCE_NOT_FOUND_ERROR,  // The message ID was not found in the DLL
            GENERAL_ERROR              // An unspecified error has occured.
        };

     /**
       * MessageResourceException (Constructor)
       * 
       * This constructs a MessageResourceException.
       *
       * @param EGenericGUIFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       * @param messageID - This is the resource ID of the message that caused the error. This ID represents the 
       *                    numerical value of the error message as defined in the XML messages spreadsheet.
       *
       */
        MessageResourceException(const EMessageResourceError errorType, const unsigned long messageID = 0) throw();

      /**
       * MessageResourceException (Constructor)
       * 
       * This constructs a MessageResourceException.
       *
       * @param string& - This contains as much information as possible about the error that occurred. This can be any
       *                  additional information about the error.
       * @param EGenericGUIFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       * @param messageID - This is the resource ID of the message that caused the error. This ID represents the 
       *                    numerical value of the error message as defined in the XML messages spreadsheet.
       *
       */
        MessageResourceException(const std::string& what, const EMessageResourceError errorType,
            const unsigned long messageID = 0) throw();


        virtual ~MessageResourceException() throw();

      /**
       * getFailType
       * 
       * This returns the type of failure that occurred in MessageResource. The value
       * returned should allow the application to determine an appropriate course of
       * action. The application should always retrieve this so it can determine
       * what to do next.
       *
       * @return EMessageResourceError This indicates the type of failure that occurred.
       *
       */
       EMessageResourceError getFailType() const { return m_failType; };

       /**
        * getMessageID
        *
        * This returns the ID of the message that was to be displayed when the error occured.
        * The value returned should allow application debuggers to determine which message
        * was the cause of the error.
        */
       unsigned int getMessageID() const { return m_messageID; };


    private:

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
        EMessageResourceError   m_failType;

        // This is the message ID that failed
        unsigned long m_messageID;
    };
}

#endif // !defined(AFX_MessageResourceException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
