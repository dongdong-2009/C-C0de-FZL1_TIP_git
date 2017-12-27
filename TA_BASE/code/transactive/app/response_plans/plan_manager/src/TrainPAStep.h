//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainPAStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_TRAINPASTEP_H__EF80FEF7_9A8E_423E_8A7F_FC84A2A18F1B__INCLUDED_)
#define AFX_TRAINPASTEP_H__EF80FEF7_9A8E_423E_8A7F_FC84A2A18F1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class TrainStepParameter;
    class    PaMessageStepParameter;
    class    TimeSpanStepParameter;
    class YesNoStepParameter;
    /////////////////////////////////////////////////////////////////////////////
    // TrainPAStep

    class TrainPAStep : public PlanStep
    {
        // Operations
    public:
        TrainPAStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        TrainPAStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~TrainPAStep();
        PaMessageStepParameter* getPAMessageParameter()        { return m_messageParameter; }
        TrainStepParameter*        getTargetTrainParameter()    { return m_trainParameter;   }
        YesNoStepParameter*        getCyclicParameter()            { return m_cyclicParameter;  }
        TimeSpanStepParameter*  getDurationParameter()        { return m_durationParameter;}

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

    protected:
        TrainPAStep(const TrainPAStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        TrainPAStep &operator=(const TrainPAStep &other);


        // Attributes
    private:
        TrainStepParameter*       m_trainParameter;
        PaMessageStepParameter*      m_messageParameter;
        YesNoStepParameter*          m_cyclicParameter;
        TimeSpanStepParameter*    m_durationParameter;
    };
}

#endif //defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)
#endif // !defined(AFX_TRAINPASTEP_H__EF80FEF7_9A8E_423E_8A7F_FC84A2A18F1B__INCLUDED_)
