/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssKnowledgeBase.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include <typeinfo>
#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/plans/src/IPlanDssDpRule.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleAccessFactory.h"
#include "app/response_plans/plan_agent/src/DssDatapointRule.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/DssKnowledgeBase.h"


namespace TA_Base_App
{

DssKnowledgeBase::DssKnowledgeBase()
{
}

DssKnowledgeBase::~DssKnowledgeBase()
{
}

void DssKnowledgeBase::init()
{
    const unsigned long ulLocationKey = PlanAgentUser::getInstance()->getAgentLocationKey();

    std::vector<TA_Base_Core::IPlanDssDpRule*> vecDpRules;

    // Retrieve all DSS DataPoint rules
    try
    {
        vecDpRules = TA_Base_Core::PlanDssDpRuleAccessFactory::getInstance().getPlanDssDpRuleByLocationKey( ulLocationKey, false );
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception when get Dss Dp rules" );
    }

    // Initialize DataPoint rules
    KeyToRuleMap mapDpRules;
    std::pair<TypeToRulesMap::iterator, bool> pirRes = m_mapDssRules.insert( TypeToRulesMap::value_type(DssRuleTypeDatapoint, mapDpRules) );

    if ( pirRes.second )
    {
        for ( std::vector<TA_Base_Core::IPlanDssDpRule*>::iterator itLoop = vecDpRules.begin(); itLoop != vecDpRules.end(); ++itLoop )
        {
            TA_Base_Core::IPlanDssDpRule* pRuleData = *itLoop;
            if ( NULL == pRuleData )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "IPlanDssDpRule is NULL" );
                continue;
            }

            DssAbstractRule* pNewRule = new DssDatapointRule( *pRuleData );
            pirRes.first->second.insert( KeyToRuleMap::value_type(pRuleData->getTriggerEntityKey(), pNewRule) );

            delete pRuleData;
            *itLoop = NULL;
        }

        vecDpRules.clear();
    }
}

void DssKnowledgeBase::uninit()
{
    // For each trigger type
    for ( TypeToRulesMap::iterator itTypeLoop = m_mapDssRules.begin(); m_mapDssRules.end() != itTypeLoop; ++itTypeLoop )
    {
        // For each item in Key to Rule map
        for ( KeyToRuleMap::iterator itLoop = itTypeLoop->second.begin(); itTypeLoop->second.end() != itLoop; ++itLoop )
        {
            delete itLoop->second;
            itLoop->second = NULL;
        }

        itTypeLoop->second.clear();
    }

    m_mapDssRules.clear();
}

void DssKnowledgeBase::getTypeAndTriggerKeys( TypeToTriggerKeysMap& refOutput )
{
    // For each trigger type
    for ( TypeToRulesMap::iterator itTypeLoop = m_mapDssRules.begin(); m_mapDssRules.end() != itTypeLoop; ++itTypeLoop )
    {
        std::vector<unsigned long> vecKeys;
        // Insert dummy rule keys
        std::pair<TypeToTriggerKeysMap::iterator, bool> pirRes = refOutput.insert( TypeToTriggerKeysMap::value_type(itTypeLoop->first, vecKeys) );
        if ( !pirRes.second )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to insert into Output map" );
            continue;
        }

        // For each item in Key to Rule map
        for ( KeyToRuleMap::iterator itLoop = itTypeLoop->second.begin(); itLoop != itTypeLoop->second.end(); ++itLoop )
        {
            pirRes.first->second.push_back( itLoop->first );
        }
    }
}

IDssRule& DssKnowledgeBase::getDssRuleByDatapointKey( const unsigned long ulKey )
{
    IDssRule* pRule = NULL;

    // For each trigger type
    for ( TypeToRulesMap::iterator itTypeLoop = m_mapDssRules.begin(); m_mapDssRules.end() != itTypeLoop; ++itTypeLoop )
    {
        KeyToRuleMap::iterator itFound = itTypeLoop->second.find( ulKey );
        if ( itTypeLoop->second.end() != itFound )
        {
            pRule = itFound->second;
            break;
        }
    }

    if ( NULL == pRule )
    {
        std::stringstream stmWhat;
        stmWhat << "Can not find the DSS rule : " << ulKey;
        TA_THROW( TA_Base_Core::TransactiveException( stmWhat.str().c_str() ) );
    }

    return *pRule;
}

}
