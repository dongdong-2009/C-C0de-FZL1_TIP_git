/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/OperatorRights.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Singleton class to facilitate access to the operator's access rights for the Plans subsystem.
  * Where the rights to a particular access controlled action cannot be determined, access is
  * automatically denied.
  *
  **/

#include "StdAfx.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "Resource.h"
#include "OperatorRights.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define OR_BEGIN_TRY    try

#define OR_END_TRY(actionType)                                                                                 \
catch (const TA_Base_Core::RightsException &ex)                                                                     \
{                                                                                                              \
    std::ostringstream errorMessage;                                                                           \
    errorMessage << "Unable to determine the operator's "#actionType" rights. Access denied. " << ex.what();   \
    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::RightsException", errorMessage.str().c_str());                   \
}                                                                                                              \
catch (const TA_Base_Core::TransactiveException &ex)                                                                \
{                                                                                                              \
    std::ostringstream errorMessage;                                                                           \
    errorMessage << "Unable to determine the operator's "#actionType" rights. Access denied. " << ex.what();   \
    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", errorMessage.str().c_str());              \
}                                                                                                              \
catch (...)                                                                                                    \
{                                                                                                              \
    std::ostringstream errorMessage;                                                                           \
    errorMessage << "Unable to determine the operator's "#actionType" rights. Access denied. ";                \
    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", errorMessage.str().c_str());                          \
}


TA_Base_App::OperatorRights *TA_Base_App::OperatorRights::s_instance = NULL;
TA_Base_Core::ReEntrantThreadLockable TA_Base_App::OperatorRights::s_singletonLock;


using namespace TA_Base_App;


////////////////////////////////////////////////////////////////////////////
// OperatorRights

OperatorRights::OperatorRights() :
    m_planMgrResourceKey(0),
    m_rightsLibrary(NULL)
{
    FUNCTION_ENTRY( "OperatorRights" );

    // Initialise the rights library
    try
    {
        std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        TA_ASSERT(!entityName.empty(), "Entity name not set. This should have been picked up on startup!!!");

        std::auto_ptr<TA_Base_Core::IEntityData> planMgrEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName));
        TA_ASSERT(planMgrEntityData.get(), "Plan Manager entity data is null");

        std::auto_ptr<TA_Base_Core::IResource> planMgrResource(
            TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(planMgrEntityData->getKey()));

        m_planMgrResourceKey = planMgrResource->getKey();

        TA_Base_Bus::RightsLibraryFactory rightsFactory;
        m_rightsLibrary = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        delete m_rightsLibrary;
        m_rightsLibrary = NULL;

        LOG_EXCEPTION_CATCH(SourceInfo, "Rights exception", ex.what());
        CString actionName, agent;
        actionName.LoadString(IDS_INITIALISE);
        agent.LoadString(IDS_RIGHTS_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }
    catch (...)
    {
        delete m_rightsLibrary;
        m_rightsLibrary = NULL;

        CString actionName, agentName;
        actionName.LoadString(IDS_INITIALISE);
        agentName.LoadString(IDS_RIGHTS_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agentName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
    }

    FUNCTION_EXIT;
}


OperatorRights::~OperatorRights()
{
    FUNCTION_ENTRY( "~OperatorRights" );

    delete m_rightsLibrary;
    m_rightsLibrary = NULL;

    s_instance = NULL;

    FUNCTION_EXIT;
}


OperatorRights &OperatorRights::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    if (!s_instance)
    {
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if (!s_instance)
        {
            s_instance = new OperatorRights();
        }
    }

    FUNCTION_EXIT;
    return *s_instance;
}


void OperatorRights::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_Base_Core::ThreadGuard guard(s_singletonLock);

    delete s_instance;

    FUNCTION_EXIT;
}


bool OperatorRights::canEditCategories()
{
    FUNCTION_ENTRY( "canEditCategories" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_EDIT_CATEGORY,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_EDIT_CATEGORY);
    }

    FUNCTION_EXIT;
    return false;
}


bool OperatorRights::canEditPlans()
{
    FUNCTION_ENTRY( "canEditPlans" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_EDIT_PLAN,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_EDIT_PLAN);
    }

    FUNCTION_EXIT;
    return false;
}


bool OperatorRights::canApprovePlans()
{
    FUNCTION_ENTRY( "canApprovePlans" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_APPROVE_PLAN,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_APPROVE_PLAN);
    }

    FUNCTION_EXIT;
    return false;
}


bool OperatorRights::canSchedulePlans()
{
    FUNCTION_ENTRY( "canSchedulePlans" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_SCHEDULE_PLAN,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_SCHEDULE_PLAN);
    }

    FUNCTION_EXIT;
    return false;
}


bool OperatorRights::canControlPlans()
{
    FUNCTION_ENTRY( "canControlPlans" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_CONTROL_PLAN,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_CONTROL_PLAN);
    }

    FUNCTION_EXIT;
    return false;
}


bool OperatorRights::canCustomisePlans()
{
    FUNCTION_ENTRY( "canCustomisePlans" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_CUSTOMISE_PLAN,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_CUSTOMISE_PLAN);
    }

    FUNCTION_EXIT;
    return false;
}


bool OperatorRights::canTakePlanOwnership()
{
    FUNCTION_ENTRY( "canTakePlanOwnership" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_CHANGE_PLAN_OWNER,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_CHANGE_PLAN_OWNER);
    }

    FUNCTION_EXIT;
    return false;
}


bool OperatorRights::canViewNonLocalActivePlans()
{
    FUNCTION_ENTRY( "canViewNonLocalActivePlans" );

    if (m_rightsLibrary)
    {
        OR_BEGIN_TRY
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!sessionId.empty(), "Session ID not set");

            TA_Base_Bus::EDecisionModule decisionModule;
            std::string reasonForDenial;

            FUNCTION_EXIT;
            return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_planMgrResourceKey,
                                                                TA_Base_Bus::aca_PLAN_VIEW_ACTIVE_ALL_LOCATIONS,
                                                                reasonForDenial, decisionModule);
        }
        OR_END_TRY(PLAN_VIEW_ACTIVE_ALL_LOCATIONS);
    }

    FUNCTION_EXIT;
    return false;
}


