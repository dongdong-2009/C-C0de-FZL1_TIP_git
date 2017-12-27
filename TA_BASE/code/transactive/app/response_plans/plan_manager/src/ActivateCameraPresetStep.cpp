/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivateCameraPresetStep.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles Activate Camera Preset step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "core/utilities/src/TAAssert.h"
#include "ActivateCameraPresetStep.h"
#include "InvalidStepParameterException.h"
#include "CameraStepParameter.h"
#include "CameraPresetStepParameter.h"
#include "resource.h"
#include "PlanStepEditActivateCameraPresetPage.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const unsigned long DEFAULT_KEY = 0;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanStep

    ActivateCameraPresetStep::ActivateCameraPresetStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
         PlanStep(parent,stepDetail),
         m_cameraParameter(NULL),
         m_cameraPresetParameter(NULL)
    {
        FUNCTION_ENTRY( "ActivateCameraPresetStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    ActivateCameraPresetStep::ActivateCameraPresetStep(PlanNode* parent,PlanStepId stepId) :
         PlanStep(parent,stepId,TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP),
         m_cameraParameter(NULL),
         m_cameraPresetParameter(NULL)
    {
        FUNCTION_ENTRY( "ActivateCameraPresetStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    ActivateCameraPresetStep::ActivateCameraPresetStep(const ActivateCameraPresetStep& planStep) :
         PlanStep(planStep),
         m_cameraParameter(NULL),
         m_cameraPresetParameter(NULL)
    {
        FUNCTION_ENTRY( "ActivateCameraPresetStep" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_cameraParameter = new CameraStepParameter(*this, *planStep.m_cameraParameter);
        m_cameraPresetParameter = new CameraPresetStepParameter(*this, *planStep.m_cameraPresetParameter);

        addPlanStepParameter(*m_cameraParameter);
        addPlanStepParameter(*m_cameraPresetParameter);

        FUNCTION_EXIT;
    }


    ActivateCameraPresetStep::~ActivateCameraPresetStep()
    {
        FUNCTION_ENTRY( "~ActivateCameraPresetStep" );

        // The parameters are deleted by PlanStep.

        FUNCTION_EXIT;
    }


    PlanStep* ActivateCameraPresetStep::clone()
    {
        FUNCTION_ENTRY( "clone" );

        // Ensure parameters are loaded.

        PlanStepParameterList stepParameters;
        getPlanStepParameters(stepParameters);

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new ActivateCameraPresetStep(*this);
    }


    void ActivateCameraPresetStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        // DEFAULT_KEY represents no Camera/Preset selected.

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_cameraParameter = new CameraStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_CAMERA_NAME), DEFAULT_KEY);
        m_cameraPresetParameter = new CameraPresetStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PRESET_NAME), DEFAULT_KEY, DEFAULT_KEY);
        stepParameters.clear();
        stepParameters.push_back(m_cameraParameter);
        stepParameters.push_back(m_cameraPresetParameter);

        FUNCTION_EXIT;
    }


    void ActivateCameraPresetStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                             PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        if (parameterSet._d() != TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP)
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for assign camera to preset step.";

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP));
        }

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_cameraParameter = new CameraStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_CAMERA_NAME), parameterSet.cameraPreset().camera);
        m_cameraPresetParameter = new CameraPresetStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PRESET_NAME),
            parameterSet.cameraPreset().camera, parameterSet.cameraPreset().preset);
        stepParameters.clear();
        stepParameters.push_back(m_cameraParameter);
        stepParameters.push_back(m_cameraPresetParameter);

        FUNCTION_EXIT;
    }


    void ActivateCameraPresetStep::getUpdatedParameters(TA_Base_Core::Step& step)
    {
        FUNCTION_ENTRY( "getUpdatedParameters" );

        TA_Base_Core::ActivateCameraPresetParameters  cameraPresetParameters;
        cameraPresetParameters.camera = m_cameraParameter->getCameraKey();
        cameraPresetParameters.preset = m_cameraPresetParameter->getPresetKey();
        step.parameters.cameraPreset(cameraPresetParameters);

        FUNCTION_EXIT;
    }


    CPlanStepEditParameterPage* ActivateCameraPresetStep::getParameterEditPage()
    {
        FUNCTION_ENTRY( "getParameterEditPage" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new CPlanStepEditActivateCameraPresetPage(this);
    }


} // namespace TA_Base_App

#endif // defined(STEPTYPE_ACTIVATE_CAMERA_PRESET)
