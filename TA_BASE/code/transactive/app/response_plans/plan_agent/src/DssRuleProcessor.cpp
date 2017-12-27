/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssRuleProcessor.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IDssRule.h"
#include "app/response_plans/plan_agent/src/DssRuleProcessor.h"


namespace TA_Base_App
{

DssRuleProcessor::DssRuleProcessor() : 
m_dssKnowledgeBase()
{
}

DssRuleProcessor::~DssRuleProcessor()
{
}

void DssRuleProcessor::getTypeAndTriggerKeys( TypeToTriggerKeysMap& refOutput )
{
    m_dssKnowledgeBase.getTypeAndTriggerKeys( refOutput );
}

void DssRuleProcessor::makeDecision( const unsigned long ulTriggerKey, const unsigned long ulTimestamp, 
    const bool bCurActive, const bool bValue, const bool bInitial, unsigned long& ulPlanId, bool& bActivate, bool& bDeactivate )
{
    // Disable all function first
    bActivate = false;
    bDeactivate = false;

    IDssRule& refRule = m_dssKnowledgeBase.getDssRuleByDatapointKey( ulTriggerKey );
    ulPlanId = refRule.getPlanId();

    // Validate timestamp
    if ( !refRule.isTimeValid( ulTimestamp, 0u ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Invalid time [%d] for the trigger entity [%d]", ulTimestamp, ulTriggerKey );
        return;
    }

    bActivate = refRule.needActivate( bCurActive, bValue );
    bDeactivate = refRule.needDeactivate( bCurActive, bValue );
}

unsigned long DssRuleProcessor::getPlanIdByTriggerKey( const unsigned long ulTriggerKey )
{
    IDssRule& refRule = m_dssKnowledgeBase.getDssRuleByDatapointKey( ulTriggerKey );

    return refRule.getPlanId();
}

void DssRuleProcessor::init()
{
    m_dssKnowledgeBase.init();
}

void DssRuleProcessor::uninit()
{
    m_dssKnowledgeBase.uninit();
}

}
