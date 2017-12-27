/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ThreadException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This file outlines a simple exception class for use with
  * the thread classes. It is derived from TransActiveException.
  */

#if !defined(AFX_THREADEXCEPTION_H__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_THREADEXCEPTION_H__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class ThreadException : public TransactiveException  
	{
	public:
		ThreadException();
		ThreadException(const std::string& msg);
		virtual ~ThreadException();
	};
}

#endif // !defined(AFX_THREADEXCEPTION_H__44BE1875_4870_11D7_B150_0080C8E32E69__INCLUDED_)

