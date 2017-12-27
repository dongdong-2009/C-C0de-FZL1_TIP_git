//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/AssignCameraToMonitorStep.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
//

#if !defined(AFX_ASSIGN_CAMERA_TO_MONITOR_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_ASSIGN_CAMERA_TO_MONITOR_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class CameraStepParameter;
    class MonitorStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // AssignCameraToMonitorStep

    class AssignCameraToMonitorStep : public PlanStep
    {
    // Operations
    public:
        AssignCameraToMonitorStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        AssignCameraToMonitorStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~AssignCameraToMonitorStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        CameraStepParameter* getCameraParameter() { return m_cameraParameter;};
        MonitorStepParameter* getMonitorParameter() { return m_monitorParameter;};

    protected:
        AssignCameraToMonitorStep(const AssignCameraToMonitorStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        AssignCameraToMonitorStep &operator=(const AssignCameraToMonitorStep &other);


    // Attributes
    private:
        CameraStepParameter*    m_cameraParameter;
        MonitorStepParameter*   m_monitorParameter;
    };
}

#endif // defined(STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#endif // !defined(AFX_ASSIGN_CAMERA_TO_MONITOR_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
