/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/DatabaseException.h $
  * @author K. Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * database code. It is derived from TransactiveException.
  * This exception class will encompass all database errors that
  * can possibly occur.
  */


#if !defined(AFX_DATABASEEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_DATABASEEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class DatabaseException : public TransactiveException  
	{
	public:

		DatabaseException(const std::string& msg) throw();
	};
} // Closes TA_Base_Core

#endif // !defined(AFX_DATABASEEXCEPTION_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)

