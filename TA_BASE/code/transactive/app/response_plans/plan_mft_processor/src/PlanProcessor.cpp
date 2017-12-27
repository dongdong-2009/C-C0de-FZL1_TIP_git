/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PlanProcessor.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include "app/response_plans/plan_mft_processor/src/PlanProcessor.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"

namespace TA_Base_App
{

    PlanProcessor::PlanProcessor()
    {
    }

    PlanProcessor::~PlanProcessor()
    {
    }

    void PlanProcessor::init( TA_Base_App::IDllMgrCallBack* callback )
    {
        ActivePlanAgency::instance()->init();
    }

    void PlanProcessor::uninit()
    {
        ActivePlanAgency::instance()->uninit();

        ActivePlanAgency::close();
    }

    std::string PlanProcessor::getDLLName() const
    {
        return "TA_Plan_Mft_Step_Message.dll";
    }

}