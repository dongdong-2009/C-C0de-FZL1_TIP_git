/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/AgentPlanAccess.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// AgentPlanAccess.cpp : implementation file
//

#include <sstream>
#include "core\exceptions\src\TransactiveException.h"
#include "core\exceptions\src\ObjectResolutionException.h"
#include "core\corba\src\CorbaUtil.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\TAAssert.h"
#include "bus\response_plans\plan_agent\src\PlanAgentLibrary.h"
#include "bus\response_plans\active_plans_display\src\PlanAgentLocationAccess.h"
#include "bus\response_plans\plan_tree\src\AgentPlanAccess.h"
#include "bus\response_plans\plan_tree\src\PlanAgentException.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// AgentPlanAccess

AgentPlanAccess::AgentPlanAccess(PlanFilter::PlanFilterType planFilterType) : 
    m_planAgentLib(NULL),
    m_planFilter(planFilterType)
{
    initialisePlanAgentLib();
}


AgentPlanAccess::AgentPlanAccess(PlanFilter::PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterablePlans) :
    m_planAgentLib(NULL),
    m_planFilter(planFilterType, nonFilterablePlans)
{
    initialisePlanAgentLib();
}


AgentPlanAccess::~AgentPlanAccess()
{
    delete m_planAgentLib;
}


void AgentPlanAccess::initialisePlanAgentLib()
{
    unsigned long location(0);

    try 
    {
        location = PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation();
    }
    catch (...)
    {
        location = atol(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
    }

    m_planAgentLib = new PlanAgentLibrary(location);
}


IPlanAccess::PlanNodeList AgentPlanAccess::getRootChildren()
{
    TA_ASSERT(m_planAgentLib != NULL, "Plan Agent library not initialised");

    try
    {
        TA_Base_Core::SiblingNodes_var rootChildren;
        m_planAgentLib->getRootNodeChildren(rootChildren);

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
    catch (TA_Base_Core::ObjectResolutionException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
            "Plan Agent access failure in getRootNodeChildren(). "
            "Caught ObjectResolutionException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve plan tree root contents." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getRootNodeChildren(). "
            "Caught TransactiveException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve plan tree root contents." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanAgentError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getRootNodeChildren(). "
            "Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve plan tree root contents." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (CORBA::Exception &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getRootNodeChildren(). "
            "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());//TD14337 yezh++

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve plan tree root contents." << std::endl << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getRootNodeChildren(). Unknown exception.");

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve plan tree root contents.";

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
}


IPlanAccess::PlanNodeList AgentPlanAccess::getChildren(TA_Base_Core::NodeId categoryId)
{
    TA_ASSERT(m_planAgentLib != NULL, "Plan Agent library not initialised");

    try
    {
        TA_Base_Core::SiblingNodes_var children;
        m_planAgentLib->getChildren(categoryId, children);

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
    catch (TA_Base_Core::ObjectResolutionException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
            "Plan Agent access failure in getChildren(). "
            "Caught ObjectResolutionException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve the contents of category ID " << categoryId << "." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getChildren(). "
            "Caught TransactiveException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve the contents of category ID " << categoryId << "." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanAgentError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getChildren(). "
            "Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve the contents of category ID " << categoryId << "." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (CORBA::Exception &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getChildren(). "
            "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());//TD14337 yezh++

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve the contents of category ID " << categoryId << "." << std::endl
                     << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getChildren(). Unknown exception.");

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve the contents of category ID " << categoryId << ".";

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
}


IPlanAccess::PlanIdList AgentPlanAccess::getNodeIdsInPlanPath(TA_Base_Core::NodeId planId)
{
    TA_ASSERT(m_planAgentLib != NULL, "Plan Agent library not initialised");

    try
    {
        TA_Base_Core::NodeIds_var nodeIds;
        m_planAgentLib->getNodeIdsInNodePath(planId, nodeIds);

        IPlanAccess::PlanIdList planIds;
        planIds.reserve(nodeIds->length());

        for (CORBA::ULong i = 0; i < nodeIds->length(); i++)
        {
            planIds.push_back(nodeIds[i]);
        }

        return planIds;
    }
    catch (TA_Base_Core::ObjectResolutionException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
            "Plan Agent access failure in getNodeIdsInNodePath(). "
            "Caught ObjectResolutionException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve node IDs in the path of plan ID " << planId << "." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeIdsInNodePath(). "
            "Caught TransactiveException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve node IDs in the path of plan ID " << planId << "." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanAgentError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeIdsInNodePath(). "
            "Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve node IDs in the path of plan ID " << planId << "." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanNotFoundError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeIdsInNodePath(). "
            "Caught IPlanAgentCorbaDef::PlanNotFoundError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve node IDs in the path of plan ID " << planId << "." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (CORBA::Exception &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeIdsInNodePath(). "
            "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());//TD14337 yezh++

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve node IDs in the path of plan ID " << planId << "." << std::endl
                     << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeIdsInNodePath(). Unknown exception.");

        std::ostringstream errorMessage;
        errorMessage << "Failed to retrieve node IDs in the path of plan ID " << planId << ".";

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
}


std::string AgentPlanAccess::getPlanPath(TA_Base_Core::NodeId planId)
{
    TA_ASSERT(m_planAgentLib != NULL, "Plan Agent library not initialised");

    try
    {
        TA_Base_Core::NodePath_var planPath;

        m_planAgentLib->getNodePath(planId, planPath);
     
        return planPath.in();
    }
    catch (TA_Base_Core::ObjectResolutionException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
            "Plan Agent access failure in getNodePath(). "
            "Caught ObjectResolutionException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan path for plan ID " << planId << "." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodePath(). "
            "Caught TransactiveException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan path for plan ID " << planId << "." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanAgentError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodePath(). "
            "Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan path for plan ID " << planId << "." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanNotFoundError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodePath(). "
            "Caught IPlanAgentCorbaDef::PlanNotFoundError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan path for plan ID " << planId << "." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (CORBA::Exception &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodePath(). "
            "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());//TD14337 yezh++

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan path for plan ID " << planId << "." << std::endl
                     << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodePath(). Unknown exception.");

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan path for plan ID " << planId << ".";

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
}


TA_Base_Core::NodeId AgentPlanAccess::getPlanId(const std::string& planPath)
{
    TA_ASSERT(m_planAgentLib != NULL, "Plan Agent library not initialised");

    try
    {
        return m_planAgentLib->getNodeId(planPath.c_str());
    }
    catch (TA_Base_Core::ObjectResolutionException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
            "Plan Agent access failure in getNodeId(). "
            "Caught ObjectResolutionException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan ID for plan '" << planPath << "'." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeId(). "
            "Caught TransactiveException: %s", ex.what());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan ID for plan '" << planPath << "'." << std::endl << ex.what();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanAgentError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeId(). "
            "Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan ID for plan '" << planPath << "'." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (IPlanAgentCorbaDef::PlanNotFoundError &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeId(). "
            "Caught IPlanAgentCorbaDef::PlanNotFoundError: %s", ex.what.in());

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan ID for plan '" << planPath << "'." << std::endl << ex.what.in();

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (CORBA::Exception &ex)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeId(). "
            "Caught CORBA::Exception: %s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());//TD14337 yezh++

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan ID for plan '" << planPath << "'." << std::endl
                     << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
    catch (...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
            "Plan Agent access failure in getNodeId(). Unknown exception.");

        std::ostringstream errorMessage;
        errorMessage << "Failed to determine plan ID for plan '" << planPath << "'.";

        TA_THROW(PlanAgentException(errorMessage.str()));
    }
}
