/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/TransactiveException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is at the top of the exception hierarchy. All
  * exception classes should be derived from this class, or
  * from a class that in turn is derived from this class.
  */

#if !defined(AFX_TransactiveException_H__E2DE1FE6_479C_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_TransactiveException_H__E2DE1FE6_479C_11D7_B150_0080C8E32E69__INCLUDED_

#include <exception>
#include <string>
#include <memory>

/** 
  * TA_THROW
  *
  * This macro throws exceptions while providing logging. It takes the
  * place of the throw keyword, and is used identically.
  * eg: TA_THROW(CCTVException("A CCTV error occurred"));
  *
  * @param exc  The exception (with parameters) to throw
  */
#define TA_THROW(exc)															\
{																				\
	LOG(SourceInfo,																\
		TA_Base_Core::DebugUtil::ExceptionConstruction,				\
		std::string(#exc).substr(0, (std::string(#exc).find("("))).c_str(),		\
		exc.what());															\
	throw exc;																	\
}


namespace TA_Base_Core
{
	class TransactiveException : public std::exception  
	{
	public:
		TransactiveException() throw();
		TransactiveException(const std::string& msg) throw();
		virtual const char* what() const throw();
		virtual ~TransactiveException() throw();

	protected:
        // Description of the reason for the exception.  It is protected (rather than
        // private) so that derived classes can build the message in their constructor
        // after the member initialisation list.
		std::string m_msg; 
	};
}

#endif // !defined(AFX_TransactiveException_H__E2DE1FE6_479C_11D7_B150_0080C8E32E69__INCLUDED_)

