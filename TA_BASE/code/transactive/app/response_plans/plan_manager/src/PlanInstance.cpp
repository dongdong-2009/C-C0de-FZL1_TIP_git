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

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "ace/Guard_T.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/ComparablePlanInstance.h"
#include "app/response_plans/plan_manager/src/Resource.h"
#include "app/response_plans/plan_manager/src/PlanInstance.h"
#include "app/response_plans/plan_manager/src/PlanNode.h"
#include "app/response_plans/plan_manager/src/PlanStep.h"
#include "app/response_plans/plan_manager/src/Utilities.h"
#include "app/response_plans/plan_manager/src/OperatorRights.h"
#include "app/response_plans/plan_manager/src/MessageBox.h"
#include "app/response_plans/plan_manager/src/PlanAgentAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanInstance

PlanInstance::PlanInstance() :
    m_stepExecDetailMap(NULL),
    m_stepCustomDetailMap(NULL),
    m_stepExecDetailMapLastUpdateId(0),
    m_stepCustomDetailMapLastUpdateId(0),
    m_customisationEnabled(false),
    m_isLocalInstance(false),
    m_ownedByCurrentSession(false)
{
    FUNCTION_ENTRY( "PlanInstance" );
    FUNCTION_EXIT;
}


PlanInstance::PlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail) :
    m_activePlanDetail(activePlanDetail),
    m_stepExecDetailMap(NULL),
    m_stepCustomDetailMap(NULL),
    m_stepExecDetailMapLastUpdateId(0),
    m_stepCustomDetailMapLastUpdateId(0),
    m_customisationEnabled(false),
    m_isLocalInstance(false),
    m_ownedByCurrentSession(false)
{
    FUNCTION_ENTRY( "PlanInstance" );

    setInstanceLocalityIndicator();
    setInstanceOwnershipIndicator();

    FUNCTION_EXIT;
}


PlanInstance::~PlanInstance()
{
    FUNCTION_ENTRY( "~PlanInstance" );

    delete m_stepExecDetailMap;
    delete m_stepCustomDetailMap;

    FUNCTION_EXIT;
}

bool PlanInstance::operator<(const PlanInstance &other) const
{
    TA_Base_Core::ActivePlanDetail stCmpDetail;
    {
        ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(other.m_activePlanDetailLock);
        stCmpDetail = other.m_activePlanDetail;
    }

    bool bRes = false;
    {
        ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);
        TA_Base_Bus::ComparablePlanInstance oMe(m_activePlanDetail);
        bRes = oMe.operator <( stCmpDetail );
    }

    return bRes;
}

const TA_Base_Core::ActivePlanId TA_Base_App::PlanInstance::getActivePlanId() const
{
    FUNCTION_ENTRY( "getActivePlanId" );

    ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);

    FUNCTION_EXIT;
    return m_activePlanDetail.plan;
}

const int TA_Base_App::PlanInstance::getActivePlanState() const
{
    FUNCTION_ENTRY( "getActivePlanState" );

    ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);

    FUNCTION_EXIT;
    return m_activePlanDetail.activePlanState;
}

PlanInstanceId PlanInstance::getInstanceId() const
{
    FUNCTION_ENTRY( "getInstanceId" );

    ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);

    FUNCTION_EXIT;
    return PlanInstanceId(m_activePlanDetail.plan.instance, m_activePlanDetail.plan.location);
}


bool PlanInstance::customisationEnabled() const
{
    FUNCTION_ENTRY( "customisationEnabled" );
    FUNCTION_EXIT;
    return m_customisationEnabled;
}


bool PlanInstance::isLocalInstance() const
{
    FUNCTION_ENTRY( "isLocalInstance" );
    FUNCTION_EXIT;
    return m_isLocalInstance;
}


bool PlanInstance::ownedByCurrentSession() const
{
    FUNCTION_ENTRY( "ownedByCurrentSession" );
    FUNCTION_EXIT;
    return m_ownedByCurrentSession;
}


TA_Base_Core::StepCustomisableDetail PlanInstance::getCustomStepDetail(PlanStep *planStep)
{
    FUNCTION_ENTRY( "getCustomStepDetail" );

    TA_Base_Core::StepCustomisableDetail customStepDetail;

    if (getStepCustomDetailMap().find(planStep->getStepPosition()))
    {
        customStepDetail = getStepCustomDetailMap()[planStep->getStepPosition()];
    }
    else
    {
        customStepDetail.position = planStep->getStepPosition();
        customStepDetail.skip = planStep->isStepSkipped();
        customStepDetail.ignoreFailure = planStep->ignoreFailure();
        customStepDetail.nowait = planStep->noWait();
        customStepDetail.delay = planStep->getStepDelay();
    }

    FUNCTION_EXIT;
    return customStepDetail;
}


void PlanInstance::updateCmdUI(CCmdUI &cmdUI, PlanNode &planNode, PlanStepNumber stepPosition)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    TA_Base_Core::EActivePlanState activePlanState = TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE;

    {
        ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);
        activePlanState = m_activePlanDetail.activePlanState;
    }

    bool isRunning = (( activePlanState == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE ) || 
                      ( activePlanState == TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE ));
    bool isPaused = (activePlanState == TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE);
    bool isDelayed = (activePlanState == TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE);
    bool isPending = (activePlanState == TA_Base_Core::PENDING_ACTIVE_PLAN_STATE);

    switch (cmdUI.m_nID)
    {
        case ID_PLAN_STOP:
            cmdUI.Enable(m_ownedByCurrentSession && (isRunning || isPaused || isDelayed || isPending) && OperatorRights::getInstance().canControlPlans());
            break;

        case ID_PLAN_PAUSE:
            cmdUI.Enable(m_ownedByCurrentSession && (isRunning || isDelayed) && OperatorRights::getInstance().canControlPlans());
            break;

        case ID_PLAN_RESUME_SELECTED_STEP:
        case ID_PLAN_RESUME:
            cmdUI.Enable(m_ownedByCurrentSession && (isPaused || isPending) && OperatorRights::getInstance().canControlPlans());
            break;

        case ID_PLAN_TAKECONTROL:
            cmdUI.Enable(!m_ownedByCurrentSession && (isRunning || isPaused || isDelayed || isPending) &&
                         OperatorRights::getInstance().canControlPlans() && OperatorRights::getInstance().canTakePlanOwnership());
            break;

        case ID_STEP_CUSTOMISE:
            m_customisationEnabled = m_ownedByCurrentSession && (isPaused || isPending) &&
                         OperatorRights::getInstance().canControlPlans() && OperatorRights::getInstance().canCustomisePlans();

            cmdUI.Enable(m_customisationEnabled);
            break;
/*
        case ID_PLAN_RELEASECONTROL:
            if (isMine && isRunning)
            {
                // must also be a 'Batch' plan - let PlanNode determine that
                planNode.updateCmdUI(cmdUI);
                break;
            }
            // fall through to default
*/
        default:
            cmdUI.Enable(FALSE);
    }

    FUNCTION_EXIT;
}


void PlanInstance::addToPlanInstanceList(CPlanInstanceListCtrl &instanceListCtrl)
{
    FUNCTION_ENTRY( "addToPlanInstanceList" );

    ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);

    instanceListCtrl.PopulateInstance(m_activePlanDetail);

    FUNCTION_EXIT;
}


void PlanInstance::addToPlanInstanceCombo(CPlanInstanceComboBox &instanceComboBox)
{
    FUNCTION_ENTRY( "addToPlanInstanceCombo" );

    instanceComboBox.AddInstance(getActivePlanState(), getActivePlanId().plan, getInstanceId());

    FUNCTION_EXIT;
}


void PlanInstance::setSinglePlanInstanceList(CSinglePlanInstanceListCtrl &instanceListCtrl)
{
    FUNCTION_ENTRY( "setSinglePlanInstanceList" );

    ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);

    instanceListCtrl.PopulateInstance(m_activePlanDetail);

    FUNCTION_EXIT;
}


void PlanInstance::setProgressIndicator(CPlanStepListCtrl &stepListCtrl)
{
    FUNCTION_ENTRY( "setProgressIndicator" );

    ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);

    stepListCtrl.UpdatePlanExecutionProgress(m_activePlanDetail);

    FUNCTION_EXIT;
}


void PlanInstance::setProgressDetails(CPlanStepListCtrl &stepListCtrl)
{
    FUNCTION_ENTRY( "setProgressDetails" );

    stepListCtrl.ResetPlanExecutionProgress();

    std::vector<TA_Base_Core::ActiveStepDetail> stepExecDetails;
    getStepExecDetailMap().getItems(stepExecDetails);

    for (std::vector<TA_Base_Core::ActiveStepDetail>::iterator iter = stepExecDetails.begin(); iter != stepExecDetails.end(); iter++)
    {
        stepListCtrl.UpdatePlanExecutionProgress(*iter);
    }

    // Let the step list know what update ID the execution history is based on so that it can discard any outdated updates.
    stepListCtrl.SetPlanExecutionUpdateId(m_stepExecDetailMapLastUpdateId);

    FUNCTION_EXIT;
}


void PlanInstance::setCustomStepDetails(CPlanStepListCtrl &stepListCtrl)
{
    FUNCTION_ENTRY( "setCustomStepDetails" );

    stepListCtrl.ResetCustomStepSettings();

    std::vector<TA_Base_Core::StepCustomisableDetail> stepCustomDetails;
    getStepCustomDetailMap().getItems(stepCustomDetails);

    for (std::vector<TA_Base_Core::StepCustomisableDetail>::iterator iter = stepCustomDetails.begin(); iter != stepCustomDetails.end(); iter++)
    {
        stepListCtrl.UpdateCustomStepSettings(*iter);
    }

    // Let the step list know what update ID the customisation history is based on so that it can discard any outdated updates.
    stepListCtrl.SetStepCustomisationUpdateId(m_stepCustomDetailMapLastUpdateId);

    FUNCTION_EXIT;
}


void PlanInstance::update(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "update" );

    // Update the execution progress map only if it has already been initialised and the update is more recent
    // that the last update inserted into the map.
    if (m_stepExecDetailMap && Utilities::compareUpdateIds(activePlanDetail.activeUpdateId, m_stepExecDetailMapLastUpdateId) > 0)
    {
        StepExecDetailMap& stepExecDetailMap = getStepExecDetailMap();

        // Remove the previous execution state for the step
        if (stepExecDetailMap.find(activePlanDetail.currentStepDetail.position))
        {
            stepExecDetailMap.erase(activePlanDetail.currentStepDetail.position);
        }

        // Insert the new state, but only if the plan was not stopped while the step was loaded, paused or delayed
        if (!(activePlanDetail.activePlanState == TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE &&
              (activePlanDetail.currentStepDetail.state == TA_Base_Core::LOADED_ACTIVE_STEP_STATE ||
               activePlanDetail.currentStepDetail.state == TA_Base_Core::PAUSED_ACTIVE_STEP_STATE ||
               activePlanDetail.currentStepDetail.state == TA_Base_Core::DELAYED_ACTIVE_STEP_STATE)))
        {
            stepExecDetailMap.insert(activePlanDetail.currentStepDetail.position, activePlanDetail.currentStepDetail);
        }

        m_stepExecDetailMapLastUpdateId = activePlanDetail.activeUpdateId;
    }

    bool bUpdateOwnership = false;

    {
        ACE_Write_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);

        // Accept the update only if it is more recent than the previously obtained active plan detail.
        if (Utilities::compareUpdateIds(activePlanDetail.activeUpdateId, m_activePlanDetail.activeUpdateId) > 0)
        {
            TA_Base_Core::EActivePlanState currentPlanState = m_activePlanDetail.activePlanState;

            m_activePlanDetail = activePlanDetail;

            if (activePlanDetail.activePlanState == TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE)
            {
                m_activePlanDetail.activePlanState = currentPlanState;

                bUpdateOwnership = true;
            }
        }
    }

    if ( bUpdateOwnership )
    {
        setInstanceOwnershipIndicator();
    }

    FUNCTION_EXIT;
}


void PlanInstance::update(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail)
{
    FUNCTION_ENTRY( "update" );

    // Update the customisation map only if it has already been initialised and the update is more recent
    // that the last update inserted into the map.
    if (m_stepCustomDetailMap && Utilities::compareUpdateIds(planCustomDetail.customUpdateId, m_stepCustomDetailMapLastUpdateId) > 0)
    {
        StepCustomDetailMap &stepCustomDetailMap = getStepCustomDetailMap();

        if (stepCustomDetailMap.find(planCustomDetail.detail.position))
        {
            stepCustomDetailMap.erase(planCustomDetail.detail.position);
        }

        stepCustomDetailMap.insert(planCustomDetail.detail.position, planCustomDetail.detail);

        m_stepCustomDetailMapLastUpdateId = planCustomDetail.customUpdateId;
    }

    FUNCTION_EXIT;
}


void PlanInstance::stop()
{
    FUNCTION_ENTRY( "stop" );

    PlanAgentAccess::getInstance().stopPlan( getActivePlanId() );

    FUNCTION_EXIT;
}


void PlanInstance::pause()
{
    FUNCTION_ENTRY( "pause" );

    PlanAgentAccess::getInstance().pausePlan( getActivePlanId() );

    FUNCTION_EXIT;
}


void PlanInstance::resume()
{
    FUNCTION_ENTRY( "resume" );

    PlanAgentAccess::getInstance().resumePlan( getActivePlanId() );

    FUNCTION_EXIT;
}


void PlanInstance::takeControl()
{
    FUNCTION_ENTRY( "takeControl" );

    PlanAgentAccess::getInstance().takeControlOfPlan( getActivePlanId() );

    FUNCTION_EXIT;
}


void PlanInstance::releaseControl()
{
    FUNCTION_ENTRY( "releaseControl" );

    // PlanAgentAccess::getInstance().releaseControlOfPlan( getActivePlanId() );

    FUNCTION_EXIT;
}


void PlanInstance::customiseStep(const TA_Base_Core::StepCustomisableDetail &customStepDetail)
{
    FUNCTION_ENTRY( "customiseStep" );

    PlanAgentAccess::getInstance().customiseStep( getActivePlanId() , customStepDetail);

    FUNCTION_EXIT;
}


PlanInstance::StepExecDetailMap &PlanInstance::getStepExecDetailMap()
{
    FUNCTION_ENTRY( "getStepExecDetailMap" );

    if (!m_stepExecDetailMap)
    {
        TA_Base_Core::ExecutionHistory_var executionHistory = getExecutionHistory();

        m_stepExecDetailMap = new StepExecDetailMap();

        for (CORBA::ULong i = 0; i < executionHistory->details.length(); i++)
        {
            m_stepExecDetailMap->insert(executionHistory->details[i].position, executionHistory->details[i]);
        }

        m_stepExecDetailMapLastUpdateId = executionHistory->activeUpdateId;
    }

    FUNCTION_EXIT;
    return *m_stepExecDetailMap;
}


PlanInstance::StepCustomDetailMap &PlanInstance::getStepCustomDetailMap()
{
    FUNCTION_ENTRY( "getStepCustomDetailMap" );

    if (!m_stepCustomDetailMap)
    {
        TA_Base_Core::CustomisationHistory_var customisationHistory = getCustomisationHistory();

        m_stepCustomDetailMap = new StepCustomDetailMap();

        for (CORBA::ULong i = 0; i < customisationHistory->details.length(); i++)
        {
            m_stepCustomDetailMap->insert(customisationHistory->details[i].position, customisationHistory->details[i]);
        }

        m_stepCustomDetailMapLastUpdateId = customisationHistory->customUpdateId;
    }

    FUNCTION_EXIT;
    return *m_stepCustomDetailMap;
}


TA_Base_Core::ExecutionHistory *PlanInstance::getExecutionHistory()
{
    FUNCTION_ENTRY( "getExecutionHistory" );
    FUNCTION_EXIT;
    return PlanAgentAccess::getInstance().getExecutionHistory( getActivePlanId() );
}


TA_Base_Core::CustomisationHistory *PlanInstance::getCustomisationHistory()
{
    FUNCTION_ENTRY( "getCustomisationHistory" );
    FUNCTION_EXIT;
    return PlanAgentAccess::getInstance().getCustomisationHistory( getActivePlanId() );
}


void PlanInstance::setInstanceLocalityIndicator()
{
    FUNCTION_ENTRY( "setInstanceLocalityIndicator" );

    try
    {
        ACE_Read_Guard<ACE_RW_Mutex> activeDetailGuard(m_activePlanDetailLock);
        m_isLocalInstance = (m_activePlanDetail.plan.location == TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation());
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        m_isLocalInstance = false;

        MessageBox::error(IDS_ESTABLISH_LOCALITY_OF_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void PlanInstance::setInstanceOwnershipIndicator()
{
    FUNCTION_ENTRY( "setInstanceOwnershipIndicator" );

    try
    {
        m_ownedByCurrentSession = PlanAgentAccess::getInstance().isOwner( getActivePlanId() );
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        m_ownedByCurrentSession = false;

        MessageBox::error(IDS_DETERMINE_OWNER_OF_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


