/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/CCTVException.h $
  * @author J. Ebedes
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: 
  * 
  * This file outlines a simple exception class for use with
  * CCTV classes. It is derived from TransActiveException.
  */


#if !defined(AFX_CCTVEXCEPTION_H__C4FD1942_47A0_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_CCTVEXCEPTION_H__C4FD1942_47A0_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>																

namespace TA_Base_Core
{
	class CCTVException : public TransactiveException  
	{
	public:
		CCTVException();
		CCTVException(const std::string& msg);
		virtual ~CCTVException();

	};
}

#endif // !defined(AFX_CCTVEXCEPTION_H__C4FD1942_47A0_11D7_B150_0080C8E32E69__INCLUDED_)

