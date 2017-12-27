/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssDatapointRule.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include "ace/OS.h"
#include "ace/Date_Time.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/plans/src/IPlanDssDpRule.h"
#include "core/data_access_interface/plans/src/PlanDssDpRuleAccessFactory.h"
#include "app/response_plans/plan_agent/src/DssDatapointRule.h"


namespace TA_Base_App
{

DssDatapointRule::DssDatapointRule( TA_Base_Core::IPlanDssDpRule& refData ) : DssAbstractRule( refData ),
m_bActivateValue( refData.getActivateValue() )
{
}

DssDatapointRule::~DssDatapointRule()
{
}

bool DssDatapointRule::needActivate( const bool bCurActive, const bool bCurValue )
{
    // Value is not for active
    if ( m_bActivateValue != bCurValue )
    {
        return false;
    }

    // Value is for active but need reject new instance
    if ( !m_bAllowMultiple && bCurActive && ( TA_Base_Core::StrategyRejectNew == m_nDenyMultipleStrategy ))
    {
        return false;
    }

    return true;
}

bool DssDatapointRule::needDeactivate( const bool bCurActive, const bool bCurValue )
{
    // Value is for deactivation
    if ( m_bNeedDeactivate && bCurActive && ( m_bActivateValue != bCurValue ))
    {
        return true;
    }

    // Exception handle for missing trigger update
    if ( !m_bAllowMultiple && bCurActive && ( m_bActivateValue == bCurValue ) && ( TA_Base_Core::StrategyDeactivateExists == m_nDenyMultipleStrategy ))
    {
        return true;
    }

    return false;
}

}
