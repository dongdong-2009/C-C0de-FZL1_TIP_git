/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/UserCancelException.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This exception is used in the Configuration Editor to determine a user cancel. A user cancel on
  * many of the dialogs means we have to undo everything we've been doing and close the application
  */


#include "core/exceptions/src/UserCancelException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{

	UserCancelException::UserCancelException() throw()
	{

	}

	UserCancelException::UserCancelException(const std::string& msg) 
		throw(): TransactiveException(msg)
	{

	}

	UserCancelException::~UserCancelException() throw()
	{

	}
}
