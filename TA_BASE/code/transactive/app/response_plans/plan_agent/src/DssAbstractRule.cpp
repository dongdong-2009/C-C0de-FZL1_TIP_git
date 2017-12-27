/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssAbstractRule.cpp $
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
#include "app/response_plans/plan_agent/src/DssAbstractRule.h"
#include "app/response_plans/plan_agent/src/GlobalConstant.h"


namespace TA_Base_App
{

DssAbstractRule::DssAbstractRule( TA_Base_Core::IPlanDssDpRule& refData ) :
m_ulLocationKey( refData.getLocationKey() ),
m_ulTriggerEntityKey( refData.getTriggerEntityKey() ),
m_ulActivePlanKey( refData.getActivePlanKey() ),
m_ulMinimumInterval( refData.getMinimumInterval() ),
m_ulTolerantTimeForActive( refData.getTolerantTimeForActive() ),
m_ulValidStartTimeInDay( refData.getValidStartTimeInDay() ),
m_ulValidEndTimeInDay( refData.getValidEndTimeInDay() ),
m_ulValidWeekday( 0u ),
m_bNeedDeactivate( refData.getNeedDeactivate() ),
m_bAllowMultiple( refData.getAllowMultiple() ),
m_nDenyMultipleStrategy( refData.getDenyMultipleStrategy() )
{
    m_ulValidWeekday |= refData.getValidSunday() ? 0x00000001u : 0u;
    m_ulValidWeekday |= refData.getValidMonday() ? 0x00000002u : 0u;
    m_ulValidWeekday |= refData.getValidTuesday() ? 0x00000004u : 0u;
    m_ulValidWeekday |= refData.getValidWednesday() ? 0x00000008u : 0u;
    m_ulValidWeekday |= refData.getValidThursday() ? 0x00000010u : 0u;
    m_ulValidWeekday |= refData.getValidFriday() ? 0x00000020u : 0u;
    m_ulValidWeekday |= refData.getValidSaturday() ? 0x00000040u : 0u;
}

DssAbstractRule::~DssAbstractRule()
{
}

bool DssAbstractRule::isTimeValid( const unsigned long ulTimestamp, const unsigned long ulMilSecond )
{
    // Validate tolerant time for activation
    if ( 0u < m_ulTolerantTimeForActive )
    {
        unsigned long ulNow = ACE_OS::time();

        if ( ulNow < ulTimestamp )
        {
            if (( ulTimestamp - ulNow ) > MAX_TOLERANT_TIME_MISMATCH )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "Time is mismatch too much between equipment [%d] and server [%d]", ulTimestamp, ulNow );
                return false;
            }
        }
        else
        {
            if (( ulNow - ulTimestamp ) > m_ulTolerantTimeForActive )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "Too late for the timestamp [%d] and tolerant time [%d] when now is [%d]", ulTimestamp, m_ulTolerantTimeForActive, ulNow );
                return false;
            }
        }
    }

    ACE_Time_Value tvTimestamp( ulTimestamp );
    ACE_Date_Time dtTimestamp( tvTimestamp );

    // Validate weekday
    const unsigned long ulCurWeekday = ( 0x00000001u << static_cast<unsigned long>(dtTimestamp.weekday()) );
    if (( ulCurWeekday & m_ulValidWeekday ) != ulCurWeekday )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Weekday [%d] is not valid for [%d]", ulCurWeekday, m_ulValidWeekday );
        return false;
    }

    // Validate time in day
    static const unsigned long SecondInHour = 3600;
    static const unsigned long SecondInMinute = 60;
    const unsigned long ulSecondInDay = dtTimestamp.hour() * SecondInHour + dtTimestamp.minute() * SecondInMinute + dtTimestamp.second();
    if (( ulSecondInDay < m_ulValidStartTimeInDay ) || ( ulSecondInDay > m_ulValidEndTimeInDay ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Current time [%d] is not in day time [%d, %d]", ulSecondInDay, m_ulValidStartTimeInDay, m_ulValidEndTimeInDay );
        return false;
    }

    // Minimum interval would not work yet unless user raise requirement.
    // Validate minimum interval

    return true;
}

unsigned long DssAbstractRule::getPlanId()
{
    return m_ulActivePlanKey;
}

}
