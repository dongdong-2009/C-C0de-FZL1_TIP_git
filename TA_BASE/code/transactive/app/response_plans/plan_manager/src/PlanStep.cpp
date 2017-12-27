//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStep.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/08/03 17:02:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "PlanStepParameter.h"
#include "PlanManagerCommonDefs.h"
#include "PlanAgentAccess.h"
#include "PlanStepEditorDlg.h"
#include "CameraStepParameter.h"
#include "PlanStepEditParameterPage.h"
#include "OperatorRights.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

PlanStep::PlanStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    m_stepDetail(stepDetail),
    m_planStepParameterList(NULL),
    m_parent(parent),
    m_newStep(false)
{
    FUNCTION_ENTRY( "PlanStep" );
    FUNCTION_EXIT;
}


PlanStep::PlanStep(PlanNode* parent,PlanStepId stepId,TA_Base_Core::EStep stepType)
  : m_planStepParameterList(NULL),
    m_parent(parent),
    m_newStep(true)
{
    FUNCTION_ENTRY( "PlanStep" );

    m_stepDetail.type = stepType;
    m_stepDetail.step = stepId;
    m_stepDetail.delay = 0;
    m_stepDetail.skip = false;
    m_stepDetail.skippable = false;
    m_stepDetail.ignoreFailure = false;
	m_stepDetail.nowait  = false;
    m_stepDetail.name = CORBA::string_dup("新步骤");
    m_stepDetail.description = CORBA::string_dup("新步骤");
    m_stepDetail.position = 0;

    FUNCTION_EXIT;
}


PlanStep::PlanStep(const PlanStep& planStep)
  : m_planStepParameterList(NULL),
    m_parent(planStep.m_parent),
    m_newStep(true),
    m_stepDetail(planStep.m_stepDetail)
{
    FUNCTION_ENTRY( "PlanStep" );

    m_stepDetail.step = 0;

    FUNCTION_EXIT;
}


PlanStep::~PlanStep()
{
    FUNCTION_ENTRY( "~PlanStep" );

    TA_Base_Core::ThreadGuard paramGuard(m_planStepParameterListLock);

    if (m_planStepParameterList)
    {
        while (!m_planStepParameterList->empty())
        {
            PlanStepParameter *planStepParameter = m_planStepParameterList->back();
            m_planStepParameterList->pop_back();

            delete planStepParameter;
        }

        // *PW 3263*
        delete m_planStepParameterList;
        m_planStepParameterList = 0;
    }

    FUNCTION_EXIT;
}


PlanStepId PlanStep::getStepId() const
{
    FUNCTION_ENTRY( "getStepId" );
    FUNCTION_EXIT;
    return m_stepDetail.step;
}


PlanStepNumber PlanStep::getStepPosition() const
{
    FUNCTION_ENTRY( "getStepPosition" );
    FUNCTION_EXIT;
    return m_stepDetail.position;
}


const char * PlanStep::getStepName() const
{
    FUNCTION_ENTRY( "getStepName" );
    FUNCTION_EXIT;
    return m_stepDetail.name;
}


const char * PlanStep::getStepDescription() const
{
    FUNCTION_ENTRY( "getStepDescription" );
    FUNCTION_EXIT;
    return m_stepDetail.description;
}


TA_Base_Core::EStep PlanStep::getStepType() const
{
    FUNCTION_ENTRY( "getStepType" );
    FUNCTION_EXIT;
    return m_stepDetail.type;
}


bool PlanStep::isStepSkipped() const
{
    FUNCTION_ENTRY( "isStepSkipped" );
    FUNCTION_EXIT;
    return m_stepDetail.skip;
}


bool PlanStep::isStepSkippable() const
{
    FUNCTION_ENTRY( "isStepSkippable" );
    FUNCTION_EXIT;
    return m_stepDetail.skippable;
}

bool PlanStep::noWait() const
{
	return m_stepDetail.nowait;
}

bool PlanStep::ignoreFailure() const
{
    FUNCTION_ENTRY( "ignoreFailure" );
    FUNCTION_EXIT;
    return m_stepDetail.ignoreFailure;
}


TA_Base_Core::Seconds PlanStep::getStepDelay() const
{
    FUNCTION_ENTRY( "getStepDelay" );
    FUNCTION_EXIT;
    return m_stepDetail.delay;
}


// Used by PlanStep copy constructor
void PlanStep::addPlanStepParameter(PlanStepParameter& parameter)
{
    FUNCTION_ENTRY( "addPlanStepParameter" );

    TA_Base_Core::ThreadGuard paramGuard(m_planStepParameterListLock);

    if (m_planStepParameterList==NULL)
    {
        m_planStepParameterList = new PlanStepParameterList();
    }
    m_planStepParameterList->push_back(&parameter);

    FUNCTION_EXIT;
}


void PlanStep::addToStepList(CPlanStepListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToStepList" );

    listCtrl.AddStep(m_stepDetail, this);

    FUNCTION_EXIT;
}


void PlanStep::setStepPosition(PlanStepNumber stepPosition)
{
    FUNCTION_ENTRY( "setStepPosition" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.position = stepPosition;

    FUNCTION_EXIT;
}


void PlanStep::setStepId(PlanStepId stepId)
{
    FUNCTION_ENTRY( "setStepId" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.step = stepId;

    FUNCTION_EXIT;
}


void PlanStep::setStepName(const char* name)
{
    FUNCTION_ENTRY( "setStepName" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.name = name;

    FUNCTION_EXIT;
}


void PlanStep::setStepDescription(const char* description)
{
    FUNCTION_ENTRY( "setStepDescription" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.description = description;

    FUNCTION_EXIT;
}


void PlanStep::setStepSkippable(bool skip)
{
    FUNCTION_ENTRY( "setStepSkippable" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.skippable = skip;

    FUNCTION_EXIT;
}


void PlanStep::setStepSkipped(bool skip)
{
    FUNCTION_ENTRY( "setStepSkipped" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.skip = skip;

    FUNCTION_EXIT;
}


void PlanStep::setIgnoreFailure(bool ignoreFailure)
{
    FUNCTION_ENTRY( "setIgnoreFailure" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.ignoreFailure = ignoreFailure;
}

void PlanStep::setNoWait(bool nowait)
{
	TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
	m_stepDetail.nowait = nowait;
}


void PlanStep::setStepDelay(unsigned long delay)
{
    FUNCTION_ENTRY( "setStepDelay" );

    TA_ASSERT(m_parent->isCopiedPlan(),"Should only update copied plans.");
    m_stepDetail.delay = delay;

    FUNCTION_EXIT;
}


bool PlanStep::editStep()
{
    FUNCTION_ENTRY( "editStep" );

    CPlanStepEditorDlg  dlg(this,true);
    if (dlg.DoModal()==IDOK)
    {
        getParentPlan()->notifyPlanStepUpdate(getStepPosition(), 1);

        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


void PlanStep::viewStep()
{
    FUNCTION_ENTRY( "viewStep" );

    CPlanStepEditorDlg  dlg(this,false);
    dlg.DoModal();

    FUNCTION_EXIT;
}


bool PlanStep::customiseStep(TA_Base_Core::StepCustomisableDetail &customStepDetail)
{
    FUNCTION_ENTRY( "customiseStep" );

    CPlanStepEditorDlg dlg(this, &customStepDetail);

    FUNCTION_EXIT;
    return (dlg.DoModal() == IDOK);
}


void PlanStep::addParametersToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addParametersToParameterList" );

    listCtrl.DeleteAllItems();

    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    listCtrl.SetItemCount(stepParameters.size());

    for (PlanStepParameterList::iterator iter = stepParameters.begin(); iter != stepParameters.end(); iter++)
    {
        PlanStepParameter *planStepParameter = *iter;

        planStepParameter->addToParameterList(listCtrl);
    }

    FUNCTION_EXIT;
}


void PlanStep::getPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "getPlanStepParameters" );

    TA_Base_Core::ThreadGuard paramGuard(m_planStepParameterListLock);

    if (m_planStepParameterList == NULL)
    {
        if (m_newStep)
        {
            // Overriden in derived classes to provide list of steps
            createPlanStepParameters(stepParameters);
        }
        else
        {
            TA_Base_Core::StepParameters_var stepParameterSet = PlanAgentAccess::getInstance().getStepParameters(m_stepDetail.step);

            // Overriden in derived classes to provide list of steps
            createPlanStepParameters(stepParameterSet, stepParameters);
        }

        // *PW 3263*
        // Ensure allocation of the parameter list, to ensure future
        // calls to getPlanStepParameters will skip this chunk of code
        if (m_planStepParameterList == NULL)
        {
            m_planStepParameterList = new PlanStepParameterList();
        }

        // Save them for re-use
        for (PlanStepParameterList::iterator iter = stepParameters.begin(); iter != stepParameters.end(); iter++)
        {
            addPlanStepParameter(**iter);
        }

        FUNCTION_EXIT;
        return;
    }

    // Just use the ones already loaded
    stepParameters.insert(stepParameters.begin(), m_planStepParameterList->begin(), m_planStepParameterList->end());

    FUNCTION_EXIT;
}


TA_Base_Core::Step PlanStep::getUpdatedStepAndParameters()
{
    FUNCTION_ENTRY( "getUpdatedStepAndParameters" );

    TA_Base_Core::Step step;

    // Setup the general step details
    step.detail = m_stepDetail;

    // Ensure step parameters are loaded.
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    // The derived classes each implement the step parameters
    getUpdatedParameters(step);

    FUNCTION_EXIT;
    return step;
}


void PlanStep::updateCmdUI(CCmdUI &cmdUI)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    switch (cmdUI.m_nID)
    {
        case ID_STEP_NEW:
        case ID_STEP_PASTE:  // Plan will disable if no step copied or cut.
            cmdUI.Enable(OperatorRights::getInstance().canEditPlans());
            break;

        case ID_STEP_EDIT:
        case ID_STEP_COPY:   // Cannot cut or copy the end step!
        case ID_STEP_CUT:
            cmdUI.Enable(getStepType()!=TA_Base_Core::END_STEP && OperatorRights::getInstance().canEditPlans());
            break;

        case ID_STEP_DELETE:
            cmdUI.Enable(canDeleteStep() && OperatorRights::getInstance().canEditPlans());
            break;

        default:
            cmdUI.Enable(FALSE);
    }

    FUNCTION_EXIT;
}


bool PlanStep::canDeleteStep()
{
    FUNCTION_ENTRY( "canDeleteStep" );

    // Cannot if we are referenced by any jumps
    FUNCTION_EXIT;
    return true;
}


std::string PlanStep::getParameterName(UINT stringResourceID)
{
    FUNCTION_ENTRY( "getParameterName" );

    CString csName;

    csName.LoadString(stringResourceID);

    FUNCTION_EXIT;
    return csName.operator LPCTSTR();
}


CPlanStepEditParameterPage* PlanStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return NULL;
}


