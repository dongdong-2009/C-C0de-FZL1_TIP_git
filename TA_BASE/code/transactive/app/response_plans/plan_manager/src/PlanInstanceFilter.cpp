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
// Used by active plan instance list to determine which instances to display
// based on the current filtering rules.
//

#include "StdAfx.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "PlanManager.h"
#include "TreeNodeFactory.h"
#include "PlanNode.h"
#include "PlanAgentAccess.h"
#include "PlanInstance.h"
#include "PlanInstanceFilter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanInstanceFilter

PlanInstanceFilter::PlanInstanceFilter()
{
    FUNCTION_ENTRY( "PlanInstanceFilter" );

    setDefaults();

    FUNCTION_EXIT;
}


PlanInstanceFilter::~PlanInstanceFilter()
{
    FUNCTION_ENTRY( "~PlanInstanceFilter" );
    FUNCTION_EXIT;
}


void PlanInstanceFilter::setDefaults()
{
    FUNCTION_ENTRY( "setDefaults" );

    // By default show all local plan instances.
    setAllOwnersNeeded(true);
    setAllLocationsNeeded(false);

    FUNCTION_EXIT;
}


void PlanInstanceFilter::setAllOwnersNeeded(bool allOwnersNeeded)
{
    FUNCTION_ENTRY( "setAllOwnersNeeded" );

    m_allOwnersNeeded = allOwnersNeeded;

    FUNCTION_EXIT;
}


void PlanInstanceFilter::setAllLocationsNeeded(bool allLocationsNeeded)
{
    FUNCTION_ENTRY( "setAllLocationsNeeded" );

    m_allLocationsNeeded = allLocationsNeeded;

    FUNCTION_EXIT;
}


bool PlanInstanceFilter::allOwnersNeeded() const
{
    FUNCTION_ENTRY( "allOwnersNeeded" );
    FUNCTION_EXIT;
    return m_allOwnersNeeded;
}


bool PlanInstanceFilter::allLocationsNeeded() const
{
    FUNCTION_ENTRY( "allLocationsNeeded" );
    FUNCTION_EXIT;
    return m_allLocationsNeeded;
}


bool PlanInstanceFilter::isRelevant(const TA_Base_Core::ActivePlanId& activePlanId)
{
    FUNCTION_ENTRY( "isRelevant" );

    if ( !TreeNodeFactory::getInstance().canTreeNodeBeShowed(activePlanId.plan) )
    {
        return false;
    }

    // See if we already have the corresponding PlanInstance in storage. It's more efficient to ask
    // PlanInstance about its ownership than going to the Plan Agent for this information.
    PlanNode *planNode = static_cast<PlanNode *>(TreeNodeFactory::getInstance().getTreeNode(activePlanId.plan));
    if (planNode)
    {
        PlanInstanceSharedPtr planInstance = planNode->getPlanInstance(PlanInstanceId(activePlanId.instance, activePlanId.location));
        if (NULL != planInstance.get())
        {
            FUNCTION_EXIT;
            return isRelevant(planInstance);
        }
    }

    // If my location only
    if (!allLocationsNeeded())
    {
        if (activePlanId.location != TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation())
        {
            FUNCTION_EXIT;
            return false;
        }
    }

    // If my instances only
    if (!allOwnersNeeded())
    {
        if (!PlanAgentAccess::getInstance().isOwner(activePlanId))
        {
            FUNCTION_EXIT;
            return false;
        }
    }

    // Satisfies the current filtering rules
    FUNCTION_EXIT;
    return true;
}


bool PlanInstanceFilter::isRelevant(const PlanInstanceSharedPtr& planInstance)
{
    FUNCTION_ENTRY( "isRelevant" );

    if ( !TreeNodeFactory::getInstance().canTreeNodeBeShowed(planInstance->getActivePlanId().plan) )
    {
        return false;
    }

    // If my location only
    if (!allLocationsNeeded())
    {
        if (!planInstance->isLocalInstance())
        {
            FUNCTION_EXIT;
            return false;
        }
    }

    // If my instances only
    if (!allOwnersNeeded())
    {
        if (!planInstance->ownedByCurrentSession())
        {
            FUNCTION_EXIT;
            return false;
        }
    }

    // Satisfies the current filtering rules
    FUNCTION_EXIT;
    return true;
}



bool TA_Base_App::PlanInstanceFilter::isRelevant( TA_Base_Core::NodeId plan, const PlanInstanceId& planInstanceId )
{
    FUNCTION_ENTRY( "isRelevant" );

    if ( !TreeNodeFactory::getInstance().canTreeNodeBeShowed(plan) )
    {
        return false;
    }

    // See if we already have the corresponding PlanInstance in storage. It's more efficient to ask
    // PlanInstance about its ownership than going to the Plan Agent for this information.
    PlanNode *planNode = static_cast<PlanNode *>(TreeNodeFactory::getInstance().getTreeNode(plan));
    if (planNode)
    {
        PlanInstanceSharedPtr planInstance = planNode->getPlanInstance(planInstanceId);
        if (NULL != planInstance.get())
        {
            FUNCTION_EXIT;
            return isRelevant(planInstance);
        }
    }

    FUNCTION_EXIT;
    return true;
}
