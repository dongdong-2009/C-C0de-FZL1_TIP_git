/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/AssignCameraToMonitorStep.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "core/utilities/src/TAAssert.h"
#include "AssignCameraToMonitorStep.h"
#include "CameraStepParameter.h"
#include "MonitorStepParameter.h"
#include "resource.h"
#include "PlanStepEditAssignCameraToMonitorPage.h"
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

    AssignCameraToMonitorStep::AssignCameraToMonitorStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
         PlanStep(parent,stepDetail),
         m_cameraParameter(NULL),
         m_monitorParameter(NULL)
    {
        FUNCTION_ENTRY( "AssignCameraToMonitorStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    AssignCameraToMonitorStep::AssignCameraToMonitorStep(PlanNode* parent,PlanStepId stepId) :
         PlanStep(parent,stepId,TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP),
         m_cameraParameter(NULL),
         m_monitorParameter(NULL)
    {
        FUNCTION_ENTRY( "AssignCameraToMonitorStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    AssignCameraToMonitorStep::AssignCameraToMonitorStep(const AssignCameraToMonitorStep& planStep) :
         PlanStep(planStep),
         m_cameraParameter(NULL),
         m_monitorParameter(NULL)
    {
        FUNCTION_ENTRY( "AssignCameraToMonitorStep" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_cameraParameter = new CameraStepParameter(*this, *planStep.m_cameraParameter);
        m_monitorParameter = new MonitorStepParameter(*this, *planStep.m_monitorParameter);

        addPlanStepParameter(*m_cameraParameter);
        addPlanStepParameter(*m_monitorParameter);

        FUNCTION_EXIT;
    }


    AssignCameraToMonitorStep::~AssignCameraToMonitorStep()
    {
        FUNCTION_ENTRY( "~AssignCameraToMonitorStep" );

        // The parameters are deleted by PlanStep.

        FUNCTION_EXIT;
    }


    PlanStep* AssignCameraToMonitorStep::clone()
    {
        FUNCTION_ENTRY( "clone" );

        // Ensure parameters are loaded.

        PlanStepParameterList stepParameters;
        getPlanStepParameters(stepParameters);

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new AssignCameraToMonitorStep(*this);
    }


    void AssignCameraToMonitorStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        // DEFAULT_KEY represents no Camera/Monitor selected.

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_cameraParameter = new CameraStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_CAMERA_NAME), DEFAULT_KEY);
        m_monitorParameter = new MonitorStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_MONITOR_NAME), DEFAULT_KEY);
        stepParameters.clear();
        stepParameters.push_back(m_cameraParameter);
        stepParameters.push_back(m_monitorParameter);

        FUNCTION_EXIT;
    }


    void AssignCameraToMonitorStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                             PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        if (parameterSet._d() != TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP)
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for assign camera to monitor step.";

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP));
        }

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_cameraParameter = new CameraStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_CAMERA_NAME), parameterSet.cameraMonitor().camera);
        m_monitorParameter = new MonitorStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_MONITOR_NAME), parameterSet.cameraMonitor().monitor);
        stepParameters.clear();
        stepParameters.push_back(m_cameraParameter);
        stepParameters.push_back(m_monitorParameter);

        FUNCTION_EXIT;
    }


    void AssignCameraToMonitorStep::getUpdatedParameters(TA_Base_Core::Step& step)
    {
        FUNCTION_ENTRY( "getUpdatedParameters" );

        TA_Base_Core::AssignCameraToMonitorParameters  cameraMonitorParameters;
        cameraMonitorParameters.camera = m_cameraParameter->getCameraKey();
        cameraMonitorParameters.monitor = m_monitorParameter->getMonitorKey();
        step.parameters.cameraMonitor(cameraMonitorParameters);

        FUNCTION_EXIT;
    }


    CPlanStepEditParameterPage* AssignCameraToMonitorStep::getParameterEditPage()
    {
        FUNCTION_ENTRY( "getParameterEditPage" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new CPlanStepEditAssignCameraToMonitorPage(this);
    }


} // namespace TA_Base_App

#endif // defined(STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
