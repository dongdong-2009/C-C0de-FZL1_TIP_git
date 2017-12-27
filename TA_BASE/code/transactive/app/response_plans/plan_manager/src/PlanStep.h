//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEP_H__8F16FA86_EFF9_4954_B444_3831A70A28DB__INCLUDED_)
#define AFX_PLANSTEP_H__8F16FA86_EFF9_4954_B444_3831A70A28DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "PlanStepListCtrl.h"
#include "PlanStepParameterListCtrl.h"
#include "SynchronisedVector.h"
#include "PlanManagerCommonDefs.h"

class CPlanStepEditDialog;

namespace TA_Base_App
{
    class PlanStepParameter;
    class PlanNode;
    class CPlanStepEditParameterPage;


    /////////////////////////////////////////////////////////////////////////////
    // PlanStep

    class PlanStep
    {
    // Operations
    public:
        PlanStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail);
        PlanStep(PlanNode* parent, PlanStepId stepId, TA_Base_Core::EStep stepType);  // For new plan steps
        virtual ~PlanStep();

        virtual PlanStep* clone() = 0;
        virtual bool isStepBranching() const = 0;
        virtual bool canIgnoreFailure() const = 0;  // step failure cannot be ignored where its outcome determines
                                                    // the next step to be executed
		virtual bool canEnableNoWait() const {return false;} ;
        // Accessor methods
        PlanStepId          getStepId() const;
        PlanStepNumber      getStepPosition() const;
        const char *        getStepName() const;
        const char *        getStepDescription() const;
        TA_Base_Core::EStep        getStepType() const;
        bool                isStepSkipped() const;
        bool                isStepSkippable() const;
        bool                ignoreFailure() const;
		bool				noWait() const;
        TA_Base_Core::Seconds    getStepDelay() const;

        // Editor methods
        void addPlanStepParameter(PlanStepParameter& parameter);
        void setStepPosition(PlanStepNumber stepPosition);
        void setStepName(const char* name);
        void setStepDescription(const char* description);
        void setStepSkippable(bool skip);
        void setStepSkipped(bool skip);
        void setIgnoreFailure(bool ignoreFailure);
		void setNoWait(bool nowait);
        void setStepDelay(unsigned long delay);
        void setStepId(PlanStepId stepId);

        virtual void addToStepList(CPlanStepListCtrl &listCtrl);
        virtual void addParametersToParameterList(CPlanStepParameterListCtrl &listCtrl);
        virtual void updateCmdUI(CCmdUI &cmdUI);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

        // Methods to get updated step information for writting back to agent
        TA_Base_Core::Step getUpdatedStepAndParameters();
        PlanNode* getParentPlan() const { return m_parent;}

        bool editStep();
        void viewStep();
        bool customiseStep(TA_Base_Core::StepCustomisableDetail &customStepDetail);

    protected:
        PlanStep(const PlanStep& planStep);

        // Get parameters for update back to agent
        virtual void getUpdatedParameters(TA_Base_Core::Step& step) = 0;

        virtual void getPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters)=0;
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters)=0;

        virtual bool canDeleteStep();

        std::string getParameterName(UINT stringResourceID);

    private:
        PlanStep &operator=(const PlanStep &other);


    // Attributes
    protected:
        PlanStepParameterList* m_planStepParameterList;
        TA_Base_Core::ReEntrantThreadLockable m_planStepParameterListLock;

    private:
        TA_Base_Core::StepDetail m_stepDetail;

        PlanNode* m_parent;
        bool m_newStep;
    };
}

#endif // !defined(AFX_PLANSTEP_H__8F16FA86_EFF9_4954_B444_3831A70A28DB__INCLUDED_)
