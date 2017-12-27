/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/CorbaException.h $
  * @author J. Ebedes
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: 
  * 
  * This file outlines a simple exception class for use with
  * CORBA. It is derived from TransActiveException.
  */

#if !defined(AFX_ORBAEXCEPTION_H__44BE1874_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_ORBAEXCEPTION_H__44BE1874_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

// TODO LPP: This is quite ugly.  When we throw a corba exception (eg. OperationModeException),
// we have to use this macro rather than TA_THROW, because corba exceptions don't have
// the what() member function.
#define TA_THROW_CORBA(exc)															\
{																				\
	LOG(SourceInfo,																\
		TA_Base_Core::DebugUtil::ExceptionConstruction,				\
		std::string(#exc).substr(0, (std::string(#exc).find("("))).c_str(),		\
		exc.reason.in());															\
	throw exc;																	\
}

namespace TA_Base_Core
{
	class CorbaException : public TransactiveException  
	{
	public:
		CorbaException() throw();
        CorbaException(const std::string& msg) throw();
		virtual ~CorbaException() throw();

	};
}

#endif // !defined(AFX_ORBAEXCEPTION_H__44BE1874_4870_11D7_B150_0080C8E32E69__INCLUDED_)

