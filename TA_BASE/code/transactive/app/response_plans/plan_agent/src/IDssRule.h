/*
 * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
 *
 * Source : $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/IDssRule.h $
 * @author  Huang Qi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2015/01/20 12:26:46 $
 * Last modified by : $Author: CM $
 *
 * 
 */

#ifndef IDSSRULE_H
#define IDSSRULE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{

class IDssRule
{
public:
    virtual bool isTimeValid( const unsigned long ulTimestamp, const unsigned long ulMilSecond ) = 0;
    virtual bool needActivate( const bool bCurActive, const bool bCurValue ) = 0;
    virtual bool needDeactivate( const bool bCurActive, const bool bCurValue ) = 0;
    virtual unsigned long getPlanId() = 0;
};

} // namespace TA_Base_App

#endif // IDSSRULE_H
