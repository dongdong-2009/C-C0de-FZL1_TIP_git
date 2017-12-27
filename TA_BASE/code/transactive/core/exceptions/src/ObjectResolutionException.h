/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ObjectResolutionException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#if !defined(OBJECT_RESOLUTION_EXCEPTION_H)
#define OBJECT_RESOLUTION_EXCEPTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{

	/** @exception ObjectResolutionException
	  *
	  * Thrown if the object cannot be resolved from the name
	  */
	enum ObjectResolutionExceptionType
	{
		OPERATION_MODE,		// Agent is not in control mode and retry has failed
		NO_AGENT,			// cannot resolve the agent service
		NO_OBJECT,         // agent does not contain a matching binding
		RESOLUTION_FAILED,  // there is a binding, but could not resolve object
		WRONG_INTERFACE,  // could not narrow object to templatised type
		UNKNOWN_ERROR
	};

	class ObjectResolutionException : public TransactiveException  
	{
	public:
		ObjectResolutionException(ObjectResolutionExceptionType eType,
			const std::string& msg) throw();

		ObjectResolutionException(ObjectResolutionExceptionType eType,
			const std::string& agentName,
			const std::string& objectName,
			const std::string& msg = "" ) throw();

		virtual ~ObjectResolutionException() throw();

		/**
		* override the what function
		*/
		virtual const char* what() const throw();

	private:
		/**
		* toString
		*
		* Static method to convert an ObjectResolutionExceptionType to a human
		* readable string
		*
		* @return	Human readable string of the exception type
		*/
		std::string toString() const;

		/**
		* The type of object resolution exception
		*/
		ObjectResolutionExceptionType m_eType;

		/**
		* Our own log message
		*/
		std::string m_msg;
	};
}

#endif // !defined(OBJECT_RESOLUTION_EXCEPTION_H)

