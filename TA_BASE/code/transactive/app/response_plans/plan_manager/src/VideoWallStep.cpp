/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VideoWallStep.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles VideoWall step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_VIDEOWALL)

#include "core/utilities/src/TAAssert.h"
#include "VideoWallStep.h"
#include "InvalidStepParameterException.h"
#include "CameraStepParameter.h"
#include "WallboardLayoutStepParameter.h"
#include "resource.h"
#include "PlanStepEditVideoWallPage.h"
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

    VideoWallStep::VideoWallStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail)
       : PlanStep(parent,stepDetail),
         m_wallboardParameter(NULL),
         m_layoutParameter(NULL)
    {
        FUNCTION_ENTRY( "VideoWallStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    VideoWallStep::VideoWallStep(PlanNode* parent,PlanStepId stepId)
       : PlanStep(parent,stepId,TA_Base_Core::VIDEO_WALL_STEP),
         m_wallboardParameter(NULL),
         m_layoutParameter(NULL)
    {
        FUNCTION_ENTRY( "VideoWallStep" );

        // Do nothing

        FUNCTION_EXIT;
    }


    VideoWallStep::VideoWallStep(const VideoWallStep& planStep)
       : PlanStep(planStep),
         m_wallboardParameter(NULL),
         m_layoutParameter(NULL)
    {
        FUNCTION_ENTRY( "VideoWallStep" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_wallboardParameter = new WallboardStepParameter(*this, *planStep.m_wallboardParameter);
        m_layoutParameter = new WallboardLayoutStepParameter(*this, *planStep.m_layoutParameter);

        addPlanStepParameter(*m_wallboardParameter);
        addPlanStepParameter(*m_layoutParameter);

        FUNCTION_EXIT;
    }


    VideoWallStep::~VideoWallStep()
    {
        FUNCTION_ENTRY( "~VideoWallStep" );

        // The parameters are deleted by PlanStep.

        FUNCTION_EXIT;
    }


    PlanStep* VideoWallStep::clone()
    {
        FUNCTION_ENTRY( "clone" );

        // Ensure parameters are loaded.

        PlanStepParameterList stepParameters;
        getPlanStepParameters(stepParameters);

        // TODO Handle failure of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new VideoWallStep(*this);
    }


    void VideoWallStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        // DEFAULT_KEY represents no Wallboard/Layout selected.

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_wallboardParameter = new WallboardStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_WALLBOARD_NAME), DEFAULT_KEY);
        m_layoutParameter = new WallboardLayoutStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_LAYOUT_NAME), DEFAULT_KEY);
        stepParameters.clear();
        stepParameters.push_back(m_wallboardParameter);
        stepParameters.push_back(m_layoutParameter);

        FUNCTION_EXIT;
    }


    void VideoWallStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                             PlanStepParameterList &stepParameters)
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        if (parameterSet._d() != TA_Base_Core::VIDEO_WALL_STEP)
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for assign camera to preset step.";

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::VIDEO_WALL_STEP));
        }

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.

        m_wallboardParameter = new WallboardStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_WALLBOARD_NAME), parameterSet.videoWallParams().wallBoard);
        m_layoutParameter = new WallboardLayoutStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_LAYOUT_NAME), parameterSet.videoWallParams().layout);
        stepParameters.clear();
        stepParameters.push_back(m_wallboardParameter);
        stepParameters.push_back(m_layoutParameter);

        FUNCTION_EXIT;
    }


    void VideoWallStep::getUpdatedParameters(TA_Base_Core::Step& step)
    {
        FUNCTION_ENTRY( "getUpdatedParameters" );

        TA_Base_Core::VideoWallParameters  videoWallParameters;
        videoWallParameters.wallBoard = m_wallboardParameter->getWallboardKey();
        videoWallParameters.layout = m_layoutParameter->getLayoutKey();
        step.parameters.videoWallParams(videoWallParameters);

        FUNCTION_EXIT;
    }


    CPlanStepEditParameterPage* VideoWallStep::getParameterEditPage()
    {
        FUNCTION_ENTRY( "getParameterEditPage" );

        // TODO Handle failures of new.  This needs to be done with respect to the whole program.
        FUNCTION_EXIT;
        return new CPlanStepEditVideoWallPage(this);
    }


} // namespace TA_Base_App

#endif // defined(STEPTYPE_VIDEOWALL)
