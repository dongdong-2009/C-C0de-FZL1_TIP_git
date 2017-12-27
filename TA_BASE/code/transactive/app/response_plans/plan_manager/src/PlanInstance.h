//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_PLANINSTANCE_H__0224FB0B_0710_42FE_AA06_EC12CC4AEC78__INCLUDED_)
#define AFX_PLANINSTANCE_H__0224FB0B_0710_42FE_AA06_EC12CC4AEC78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "ace/RW_Mutex.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_manager/src/PlanInstanceListCtrl.h"
#include "app/response_plans/plan_manager/src/PlanInstanceComboBox.h"
#include "app/response_plans/plan_manager/src/SinglePlanInstanceListCtrl.h"
#include "app/response_plans/plan_manager/src/PlanStepListCtrl.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/SynchronisedMap.h"


namespace TA_Base_App
{
    class PlanNode;
    class PlanStep;
    class PlanInstanceFilter;


    /////////////////////////////////////////////////////////////////////////////
    // PlanInstance

    class PlanInstance
    {
    // Operations
    public:
        PlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail);
        virtual ~PlanInstance();

        virtual bool operator<(const PlanInstance &other) const;

        const int getActivePlanState() const;
        const TA_Base_Core::ActivePlanId getActivePlanId() const;
        virtual PlanInstanceId getInstanceId() const;
        virtual bool customisationEnabled() const;
        virtual bool isLocalInstance() const;
        virtual bool ownedByCurrentSession() const;
        virtual TA_Base_Core::StepCustomisableDetail getCustomStepDetail(PlanStep *planStep); // returns the configured details if
                                                                                              // step has not been customised

        // Enable actions that can be performed on this instance
        virtual void updateCmdUI(CCmdUI &cmdUI, PlanNode &planNode, PlanStepNumber stepPosition = 0);

        // Window controls modifiers
        virtual void addToPlanInstanceList(CPlanInstanceListCtrl &instanceListCtrl);
        virtual void addToPlanInstanceCombo(CPlanInstanceComboBox &instanceComboBox);
        virtual void setSinglePlanInstanceList(CSinglePlanInstanceListCtrl &instanceListCtrl);
        virtual void setProgressIndicator(CPlanStepListCtrl &stepListCtrl);
        virtual void setCustomStepDetails(CPlanStepListCtrl &stepListCtrl);
        virtual void setProgressDetails(CPlanStepListCtrl &stepListCtrl);

        // Active plan and plan customisation update handlers. Return true if update is accepted.
        virtual void update(const TA_Base_Core::ActivePlanDetail &activePlanDetail);
        virtual void update(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail);

        // Plan control
        virtual void stop();
        virtual void pause();
        virtual void resume();
        virtual void takeControl();
        virtual void releaseControl();
        virtual void customiseStep(const TA_Base_Core::StepCustomisableDetail &customStepDetail);

    protected:
        PlanInstance();

        typedef SynchronisedMap<PlanStepNumber, TA_Base_Core::ActiveStepDetail> StepExecDetailMap;
        typedef SynchronisedMap<PlanStepNumber, TA_Base_Core::StepCustomisableDetail> StepCustomDetailMap;

        virtual StepExecDetailMap &getStepExecDetailMap();
        virtual StepCustomDetailMap &getStepCustomDetailMap();

        virtual TA_Base_Core::ExecutionHistory *getExecutionHistory();
        virtual TA_Base_Core::CustomisationHistory *getCustomisationHistory();

        void setInstanceLocalityIndicator();
        void setInstanceOwnershipIndicator();

    private:
        PlanInstance(const PlanInstance &other);
        PlanInstance &operator=(const PlanInstance &other);


    // Attributes
    private:
        mutable ACE_RW_Mutex m_activePlanDetailLock;
        TA_Base_Core::ActivePlanDetail m_activePlanDetail;
        StepExecDetailMap *m_stepExecDetailMap;
        StepCustomDetailMap *m_stepCustomDetailMap;
        TA_Base_Core::UpdateId m_stepExecDetailMapLastUpdateId;
        TA_Base_Core::UpdateId m_stepCustomDetailMapLastUpdateId;
        bool m_customisationEnabled;
        bool m_isLocalInstance;
        bool m_ownedByCurrentSession;
    };
}

#endif // !defined(AFX_PLANINSTANCE_H__0224FB0B_0710_42FE_AA06_EC12CC4AEC78__INCLUDED_)
