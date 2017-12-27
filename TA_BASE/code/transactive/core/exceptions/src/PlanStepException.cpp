/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/PlanStepException.cpp $
  *  @author:  San Teo
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

// namespace alias for Corba structures/types
 
#include "PlanStepException.h"

namespace TA_Base_Core
{
	PlanStepException::PlanStepException() throw() :
        TA_Base_Core::TransactiveException("Unknown exception"),
        m_stepType(TA_Base_Core::UNDEFINED_STEP)
	{
	}

	PlanStepException::PlanStepException(const std::string& msg, 
                                const TA_Base_Core::EStep stepType) throw() :
		TA_Base_Core::TransactiveException(msg),
        m_stepType(stepType)
	{
	}
	
	PlanStepException::~PlanStepException() throw()
	{
	}

} // TA_Base_Core

