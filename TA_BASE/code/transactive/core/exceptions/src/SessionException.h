/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/SessionException.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * the SessionManager class in the Control Station. It is
  * derived from TransactiveException.
  * This exception class is used when errors are encountered 
  * when attempting login, load a list of Sessions etc etc.
  */

#if !defined(AFX_SessionEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_SessionEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    class SessionException : public TransactiveException  
    {
    public:
 
        enum EFailType
        {
            GENERAL_ERROR,                  // A common error
            AUTHENTICATION_FAILED,          // Authentication failed i.e. the password and login details do not match what is in the database
            PASSWORD_MISMATCH,              // The new passwords do not match
            INVALID_PASSWORD_LENGTH,        // The new password is longer or shorter than what's allowed
            INVALID_SESSION_CONFIGURATION,  // The configuration for operator/location/profile that the session is using has been deleted
            INVALID_SESSION,                // The session is not valid
            AUTHENTICATION_AGENT_FAILED,     // The authentication agent has failed
			USER_LOCKED,			//The user is locked for failed login several times
			PASSWORD_USEDBEFORE					//The new password has used before;
        };

     /**
       * SessionException (Constructor)
       * 
       * This constructs an SessionException.
       *
       * @param char* - This contains as much information as possible about the error that occurred. This can be any
       *                additional information about the error.
       *
       */
        SessionException(const char *msg) throw();

     /**
       * SessionException (Constructor)
       * 
       * This constructs an SessionException.
       *
       * @param EFailType - This indicates what sort of failure occurred. This is for use with
       *                    functions such as logging in where it can fail because of an error
       *                    or it can fail because the password etc was not correct or the user
       *                    does not have access rights.
       * @param char* - This contains as much information as possible about the error that occurred. This can be any
       *                additional information about the error.
       *
       */
        SessionException(const EFailType failType, const char *msg) throw();

        virtual ~SessionException() throw();

      /**
       * getFailType
       *
       * @return EFailType - The type of failure that occured. This is for use with
       *                     functions such as logging in where it can fail because of an error
       *                     or it can fail because the password etc was not correct or the user
       *                     does not have access rights.
       */
        EFailType getFailType() const
        {
            return m_failType;
        };

    private:
        EFailType m_failType;
    };
} // Closes TA_Base_Core

#endif // !defined(AFX_SessionEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

