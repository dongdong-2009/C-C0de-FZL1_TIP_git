/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/PlanAgentException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Thrown by AgentPlanAccess when an error occurs while attempting to communicate with the Plan Agent.
  */

#include "bus\response_plans\plan_tree\src\PlanAgentException.h"


using namespace TA_Base_Bus;


PlanAgentException::PlanAgentException() throw()
{
}


PlanAgentException::PlanAgentException(const std::string& msg) throw() :
    TA_Base_Core::TransactiveException(msg)
{
}


PlanAgentException::~PlanAgentException() throw()
{
}
