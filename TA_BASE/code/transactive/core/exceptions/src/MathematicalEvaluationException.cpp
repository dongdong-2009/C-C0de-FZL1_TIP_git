/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/MathematicalEvaluationException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// ApplicationException.cpp: implementation of the ApplicationException class.
//
//////////////////////////////////////////////////////////////////////

#include "MathematicalEvaluationException.h"
//#include "core\utilities\src\DebugUtil.h"


namespace TA_Base_Core
{
    MathematicalEvaluationException::MathematicalEvaluationException(const std::string& message) throw()
        : TransactiveException(message)
    {
    }

    MathematicalEvaluationException::~MathematicalEvaluationException() throw()
    {
    }
} // Closes TA_Base_Core
