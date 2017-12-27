/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsSetSceneStep.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_VMS_SET_SCENE)

#include "core/utilities/src/TAAssert.h"
#include "VmsSetSceneStep.h"
#include "InvalidStepParameterException.h"
#include "VmsStepParameter.h"
#include "VmsSceneStepParameter.h"
#include "resource.h"
#include "PlanStepEditVmsSetScenePage.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const unsigned long DEFAULT_VMS_KEY = 0;
    const unsigned long DEFAULT_SCENE_KEY = 0;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanStep

    VmsSetSceneStep::VmsSetSceneStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail)
       : PlanStep(parent,stepDetail),
         m_vmsParameter(NULL),
         m_vmsSceneParameter(NULL)
    {
        FUNCTION_ENTRY( "VmsSetSceneStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    VmsSetSceneStep::VmsSetSceneStep(PlanNode* parent,PlanStepId stepId)
       : PlanStep(parent,stepId,TA_Base_Core::VMS_SET_SCENE_STEP),
         m_vmsParameter(NULL),
         m_vmsSceneParameter(NULL)
    {
        FUNCTION_ENTRY( "VmsSetSceneStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    VmsSetSceneStep::VmsSetSceneStep(const VmsSetSceneStep& planStep)
       : PlanStep(planStep),
         m_vmsParameter(NULL),
         m_vmsSceneParameter(NULL)
    {
        FUNCTION_ENTRY( "VmsSetSceneStep" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_vmsParameter = new VmsStepParameter(*this, *planStep.m_vmsParameter);
        m_vmsSceneParameter = new VmsSceneStepParameter(*this, *planStep.m_vmsSceneParameter);

        addPlanStepParameter(*m_vmsParameter);
           addPlanStepParameter(*m_vmsSceneParameter);

        FUNCTION_EXIT;
    }


    VmsSetSceneStep::~VmsSetSceneStep()
    {
        FUNCTION_ENTRY( "~VmsSetSceneStep" );

        // The parameters are deleted by PlanStep.

        FUNCTION_EXIT;
    }


    PlanStep* VmsSetSceneStep::clone()
    {
        FUNCTION_ENTRY( "clone" );

        // Ensure parameters are loaded.

        PlanStepParameterList stepParameters;
        getPlanStepParameters(stepParameters);

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new VmsSetSceneStep(*this);
    }


    void VmsSetSceneStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        // DEFAULT_VMS_KEY represents no VMS/scene selected.

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_vmsParameter = new VmsStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_VMS_NAME), DEFAULT_VMS_KEY);
        m_vmsSceneParameter = new VmsSceneStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_VMS_SCENE), DEFAULT_SCENE_KEY);
        stepParameters.clear();
        stepParameters.push_back(m_vmsParameter);
        stepParameters.push_back(m_vmsSceneParameter);

        FUNCTION_EXIT;
    }


    void VmsSetSceneStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                             PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        if (parameterSet._d() != TA_Base_Core::VMS_SET_SCENE_STEP)
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for assign vms to monitor step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::VMS_SET_SCENE_STEP));
        }

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_vmsParameter = new VmsStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_VMS_NAME), parameterSet.vmsSetScene().vmsSign);
        m_vmsSceneParameter = new VmsSceneStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_VMS_SCENE), parameterSet.vmsSetScene().scene);
        stepParameters.clear();
        stepParameters.push_back(m_vmsParameter);
        stepParameters.push_back(m_vmsSceneParameter);

        FUNCTION_EXIT;
    }


    void VmsSetSceneStep::getUpdatedParameters(TA_Base_Core::Step& step)
    {
        FUNCTION_ENTRY( "getUpdatedParameters" );

        TA_Base_Core::VmsSetSceneParameters vmsSetSceneParameters;
        vmsSetSceneParameters.vmsSign = m_vmsParameter->getVmsKey();
        vmsSetSceneParameters.scene = m_vmsSceneParameter->getVmsSceneKey();
        step.parameters.vmsSetScene(vmsSetSceneParameters);

        FUNCTION_EXIT;
    }


    CPlanStepEditParameterPage* VmsSetSceneStep::getParameterEditPage()
    {
        FUNCTION_ENTRY( "getParameterEditPage" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new CPlanStepEditVmsSetScenePage(this);
    }


} // namespace TA_Base_App

#endif // defined(STEPTYPE_VMS_SET_SCENE)
