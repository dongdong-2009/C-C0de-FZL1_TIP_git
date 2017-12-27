#if defined ( STEPTYPE_LAUNCH_SCHEMATIC )

#ifndef ACTIVE_PLAN_LAUNCH_SCHEMATIC_STEP_MSG_HEADER_INCLUDED
#define ACTIVE_PLAN_LAUNCH_SCHEMATIC_STEP_MSG_HEADER_INCLUDED

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
  *  Generated when the result of a Launch Schematic interactive step execution request
  *  is received back from the Plan Manager.
  */

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanLaunchSchematicStepMessage : public IActivePlanMessage
    {
    //public methods
    public:
        ActivePlanLaunchSchematicStepMessage(const int status, const std::string& error);
        virtual ~ActivePlanLaunchSchematicStepMessage();
        
        virtual void execute( ActivePlan& activePlan ) const;
    
    private:
        ActivePlanLaunchSchematicStepMessage& operator=( const ActivePlanLaunchSchematicStepMessage& );
        ActivePlanLaunchSchematicStepMessage( const ActivePlanLaunchSchematicStepMessage& );

        const int m_status;
        std::string m_error;
    };

}

#endif // ACTIVE_PLAN_LAUNCH_SCHEMATIC_STEP_MSG_HEADER_INCLUDED

#endif