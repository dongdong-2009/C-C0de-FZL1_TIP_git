/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ApplicationException.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * the RunningApplication class in the Control Station. It is
  * derived from TransactiveException.
  * This exception class is used when errors are encountered 
  * when attempting to perform actions on a process such as launch
  * or terminate.
  */

#if !defined(AFX_APPLICATIONEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_APPLICATIONEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class ApplicationException : public TransactiveException  
	{
	public:
 
	 /**
	   * ApplicationException (Constructor)
	   * 
	   * This constructs an ApplicationException.
	   *
       * @param string& - This contains the name of the executable that encountered the error.
	   * @param string& - This contains as much information as possible about the error that occurred. This can be any
	   *                  additional information about the error.
	   *
	   */
        ApplicationException(const std::string& executable, const std::string& msg) throw();

	 /**
	   * ApplicationException (Constructor)
	   * 
	   * This constructs an ApplicationException.
	   *
	   * @param string& - This contains as much information as possible about the error that occurred. This can be any
	   *                  additional information about the error.
	   *
	   */
        ApplicationException(const std::string& msg) throw();

		virtual ~ApplicationException() throw();

	};
} // Closes TA_Base_Core

#endif // !defined(AFX_APPLICATIONEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

