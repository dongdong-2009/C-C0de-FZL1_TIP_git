#if defined ( STEPTYPE_LAUNCH_GUI_APP )

#ifndef ACTIVE_PLAN_LAUNCH_GUI_APP_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_LAUNCH_GUI_APP_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Bart Golab
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Generated when the result of a Launch GUI App interactive step execution request
  *  is received back from the Plan Manager.
  */

#include <string>

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanLaunchGuiAppStepMessage : public IActivePlanMessage
    {
    //public methods
    public:
        ActivePlanLaunchGuiAppStepMessage(const int status, const std::string& error);
        virtual ~ActivePlanLaunchGuiAppStepMessage();
        
        virtual void execute( ActivePlan& activePlan ) const;
    
    private:
        ActivePlanLaunchGuiAppStepMessage& operator=( const ActivePlanLaunchGuiAppStepMessage& );
        ActivePlanLaunchGuiAppStepMessage( const ActivePlanLaunchGuiAppStepMessage& );

        const int m_status;
        std::string m_error;
    };

}

#endif //ACTIVE_PLAN_LAUNCH_GUI_APP_STEP_HEADER_INCLUDED

#endif