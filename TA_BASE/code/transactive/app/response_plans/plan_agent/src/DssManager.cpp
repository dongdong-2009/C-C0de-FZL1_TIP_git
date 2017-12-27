/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssManager.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:58:54 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/DatapointTriggerObserver.h"
#include "app/response_plans/plan_agent/src/DssEngine.h"
#include "app/response_plans/plan_agent/src/DssManager.h"


namespace TA_Base_App
{

DssManager::DssManager() :
m_pDssEngine( NULL ),
m_lstTriggerObservers()
{
}

DssManager::~DssManager()
{
    uninit();
}

void DssManager::initTriggerObservers( const bool bEnable )
{
    if ( NULL == m_pDssEngine )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DssEngine is NULL" );
        return;
    }

    TypeToTriggerKeysMap mapTriggerKeys;
    m_pDssEngine->getTypeAndTriggerKeys( mapTriggerKeys );

    for ( TypeToTriggerKeysMap::iterator itLoop = mapTriggerKeys.begin(); mapTriggerKeys.end() != itLoop; ++itLoop )
    {
        for ( std::vector<unsigned long>::iterator itKeyLoop = itLoop->second.begin(); itKeyLoop != itLoop->second.end(); ++itKeyLoop )
        {
            ITriggerObserver* pNewObserver = createTriggerObserver( bEnable, itLoop->first, *itKeyLoop );

            if ( NULL == pNewObserver )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid trigger type" );
                continue;
            }

            m_lstTriggerObservers.push_back( pNewObserver );
        }
    }
}

void DssManager::uninitTriggerObservers()
{
    for ( std::list<ITriggerObserver*>::iterator itLoop = m_lstTriggerObservers.begin(); m_lstTriggerObservers.end() != itLoop; ++itLoop )
    {
        delete *itLoop;
        *itLoop = NULL;
    }

    m_lstTriggerObservers.clear();
}

ITriggerObserver* DssManager::createTriggerObserver( const bool bEnable, const int nType, const unsigned long ulKey )
{
    ITriggerObserver* pObserver = NULL;

    switch ( nType )
    {
    case DssRuleTypeInvalid:
    	break;

    case DssRuleTypeDatapoint:
        pObserver = new DatapointTriggerObserver( *m_pDssEngine, ulKey );
        break;

    default:
        break;
    }

    return pObserver;
}

void DssManager::enable( const bool bEnable )
{
    if ( NULL != m_pDssEngine )
    {
        m_pDssEngine->enable( bEnable );
    }
}

void DssManager::init( const bool bEnable )
{
    if ( NULL == m_pDssEngine )
    {
        m_pDssEngine = new DssEngine();
    }

    if ( NULL != m_pDssEngine )
    {
        m_pDssEngine->initAndStart( bEnable );
    }

    initTriggerObservers( bEnable );
}

void DssManager::uninit()
{
    uninitTriggerObservers();

    if ( NULL != m_pDssEngine )
    {
		m_pDssEngine->uninitAndTerminate();
        delete m_pDssEngine;
        m_pDssEngine = NULL;
    }
}

}
