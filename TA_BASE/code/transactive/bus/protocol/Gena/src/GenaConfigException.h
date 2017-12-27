/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaConfigException.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for GenaConfigException
  */

#ifndef __GENA_CONFIG_EXCEPTION_H_INCLUDED__
#define __GENA_CONFIG_EXCEPTION_H_INCLUDED__

#include "core/exceptions/src/TransactiveException.h"
#include <string>

#pragma once

namespace TA_Base_Core
{
	class GenaConfigException : public TransactiveException  
	{
	public:
		GenaConfigException() throw();
		GenaConfigException(const std::string& msg) throw();
		virtual ~GenaConfigException() throw();
	};

} //TA_Base_Core

#endif //__GENA_CONFIG_EXCEPTION_H_INCLUDED__