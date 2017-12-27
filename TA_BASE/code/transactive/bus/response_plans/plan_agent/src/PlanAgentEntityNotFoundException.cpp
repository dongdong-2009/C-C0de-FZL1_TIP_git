//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/19 17:43:23 $
// Last modified by:  $Author: CM $
//
// Thrown by PlanAgentLibrary when no entity of type PlanAgent can be found at the specified location.

#include "PlanAgentEntityNotFoundException.h"

using namespace TA_Base_Bus;


PlanAgentEntityNotFoundException::PlanAgentEntityNotFoundException() throw() : 
    TA_Base_Core::TransactiveException()
{
}


PlanAgentEntityNotFoundException::PlanAgentEntityNotFoundException(std::string msg) throw() :
    TA_Base_Core::TransactiveException(msg)
{
}


PlanAgentEntityNotFoundException::~PlanAgentEntityNotFoundException() throw()
{
}
