#if defined ( STEPTYPE_PROMPT )

#ifndef ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanPromptStepMessage : public IActivePlanMessage
    {
    //public methods
    public:
        ActivePlanPromptStepMessage(const int status);

        virtual void execute( ActivePlan& activePlan ) const;

    private:
        ActivePlanPromptStepMessage& operator=( const ActivePlanPromptStepMessage& );
        ActivePlanPromptStepMessage( const ActivePlanPromptStepMessage& );

        const int m_status;
    };

}

#endif //ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED

#endif