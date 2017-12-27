/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/InterlockingException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "InterlockingException.h"


namespace TA_Base_Core
{
    InterlockingException::InterlockingException(const std::string& message) throw()
        : TransactiveException(message)
    {
    }

    InterlockingException::~InterlockingException() throw()
    {
    }
} // Closes TA_Base_Core
