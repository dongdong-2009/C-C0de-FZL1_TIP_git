/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/PlanFilter.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// PlanFilter.cpp : implementation file
//

#include "core\utilities\src\TAAssert.h"
#include "bus\response_plans\plan_tree\src\PlanFilter.h"

using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// PlanFilter

PlanFilter::PlanFilter() : 
    m_planFilterType(PlanFilter::PFT_ALL_PLANS)
{
    m_nonFilterableNodes.clear();
}


PlanFilter::PlanFilter(PlanFilter::PlanFilterType planFilterType) : 
    m_planFilterType(planFilterType)
{
    m_nonFilterableNodes.clear();
}


PlanFilter::PlanFilter(PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterableNodes) :
    m_planFilterType(planFilterType),
    m_nonFilterableNodes(nonFilterableNodes)
{
}


PlanFilter::~PlanFilter()
{
    m_nonFilterableNodes.clear();
}


void PlanFilter::setPlanFilterType(PlanFilter::PlanFilterType planFilterType)
{
    m_planFilterType = planFilterType;
}


PlanFilter::PlanFilterType PlanFilter::getPlanFilterType() const
{
    return m_planFilterType;
}


bool PlanFilter::isRelevantNode(const TA_Base_Core::NodeSummary& nodeSummary) const
{
    if (isFilterableNode(nodeSummary.node))
    {
        if (getPlanFilterType() == PlanFilter::PFT_APPROVED_PLANS)
        {
            switch (nodeSummary.type)
            {
                case TA_Base_Core::PLAN_NODE:
                    return (nodeSummary.approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE);

                case TA_Base_Core::CATEGORY_NODE:
                    return nodeSummary.hasApprovedDescendant;

                default:
                    TA_ASSERT(false, "Invalid node type");
            }
        }
    }

    return true;
}


bool PlanFilter::isFilterableNode(TA_Base_Core::NodeId nodeId) const
{
    for (std::vector<TA_Base_Core::NodeId>::const_iterator iter = m_nonFilterableNodes.begin(); iter != m_nonFilterableNodes.end(); iter++)
    {
        if (*iter == nodeId)
        {
            return false;
        }
    }

    return true;
}
