/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/MathematicalEvaluationException.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Exception for the MathematicalEvaluation library
  */

#ifndef MathematicalEvaluationException_H
#define MathematicalEvaluationException_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
    class MathematicalEvaluationException : public TransactiveException
    {
    public:

     /**
       * Constructor
       *
       * This constructs an MathematicalEvaluationException.
       *
       * @param message - This contains as much information as possible about the error that occurred. This can be any
       *                additional information about the error.
       *
       */
       MathematicalEvaluationException(const std::string& message) throw();


       virtual ~MathematicalEvaluationException() throw();
    };
} // Closes TA_Base_Core

#endif // MathematicalEvaluationException_H

