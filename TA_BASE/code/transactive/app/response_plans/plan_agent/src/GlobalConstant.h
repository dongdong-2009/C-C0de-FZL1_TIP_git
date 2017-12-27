#ifndef GLOBALCONSTANT_H_INCLUDED
#define GLOBALCONSTANT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/GlobalConstant.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/20 12:26:46 $
  *  Last modified by:  $Author: CM $
  *
  *  General include for the Plan Agent.
  */

// Enum for ActivePlanBulletinBoard and ActivePlan but should also be used
// for returns from all functions that wait for a result
namespace TA_Base_App
{
	enum EWaitResult
	{
		TIMEOUT_WR  ,
		SUCCESS_WR  ,
		FAILURE_WR  ,
		UNDEFINED_WR
	};

    const unsigned long MAX_TOLERANT_TIME_MISMATCH = 5u;
}


#endif //GLOBALCONSTANT_H_INCLUDED
