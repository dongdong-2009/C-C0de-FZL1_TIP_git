/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PlanAgentUpdateProcessor.h $
  * @author:  Huang Jian
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/06/12 12:20:17 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#ifndef LAUNCHSCHEMATICACTIVESTEP_H
#define LAUNCHSCHEMATICACTIVESTEP_H

#include "app/response_plans/plan_mft_processor/src/ActivePlanStep.h"


namespace TA_Base_App
{

    class LaunchSchematicActiveStep : public TA_Base_App::ActivePlanStep
    {
        // Operations
    public:
        LaunchSchematicActiveStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

        // ActivePlanStep interface
        virtual void execute();
        virtual void stop();

        virtual ~LaunchSchematicActiveStep();

    private:
        void launch();
    };

}

#endif //LAUNCHSCHEMATICACTIVESTEP_H