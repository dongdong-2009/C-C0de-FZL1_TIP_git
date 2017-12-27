/*
 * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
 *
 * Source : $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssDatapointRule.h $
 * @author  Huang Qi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2015/01/20 12:26:46 $
 * Last modified by : $Author: CM $
 *
 * 
 */

#ifndef DSSDATAPOINTRULE_H
#define DSSDATAPOINTRULE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_agent/src/DssAbstractRule.h"

namespace TA_Base_Core
{
    class IPlanDssDpRule;
}

namespace TA_Base_App
{

class DssDatapointRule : public DssAbstractRule
{
public:
    DssDatapointRule(TA_Base_Core::IPlanDssDpRule& refData);
    virtual ~DssDatapointRule();

    virtual bool needActivate( const bool bCurActive, const bool bCurValue );
    virtual bool needDeactivate( const bool bCurActive, const bool bCurValue );

private:
    DssDatapointRule();
	DssDatapointRule( const DssDatapointRule& );
	const DssDatapointRule& operator=( const DssDatapointRule& );

private:
    bool m_bActivateValue;
};

} // namespace TA_Base_App

#endif // DSSDATAPOINTRULE_H
