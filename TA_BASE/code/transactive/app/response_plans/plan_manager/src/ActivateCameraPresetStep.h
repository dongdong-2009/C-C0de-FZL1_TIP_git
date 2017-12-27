//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivateCameraPresetStep.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Handles Activate Camera Preset step.

#if !defined(AFX_ACTIVATE_CAMERA_PRESET_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_ACTIVATE_CAMERA_PRESET_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class CameraStepParameter;
    class CameraPresetStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // ActivateCameraPresetStep

    class ActivateCameraPresetStep : public PlanStep
    {
    // Operations
    public:
        ActivateCameraPresetStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        ActivateCameraPresetStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~ActivateCameraPresetStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        CameraStepParameter* getCameraParameter() { return m_cameraParameter;};
        CameraPresetStepParameter* getPresetParameter() { return m_cameraPresetParameter;};

    protected:
        ActivateCameraPresetStep(const ActivateCameraPresetStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        ActivateCameraPresetStep &operator=(const ActivateCameraPresetStep &other);


    // Attributes
    private:
        CameraStepParameter* m_cameraParameter;
        CameraPresetStepParameter* m_cameraPresetParameter;
    };
}

#endif // defined(STEPTYPE_ACTIVATE_CAMERA_PRESET)

#endif // !defined(AFX_ACTIVATE_CAMERA_PRESET_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
