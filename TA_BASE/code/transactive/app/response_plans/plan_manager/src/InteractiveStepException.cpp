/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/InteractiveStepException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Thrown to indicate problems while executing an interactive step.
  */

#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


InteractiveStepException::InteractiveStepException() throw() :
    PlanManagerException()
{
}


InteractiveStepException::InteractiveStepException(std::string msg) throw() :
    PlanManagerException(msg)
{
}


InteractiveStepException::InteractiveStepException(std::string msg, std::string detail) throw() :
    PlanManagerException(msg, detail)
{
}


InteractiveStepException::InteractiveStepException(std::string msg, std::string detail, std::string comment) throw() :
    PlanManagerException(msg, detail, comment)
{
}


InteractiveStepException::~InteractiveStepException() throw()
{
}
