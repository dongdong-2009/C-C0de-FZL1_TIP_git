/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainTisStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents a Train Traveller Info System step.
  *
  **/

#if !defined(AFX_TRAINTISSTEP_H__B56D2E47_86BD_4B4E_8455_8BB3DA730B57__INCLUDED_)
#define AFX_TRAINTISSTEP_H__B56D2E47_86BD_4B4E_8455_8BB3DA730B57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class TrainStepParameter;
    class TisMessageStepParameter;
    class TisPriorityStepParameter;
    class TimeSpanStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // TrainTisStep

    class TrainTisStep : public PlanStep
    {
    // Operations
    public:
        TrainTisStep(PlanNode* parent,const TA_Base_Core::StepDetail& stepDetail);
        TrainTisStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~TrainTisStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const  { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        TrainStepParameter* getTrainParameter()          { return m_trainParameter; }
        TisMessageStepParameter* getMessageParameter()   { return m_messageParameter; }
        TisPriorityStepParameter* getPriorityParameter() { return m_priorityParameter; }
        TimeSpanStepParameter* getDurationParameter()    { return m_durationParameter; }

    protected:
        TrainTisStep(const TrainTisStep& other);

        virtual void createPlanStepParameters(PlanStepParameterList& stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters& parameterSet,
                                              PlanStepParameterList& stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        TrainTisStep& operator=(const TrainTisStep& other);


    // Attributes
    private:
        TrainStepParameter*       m_trainParameter;
        TisMessageStepParameter*  m_messageParameter;
        TisPriorityStepParameter* m_priorityParameter;
        TimeSpanStepParameter*    m_durationParameter;
    };
}

#endif // defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_TRAINTISSTEP_H__B56D2E47_86BD_4B4E_8455_8BB3DA730B57__INCLUDED_)
