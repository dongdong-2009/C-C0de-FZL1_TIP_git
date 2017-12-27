/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/UserSettingsException.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This exception is used by the UserSettings Manager to indicate when the saving or retrieving
  * of user settings did not succeed.
  */


#if !defined(AFX_UserSettingException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_UserSettingException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core\exceptions\src\TransactiveException.h"

namespace TA_Base_Core
{
    class UserSettingsException : public TransactiveException  
    {
    public:

     /**
       * UserSettingException (Constructor)
       * 
       * This constructs a UserSettingsException.
       *
	   * @param char* - This contains as much information as possible about the error that occurred.
       */
        UserSettingsException(const char* msg) throw();

        virtual ~UserSettingsException() throw();
    };
}

#endif // !defined(AFX_UserSettingException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
