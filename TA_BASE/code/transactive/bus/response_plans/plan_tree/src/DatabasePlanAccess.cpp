/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/DatabasePlanAccess.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// DatabasePlanAccess.cpp : implementation file
//

#include "bus\response_plans\PlanDataAccess\src\PlanDataReader.h"
#include "bus\response_plans\plan_tree\src\DatabasePlanAccess.h"

using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// DatabasePlanAccess

DatabasePlanAccess::DatabasePlanAccess(PlanFilter::PlanFilterType planFilterType) : 
    m_planFilter(planFilterType)
{
}


DatabasePlanAccess::DatabasePlanAccess(PlanFilter::PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterablePlans) :
    m_planFilter(planFilterType, nonFilterablePlans)
{
}



DatabasePlanAccess::~DatabasePlanAccess()
{
}


IPlanAccess::PlanNodeList DatabasePlanAccess::getRootChildren()
{
    TA_Base_Core::SiblingNodes_var rootChildren = PlanDataReader::instance()->getRootNodeChildren();

    IPlanAccess::PlanNodeList filteredChildren;
    filteredChildren.reserve(rootChildren->length());

    for (CORBA::ULong i = 0; i < rootChildren->length(); i++)
    {
        if (m_planFilter.isRelevantNode(rootChildren[i]))
        {
            filteredChildren.push_back(rootChildren[i]);
        }
    }

    return filteredChildren;
}


IPlanAccess::PlanNodeList DatabasePlanAccess::getChildren(TA_Base_Core::NodeId categoryId)
{
    TA_Base_Core::SiblingNodes_var children = PlanDataReader::instance()->getChildren(categoryId);

    IPlanAccess::PlanNodeList filteredChildren;
    filteredChildren.reserve(children->length());

    for (CORBA::ULong i = 0; i < children->length(); i++)
    {
        if (m_planFilter.isRelevantNode(children[i]))
        {
            filteredChildren.push_back(children[i]);
        }
    }

    return filteredChildren;
}


IPlanAccess::PlanIdList DatabasePlanAccess::getNodeIdsInPlanPath(TA_Base_Core::NodeId planId)
{
    TA_Base_Core::NodeIds_var nodeIds = PlanDataReader::instance()->getNodeIdsInNodePath(planId);

    IPlanAccess::PlanIdList planIds;
    planIds.reserve(nodeIds->length());

    for (CORBA::ULong i = 0; i < nodeIds->length(); i++)
    {
        planIds.push_back(nodeIds[i]);
    }

    return planIds;
}


std::string DatabasePlanAccess::getPlanPath(TA_Base_Core::NodeId planId)
{
    return PlanDataReader::instance()->getNodePath(planId);
}


TA_Base_Core::NodeId DatabasePlanAccess::getPlanId(const std::string& planPath)
{
    return PlanDataReader::instance()->getNodeId(planPath);
}
