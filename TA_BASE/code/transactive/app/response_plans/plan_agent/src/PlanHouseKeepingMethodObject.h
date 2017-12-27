#ifndef PLANHOUSEKEEPINGMETHODOBJECT_H_INCLUDED
#define PLANHOUSEKEEPINGMETHODOBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanHouseKeepingMethodObject.h $
  *  @author:  jian.huang
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/20 12:26:46 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */

#include "ace/Method_Request.h"

namespace TA_Base_App
{

    class PlanExecutionEngine;

    //Class declaration
    class PlanHouseKeepingMethodObject : public ACE_Method_Request
    {
        // Public methods
    public:
        PlanHouseKeepingMethodObject( PlanExecutionEngine& task );
        virtual ~PlanHouseKeepingMethodObject();

        virtual int call();

    private:
        PlanHouseKeepingMethodObject();
        PlanHouseKeepingMethodObject& operator=( const PlanHouseKeepingMethodObject& );
        PlanHouseKeepingMethodObject( const PlanHouseKeepingMethodObject& );

    private:
        PlanExecutionEngine& m_engine;
    }; // PlanHouseKeepingMethodObject

} // TA_Base_App


#endif //PLANHOUSEKEEPINGMETHODOBJECT_H_INCLUDED