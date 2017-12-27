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

#include "NoActiveConException.h"

namespace TA_Base_Core
{
	NoActiveConException::NoActiveConException() throw()
	{
	}

	NoActiveConException::NoActiveConException(const std::string& msg) 
		throw() : TransactiveException(msg)
	{
	}
	
	NoActiveConException::~NoActiveConException() throw()
	{
	}
}

