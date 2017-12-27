/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/ComparablePlanInstance.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TA_Base_Core::ActivePlanDetail wrapper that allows two plan instances to be compared.
  *
  **/

#include "StdAfx.h"

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <math.h>
#include <map>
#include <string>
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/response_plans/active_plans_display/src/ComparablePlanInstance.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

std::map<TA_Base_Core::EActivePlanState, int> TA_Base_Bus::ComparablePlanInstance::m_execStatePriorityMap;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_Bus::ComparablePlanInstance::m_execStatePriorityMapLock;

using namespace TA_Base_Bus;


ComparablePlanInstance::ComparablePlanInstance(const TA_Base_Core::ActivePlanDetail& activePlanDetail) :
    m_activePlanDetail(activePlanDetail)
{
}


ComparablePlanInstance::ComparablePlanInstance(const ComparablePlanInstance& comparablePlanInstance)
{
    operator=(comparablePlanInstance);
}


ComparablePlanInstance::~ComparablePlanInstance()
{
}


ComparablePlanInstance& ComparablePlanInstance::operator=(const TA_Base_Core::ActivePlanDetail& activePlanDetail)
{
    m_activePlanDetail = activePlanDetail;

    return *this;
}


ComparablePlanInstance& ComparablePlanInstance::operator=(const ComparablePlanInstance& comparablePlanInstance)
{
    return operator=(comparablePlanInstance.m_activePlanDetail);
}


bool ComparablePlanInstance::operator==(const TA_Base_Core::ActivePlanDetail& activePlanDetail)
{
    // Compare plan paths
    if (std::string(m_activePlanDetail.path.in()) != std::string(activePlanDetail.path.in()))
    {
        return false;
    }

    // Compare locations
    if (m_activePlanDetail.plan.location != activePlanDetail.plan.location)
    {
        return false;
    }

    // Compare instance numbers
    if (m_activePlanDetail.plan.instance != activePlanDetail.plan.instance)
    {
        return false;
    }

    // Compare activation times
    if (m_activePlanDetail.instantiationTime != activePlanDetail.instantiationTime)
    {
        return false;
    }
    
    return true;
}


bool ComparablePlanInstance::operator==(const ComparablePlanInstance& comparablePlanInstance)
{
    return operator==(comparablePlanInstance.m_activePlanDetail);
}


bool ComparablePlanInstance::operator<(const TA_Base_Core::ActivePlanDetail& activePlanDetail)
{
    // 1. Compare execution states. Skip if either execution state is not comparable.
    int thisExecutionStatePriority(getExecutionStatePriority(m_activePlanDetail.activePlanState));
    int otherExecutionStatePriority(getExecutionStatePriority(activePlanDetail.activePlanState));

    if (thisExecutionStatePriority < otherExecutionStatePriority)
    {
        return true;
    }

    if (thisExecutionStatePriority > otherExecutionStatePriority)
    {
        return false;
    }

    // 2. Execution state priorities are identical. Compare activation times. More recent takes precedence.
    if (m_activePlanDetail.instantiationTime > activePlanDetail.instantiationTime)
    {
        return true;
    }

    if (m_activePlanDetail.instantiationTime < activePlanDetail.instantiationTime)
    {
        return false;
    }

    // 3. Activation times are identical. Compare plan names and, if necessary, plan categories.
    if (m_activePlanDetail.plan.plan != activePlanDetail.plan.plan)
    {
        std::string thisPlanName((LPCTSTR) StringUtilities::extractPlanName(m_activePlanDetail.path.in()));
        std::string otherPlanName((LPCTSTR) StringUtilities::extractPlanName(activePlanDetail.path.in()));
      
        if (thisPlanName < otherPlanName)
        {
            return true;
        }

        if (thisPlanName > otherPlanName)
        {
            return false;
        }

        std::string thisCategoryPath((LPCTSTR) StringUtilities::extractCategoryPath(m_activePlanDetail.path.in()));
        std::string otherCategoryPath((LPCTSTR) StringUtilities::extractCategoryPath(activePlanDetail.path.in()));
      
        if (thisCategoryPath < otherCategoryPath)
        {
            return true;
        }

        if (thisCategoryPath > otherCategoryPath)
        {
            return false;
        }
    }

    // 4. Plan paths are identical. Compare location names.
    if (m_activePlanDetail.plan.location != activePlanDetail.plan.location)
    {
        std::string thisLocationName(PlanAgentLocationAccess::getInstance().getLocationName(m_activePlanDetail.plan.location));
        std::string otherLocationName(PlanAgentLocationAccess::getInstance().getLocationName(activePlanDetail.plan.location));

        if (thisLocationName < otherLocationName)
        {
            return true;
        }
        
        if (thisLocationName > otherLocationName)
        {
            return false;
        }        
    }

    // 5. Locations are identical. Compare instance numbers. The instance with the higher number should take
    // precedence as it would be more recent of two instances of the same plan activated at the same time.
    if (m_activePlanDetail.plan.instance > activePlanDetail.plan.instance)
    {
        return true;
    }
    
    return false;
}


bool ComparablePlanInstance::operator<(const ComparablePlanInstance& comparablePlanInstance)
{
    return operator<(comparablePlanInstance.m_activePlanDetail);
}


bool ComparablePlanInstance::operator>(const TA_Base_Core::ActivePlanDetail& activePlanDetail)
{
    return (false == operator==(activePlanDetail) && false == operator<(activePlanDetail));
}


bool ComparablePlanInstance::operator>(const ComparablePlanInstance& comparablePlanInstance)
{
    return operator>(comparablePlanInstance.m_activePlanDetail);
}


const TA_Base_Core::ActivePlanDetail& ComparablePlanInstance::getActivePlanDetail() const
{
    return m_activePlanDetail;
}


int ComparablePlanInstance::getExecutionStatePriority(const TA_Base_Core::EActivePlanState& executionState)
{
    TA_Base_Core::ThreadGuard mapGuard(m_execStatePriorityMapLock);

    // Map instance execution state to its priority. The lower the value the higher the priority.
    // Note: Running and Delayed states have equal priority.
    if (m_execStatePriorityMap.empty())
    {
        int priority(m_execStatePriorityMap.size());

        m_execStatePriorityMap.insert(std::make_pair(TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE, priority));
        m_execStatePriorityMap.insert(std::make_pair(TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE,   priority));
        m_execStatePriorityMap.insert(std::make_pair(TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE,    ++priority));
        m_execStatePriorityMap.insert(std::make_pair(TA_Base_Core::PENDING_ACTIVE_PLAN_STATE,   ++priority));
        m_execStatePriorityMap.insert(std::make_pair(TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, ++priority));
        m_execStatePriorityMap.insert(std::make_pair(TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE,   ++priority));
        m_execStatePriorityMap.insert(std::make_pair(TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE,   ++priority));
    }

    std::map<TA_Base_Core::EActivePlanState, int>::const_iterator iter = m_execStatePriorityMap.find(executionState);

    if (iter != m_execStatePriorityMap.end())
    {
        return iter->second;
    }

    // Unknown execution state. Let it take priority.
    return -1;   
}


int ComparablePlanInstance::compareUpdateIds(TA_Base_Core::UpdateId updateId1, TA_Base_Core::UpdateId updateId2)
{
    static const double WRAPAROUND_GAP = (TA_Base_Core::MAX_UPDATE_ID - TA_Base_Core::MIN_UPDATE_ID) / 2;

    if (updateId2 == updateId1)
    {
        return 0;
    }

    // If update ID has wrapped around
    if (::fabs((double) updateId2 - (double) updateId1) > WRAPAROUND_GAP)
    {
        return (updateId1 < updateId2? 1 : -1);
    }

    return (updateId1 > updateId2? 1 : -1);
}
