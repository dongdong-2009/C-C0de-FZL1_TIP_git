/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsBlankoutStep.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_VMS_BLANKOUT)

#include "core/utilities/src/TAAssert.h"
#include "VmsBlankoutStep.h"
#include "InvalidStepParameterException.h"
#include "VmsStepParameter.h"
#include "resource.h"
#include "PlanStepEditSelectVmsPage.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const unsigned long DEFAULT_VMS_KEY = 0;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanStep

    VmsBlankoutStep::VmsBlankoutStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail)
       : PlanStep(parent,stepDetail),
         m_vmsParameter(NULL)
    {
        FUNCTION_ENTRY( "VmsBlankoutStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    VmsBlankoutStep::VmsBlankoutStep(PlanNode* parent,PlanStepId stepId)
       : PlanStep(parent,stepId,TA_Base_Core::VMS_BLANKOUT_STEP),
         m_vmsParameter(NULL)
    {
        FUNCTION_ENTRY( "VmsBlankoutStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    VmsBlankoutStep::VmsBlankoutStep(const VmsBlankoutStep& planStep)
       : PlanStep(planStep),
         m_vmsParameter(NULL)
    {
        FUNCTION_ENTRY( "VmsBlankoutStep" );

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.

        m_vmsParameter = new VmsStepParameter(*this, *planStep.m_vmsParameter);

        addPlanStepParameter(*m_vmsParameter);

        FUNCTION_EXIT;
    }


    VmsBlankoutStep::~VmsBlankoutStep()
    {
        FUNCTION_ENTRY( "~VmsBlankoutStep" );

        // The parameters are deleted by PlanStep.

        FUNCTION_EXIT;
    }


    PlanStep* VmsBlankoutStep::clone()
    {
        FUNCTION_ENTRY( "clone" );

        // Ensure parameters are loaded.

        PlanStepParameterList stepParameters;
        getPlanStepParameters(stepParameters);

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new VmsBlankoutStep(*this);
    }


    void VmsBlankoutStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        // DEFAULT_VMS_KEY represents no VMS/scene selected.

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.

        m_vmsParameter = new VmsStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_VMS_NAME), DEFAULT_VMS_KEY);
        stepParameters.clear();
        stepParameters.push_back(m_vmsParameter);

        FUNCTION_EXIT;
    }


    void VmsBlankoutStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                             PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        if (parameterSet._d() != TA_Base_Core::VMS_BLANKOUT_STEP)
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for assign vms to monitor step.";

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::VMS_BLANKOUT_STEP));
        }

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.

        m_vmsParameter = new VmsStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_VMS_NAME), parameterSet.vmsBlankout().vmsSign);
        stepParameters.clear();
        stepParameters.push_back(m_vmsParameter);

        FUNCTION_EXIT;
    }


    void VmsBlankoutStep::getUpdatedParameters(TA_Base_Core::Step& step)
    {
        FUNCTION_ENTRY( "getUpdatedParameters" );

        TA_Base_Core::VmsBlankoutParameters  vmsBlankoutParameters;
        vmsBlankoutParameters.vmsSign = m_vmsParameter->getVmsKey();
        step.parameters.vmsBlankout(vmsBlankoutParameters);

        FUNCTION_EXIT;
    }


    CPlanStepEditParameterPage* VmsBlankoutStep::getParameterEditPage()
    {
        FUNCTION_ENTRY( "getParameterEditPage" );

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new CPlanStepEditSelectVmsPage(this);
    }


} // namespace TA_Base_App

#endif // defined(STEPTYPE_VMS_BLANKOUT)
