/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepMap.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is a container used by a PlanNode to store its PlanStep's. The steps are stored in a map
  * where the key is their step ID. This class allows steps to be added, deleted, copied and moved.
  *
  **/

#if !defined(AFX_PLANSTEPMAP_H__7D11B3AB_F456_49BF_8BD9_807BD007CF24__INCLUDED_)
#define AFX_PLANSTEPMAP_H__7D11B3AB_F456_49BF_8BD9_807BD007CF24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "SynchronisedMap.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class PlanStep;
    class PlanNode;


    /////////////////////////////////////////////////////////////////////////////
    // PlanStep

    class PlanStepMap
    {
    // Operations
    public:
        PlanStepMap(PlanNode* parent, const TA_Base_Core::StepDetails &stepDetails);
        virtual ~PlanStepMap();

        void getPlanSteps(PlanStepList &planSteps);
        PlanStep *getPlanStep(PlanStepId stepId);

        void insertPlanStep(TA_Base_Core::EStep stepType, PlanStepNumber position);
        PlanStepNumber deletePlanSteps(PlanStepList &deleteSteps);
        void copyPlanSteps(const PlanStepList &copySteps, PlanStepNumber position);
        PlanStepNumber movePlanSteps(const PlanStepList &moveSteps, PlanStepNumber position);
        PlanStepNumber getLastStepPosition();

    protected:
        PlanStep* createPlanStep(const TA_Base_Core::StepDetail &stepDetail);
        PlanStep* createNewPlanStep(const TA_Base_Core::EStep stepType,PlanStepId stepId);

    private:
        PlanStepMap(const PlanStepMap &other);
        PlanStepMap &operator=(const PlanStepMap &other);


    // Attributes
    private:
        SynchronisedMap<PlanStepId, PlanStep *> m_planStepMap;

        PlanNode*   m_parent;
    };
}

#endif // !defined(AFX_PLANSTEPMAP_H__7D11B3AB_F456_49BF_8BD9_807BD007CF24__INCLUDED_)
