/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AssertException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// AssertException.cpp: implementation of the AssertException class.
//
//////////////////////////////////////////////////////////////////////

#include "AssertException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
	AssertException::AssertException() 
		throw()
	{

	}

	AssertException::AssertException(const std::string& msg) 
		throw(): TransactiveException(msg)
	{

	}

	AssertException::~AssertException() throw()
	{

	}
}
