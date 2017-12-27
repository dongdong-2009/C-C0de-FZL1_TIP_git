/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PlanProcessor.h $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  **/

#ifndef PLANPROCESSOR_H_INCLUDED
#define PLANPROCESSOR_H_INCLUDED

#include "app/system_control/dll_host/dll_components/dll_component_helper/src/AbstractDllComponent.h"

namespace TA_Base_App
{
    class PlanProcessor : public TA_Base_App::AbstractDllComponent
    {
    public:

        PlanProcessor();
        ~PlanProcessor();

        virtual void init( TA_Base_App::IDllMgrCallBack* callback );
        virtual void uninit();
        virtual std::string getDLLName() const;
    };
}

#endif