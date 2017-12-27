//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanInstanceNone.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "PlanNode.h"
#include "PlanInstanceNone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanInstanceNone

PlanInstanceNone::PlanInstanceNone()
{
    FUNCTION_ENTRY( "PlanInstanceNone" );
    FUNCTION_EXIT;
}


PlanInstanceNone::~PlanInstanceNone()
{
    FUNCTION_ENTRY( "~PlanInstanceNone" );
    FUNCTION_EXIT;
}


// Compares this plan instance with the other instance for display purposes. Plan instance "None"
// should always precede all other instances.
bool PlanInstanceNone::operator<(const PlanInstance &other) const
{
    return true;
}


PlanInstanceId PlanInstanceNone::getInstanceId() const
{
    FUNCTION_ENTRY( "getInstanceId" );
    FUNCTION_EXIT;
    return PlanInstanceId(0, 0);
}


bool PlanInstanceNone::customisationEnabled() const
{
    FUNCTION_ENTRY( "customisationEnabled" );
    FUNCTION_EXIT;
    return false;
}


bool PlanInstanceNone::isLocalInstance() const
{
    FUNCTION_ENTRY( "isLocalInstance" );
    FUNCTION_EXIT;
    return true;
}


bool PlanInstanceNone::ownedByCurrentSession() const
{
    FUNCTION_ENTRY( "ownedByCurrentSession" );
    FUNCTION_EXIT;
    return true;
}


void PlanInstanceNone::updateCmdUI(CCmdUI &cmdUI, PlanNode & /*planNode*/)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    cmdUI.Enable(FALSE);

    FUNCTION_EXIT;
}


void PlanInstanceNone::addToPlanInstanceList(CPlanInstanceListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToPlanInstanceList" );

    // The static instance "None" is not included in the active instance list

    FUNCTION_EXIT;
}


void PlanInstanceNone::setSinglePlanInstanceList(CSinglePlanInstanceListCtrl &instanceListCtrl)
{
    FUNCTION_ENTRY( "setSinglePlanInstanceList" );

    // The static instance "None" has no attributes to show, so blank out the single instance list
    instanceListCtrl.DeleteAllItems();

    FUNCTION_EXIT;
}


TA_Base_Core::ExecutionHistory *PlanInstanceNone::getExecutionHistory()
{
    FUNCTION_ENTRY( "getExecutionHistory" );

    TA_Base_Core::ExecutionHistory *executionHistory = new TA_Base_Core::ExecutionHistory();
    executionHistory->details.length(0);

    FUNCTION_EXIT;
    return executionHistory;
}


TA_Base_Core::CustomisationHistory *PlanInstanceNone::getCustomisationHistory()
{
    FUNCTION_ENTRY( "getCustomisationHistory" );

    TA_Base_Core::CustomisationHistory *customisationHistory = new TA_Base_Core::CustomisationHistory();
    customisationHistory->details.length(0);

    FUNCTION_EXIT;
    return customisationHistory;
}


