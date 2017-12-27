/**
 * The source code in this file is the property of 
 * Ripple Systems Pty Ltd and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AuthenticationSecurityException.h $
 * @author N. Jardine
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This file outlines a simple exception class for use with
 * authentication code. It is derived from TransactiveException.
 * This exception will be thrown when Authentication is denied,
 * and is used to specifify the reason for the denial.
 */


#if !defined(AFX_AUTHENTICATIONSECURITYEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_AUTHENTICATIONSECURITYEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
    class AuthenticationSecurityException : public TransactiveException  
    {
    public:

        enum ESecurityReason    // Used to specify why the authentication was denied
        {
            INVALID_USER,           // The supplied user is not known to the system
            INVALID_PASSWORD,       // The supplied password is incorrect for the user
            INVALID_PROFILE,        // The supplied profile is not known to the system
            INVALID_LOCATION,       // The supplied location is not known to the system
            INVALID_WORKSTATION,    // The supplied workstation is not known to the system
            PROFILE_NOT_PERMITTED,  // The profile is not allowed for the supplied user
            LOCATION_NOT_PERMITTED, // The location is not allowed for the supplied profile
            INVALID_SESSION,        // The session is not a current, valid, session
            INVALID_PASSWORD_LENGTH, // The new password is too long or too short
			USER_LOCKED,			//The user is locked for failed login several times
			PASSWORD_USEDBEFORE		//The new password has used before;
        };

        /**
         * AuthenticationSecurityException (Constructor)
         * 
         * This constructs a AuthenticationSecurityException.
         *
         * @param char* - This contains as much information as possible about the error that occurred. This can be any
         *                additional information about the error.
         * @param ESecurityReason - This indicates why authentication waq denied. This allows the code catching the
         *                        exception to decide how it wants to deal with the error.
         *
         */
        AuthenticationSecurityException(const char *msg,const ESecurityReason failType) throw();


        virtual ~AuthenticationSecurityException() throw();

        /**
         * getReason
         * 
         * This returns the reason the authentication was denied. The value
         * returned should allow the application to determine an appropriate course of
         * action. The application should always retrieve this so it can determine
         * what to do next.
         *
         * @return ESecurityReason This indicates the type of failure that occurred.
         *
         */
        ESecurityReason getReason() const { return m_reason; };


    private:

        /**
         * logException
         * 
         * This class must log the exception details that are specific to it. ie Everything except the
         * string message passed in. There are multiple constructors and each must log so this method
         * is used to put the logging code in a common place.
         *
         */
        void logException();

        // Member Variables
        ESecurityReason   m_reason;     // This is a set failure type so the application can deal with it appropriately
    };
} // Closes TA_Base_Core

#endif // !defined(AFX_AUTHENTICATIONSECURITYEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

