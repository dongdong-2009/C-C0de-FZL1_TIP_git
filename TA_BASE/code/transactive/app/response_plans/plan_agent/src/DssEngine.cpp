/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssEngine.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/DssEngine.h"
#include "app/response_plans/common/src/StringConstants.h"


namespace TA_Base_App
{

struct TriggerRequest 
{
    unsigned long ulTriggerKey;
    unsigned long ulTimestamp;
    bool bValue;
    bool bInitial;
};

DssEngine::DssEngine() : 
m_bRunning( false ),
m_bEnable( false ),
m_dssRuleProcessor(),
m_mtxRequests(),
m_smaWorkLock( 0 ),
m_lstRequests()
{
}

DssEngine::~DssEngine()
{
}

void DssEngine::getTypeAndTriggerKeys( TypeToTriggerKeysMap& refOutput )
{
    m_dssRuleProcessor.getTypeAndTriggerKeys( refOutput );
}

void DssEngine::triggerUpdate( const unsigned long ulTriggerKey, const unsigned long ulTimestamp, const bool bValue, const bool bInitial  )
{
    TriggerRequest* pNewRequest = new TriggerRequest();
    pNewRequest->bInitial = bInitial;
    pNewRequest->bValue = bValue;
    pNewRequest->ulTimestamp = ulTimestamp;
    pNewRequest->ulTriggerKey = ulTriggerKey;

    {
        ACE_Guard<ACE_Mutex> guard(m_mtxRequests);
        m_lstRequests.push_back( pNewRequest );
    }

    m_smaWorkLock.release();
}

void DssEngine::run()
{
    m_bRunning = true;

    while ( m_bRunning )
    {
        m_smaWorkLock.acquire();

        if ( !m_bRunning )
        {
            break;
        }

        processTriggerUpdate();
    }
}

void DssEngine::terminate()
{
    m_bRunning = false;
    m_bEnable = false;
    m_smaWorkLock.release();
}

void DssEngine::enable( const bool bEnable )
{
    m_bEnable = bEnable;
}

void DssEngine::processTriggerUpdate()
{
    if ( !m_bEnable )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignore a trigger update in Monitor Mode" );
        return;
    }

    if ( m_lstRequests.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Trigger request list is empty" );
        return;
    }

    unsigned long ulTriggerKey = 0u;
    unsigned long ulTimestamp = 0u;
    bool bValue = false;
    bool bInitial = false;

    {
        TriggerRequest* pCurRequest = NULL;

        ACE_Guard<ACE_Mutex> guard(m_mtxRequests);
        pCurRequest = m_lstRequests.front();
        m_lstRequests.pop_front();

        if ( NULL == pCurRequest )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of TriggerRequest" );
            return;
        }

        ulTriggerKey = pCurRequest->ulTriggerKey;
        ulTimestamp = pCurRequest->ulTimestamp;
        bValue = pCurRequest->bValue;
        bInitial = pCurRequest->bInitial;

        delete pCurRequest;
        pCurRequest = NULL;
    }

    unsigned long ulPlanId = m_dssRuleProcessor.getPlanIdByTriggerKey( ulTriggerKey );
    bool bCurActive = false;

    std::list<TA_Base_Core::ActivePlanId> lstActivePlans;
    ActivePlanBulletinBoard::instance()->getActiveInstances( ulPlanId, lstActivePlans );
    for ( std::list<TA_Base_Core::ActivePlanId>::iterator itLoop = lstActivePlans.begin(); lstActivePlans.end() != itLoop; ++itLoop )
    {
        if ( itLoop->triggerEntity == ulTriggerKey )
        {
            bCurActive = true;
            break;
        }
    }

    bool bNeedActivate = false;
    bool bNeedDeactivate = false;
    m_dssRuleProcessor.makeDecision( ulTriggerKey, ulTimestamp, bCurActive, bValue, bInitial, ulPlanId, bNeedActivate, bNeedDeactivate );

    if ( bNeedDeactivate )
    {
        for ( std::list<TA_Base_Core::ActivePlanId>::iterator itLoop = lstActivePlans.begin(); lstActivePlans.end() != itLoop; ++itLoop )
        {
            if ( itLoop->triggerEntity != ulTriggerKey )
            {
                continue;
            }

            try
            {
                PlanExecutor::instance()->stop( *itLoop, RemarkDssEngineStopPlan );
            }
            catch ( const std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception process entity update" );
            }
        }
    }

    if ( bNeedActivate )
    {
        static const std::string strSession = TA_Base_Core::SessionAccessFactory::getInstance().getSuperSessionId().c_str();
        TA_Base_Core::ActivePlanId stPlanId = { ulPlanId, 0u, 0u, ulTriggerKey, -1 };

        try
        {
            PlanExecutor::instance()->run( strSession, "", stPlanId );
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception process entity update" );
        }
    }
}

void DssEngine::initAndStart( const bool bEnable )
{
    m_bEnable = bEnable;

    m_dssRuleProcessor.init();

    start();
}

void DssEngine::uninitAndTerminate()
{
    terminateAndWait();

    m_dssRuleProcessor.uninit();
}

bool DssEngine::isEnabled()
{
    return m_bEnable;
}

}
