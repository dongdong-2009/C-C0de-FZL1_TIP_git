#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP )

#ifndef ACTIVE_PLAN_FAILURE_SUMMARY_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_FAILURE_SUMMARY_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Lizette Lingo
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Generated when the result of a Failure summary dialog display
  *  is received back from the Plan Manager.
  */

#include <string>

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanFailureSummaryStepMessage : public IActivePlanMessage
    {
    //public methods
    public:
        ActivePlanFailureSummaryStepMessage( const int status, const std::string& error);
        virtual ~ActivePlanFailureSummaryStepMessage();
        
        virtual void execute( ActivePlan& activePlan ) const;
    
    private:
        ActivePlanFailureSummaryStepMessage& operator=( const ActivePlanFailureSummaryStepMessage& );
        ActivePlanFailureSummaryStepMessage( const ActivePlanFailureSummaryStepMessage& );

        const int m_status;
        std::string m_error;
    };

}

#endif //ACTIVE_PLAN_FAILURE_SUMMARY_STEP_HEADER_INCLUDED

#endif