/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/DecisionActiveStep.h $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  **/

#pragma once

#include "app/response_plans/plan_mft_processor/src/ActivePlanStep.h"

namespace TA_Base_App
{
    class CDecisionDlg;

    class DecisionActiveStep: public TA_Base_App::ActivePlanStep
    {
        // Operations
    public:
        DecisionActiveStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

        // ActivePlanStep interface
        virtual void execute();
        virtual void stop();

        virtual ~DecisionActiveStep();

    protected:
        virtual void launch();

    private:
        TA_Base_App::CDecisionDlg* m_pDecisionDlg;
    };

}
