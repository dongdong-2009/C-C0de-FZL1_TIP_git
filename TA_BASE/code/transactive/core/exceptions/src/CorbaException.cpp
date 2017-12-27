/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/CorbaException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// CorbaException.cpp: implementation of the CorbaException class.
//
//////////////////////////////////////////////////////////////////////

#include "CorbaException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
	CorbaException::CorbaException()
	{

	}

	CorbaException::CorbaException(const std::string& msg) 
		: TransactiveException(msg)
	{

	}

	CorbaException::~CorbaException()
	{

	}
}

