/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DataUserSettingException.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * UserSettings Data Access Interface. It is derived from TransactiveException.
  */


#if !defined(AFX_DataUserSettingException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_DataUserSettingException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core\exceptions\src\TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
    class DataUserSettingException : public TransactiveException  
    {
    public:

        enum Type
        {
            GENERIC,            // Some sort of error occurred and it doesn't really matter what
                                // to the caller.
            SETTING_NOT_FOUND   // This means the setting was not found.
        };

     /**
       * DataUserSettingException (Constructor)
       * 
       * This constructs a DataUserSettingException.
       *
	   * @param char* - This contains as much information as possible about the error that occurred.
	   *                 This can be any additional information about the error.
       */
        DataUserSettingException(const char* msg) throw();

     /**
       * DataUserSettingException (Constructor)
       * 
       * This constructs a DataUserSettingException.
       *
	   * @param char* - This contains as much information as possible about the error that occurred.
	   *                 This can be any additional information about the error.
       * @param Type - This is the type of failure. This will only be set if it is something that
       *               we care about.
       */
        DataUserSettingException(const char* msg, const Type failType) throw();

        virtual ~DataUserSettingException() throw();

      /**
       * getFailType
       *
       * This returns the type of failure that occurred.
       *
       * @return Type - The type of failure that occurred
       */
        Type getFailType() const { return m_failType; };

    private:
        // This indicates the failure that occurred.
        Type m_failType;
    };
}

#endif // !defined(AFX_DataUserSettingException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
