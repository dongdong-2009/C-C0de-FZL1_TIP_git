/*
 * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
 *
 * Source : $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssAbstractRule.h $
 * @author  Huang Qi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2015/01/20 12:26:46 $
 * Last modified by : $Author: CM $
 *
 * 
 */

#ifndef DSSABSTRACTRULE_H
#define DSSABSTRACTRULE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_agent/src/IDssRule.h"

namespace TA_Base_Core
{
    class IPlanDssDpRule;
}

namespace TA_Base_App
{

class DssAbstractRule : public IDssRule
{
public:
    DssAbstractRule( TA_Base_Core::IPlanDssDpRule& refData );
    virtual ~DssAbstractRule();

    virtual bool isTimeValid( const unsigned long ulTimestamp, const unsigned long ulMilSecond );
    virtual unsigned long getPlanId();

protected:
    DssAbstractRule();
	DssAbstractRule( const DssAbstractRule& );
	const DssAbstractRule& operator=( const DssAbstractRule& );

protected:
    unsigned long m_ulLocationKey;
    unsigned long m_ulTriggerEntityKey;
    unsigned long m_ulActivePlanKey;
    unsigned long m_ulMinimumInterval;
    unsigned long m_ulTolerantTimeForActive;

    unsigned long m_ulValidStartTimeInDay;
    unsigned long m_ulValidEndTimeInDay;
    unsigned long m_ulValidWeekday;

    bool m_bNeedDeactivate;
    bool m_bAllowMultiple;
    int m_nDenyMultipleStrategy;
};

} // namespace TA_Base_App

#endif // DSSABSTRACTRULE_H
