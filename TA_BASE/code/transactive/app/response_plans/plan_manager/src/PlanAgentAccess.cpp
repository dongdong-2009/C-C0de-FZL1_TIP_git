/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Thread Safe Singleton to wrap access to the Plan Agent's CORBA Interface
  */

#include "stdafx.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.h"
#include "PlanAgentAccess.h"
#include "TreeNode.h"
#include "TreeNodeFactory.h"
#include "Utilities.h"

#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

//#include "PlanAgentAccessException.h"
//#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
//#include "core/exceptions/src/PlanStepDetailException.h"
//#include "core/exceptions/src/RightsException.h"
//#include "core/exceptions/src/PlanAgentException.h"
//#define PAA_BEGIN_TRY    try
//
//#define PAA_END_TRY(paaFn)                                                           \
//catch (const PlanAgentAccessException& ex)                                                        \
//{                                                                                                                \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught PlanAgentAccessException by ISMAR: %s", ex.what());\
//    TA_THROW (PlanAgentAccessException (ex.what(), ex.getDetail ()))                                                        \
//}                                                                                                                \
//catch (const TA_Base_Core::ObjectResolutionException &ex)                                 \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught ObjectResolutionException: %s", ex.what());                          \
//    throw;          \
//}                                                                                    \
//catch (const TA_Base_Core::TransactiveException &ex)                                      \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught TransactiveException: %s", ex.what());                               \
//    throw;                                                                           \
//}                                                                                    \
//catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError &ex)                         \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());              \
//    TA_THROW(TA_Base_Core::PlanAgentException(ex.what.in(), ex.errorType, ex.itemId));    \
//}                                                                                    \
//catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError &ex)                      \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught IPlanAgentCorbaDef::PlanNotFound: %s", ex.what.in());                \
//    TA_THROW(TA_Base_App::PlanNotFoundException(ex.what.in(), ex.errorType, ex.planId, ex.instanceId)); \
//}                                                                                    \
//catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanStepDetailError &ex)                    \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught IPlanAgentCorbaDef::PlanStepDetailError: %s", ex.what.in());         \
//    TA_THROW(TA_Base_Core::PlanStepDetailException(ex.what.in(), ex.detailType, ex.errorType, ex.stepId)); \
//}                                                                                    \
//catch (const TA_Base_Bus::IPlanAgentCorbaDef::NoPermission &ex)                           \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught IPlanAgentCorbaDef::NoPermission: %s", ex.what.in());                \
//    TA_THROW(TA_Base_Core::RightsException(ex.what.in()));                                \
//}                                                                                    \
//catch (const CORBA::Exception &ex)                                                   \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). "                                  \
//        "Caught CORBA::Exception: %s",                                               \
//        TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());            \
//    TA_THROW(TA_Base_Core::TransactiveException(std::string("CORBA::Exception: ") +       \
//             TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)));              \
//}                                                                                    \
//catch (...)                                                                          \
//{                                                                                    \
//    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
//        "Plan Agent access failure in "#paaFn"(). Unknown exception.");              \
//    TA_THROW(TA_Base_Core::TransactiveException("Unknown error"));                        \
//}

// Initialise Statics
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::PlanAgentAccess::m_singletonLock;
TA_Base_App::PlanAgentAccess* TA_Base_App::PlanAgentAccess::m_thePlanAgentAccess = NULL;


using namespace TA_Base_App;

//TD15049++
//PLAN_AGENT_RESPONSE_TIMEOUT = 20;
//const unsigned short PLAN_AGENT_LONG_RESPONSE_TIMEOUT = 300;
//++TD15049


////////////////////////////////////////////////////////////////////////////
// PlanAgentAccess

PlanAgentAccess::PlanAgentAccess()
{
    FUNCTION_ENTRY( "PlanAgentAccess" );

    m_sessionID.erase();

    FUNCTION_EXIT;
}


PlanAgentAccess::~PlanAgentAccess()
{
    FUNCTION_ENTRY( "~PlanAgentAccess" );

    std::vector<Timer*> timers;
    m_editLockTimerMap.getKeys(timers);

    for (std::vector<Timer*>::iterator iter = timers.begin(); iter != timers.end(); iter++)
    {
        Timer *timer = *iter;

        delete timer;
        timer = NULL;
    }
    timers.clear();
    m_editLockTimerMap.clear();

    m_thePlanAgentAccess = NULL;

    FUNCTION_EXIT;
}


void PlanAgentAccess::timerExpired(Timer &timer)
{
    FUNCTION_ENTRY( "timerExpired" );

    // It is time to refresh the edit lock on the node associated with the elapsed timer.
    TA_ASSERT(m_editLockTimerMap.find(&timer), "Expired timer not found in edit lock refresh timer map");

    TA_Base_Core::NodeId nodeId = m_editLockTimerMap[&timer];

    try
    {
          // Guard the method to ensure access by client code is synchronous
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

        PAA_BEGIN_TRY
        {
            // TD15049++
            m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_REGISTER_TIMEOUT);
            // ++TD15049
            m_planAgentMap.getDefaultPlanAgent().getEditPlanLock(getSessionId().c_str(),
                                                                 Utilities::getPlanManagerId().c_str(),
                                                                 nodeId);
        }
        PAA_END_TRY(getEditPlanLock);
    }
    catch (...)
    {
        TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(nodeId);
        TA_ASSERT(treeNode != NULL, "Invalid tree node");

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
            "Edit lock refresh failed for node ID %d ('%s').", nodeId, treeNode->getNodeName().c_str());
    }

    FUNCTION_EXIT;
}


Timer *PlanAgentAccess::getEditLockTimer(TA_Base_Core::NodeId nodeId)
{
    FUNCTION_ENTRY( "getEditLockTimer" );

    std::vector<Timer*> timers;
    m_editLockTimerMap.getKeys(timers);

    for (std::vector<Timer*>::iterator iter = timers.begin(); iter != timers.end(); iter++)
    {
        Timer *timer = *iter;

        if (nodeId == m_editLockTimerMap[timer])
        {
            FUNCTION_EXIT;
            return timer;
        }
    }

    FUNCTION_EXIT;
    return NULL;
}


std::string PlanAgentAccess::getSessionId( bool useRunparam ) const
{
    FUNCTION_ENTRY( "getSessionId" );

    // TD 10849 Store the sessionID for use after the runParam
    // sessionID has changed. Refresh the stored sessionID if
    // the caller has specifically requested the runparam value.
    if (! (useRunparam || m_sessionID.empty() ) )
    {
        FUNCTION_EXIT;
        return m_sessionID;
    }
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");
    m_sessionID = sessionId;

    FUNCTION_EXIT;
    return sessionId;
}


PlanAgentAccess& PlanAgentAccess::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    // The first call to this function will create the object,
    // thereafter it will return the object
    if ( m_thePlanAgentAccess == NULL )
    {
        // Guard the method to ensure access by client code is synchronous
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

        if ( m_thePlanAgentAccess == NULL )
        {
            m_thePlanAgentAccess = new PlanAgentAccess();
        }
    }

    FUNCTION_EXIT;
    return *m_thePlanAgentAccess;
}


void PlanAgentAccess::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    if ( NULL != m_thePlanAgentAccess )
    {
        delete m_thePlanAgentAccess;

        m_thePlanAgentAccess = NULL;
    }

    FUNCTION_EXIT;
}


TA_Base_Core::SiblingNodes *PlanAgentAccess::getRootChildNodes()
{
    FUNCTION_ENTRY( "getRootChildNodes" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::SiblingNodes_var childNodeSeqVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getRootNodeChildren(childNodeSeqVar.out());

        FUNCTION_EXIT;
        return childNodeSeqVar._retn();
    }
    PAA_END_TRY(getRootNodeChildren);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::SiblingNodes *PlanAgentAccess::getChildNodes(TA_Base_Core::NodeId parentNodeId)
{
    FUNCTION_ENTRY( "getChildNodes" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::SiblingNodes_var childNodeSeqVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getChildren(parentNodeId, childNodeSeqVar.out());

        FUNCTION_EXIT;
        return childNodeSeqVar._retn();
    }
    PAA_END_TRY(getChildren);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::NodeSummary *PlanAgentAccess::getNodeSummary(TA_Base_Core::NodeId nodeId)
{
    FUNCTION_ENTRY( "getNodeSummary" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::NodeSummary_var nodeSummaryVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getNodeSummary(nodeId, nodeSummaryVar.out());

        FUNCTION_EXIT;
        return nodeSummaryVar._retn();
    }
    PAA_END_TRY(getNodeSummary);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::NodeDetail *PlanAgentAccess::getNodeDetail(TA_Base_Core::NodeId nodeId)
{
    FUNCTION_ENTRY( "getNodeDetail" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::NodeDetail_var nodeDetailVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getNodeDetail(nodeId, nodeDetailVar.out());

        FUNCTION_EXIT;
        return nodeDetailVar._retn();
    }
    PAA_END_TRY(getNodeDetail);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::StepDetails *PlanAgentAccess::getStepDetails(TA_Base_Core::NodeId planNodeId)
{
    FUNCTION_ENTRY( "getStepDetails" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::StepDetails_var stepDetailsVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getAllStepDetails(planNodeId, stepDetailsVar.out());

        FUNCTION_EXIT;
        return stepDetailsVar._retn();
    }
    PAA_END_TRY(getAllStepDetails);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::ParameterNames *PlanAgentAccess::getParameterNames(TA_Base_Core::EStep stepType)
{
    FUNCTION_ENTRY( "getParameterNames" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::ParameterNames_var stepParameterNamesVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getParameterNames(stepType, stepParameterNamesVar.out());

        FUNCTION_EXIT;
        return stepParameterNamesVar._retn();
    }
    PAA_END_TRY(getParameterNames);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::StepParameters *PlanAgentAccess::getStepParameters(TA_Base_Core::StepId stepId)
{
    FUNCTION_ENTRY( "getStepParameters" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::StepParameters_var stepParametersVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getStepIdParameters(stepId, stepParametersVar.out());

        FUNCTION_EXIT;
        return stepParametersVar._retn();
    }
    PAA_END_TRY(getStepIdParameters);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::StepParameters *PlanAgentAccess::getStepParameters(TA_Base_Core::NodeId planNodeId, TA_Base_Core::StepNumber stepNumber)
{
    FUNCTION_ENTRY( "getStepParameters" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::StepParameters_var stepParametersVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getStepNumberParameters(planNodeId, stepNumber, stepParametersVar.out());

        FUNCTION_EXIT;
        return stepParametersVar._retn();
    }
    PAA_END_TRY(getStepNumberParameters);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::NodeId PlanAgentAccess::getNodeId(std::string nodePath)
{
    FUNCTION_ENTRY( "getNodeId" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // Ensure the path has a leading slash.
        if (!nodePath.empty() && nodePath[0] != '/')
        {
            nodePath = '/' + nodePath;
        }

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        FUNCTION_EXIT;
        return m_planAgentMap.getDefaultPlanAgent().getNodeId(const_cast<TA_Base_Core::NodePath>(nodePath.c_str()));
    }
    PAA_END_TRY(getNodeId);
}


std::string PlanAgentAccess::getNodePath(TA_Base_Core::NodeId nodeId)
{
    FUNCTION_ENTRY( "getNodePath" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::NodePath_var nodePathVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getNodePath(nodeId, nodePathVar.out());

        FUNCTION_EXIT;
        return nodePathVar.in();
    }
    PAA_END_TRY(getNodePath);
}


TA_Base_Core::NodeIds *PlanAgentAccess::getNodeIdsInNodePath(TA_Base_Core::NodeId nodeId)
{
    FUNCTION_ENTRY( "getNodeIdsInNodePath" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::NodeIds_var nodeIdsVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getNodeIdsInNodePath(nodeId, nodeIdsVar.out());

        FUNCTION_EXIT;
        return nodeIdsVar._retn();
    }
    PAA_END_TRY(getNodeIdsInNodePath);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::StepNumber PlanAgentAccess::getStepNumber(TA_Base_Core::StepId stepId)
{
    FUNCTION_ENTRY( "getStepNumber" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::StepNumber stepNumber;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getStepNumber(stepId, stepNumber);

        FUNCTION_EXIT;
        return stepNumber;
    }
    PAA_END_TRY(getStepNumber);
}


TA_Base_Core::ActivePlanDetails *PlanAgentAccess::getActivePlanDetails(TA_Base_Core::NodeId planNodeId)
{
    FUNCTION_ENTRY( "getActivePlanDetails" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::ActivePlanDetails_var activePlanDetailsVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getActivePlanDetail(planNodeId, activePlanDetailsVar.out());

        FUNCTION_EXIT;
        return activePlanDetailsVar._retn();
    }
    PAA_END_TRY(getActivePlanDetail);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::ActivePlanDetail* PlanAgentAccess::getAssocAlarmPlanDetail( const std::string& alarmId, const unsigned long ulLocation )
{
    FUNCTION_ENTRY( "getAssocAlarmPlanDetail" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::ActivePlanDetail_var activePlanDetailVar;

        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);

        m_planAgentMap.getPlanAgent( ulLocation ).getAssocAlarmPlanDetail( alarmId.c_str(), activePlanDetailVar.out() );

        FUNCTION_EXIT;
        return activePlanDetailVar._retn();
    }
    PAA_END_TRY(getActivePlanDetail);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::ActivePlanDetails *PlanAgentAccess::getAllActivePlanDetails()
{
    FUNCTION_ENTRY( "getAllActivePlanDetails" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::ActivePlanDetails_var activePlanDetailsVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getAllActivePlanDetails(activePlanDetailsVar.out());

        FUNCTION_EXIT;
        return activePlanDetailsVar._retn();
    }
    PAA_END_TRY(getAllActivePlanDetails);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::ActivePlanDetails *PlanAgentAccess::getActivePlanDetailsAtLocation(TA_Base_Core::NodeId planNodeId, unsigned long location)
{
    FUNCTION_ENTRY( "getActivePlanDetailsAtLocation" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        try
        {
            TA_Base_Core::ActivePlanDetails_var activePlanDetailsVar;

            // TD15049++
            m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
            // ++TD15049
            m_planAgentMap.getPlanAgent(location).getActivePlanDetail(planNodeId, activePlanDetailsVar.out());

            FUNCTION_EXIT;
            return activePlanDetailsVar._retn();
        }
        catch (const TA_Base_Bus::PlanAgentEntityNotFoundException &)
        {
            // No Plan Agent configured for the specified location. Ignore it.

            TA_Base_Core::ActivePlanDetails *empty = new TA_Base_Core::ActivePlanDetails();
            empty->length(0);

            FUNCTION_EXIT;
            return empty;
        }
        catch (const TA_Base_Core::ObjectResolutionException& ex)
        {   //as per TD 13167
            CString tmp;
            tmp.Format ("%u", location);
            std::string locationId (tmp.GetBuffer (1));
            TA_THROW (PlanAgentAccessException (ex.what (), locationId));
        }

    }
    PAA_END_TRY(getActivePlanDetail);

   FUNCTION_EXIT;
   return NULL;
}


TA_Base_Core::ActivePlanDetails *PlanAgentAccess::getAllActivePlanDetailsAtLocation(unsigned long location)
{
    FUNCTION_ENTRY( "getAllActivePlanDetailsAtLocation" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        try
        {
            TA_Base_Core::ActivePlanDetails_var activePlanDetailsVar;

            // TD15049++
            m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
            // ++TD15049
            m_planAgentMap.getPlanAgent(location).getAllActivePlanDetails(activePlanDetailsVar.out());

            FUNCTION_EXIT;
            return activePlanDetailsVar._retn();
        }
        catch (const TA_Base_Bus::PlanAgentEntityNotFoundException &)
        {
            // No Plan Agent configured for the specified location. Ignore it.
            TA_Base_Core::ActivePlanDetails *empty = new TA_Base_Core::ActivePlanDetails();
            empty->length(0);

            FUNCTION_EXIT;
            return empty;
        }
    }
    PAA_END_TRY(getAllActivePlanDetails);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::ExecutionHistory *PlanAgentAccess::getExecutionHistory(TA_Base_Core::ActivePlanId activePlanId)
{
    FUNCTION_ENTRY( "getExecutionHistory" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::ExecutionHistory_var executionHistoryVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).getPlanExecutionHistory(activePlanId, executionHistoryVar.out());

        FUNCTION_EXIT;
        return executionHistoryVar._retn();
    }
    PAA_END_TRY(getPlanExecutionHistory);

    FUNCTION_EXIT;
    return NULL;
}


TA_Base_Core::CustomisationHistory *PlanAgentAccess::getCustomisationHistory(TA_Base_Core::ActivePlanId activePlanId)
{
    FUNCTION_ENTRY( "getCustomisationHistory" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::CustomisationHistory_var customisationHistoryVar;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).getPlanCustomisationHistory(activePlanId, customisationHistoryVar.out());

        FUNCTION_EXIT;
        return customisationHistoryVar._retn();
    }
    PAA_END_TRY(getPlanCustomisationHistory);

    FUNCTION_EXIT;
    return NULL;
}


bool PlanAgentAccess::isOwner(TA_Base_Core::ActivePlanId activePlanId)
{
    FUNCTION_ENTRY( "isOwner" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        try
        {
            // TD15049++
            m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
            // ++TD15049
            FUNCTION_EXIT;
            return m_planAgentMap.getPlanAgent(activePlanId.location).isOwner(getSessionId().c_str(), activePlanId);
        }
        catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError &ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError", ex.what.in());

            // Instance must have expired. As such it no longer has an owner.
            FUNCTION_EXIT;
            return false;
        }
    }
    PAA_END_TRY(isOwner);

    FUNCTION_EXIT;
    return false;
}


void PlanAgentAccess::startPlan(TA_Base_Core::NodeId planNodeId)
{
    FUNCTION_ENTRY( "startPlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::ActivePlanId activePlanId;
        activePlanId.plan = planNodeId;
        activePlanId.instance = 0u; // unused
        activePlanId.triggerEntity = 0u;

        // TD13521
        // marvin++
        activePlanId.entitykey = -1;
        // TD13521
        // ++marvin

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().runOperatorTriggeredPlan(getSessionId().c_str(),
                                                                      Utilities::getPlanManagerId().c_str(),
                                                                      activePlanId);
    }
    PAA_END_TRY(runOperatorTriggeredPlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::customisePlan(TA_Base_Core::NodeId planNodeId)
{
    FUNCTION_ENTRY( "customisePlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::ActivePlanId activePlanId;
        activePlanId.plan = planNodeId;
        activePlanId.instance = 0; // unused

        // TD13521
        // marvin++
        activePlanId.entitykey = -1;
        // TD13521
        // ++marvin

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().customisePlan(getSessionId().c_str(),
                                                           Utilities::getPlanManagerId().c_str(),
                                                           activePlanId);
    }
    PAA_END_TRY(customisePlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::pausePlan(TA_Base_Core::ActivePlanId activePlanId)
{
    FUNCTION_ENTRY( "pausePlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).pausePlan(getSessionId().c_str(), activePlanId);
    }
    PAA_END_TRY(pausePlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::resumePlan(TA_Base_Core::ActivePlanId activePlanId)
{
    FUNCTION_ENTRY( "resumePlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).resumePlan(getSessionId().c_str(), activePlanId);
    }
    PAA_END_TRY(resumePlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::stopPlan(TA_Base_Core::ActivePlanId activePlanId)
{
    FUNCTION_ENTRY( "stopPlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).stopPlan(getSessionId().c_str(), activePlanId);
    }
    PAA_END_TRY(stopPlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::takeControlOfPlan(TA_Base_Core::ActivePlanId activePlanId)
{
    FUNCTION_ENTRY( "takeControlOfPlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).takeControl(getSessionId().c_str(),
                                                                       Utilities::getPlanManagerId().c_str(),
                                                                       activePlanId);
    }
    PAA_END_TRY(takeControl);

    FUNCTION_EXIT;
}


void PlanAgentAccess::customiseStep(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::StepCustomisableDetail customStepDetail)
{
    FUNCTION_ENTRY( "customiseStep" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).customiseStep(getSessionId().c_str(), activePlanId, customStepDetail);
    }
    PAA_END_TRY(customiseStep);

    FUNCTION_EXIT;
}


void PlanAgentAccess::reportDecisionStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status)
{
    FUNCTION_ENTRY( "reportDecisionStepResult" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).processDecisionStepResponse(getSessionId().c_str(), activePlanId, status);
    }
    PAA_END_TRY(reportDecisionStepResult);

    FUNCTION_EXIT;
}


void PlanAgentAccess::reportPromptStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status)
{
    FUNCTION_ENTRY( "reportPromptStepResult" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).processPromptStepResponse(getSessionId().c_str(), activePlanId, status);
    }
    PAA_END_TRY(reportPromptStepResult);

    FUNCTION_EXIT;
}


void PlanAgentAccess::reportLogStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, std::string logMessage)
{
    FUNCTION_ENTRY( "reportLogStepResult" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).processLogStepResponse(getSessionId().c_str(), activePlanId, status, logMessage.c_str());
    }
    PAA_END_TRY(reportLogStepResult);

    FUNCTION_EXIT;
}


void PlanAgentAccess::reportFailureSummaryStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
{
    FUNCTION_ENTRY( "reportFailureSummaryStepResult" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
         m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        m_planAgentMap.getPlanAgent(activePlanId.location).processFailureSummaryStepResult(getSessionId().c_str(), activePlanId, status, error.c_str());
    }
    PAA_END_TRY(reportFailureSummaryStepResult);

    FUNCTION_EXIT;
}


//++lizettejl

void PlanAgentAccess::reportRunPlanStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
{
    FUNCTION_ENTRY( "reportRunPlanStepResult" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).processRunPlanStepResult(getSessionId().c_str(), activePlanId, status, error.c_str());
    }
    PAA_END_TRY(reportPromptStepResult);

    FUNCTION_EXIT;
}


void PlanAgentAccess::reportLaunchGuiAppStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
{
    FUNCTION_ENTRY( "reportLaunchGuiAppStepResult" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).processLaunchGuiAppStepResult(getSessionId().c_str(), activePlanId, status, error.c_str());
    }
    PAA_END_TRY(reportPromptStepResult);

    FUNCTION_EXIT;
}


void PlanAgentAccess::reportLaunchSchematicStepResult(TA_Base_Core::ActivePlanId activePlanId, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
{
    FUNCTION_ENTRY( "reportLaunchSchematicStepResult" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getPlanAgent(activePlanId.location).processLaunchSchematicStepResult(getSessionId().c_str(), activePlanId, status, error.c_str());
    }
    PAA_END_TRY(reportPromptStepResult);

    FUNCTION_EXIT;
}


void PlanAgentAccess::setPlanApprovalState(TA_Base_Core::NodeId planNodeId, TA_Base_Core::EApprovalState state)
{
    FUNCTION_ENTRY( "setPlanApprovalState" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().setPlanApprovalState(getSessionId().c_str(), planNodeId, state);
    }
    PAA_END_TRY(setPlanApprovalState);

    FUNCTION_EXIT;
}

void PlanAgentAccess::setPlanType(TA_Base_Core::NodeId planNodeId, TA_Base_Core::EPlanType state)
{
    FUNCTION_ENTRY( "setPlanVisibleState" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().setPlanType(getSessionId().c_str(), planNodeId, state);
    }
    PAA_END_TRY(setPlanVisibleState);

    FUNCTION_EXIT;
}


void PlanAgentAccess::getPlanEditingLock(TA_Base_Core::NodeId nodeId)
{
    FUNCTION_ENTRY( "getPlanEditingLock" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getEditPlanLock(getSessionId().c_str(),
                                                             Utilities::getPlanManagerId().c_str(),
                                                             nodeId);
    }
    PAA_END_TRY(getEditPlanLock);

    // If necessary start a lock refresh timer for this node
    Timer *timer = getEditLockTimer(nodeId);
    if (!timer)
    {
        timer = new Timer();
        timer->subscribe(this);

        static const unsigned int PLANEDITLOCK_TIMEOUT_SECONDS = 60u;
        timer->start((PLANEDITLOCK_TIMEOUT_SECONDS / 2) * 1000);

        m_editLockTimerMap.insert(timer, nodeId);
    }

    FUNCTION_EXIT;
}


void PlanAgentAccess::releasePlanEditingLock(TA_Base_Core::NodeId planNodeId)
{
    FUNCTION_ENTRY( "releasePlanEditingLock" );

    // Stop the lock refresh timer for this node
    Timer *timer = getEditLockTimer(planNodeId);
    TA_ASSERT(timer != NULL, "Attempt to release an edit lock without an associated refresh timer");

    m_editLockTimerMap.erase(timer);
    timer->stop();
    timer->unsubscribe();
    delete timer;

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().releaseEditPlanLock(getSessionId().c_str(), planNodeId);
    }
    PAA_END_TRY(releaseEditPlanLock);

    FUNCTION_EXIT;
}


TA_Base_Core::StepId PlanAgentAccess::getNextStepId()
{
    FUNCTION_ENTRY( "getNextStepId" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        TA_Base_Core::StepId   stepId;

        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_MEDIUM_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().getNextStepId(getSessionId().c_str(), stepId);

        FUNCTION_EXIT;
        return stepId;
    }
    PAA_END_TRY(releaseEditPlanLock);
}


void PlanAgentAccess::addPlan(TA_Base_Core::NodeId categoryNodeId)
{
    FUNCTION_ENTRY( "addPlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().insertPlan(getSessionId().c_str(), categoryNodeId);
    }
    PAA_END_TRY(insertPlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::addPlanToRoot()
{
    FUNCTION_ENTRY( "addPlanToRoot" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().insertPlanIntoRoot(getSessionId().c_str());
    }
    PAA_END_TRY(insertPlanIntoRoot);

    FUNCTION_EXIT;
}


void PlanAgentAccess::deletePlan(TA_Base_Core::NodeId planNodeId)
{
    FUNCTION_ENTRY( "deletePlan" );

    TA_Base_Core::NodeIds planNodeIds;
    planNodeIds.length(1);
    planNodeIds[0] = planNodeId;

    deletePlans(planNodeIds);

    FUNCTION_EXIT;
}


void PlanAgentAccess::deletePlans(TA_Base_Core::NodeIds planNodeIds)
{
    FUNCTION_ENTRY( "deletePlans" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().deletePlans(getSessionId().c_str(), planNodeIds);
    }
    PAA_END_TRY(deletePlans);

    FUNCTION_EXIT;
}


void PlanAgentAccess::movePlan(TA_Base_Core::NodeId planNodeId, TA_Base_Core::NodeId categoryNodeId, bool overwrite)
{
    FUNCTION_ENTRY( "movePlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().movePlan(getSessionId().c_str(), planNodeId, categoryNodeId, overwrite);
    }
    PAA_END_TRY(movePlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::movePlanToRoot(TA_Base_Core::NodeId planNodeId, bool overwrite)
{
    FUNCTION_ENTRY( "movePlanToRoot" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().movePlanToRoot(getSessionId().c_str(), planNodeId, overwrite);
    }
    PAA_END_TRY(movePlanToRoot);

    FUNCTION_EXIT;
}


void PlanAgentAccess::copyPlan(TA_Base_Core::NodeId planNodeId, TA_Base_Core::NodeId categoryNodeId, bool overwrite)
{
    FUNCTION_ENTRY( "copyPlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().copyPlan(getSessionId().c_str(), planNodeId, categoryNodeId, overwrite);
    }
    PAA_END_TRY(copyPlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::copyPlanToRoot(TA_Base_Core::NodeId planNodeId, bool overwrite)
{
    FUNCTION_ENTRY( "copyPlanToRoot" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().copyPlanToRoot(getSessionId().c_str(), planNodeId, overwrite);
    }
    PAA_END_TRY(copyPlanToRoot);

    FUNCTION_EXIT;
}


void PlanAgentAccess::updatePlan(TA_Base_Core::Plan plan)
{
    FUNCTION_ENTRY( "updatePlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().updatePlan(getSessionId().c_str(), plan);
    }
    PAA_END_TRY(updatePlan);

    FUNCTION_EXIT;
}


void PlanAgentAccess::addCategory(TA_Base_Core::NodeId categoryNodeId)
{
    FUNCTION_ENTRY( "addCategory" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().insertCategory(getSessionId().c_str(), categoryNodeId);
    }
    PAA_END_TRY(insertCategory);

    FUNCTION_EXIT;
}


void PlanAgentAccess::addCategoryToRoot()
{
    FUNCTION_ENTRY( "addCategoryToRoot" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().insertCategoryIntoRoot(getSessionId().c_str());
    }
    PAA_END_TRY(insertCategoryIntoRoot);

    FUNCTION_EXIT;
}


void PlanAgentAccess::deleteCategory(TA_Base_Core::NodeId categoryNodeId)
{
    FUNCTION_ENTRY( "deleteCategory" );

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().deleteCategory(getSessionId().c_str(), categoryNodeId);
    }
    PAA_END_TRY(deleteCategory);

    FUNCTION_EXIT;
}


void PlanAgentAccess::updateCategory(TA_Base_Core::CategoryDetail categoryDetail)
{
    FUNCTION_ENTRY( "updateCategory" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().updateCategoryDetail(getSessionId().c_str(), categoryDetail);
    }
    PAA_END_TRY(updateCategoryDetail);

    FUNCTION_EXIT;
}


TA_Base_Core::AuxPlanDetail* PlanAgentAccess::exportPlan(TA_Base_Core::NodeId planNodeId)
{
    FUNCTION_ENTRY( "exportPlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
          TA_Base_Core::AuxPlanDetail_var auxPlanVar;
        m_planAgentMap.getDefaultPlanAgent().exportPlan(getSessionId().c_str(), auxPlanVar.out(), planNodeId);

          FUNCTION_EXIT;
          return auxPlanVar._retn();
    }
    PAA_END_TRY(exportPlan);
}


void PlanAgentAccess::importPlan(TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId)
{
    FUNCTION_ENTRY( "importPlan" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_PLAN_LONG_TIMEOUT);
        // ++TD15049
        m_planAgentMap.getDefaultPlanAgent().importPlan(getSessionId().c_str(), plan, parentId);
    }
    PAA_END_TRY(importPlan);

    FUNCTION_EXIT;
}


bool PlanAgentAccess::isRegisteredWithAgent (unsigned long location)
{
    FUNCTION_ENTRY( "isRegisteredWithAgent" );

    // Guard the method to ensure access by client code is synchronous
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    PAA_BEGIN_TRY
    {
        try
        {
        // TD15049++
        m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_REGISTER_TIMEOUT);
        // ++TD15049
        FUNCTION_EXIT;
        return (m_planAgentMap.getPlanAgent(location).isRegisteredManager (getSessionId(true).c_str(),
                                                                                                 Utilities::getPlanManagerId().c_str()));
        }
        catch (const TA_Base_Bus::PlanAgentEntityNotFoundException &)
        {
            // No Plan Agent configured for the specified location
            FUNCTION_EXIT;
            return false;
        }
    }
    PAA_END_TRY(isRegisteredWithAgent);

}


void PlanAgentAccess::registerPlanManager(TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerObjRef, unsigned long location)
{
    FUNCTION_ENTRY( "registerPlanManager" );
    // Guard the method to ensure access by client code is synchronous
    //libo  this Guard is 
    //TA_Base_Core::ThreadGuard guard(m_singletonLock);
	//libo 
    PAA_BEGIN_TRY
    {
        try
        {
            // TD15049++
            m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_REGISTER_TIMEOUT);
            // ++TD15049
            m_planAgentMap.getPlanAgent(location).registerPlanManager(getSessionId(true).c_str(),
                                                                      Utilities::getPlanManagerId().c_str(),
                                                                      planManagerObjRef);
        }
        catch (const TA_Base_Bus::PlanAgentEntityNotFoundException &)
        {
            // No Plan Agent configured for the specified location. Ignore it.
        }
    }
    PAA_END_TRY(registerPlanManager);

    FUNCTION_EXIT;
}


TA_Base_Bus::PlanAgentMap&  PlanAgentAccess::getPlanAgentMap()
{
	return m_planAgentMap;
}

void PlanAgentAccess::deregisterPlanManager(unsigned long location)
{
    FUNCTION_ENTRY( "deregisterPlanManager" );

    // Guard the method to ensure access by client code is synchronous
	//libo
    //  TA_Base_Core::ThreadGuard guard(m_singletonLock);
	//libo 
    PAA_BEGIN_TRY
    {
        try
        {
            // TD15049++
            //m_planAgentMap.getDefaultPlanAgent().setPlanAgentTimeout(PLAN_AGENT_REGISTER_TIMEOUT);
            // ++TD15049
            m_planAgentMap.getPlanAgent(location).deregisterPlanManager(getSessionId().c_str(),Utilities::getPlanManagerId().c_str());
        }
        catch (const TA_Base_Bus::PlanAgentEntityNotFoundException &)
        {
            // No Plan Agent configured for the specified location. Ignore it.
        }
    }
    PAA_END_TRY(deregisterPlanManager);

    FUNCTION_EXIT;
}


void PlanAgentAccess::generateReport (const std::string& planPath)
{
    FUNCTION_ENTRY( "generateReport" );

    // as per TD 11721
    // we want to give user the option of
    // creating a report which logs any
    // broken links which may result
    // after category/plan is deleted (plans only), renamed or moved (plans only)

    std::string appName = "Report Manager";
    CWaitCursor wait;
    try
    {
        // launch report manager
        std::ostringstream commandLine;
        // TD 13265
        // //skeleton report for now - replace with the appropriate one, once it becomes available
        // std::string reportPath = "C:\\4669_Sandbox\\CONFIG_4669\\config\\R-Report\\R01-Generic\\R0101-Report\\BrokenPlanLinks.rpt";
        std::string reportPath = TA_Base_Core::RunParams::getInstance().get(TA_Base_App::RPARAM_REPORTTEMPLATE);

        // TD 18040
        CString strReportPath = reportPath.c_str();

        strReportPath.Replace("C:", "\\\\adm-occ-1\\C$" );
        strReportPath.Replace("c:", "\\\\adm-occ-1\\C$" );
        strReportPath.Replace(" ", "{%Blank%}");    // replace blank in planpath with {%Blank%} to accept the Plan Categories with blanks, 3.Jan.2007, hongzhi

        reportPath = strReportPath.GetBuffer( strReportPath.GetLength() );


        commandLine << "--file=" << reportPath << " --view";

        // replace blank in planpath with {%Blank%} to accept the Plan name with blanks,
        // 3.Jan.2007, hongzhi
        CString strPlanPath = planPath.c_str();
        strPlanPath.Replace(" ", "{%Blank%}");
        commandLine << " --report-parameters=p_planPath:" << LPCTSTR(strPlanPath);

        CPoint pt = GetMessagePos();
        RECT newRect = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                                                                                 TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, pt.x);
        TA_Base_Bus::AppLauncher::getInstance().launchApplication(REPORT_MANAGER_GUI_APPTYPE, commandLine.str(),
                         TA_Base_Bus::TA_GenericGui::POS_BOUNDED, TA_Base_Bus::TA_GenericGui::ALIGN_FIT, NULL, &newRect);
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "TransactiveException", ex.what());
        TA_Base_Bus::TransActiveMessage msgBox;
        msgBox << appName;
        msgBox.showMsgBox (IDS_UE_010002);
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "UnknownException", "");
        TA_Base_Bus::TransActiveMessage msgBox;
        msgBox << appName;
        msgBox.showMsgBox (IDS_UE_010002);
    }

    FUNCTION_EXIT;
}


