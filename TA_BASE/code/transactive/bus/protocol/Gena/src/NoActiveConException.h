/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: NoActiveConException.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for NoActiveConException
  */

#ifndef __NO_ACTIVE_CONNECTION_EXCEPTION_H_INCLUDED__
#define __NO_ACTIVE_CONNECTION_EXCEPTION_H_INCLUDED__

#include "core/exceptions/src/TransactiveException.h"
#include <string>

#pragma once

namespace TA_Base_Core
{
	class NoActiveConException : public TransactiveException  
	{
	public:
		NoActiveConException() throw();
		NoActiveConException(const std::string& msg) throw();
		virtual ~NoActiveConException() throw();
	};

} //TA_Base_Core

#endif //__NO_ACTIVE_CONNECTION_EXCEPTION_H_INCLUDED__