/**
 * The source code in this file is the property of 
 * Ripple Systems Pty Ltd and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AuthenticationAgentException.h $
 * @author N. Jardine
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This file outlines a simple exception class for use with
 * authentication code. It is derived from TransactiveException.
 * This exception is thrown when an unexpected error occurrs in 
 * in the Authentication Agent.
 */


#if !defined(AFX_AuthenticationAgentException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_AuthenticationAgentException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
    class AuthenticationAgentException : public TransactiveException  
    {
    public:

        /**
         * AuthenticationAgentException (Constructor)
         * 
         * This constructs a AuthenticationAgentException.
         *
         * @param char* - This contains as much information as possible about the error that occurred. This can be any
         *                additional information about the error.
         *
         */
        AuthenticationAgentException(const char *msg) throw();


        virtual ~AuthenticationAgentException() throw();
    };
} // Closes TA_Base_Core

#endif // !defined(AFX_AuthenticationAgentException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

