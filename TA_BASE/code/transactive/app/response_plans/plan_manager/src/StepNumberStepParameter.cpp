/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/StepNumberStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "core/utilities/src/TAAssert.h"
#include "StepNumberStepParameter.h"
#include "PlanStepParameterStepNumberEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

StepNumberStepParameter::StepNumberStepParameter(PlanStep& parent,std::string name, PlanStepId stepId) :
    PlanStepParameter(parent,name),
    m_stepId(stepId)
{
    FUNCTION_ENTRY( "StepNumberStepParameter" );
    FUNCTION_EXIT;
}


StepNumberStepParameter::StepNumberStepParameter(PlanStep& parent, StepNumberStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_stepId(parameter.getStepId())
{
    FUNCTION_ENTRY( "StepNumberStepParameter" );
    FUNCTION_EXIT;
}


StepNumberStepParameter::StepNumberStepParameter(StepNumberStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_stepId(parameter.getStepId())
{
    FUNCTION_ENTRY( "StepNumberStepParameter" );
    FUNCTION_EXIT;
}


StepNumberStepParameter::~StepNumberStepParameter()
{
    FUNCTION_ENTRY( "~StepNumberStepParameter" );
    FUNCTION_EXIT;
}


void StepNumberStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    PlanStepNumber stepNumber = 0;

    // Get from plan as editor may have differennt step numbers for the plan than the factory.
    PlanStep* destinationStep = getParentStep()->getParentPlan()->getPlanStep(m_stepId);
    if (destinationStep!=NULL)
    {
        stepNumber = destinationStep->getStepPosition();
    }

    listCtrl.AddParameter(getName().c_str(), getDestinationStepDescription().c_str(),this);

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* StepNumberStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterStepNumberEditDlg(*this,canEdit);
}


void StepNumberStepParameter::setStepId(PlanStepId stepId)
{
    FUNCTION_ENTRY( "setStepId" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_stepId = stepId;

    FUNCTION_EXIT;
}


std::string StepNumberStepParameter::getDestinationStepDescription()
{
    FUNCTION_ENTRY( "getDestinationStepDescription" );

    std::string   stepDescription;

    // Get the step to which we will be going to
    PlanStep* destinationStep = getParentStep()->getParentPlan()->getPlanStep(m_stepId);
    if (destinationStep!=NULL)
    {
        PlanStepNumber stepNumber = destinationStep->getStepPosition();

        // Return step "<position> - <step name>"
        stepDescription = (LPCTSTR)TA_Base_Bus::StringUtilities::convertToString(stepNumber);
        if (stepNumber > 0)
        {
            stepDescription += " - ";
            stepDescription += destinationStep->getStepName();
        }
    }

    FUNCTION_EXIT;
    return stepDescription;
}


