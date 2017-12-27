/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlanMessageException.h"

namespace TA_Base_App
{
    ActivePlanMessageException::ActivePlanMessageException() throw() : TransactiveException( "" )
	{
	}

	ActivePlanMessageException::ActivePlanMessageException(const std::string& msg) throw() : TransactiveException( msg )
	{
	}

	ActivePlanMessageException::~ActivePlanMessageException() throw()
	{
	}

} // TA_Base_App
