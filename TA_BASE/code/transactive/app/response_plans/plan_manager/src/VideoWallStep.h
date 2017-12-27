//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VideoWallStep.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Handles VideoWall step.

#if !defined(AFX_VIDEO_WALL_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_VIDEO_WALL_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_VIDEOWALL)

#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class WallboardStepParameter;
    class WallboardLayoutStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // VideoWallStep

    class VideoWallStep : public PlanStep
    {
    // Operations
    public:
        VideoWallStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        VideoWallStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~VideoWallStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        WallboardStepParameter* getWallboardParameter() { return m_wallboardParameter;};
        WallboardLayoutStepParameter* getLayoutParameter() { return m_layoutParameter;};

    protected:
        VideoWallStep(const VideoWallStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        VideoWallStep &operator=(const VideoWallStep &other);


    // Attributes
    private:
        WallboardStepParameter* m_wallboardParameter;
        WallboardLayoutStepParameter* m_layoutParameter;
    };
}

#endif // defined(STEPTYPE_VIDEOWALL)

#endif // !defined(AFX_VIDEO_WALL_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
