/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/PlanAgentException.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Thrown by AgentPlanAccess when an error occurs while attempting to communicate with the Plan Agent.
  */

#if !defined(AFX_PLANAGENTEXCEPTION_H__8D121A2B_319E_47A4_ACBA_D701A04E8D75__INCLUDED_)
#define AFX_PLANAGENTEXCEPTION_H__8D121A2B_319E_47A4_ACBA_D701A04E8D75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core\exceptions\src\TransactiveException.h"


namespace TA_Base_Bus
{
    class PlanAgentException : public TA_Base_Core::TransactiveException  
    {
    public:
        PlanAgentException() throw();
        PlanAgentException(const std::string& msg) throw();
	    virtual ~PlanAgentException() throw();
    };
}

#endif // !defined(AFX_PLANAGENTEXCEPTION_H__8D121A2B_319E_47A4_ACBA_D701A04E8D75__INCLUDED_)
