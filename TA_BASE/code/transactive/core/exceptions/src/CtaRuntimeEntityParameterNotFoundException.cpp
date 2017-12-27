/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/CtaRuntimeEntityParameterNotFoundException.cpp $
  * @author K. Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Exception to be thrown from CtaRuntime when entity configuration cannot be found
  */

#include "CtaRuntimeEntityParameterNotFoundException.h"


namespace TA_Base_Core
{
    CtaRuntimeEntityParameterNotFoundException::CtaRuntimeEntityParameterNotFoundException(const std::string& msg)
        throw(): TransactiveException(msg)
    {
        // Empty
    }


    CtaRuntimeEntityParameterNotFoundException::~CtaRuntimeEntityParameterNotFoundException() throw()
    {
        // Empty
    }
}