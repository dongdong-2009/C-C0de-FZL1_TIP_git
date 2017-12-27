/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/PlanAgentMap.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Maps location keys to PlanAgentLibrary objects. Facilitates access to Plan Agent based on location.
  * 
  */

#ifdef __WIN32__
	#pragma warning(disable:4786)
	#pragma warning(disable:4250)
#endif

#include "bus/response_plans/active_plans_display/src/PlanAgentMap.h"
#include "bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"

namespace
{
    const unsigned short PLAN_AGENT_RESPONSE_TIMEOUT = 300;
}

using namespace TA_Base_Bus;


PlanAgentMap::PlanAgentMap()
{
}


PlanAgentMap::~PlanAgentMap()
{
    for (LocationToPlanAgentMap::const_iterator iter = m_locationToPlanAgentMap.begin(); iter != m_locationToPlanAgentMap.end(); iter++)
    {
        PlanAgentLibrary* planAgent = iter->second;
        delete planAgent;
    }

    m_locationToPlanAgentMap.clear(); 
}


PlanAgentLibrary& PlanAgentMap::getPlanAgent(unsigned long location)
{
    PlanAgentLibrary* planAgent = NULL;

    LocationToPlanAgentMap::const_iterator iter = m_locationToPlanAgentMap.find(location);
    if (iter != m_locationToPlanAgentMap.end())
    {
        planAgent = iter->second;

        return *planAgent;
    }

    try
    {
        PlanAgentLibrary* planAgent = new PlanAgentLibrary(location);
        planAgent->setPlanAgentTimeout(PLAN_AGENT_RESPONSE_TIMEOUT);        

        m_locationToPlanAgentMap.insert(std::make_pair(location, planAgent));

        return *planAgent;
    }
    catch (...)
    {
        delete planAgent;
        throw;
    }    
}


PlanAgentLibrary& PlanAgentMap::getDefaultPlanAgent()
{
    return getPlanAgent(PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation());
}
