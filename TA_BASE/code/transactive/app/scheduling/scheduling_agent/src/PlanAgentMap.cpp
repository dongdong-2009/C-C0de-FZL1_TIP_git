/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanAgentMap.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Maps location keys to PlanAgentLibrary objects. Facilitates access to Plan Agent based on location.
  * 
  */

#ifdef __WIN32__
	#pragma warning(disable:4786)
	#pragma warning(disable:4250)
#endif

#include "app/scheduling/scheduling_agent/src/PlanAgentMap.h"
#include "app/scheduling/scheduling_agent/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.h"

namespace TA_Base_App
{

const unsigned short PLAN_AGENT_RESPONSE_TIMEOUT = 300;


PlanAgentMap::PlanAgentMap()
{
}


PlanAgentMap::~PlanAgentMap()
{
    for (LocationToPlanAgentMap::const_iterator iter = m_locationToPlanAgentMap.begin(); iter != m_locationToPlanAgentMap.end(); iter++)
    {
        delete iter->second;
    }

    m_locationToPlanAgentMap.clear(); 
}


TA_Base_Bus::PlanAgentLibrary& PlanAgentMap::getPlanAgent(unsigned long location)
{

    LocationToPlanAgentMap::const_iterator iter = m_locationToPlanAgentMap.find(location);

    if (iter != m_locationToPlanAgentMap.end())
    {
        return *(iter->second);
    }

    TA_Base_Bus::PlanAgentLibrary* planAgent = NULL;

    try
    {
        planAgent = new TA_Base_Bus::PlanAgentLibrary(location);
        planAgent->setPlanAgentTimeout(PLAN_AGENT_RESPONSE_TIMEOUT);        

        m_locationToPlanAgentMap.insert(std::make_pair(location, planAgent));
    }
    catch (...)
    {
        delete planAgent;
        throw;
    }    

    return *planAgent;
}


TA_Base_Bus::PlanAgentLibrary& PlanAgentMap::getDefaultPlanAgent()
{
    return getPlanAgent(PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation());
}

}