/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/CannotUseSysControllerException.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is used by the Configuration Editor. It is thrown when it cannot connect to the system using
  * the System Controller. This basically means it will have to connect using a direct database connection
  */

#include "core/exceptions/src/CannotUseSysControllerException.h"

namespace TA_Base_Core
{

    CannotUseSysControllerException::CannotUseSysControllerException(const std::string& msg) throw() 
            : TA_Base_Core::TransactiveException(msg)
    {

	}
 
    CannotUseSysControllerException::~CannotUseSysControllerException() throw()
    {

    }
};
