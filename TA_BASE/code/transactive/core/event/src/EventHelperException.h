/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventHelperException.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_HELPER_EXCEPTION_H_INCLUDED__
#define __EVENT_HELPER_EXCEPTION_H_INCLUDED__

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class EventHelperException : public TA_Base_Core::TransactiveException  
	{
	public:
		enum ERROR_CODE
		{
			ERR_SYSTEM_EX=0,		// CORBA::SystemException
			ERR_OBJECT_RES,			// Object Resolution
			ERR_OPERATION_MODE,		// Operation mode exception
			ERR_TRANSACTIVE_EX,		// Transactive exception
			ERR_ID_NOT_FOUND,		// Event ID not found exception
			ERR_EXCEPTION,			// Generic Corba exception
			ERR_INIT_AGENT,			// Agent not initialized/error on initialization 
			ERR_TIMEOUT,			// Timeout when calling agent
			ERR_UNKNOWN				// Unknown exception
		};

		EventHelperException(const ERROR_CODE errCode) throw();
		EventHelperException(const ERROR_CODE errCode, const std::string& what) throw();
		virtual ~EventHelperException() throw();

		ERROR_CODE getErrorCode() const { return m_errCode; };

	private: 
		ERROR_CODE m_errCode;
	};    
}

#endif //__EVENT_HELPER_EXCEPTION_H_INCLUDED__