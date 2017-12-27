//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsSetSceneStep.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
//

#if !defined(AFX_VMS_SET_SCENE_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_VMS_SET_SCENE_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_VMS_SET_SCENE)

#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class VmsStepParameter;
    class VmsSceneStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // VmsSetSceneStep

    class VmsSetSceneStep : public PlanStep
    {
    // Operations
    public:
        VmsSetSceneStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        VmsSetSceneStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~VmsSetSceneStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        VmsStepParameter* getVmsParameter() { return m_vmsParameter; };
        VmsSceneStepParameter* getVmsSceneParameter() { return m_vmsSceneParameter; };

    protected:
        VmsSetSceneStep(const VmsSetSceneStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        VmsSetSceneStep &operator=(const VmsSetSceneStep &other);


    // Attributes
    private:
        VmsStepParameter* m_vmsParameter;
        VmsSceneStepParameter* m_vmsSceneParameter;
    };

} // namespace TA_Base_App

#endif // defined(STEPTYPE_VMS_SET_SCENE)

#endif // !defined(AFX_VMS_SET_SCENE_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
