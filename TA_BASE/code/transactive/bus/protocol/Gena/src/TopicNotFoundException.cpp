/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: SIDNotFoundException.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for SIDNotFoundException
  */

#include "TopicNotFoundException.h"

namespace TA_Base_Core
{
	TopicNotFoundException::TopicNotFoundException() throw()
	{
	}

	TopicNotFoundException::TopicNotFoundException(const std::string& msg) 
		throw() : TransactiveException(msg)
	{
	}
		

	TopicNotFoundException::~TopicNotFoundException() throw()
	{
	}
}

